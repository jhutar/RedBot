#!/usr/bin/env python
# -*- coding: UTF-8 -*-

class Strat():
  """Object to hold data about strategies"""
  def __init__(self, dat):
    dat_list = dat.split(',')
    self.id = int(dat_list[0])
    assert 3 >= self.id >= 0
    self.points = int(dat_list[1])
    assert self.points >= 0
    self.stones = int(dat_list[2])
    assert self.stones >= 0
    self.potion = list(dat_list[3])
    assert len(self.potion) == 3
