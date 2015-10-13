#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys

from strat import Strat
from plan import Plan
from game import Game

def main():
  game = Game('playfields/basic.txt', sys.argv[1:])
  for r in range(7):
    game.play_round()

if __name__ == '__main__':
  main()
