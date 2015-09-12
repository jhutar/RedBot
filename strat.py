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

  def want_to_use(self, plan):
    """Return structure with list of ingredients (connected by this strategy
       paths) this strategy could turn into potion. Format of the structure
       is: {
             x: {
               y: { ...what strategy would like to use from this cell... }
               y_another: { ... }
               ...
             }
             ...
           }
       Maximum amount of relevant ingredients should be returned."""
    # Just for testing purposes: assuming this strategy have to mix H+F+G
    # potion, so although it is connected, "0: { 6: {'B': 1} }" should not
    # be part of the response.
    if self.id == 0:
      return {
        6: {
          8: {'H': 1},
        },
        8: {
          2: {'F': 1},
          8: {'G': 3, 'H': 3},
        }
      }
