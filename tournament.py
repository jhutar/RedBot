#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys
import itertools

from redbot import RedBot

PLAYFIELDS = ['playfields/basic.txt']
MAX_PLAYERS_IN_TOURNAMENT = 4
REPEAT = 2

class Tournament():
  def __init__(self, playfields, players, repeat=REPEAT):
    self.playfields = playfields
    self.players = players
    self.repeat = repeat
    self.playfied_scores = dict()
    self.players_in_tournament = min(len(players), MAX_PLAYERS_IN_TOURNAMENT)
    self.total_scores = dict()
    for player in players:
        self.total_scores[player] = 0
    for playfield in playfields:
      self.playfied_scores[playfield] = dict()
      for player in players:
        self.playfied_scores[playfield][player] = 0

  def run(self):
    groups = itertools.permutations(self.players, self.players_in_tournament)
    for group in groups:
      for i in range(self.repeat):
        for playfield in self.playfields:
          redbot_game = RedBot(playfield, group)
          redbot_game.play()
          # TODO: store results, make video of images
          for winner in redbot_game.get_winners():
            self.playfied_scores[playfield][winner] += 1
            self.total_scores[winner] += 1

  def get_winners_of_playfield(self, playfield):
    maxpoints = max(self.playfied_scores[playfield].values())
    return [player for player in self.players if self.playfied_scores[playfield][player] == maxpoints]

  def get_winners(self):
    maxpoints = max(self.total_scores.values())
    return [player for player in self.players if self.total_scores[player] == maxpoints]

def main():
  tournament = Tournament(PLAYFIELDS, sys.argv[1:])
  tournament.run()
  winners = tournament.get_winners()
  if len(winners) == 1:
    print("Winner of the tournament is %s" % winners[0])
  else:
    print("Winners of the tournament are " +  ", ".join(winners))

if __name__ == '__main__':
  main()
