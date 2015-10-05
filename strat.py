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

  def __str__(self):
    return "[%s] %s (points: %s, stones: %s, main potion: %s, main potion done: %s)" % (self.id, self.stratbin, self.points, self.stones, self.potion, self.potion_done)

  def set_cookbook(self, cookbook):
    self.cookbook = cookbook

  def get_cookbook(self):
    """Return list of ingredients for each potion"""
    if self.potion_done:
      return self.cookbook
    else:
      return [self.potion]

  def execute(self, plan):
    """Execute the strategy (in self.strat) in its directory with
       following options: <strategy_id> <datafile_path> <potion>
       Make sure we have some way how to run strategy as different
       user, under some ulimit settings (both not by default) and
       with timeout of, say, 3 seconds. Return STDOUT, log STDERR.
       """
    return ""

  def brew(self, plan, can_use):
    """Brew potions:
        * check if usable ingredients are connected by our path
        * add points to strategies which brewed something
        * remove used ingredients from the map
       """
    paths = plan.get_paths_for_strat(self.id)
    potions = self.get_cookbook()
    used = {}
    # Create structure we will use to store ingredients we will actually use
    for potion in self.get_cookbook():
      for ingredient in potion:
        used[ingredient] = []
    # Go through all paths and potions and check which of them we can brew
    for potion in self.get_cookbook():
      ###print ">>> brew: Checking potion", potion
      ingredient_use = {}
      for ingredient in potion:
        ingredient_use[ingredient] = []
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
                    ingredient_use[ingredient].append(coord)
      # Count potions we will create
      count = 1000000   # FIXME
      for coords in ingredient_use.values():
        if len(coords) < count:
          count = len(coords)
      ###print ">>> brew: Going to brew %s of %s potion: %s" % (count, potion, ingredient_use)
      # Decrease ingredient counts on the map
      for ingredient, coords in ingredient_use.iteritems():
        for coord in coords:
          plan.use_ingredient(ingredient, coord, count)
      # Add points for brewed potion
      self.points += count
      # If this was our main potion, mark it as done
      if potion == self.potion:
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
    for potion in self.get_cookbook():
      interesting += potion
    ###print ">>> want_to_use: Interesting:", interesting
    # Now only get ingredients we are interested in
    interested_ingredients = {}
    for ingredient in ingredients:
      if ingredient in interesting:
        interested_ingredients[ingredient] = ingredients[ingredient]
    ###print ">>> want_to_use: Iterested in these ingredients:", interested_ingredients
    return interested_ingredients
