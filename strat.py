#!/usr/bin/env python
# -*- coding: UTF-8 -*-

class Strat():
  """Object to hold data about strategies"""
  def __init__(self, stratbin, stratid, dat):
    self.stratbin = stratbin   # name of executable (will be executed in its WD)
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
    self.cookbook = {}   # dict with key == potion ingrediencies and value == points for completing

  def set_cookbook(self, cookbook):
    self.cookbook = cookbook

  def execute(self, plan):
    """Execute the strategy (in self.strat) in its directory with
       following options: <strategy_id> <datafile_path> <potion>
       Make sure we have some way how to run strategy as different
       user, under some ulimit settings (both not by default) and
       with timeout of, say, 3 seconds. Return STDOUT, log STDERR.
       """
    return ""

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
      for potion in self.cookbook.keys():
        interesting += potion
    else:
      interesting += self.potion
    ###print ">>> want_to_use: Interesting:", interesting
    # Now only get ingredients we are interested in
    interested_ingredients = {}
    for ingredient in ingredients:
      if ingredient in interesting:
        interested_ingredients[ingredient] = ingredients[ingredient]
    ###print ">>> want_to_use: Iterested in these ingredients:", interested_ingredients
    return interested_ingredients
