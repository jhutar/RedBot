#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from plan import Plan
from strat import Strat

class Game():
  """Object to hold data (game plan, strategies) about game"""

  def __init__(self, dat_file):
    self.dat_file = dat_file
    self.dat = []
    self.plan = Plan(self._get_plan())
    self.strats = []
    cookbook = self._get_cookbook()
    for i in range(4):
      strategy = Strat(*self._get_strat(i))
      strategy.set_cookbook(cookbook)
      self.strats.append(strategy)
    print self._get_want_to_use()

  def _ensure_dat(self):
    if self.dat == []:
      fp = open(self.dat_file)
      for line in fp.readlines():
        self.dat.append(line.strip())
      self.dat = filter(lambda x: x != '', self.dat)

  def _get_plan(self):
    """Parse plan file and return only part with map we can feed
       to Plan constructor"""
    self._ensure_dat()
    assert "Mapa:" in self.dat
    assert len(self.dat) > self.dat.index("Mapa:") + 1
    ###print ">>> mapa from plan.dat: ", self.dat[self.dat.index("Mapa:") + 1:]
    return self.dat[self.dat.index("Mapa:") + 1:]

  def _get_strat(self, strat_id):
    """Parse plan file and return only parts with strategies we can
       feed to Start constructor"""
    assert type(strat_id) is int
    id_str = str(strat_id)
    self._ensure_dat()
    found_strats = filter(lambda l: l.startswith("Strategie" + id_str + ": "), self.dat)
    assert len(found_strats) == 1
    strat_data = found_strats[0].split(": ")[1].strip()
    return ["strat" + id_str + "/strat" + id_str + ".sh", strat_id, strat_data]

  def _get_cookbook(self):
    """Parse plan file and return cookbook:
       dict with key == potion ingrediencies and value == points for completing"""
    self._ensure_dat()
    assert "Kucharka:" in self.dat
    assert "Mapa:" in self.dat
    assert len(self.dat) > self.dat.index("Mapa:") + 1 > self.dat.index("Kucharka:")
    return {a[0]:a[1] for a in map(str.split, self.dat[self.dat.index("Kucharka:")+1:self.dat.index("Mapa:")])}

  def _get_want_to_use(self):
    """Execute strat.want_to_use() for all strategies"""
    return [strat.want_to_use(self.plan) for strat in self.strats]

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
