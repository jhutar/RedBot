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
    pass

  def _get_strat(self):
    pass

  def round(self):
    answer = ["", "", "", ""]
