#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from plan import Plan, PATHS
from strat import Strat
import random
import os.path

class Game():
  """Object to hold data (game plan, strategies) about game"""

  def __init__(self, dat_file, executables):
    ###print ">>> __init__: dat_file:", dat_file
    ###print ">>> __init__: executables:", executables
    assert os.path.isfile(dat_file)
    self.dat_file = dat_file
    self.dat = []
    self.executables = executables
    self.round = self._get_round()
    self.playfield = self._get_plan()
    self.strats = []
    self.cookbook = self._get_cookbook()
    for i in range(min(len(self.executables), self._get_strat_cnt())):
      strategy = self._get_strat(self.executables[i], i)
      strategy.cookbook = self.cookbook
      self.strats.append(strategy)

  def _ensure_dat(self):
    if self.dat == []:
      fp = open(self.dat_file)
      for line in fp.readlines():
        self.dat.append(line.strip())
      self.dat = filter(lambda x: x != '', self.dat)

  def _get_round(self):
    """Parse plan file and return round we are on now"""
    self._ensure_dat()
    assert self.dat[0].startswith("Kolo: ")
    return int(self.dat[0][6:])

  def _get_plan_data(self):
    self._ensure_dat()
    assert "Mapa:" in self.dat
    assert len(self.dat) > self.dat.index("Mapa:") + 1
    return self.dat[self.dat.index("Mapa:") + 1:]

  def _get_plan(self):
    """Parse plan file and return Plan object instance"""
    ###print ">>> mapa from plan.dat: ", self.dat[self.dat.index("Mapa:") + 1:]
    return Plan(self._get_plan_data())

  def _get_strat_cnt(self):
    self._ensure_dat()
    found_strats = filter(lambda l: l.startswith("Strategie"), self.dat)
    return len(found_strats)

  def _get_strat_data(self, strat_id):
    assert type(strat_id) is int
    self._ensure_dat()
    found_strats = filter(lambda l: l.startswith("Strategie%s: " % strat_id), self.dat)
    assert len(found_strats) == 1
    return found_strats[0].split(": ")[1]

  def _get_strat(self, strat_exe, strat_id):
    """Parse plan file and return Strat object instance"""
    assert type(strat_exe) is str
    assert type(strat_id) is int
    strat_data = self._get_strat_data(strat_id)
    return Strat(strat_exe, strat_id, strat_data)

  def _get_cookbook(self):
    """Parse plan file and return cookbook:
       dict with key == potion ingrediencies and value == points for completing"""
    self._ensure_dat()
    assert "Kucharka:" in self.dat
    assert "Mapa:" in self.dat
    assert len(self.dat) > self.dat.index("Mapa:") + 1 > self.dat.index("Kucharka:")
    cookbook_str = self.dat[self.dat.index("Kucharka:")+1:self.dat.index("Mapa:")]
    ###print ">>> _get_cookbook: Read:", cookbook_str
    ###print ">>> _get_cookbook: Parsed:", [list(recipe_str) for recipe_str in cookbook_str]
    return [list(recipe_str) for recipe_str in cookbook_str]

  def _get_want_to_use(self):
    """Execute strat.want_to_use() for all strategies"""
    return [strat.want_to_use(self.playfield) for strat in self.strats]

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
      for s in range(len(self.executables)):   # for all strategies
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
              for ss in range(len(self.executables)):
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

  def get_scores(self):
    """Returns dict {id: score}"""
    return {player.id : player.points for player in self.strats}

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
    out = []
    for s in self.strats:
      self.playfield.dump_for_strat(self.round, s)
      out.append(s.execute(self.round))
    print ">>> _get_answers:", out
    return out

  def play_round(self):
    print "===== round %s =====" % self.round
    print self.playfield
    for s in self.strats:
      print s
    strat_ids = range(len(self.strats))
    # TODO: Do something if we are above allowed rounds cout
    # Load all data - done in the __init__
    # Execute all competing strategies to get responses
    answers = self._get_answers()
    # Put stones on the map if some strategy wants to
    for i in strat_ids:
      if len(answers[i]) != 0 and answers[i][0] == '#':
        if self.strats[i].stones > 0:
          try:
            self.playfield.put('#', i, [answers[i][1], answers[i][2]])
            self.strats[i].stones -= 1
          except Exception:
            print "ERROR: failed to put %s on %s by %s" % (answers[i][0], [answers[i][1], answers[i][2]], i)
        else:
          print "ERROR: failed to put %s on %s by %s because we are out of limit for stones" % (answers[i][0], [answers[i][1], answers[i][2]], i)
    # Build paths on the map if some strategy wants to
    random.shuffle(strat_ids)   # without shuffle, last strategy would have
                                # advantage, because it would be building
                                # path on the map with more paths than the
                                # first one
    for i in strat_ids:
      if len(answers[i]) != 0 and answers[i][0] in PATHS:
        try:
          self.playfield.put(answers[i][0], i, [answers[i][1], answers[i][2]])
        except Exception:
          print "ERROR: failed to put %s on %s by %s" % (answers[i][0], [answers[i][1], answers[i][2]], i)
    # Count potions completed by each strategy in this round
    can_uses = self._get_can_use()
    for i in strat_ids:
      self.strats[i].brew(self.playfield, can_uses[i])
    # Increase rounds counter because this round is over
    self.round += 1
    # Dump the map
    self.playfield.dump_globaly("playfield-%.4d.txt"%self.round, self.round, self.strats, self.cookbook)
    print "===== end ====="
