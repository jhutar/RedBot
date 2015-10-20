#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys

from strat import Strat
from plan import Plan
from game import Game

def main():
  game = Game('playfields/basic.txt', sys.argv[1:])
  rounds = 2 * game.playfield.columns_count \
           + 2 * game.playfield.rows_count
  for r in range(rounds):
    game.play_round()

if __name__ == '__main__':
  main()
