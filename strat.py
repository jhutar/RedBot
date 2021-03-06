#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os
import os.path
import subprocess
import random

from plan import PATHS

class Strat():
  """Object to hold data about strategies"""
  def __init__(self, stratbin, stratid, dat):
    self.stratwd = os.path.dirname(stratbin)   # directory of this executable, we will cd there before executing it
    assert os.path.isdir(self.stratwd)
    self.stratbin = os.path.basename(stratbin)   # name of executable (will be executed in its WD via './something')
    assert os.path.isfile(os.path.join(self.stratwd, self.stratbin))
    assert os.access(os.path.join(self.stratwd, self.stratbin), os.X_OK)
    # TODO: assert the file exists and is executable
    self.id = stratid   # ID strategy have (0 to 3)
    assert 3 >= self.id >= 0
    dat_list = dat.split(',')
    self.points = int(dat_list[0])   # points strategy have
    assert self.points >= 0
    self.stones = int(dat_list[1])   # how many stones can strategy place
    assert self.stones >= 0
    self.potion = list(dat_list[2])   # basic potion recipe
    assert len(self.potion) == 3
    self.potion_done = False   # did this strategy brew its basic potion (i.e. can it now brew potions from cookbook)?
    if dat_list[3] == '1':
      self.potion_done = True
    assert self.potion_done in (True, False)
    self.cookbook = []   # list of lists of ingrediens for each potion, please set it directly

  def __str__(self):
    return "[%s] %s (points: %s, stones: %s, main potion: %s, main potion done: %s)" % (self.id, self.stratbin, self.points, self.stones, self.potion, self.potion_done)

  def get_cookbook(self):
    """Return list of ingredients for each potion"""
    if self.potion_done:
      ###print ">>> get_cookbook: [%s] potion done? %s, returning %s" % (self.id, self.potion_done, self.cookbook)
      return self.cookbook
    else:
      ###print ">>> get_cookbook: [%s] potion done? %s, returning %s" % (self.id, self.potion_done, [self.potion])
      return [self.potion]

  def execute(self, current_round=0):
    """Execute the strategy (in self.strat) in its directory with
       <strategy_id> option. Threre will be file playfield.txt with
       all the required data in CWD of the strategy.
       Make sure we have some way how to run strategy as different
       user, under some ulimit settings (both not by default) and
       with timeout of, say, 3 seconds. Return STDOUT, log STDERR.
       """
    # We do expect playfield.txt file for this strategy exists
    # Execute the strategy
    ###print ">>> execute: Going to run './%s %s'" % (self.stratbin, str(self.id))
    def runuser(uid=os.getuid(), gid=os.getgid()):
      return lambda : (os.setgid(gid), os.setuid(uid)) # group id has to be set as the first

    t_limit = 120 # seconds of cpu time
    m_limit = 2**24 # kbytes
    ulimit_settings = "ulimit -t %d -m %d -v %d -d %d" % (t_limit, m_limit, m_limit, m_limit)
    ###print ">>> ulimit settings: ", ulimit_settings
    # log STDERR
    stderr_log = "2>%s_round_%d_stderr.txt" % (self.stratbin, current_round)
    command_str = "%s; ./%s %d %s" % (ulimit_settings, self.stratbin, self.id, stderr_log)
    # shell execute
    try:
      out = subprocess.check_output([command_str], shell=True, cwd=self.stratwd, preexec_fn=runuser(1000, 1000))
    except subprocess.CalledProcessError as command_exception:
      #print ">>> strategy %s exit with code %d\toutput: %s" % (self.stratbin, command_exception.returncode, command_exception.output)
      return []
    ###print ">>> execute: Returned:", out
    out_split = out.split(' ')
    if len(out_split) != 3:
      return []
    if out_split[0] not in PATHS + ['#']:
      return []
    try:
      out_x = int(out_split[1])
      out_y = int(out_split[2])
    except ValueError:
      return []
    out_list = [out_split[0], out_x, out_y]
    return out_list

  def brew(self, plan, can_use):
    """Brew potions:
        * check if usable ingredients are connected by our path
        * add points to strategies which brewed something
        * remove used ingredients from the map
       """
    ###print ">>> brew: Strat %s can use %s" % (self.id, can_use)
    paths = plan.get_paths_for_strat(self.id)
    potions = self.get_cookbook()
    used = {}
    # Create structure we will use to store ingredients we will actually use
    for potion in potions:
      for ingredient in potion:
        used[ingredient] = []
    # Go through all paths and potions and check which of them we can brew
    for potion in potions:
      ###print ">>> brew: Checking ingredient_use", potion
      ingredient_use = {}
      for ingredient in potion:
        ingredient_use[ingredient] = set()
      for path in paths:
        ###print ">>> brew: Checking that potion on path", path
        for ingredient in potion:
          if ingredient in can_use:
            ###print ">>> brew: Ingredient is present in can use", ingredient
            for coord in can_use[ingredient]:
              ###print ">>> brew: Checking if coords %s are part of given path" % coord
              if coord in path:
                for ingredient in potion:
                  if ingredient in plan[coord[0]][coord[1]]:
                    ###print ">>> brew: There is %s on %s. Taking it." % (ingredient, coord)
                    ingredient_use[ingredient].add((coord, plan[coord[0]][coord[1]][ingredient]))
      # Count potions we will create
      count = reduce(min, map(lambda lst: sum(map(lambda (_, cnt): cnt, lst)), ingredient_use.values()))
      ###print ">>> brew: Going to brew %s of %s potion from %s" % (count, potion, ingredient_use)
      # Decrease ingredient counts on the map
      for ingredient, crds in ingredient_use.iteritems():
        coords = list(crds)
        to_remove = count
        random.shuffle(coords) # FIXME: Should we remove all ingredients from the coord, or use some distribution?
        for (coord, ingredient_cnt) in coords:
          if to_remove == 0:
            break
          remove_at = min(to_remove, ingredient_cnt)
          plan.use_ingredient(ingredient, coord, remove_at)
          to_remove = to_remove - remove_at

      # Add points for brewed potion
      self.points += count
      # FIXME: This all above should be per strat and per its connected path!
      # If this was our main potion, mark it as done
      if count > 0 and potion == self.potion:
        self.potion_done = True

  def want_to_use(self, plan):
    """Return structure with list of ingredients (connected by this strategy
       paths) this strategy could turn into potion. Format of the structure
       is: {
             <ingredient>: [<coords>],
             ...
           }
       Maximum amount of relevant ingredients should be returned."""
    # Get list of all ingredients available
    # (we do not care about stones - i.e. multiple paths, these will be
    # taken care later when brewing the potion)
    ingredients = {}
    for path in plan.get_paths_for_strat(self.id):
      for cell in path:
        for ingredient in plan[cell[0]][cell[1]]:
          if ingredient not in ingredients:
            ingredients[ingredient] = []
          ingredients[ingredient].append(cell)
    ###print ">>> want_to_use: Ingredients:", ingredients
    # Now compile list of ingrediens we are interested in
    # i.e. if whe have not brew basic potion it is only these from basic
    # potion, if we brew it already, it is these from cookbook
    interesting = []
    if self.potion_done:
      for potion in self.get_cookbook():
        interesting += potion
    else:
      interesting = self.potion
    ###print ">>> want_to_use: Interesting:", interesting
    # Now only get ingredients we are interested in
    interested_ingredients = {}
    for ingredient in ingredients:
      if ingredient in interesting:
        interested_ingredients[ingredient] = ingredients[ingredient]
    ###print ">>> want_to_use: Iterested in these ingredients:", interested_ingredients
    return interested_ingredients
