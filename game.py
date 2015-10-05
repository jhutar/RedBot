#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from plan import Plan
from strat import Strat
import random

class Game():
  """Object to hold data (game plan, strategies) about game"""

  def __init__(self, dat_file, executables):
    self.dat_file = dat_file
    self.dat = []
    self.executables = executables
    self.playfield = self._get_plan()
    self.strats = []
    cookbook = self._get_cookbook()
    for i in range(4):
      strategy = self._get_strat(self.executables[i], i)
      strategy.set_cookbook(cookbook)
      self.strats.append(strategy)

  def _ensure_dat(self):
    if self.dat == []:
      fp = open(self.dat_file)
      for line in fp.readlines():
        self.dat.append(line.strip())
      self.dat = filter(lambda x: x != '', self.dat)

  def _get_plan(self):
    """Parse plan file and return Plan object instance"""
    self._ensure_dat()
    assert "Mapa:" in self.dat
    assert len(self.dat) > self.dat.index("Mapa:") + 1
    ###print ">>> mapa from plan.dat: ", self.dat[self.dat.index("Mapa:") + 1:]
    return Plan(self.dat[self.dat.index("Mapa:") + 1:])

  def _get_strat(self, strat_exe, strat_id):
    """Parse plan file and return Strat object instance"""
    assert type(strat_exe) is str
    assert type(strat_id) is int
    self._ensure_dat()
    found_strats = filter(lambda l: l.startswith("Strategie%s: " % strat_id), self.dat)
    assert len(found_strats) == 1
    strat_data = found_strats[0].split(": ")[1]
    return Strat(strat_exe, strat_id, strat_data)

  def _get_cookbook(self):
    """Parse plan file and return cookbook:
       dict with key == potion ingrediencies and value == points for completing"""
    self._ensure_dat()
    assert "Kucharka:" in self.dat
    assert "Mapa:" in self.dat
    assert len(self.dat) > self.dat.index("Mapa:") + 1 > self.dat.index("Kucharka:")
    return {a[0]:int(a[1]) for a in map(str.split, self.dat[self.dat.index("Kucharka:")+1:self.dat.index("Mapa:")])}

  def _get_want_to_use(self):
    """Execute strat.want_to_use() for all strategies"""
    r = []
    for s in self.strats:
      r.append(s.want_to_use(self.playfield))
    ###print ">>> _get_want_to_use: Returning", r
    return r

  def _get_can_use(self, want_to_use=None):
    """Find ingredients wanted by more than one strategy and distribute
       these fairly"""
    if not want_to_use:
      # Just for testing. This should be None all the time in normal use
      want_to_use = self._get_want_to_use()
    ###print ">>> _get_want_to_use: Detected these want_to_use:", want_to_use
    can_use = [{}, {}, {}, {}]
    for i in self.playfield.get_all_ingredients():   # for all ingredients
      seen = []   # store already processed coords here to lower count
                  # of required iterations
      conflicts = {'coords': [], 'strats': []}
      for s in range(4):   # for all strategies
        # Now check all the involved coords that they are not requested
        # by other strategy
        if i in want_to_use[s]:
          for c in want_to_use[s][i]:
            if c not in seen:   # othervise we have already considered
                                # this coord for this ingredient
              seen.append(c)
              # Go through rest of strategies to find conflicts and store them
              # in 'conflicts' structure
              problematic = False
              for ss in range(4):
                if ss == s:
                  continue
                # Now finally detect conflicts of interests
                if i in want_to_use[ss] and c in want_to_use[ss][i]:
                  problematic = True
                  if c in conflicts['coords']:
                    conflicts['strats'][-1:].append(ss)   # FIXME: this seems fragile
                  else:
                    ###print ">>> _get_can_use: Detected conflict", c, i, s, ss
                    conflicts['coords'].append(c)
                    conflicts['strats'].append([s, ss])
              # If there is no conflict for given ingredient/coords, just add
              # it to the 'can_use' structure
              if not problematic:
                can_use[s][i] = []
                can_use[s][i].append(c)
      ###print ">>> _get_want_to_use: Detected these conflicts:", conflicts
      # Now resolve the conflicts
      for c in range(len(conflicts['coords'])):
        ###print ">>> _get_can_use: Processing %s/%s conflict of %s" % (i, conflicts['coords'][c], conflicts['strats'][c])
        winner = random.choice(conflicts['strats'][c])
        ###print ">>> _get_can_use: And the winner is", winner
        if i not in can_use[winner]:
          can_use[winner][i] = []
        can_use[winner][i].append(conflicts['coords'][c])
    ###print ">>> _get_can_use: Returning", can_use
    return can_use

  def _brew_potions(self):
    """Brew potions:
        * check if usable ingredients are connected by our path
        * add points to strategies which brewed something
        * remove used ingredients from the map
       """
    can_use = self._get_can_use()
    for i in range(len(can_use)):
      self.strats[i].brew(self.playfield, can_use[i])

  def _get_answers(self):
    """Execute strat.execute() for all strategies. Ideally in parallel
       (if Python can do reall parallel computation (?))."""
    return ['', '', '', '']

  def round(self):
    answer = self._get_answers()
    # And now do all the rest
