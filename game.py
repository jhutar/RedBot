#!/usr/bin/env python
# -*- coding: UTF-8 -*-

class Game():
  """Object to hold data (game plan, strategies) about game"""

  def __init__(self, dat_file):
    self.dat_file = dat_file
    self.plan = Plan(self._get_plan())
    self.strats = []
    for i in range(4):
      self.strats.append(Strat(self._get_strat(i)))

  def _ensure_dat(self):
    if self.dat == None:
      fp = open(self.dat_file)
      for line in fp.readlines():
        self.dat.append(line.strip())

  def _get_plan(self):
    """Parse plan file and return only part with map we can feed
       to Plan constructor"""
    pass

  def _get_strat(self):
    """Parse plan file and return only parts with strategies we can
       feed to Start constructor"""
    pass

  def _get_want_to_use(self):
    """Execute strat.want_to_use() for all strategies"""
    return [{}, {}, {}, {}]

  def _get_can_use(self):
    """Find ingredients wanted by more than one strategy and distribute
       these fairly"""
    want_to_use = self._get_want_to_use()
    return [{}, {}, {}, {}]

  def _brew_potions(self):
    """Brew potions:
        * add points to strategies which brewed something
        * remove used ingredients from the map
       """
    pass

  def _get_answers(self):
    """Execute strat.execute() for all strategies. Ideally in parallel
       (if Python can do reall parallel computation (?))."""
    return ['', '', '', '']

  def round(self):
    answer = self._get_answers()
    # And now do all the rest
