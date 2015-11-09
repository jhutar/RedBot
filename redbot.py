#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys

from strat import Strat
from plan import Plan
from game import Game

class RedBot():
  """Helper for running tournaments"""
  def __init__(self, playfield='playfields/basic.txt', players=[]):
    self.game = Game(playfield, players)
    self.players = players

  def play(self):
    rounds = 2 * self.game.playfield.columns_count \
             + 2 * self.game.playfield.rows_count
    for r in range(rounds):
      self.game.play_round()

  def get_winners(self):
    scores = self.game.get_scores()
    if scores == []:
      return []

    maxscore = max(scores.values())
    return [self.players[i] for i in scores.keys() if scores[i] == maxscore]

def main():
  game = Game('playfields/basic.txt', sys.argv[1:])
  rounds = 2 * game.playfield.columns_count \
           + 2 * game.playfield.rows_count
  for r in range(rounds):
    game.play_round()

if __name__ == '__main__':
  main()
