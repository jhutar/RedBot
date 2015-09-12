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


class Plan():
  """Object to hold data about game plan. it can be adressed by x and y coordinates and [0,0] is in bottom left corner"""

  class PlanColumn():
    """Helper object to implement Plan[x][y] type of access"""
    def __init__(self, plan, x):
      self.x = x
      self.column = []
      for line in plan:
        self.column.append(line[self.x])
      ###print ">>> self.column:", self.column

    def __getitem__(self, y):
      return self.column[y]

    def __setitem__(self, y, value):
      # If value is stone (i.e. "{'#': startID}"), then replace whatever is in the cell
      # If value is path (e.g. "{'\': startID}"), then just add it to whatever is already in the cell
      ###print ">>> Setting [%s,%s] to '%s'" % (self.x, y, value)
      if '#' in value:
        if '#' not in self.column[y]:
          self.column[y] = value

  def __init__(self, dat):
    self.dat = dat
    self.dat.reverse()   # because y axis have beginning at bottom
    self.columns = {}
    self.plan = []
    for line in self.dat:
      # Line looks like: "A:3;B:3,,A:1,-:1,,,H:1,-:2,G:3;H:3"
      l = []
      for cell in line.split(','):
        # Cell looks like: "A:3;B:3"
        c = {}
        for item in cell.split(';'):
          # Item looks like "A:3"
          if item != '':
            key, val = item.split(':')
            assert key not in c
            c[key] = int(val)
        l.append(c)
      self.plan.append(l)
    ###print ">>> self.plan:", self.plan
    pass

  def __getitem__(self, x):
    if x not in self.columns:
      self.columns[x] = self.PlanColumn(self.plan, x)
    return self.columns[x]

  def __str__(self):
    out = ''
    for y in range(len(self.dat)-1, -1, -1):
      for x in range(len(self.dat)):
        out += str(self[x][y])
      out += "\n"
    return out


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


def test_start_init():
  strat = Strat('0,0,5,ABC')
  assert strat.id == 0
  assert strat.points == 0
  assert strat.stones == 5
  assert strat.potion == ['A', 'B', 'C']
  strat = Strat('1,100,0,XYZ')
  assert strat.id == 1
  assert strat.points == 100
  assert strat.stones == 0
  assert strat.potion == ['X', 'Y', 'Z']

plan_list = [
  'A:3;B:3,-:1,A:1,,,,H:1,,G:3;H:3',
  ',,,,,,,,',
  'B:1,,,,,,,,G:1',
  ',,,,,,,,',
  ',,,,,,,,',
  ',,,,,,,,',
  'C:1,,,,,,,,F:1',
  ',,,,,,,,',
  'C:3;D:3,,D:1,,,,E:1,,E:3;F:3'
]
def test_plan_init():
  plan = Plan(plan_list)
  assert plan[0][0] == {'C': 3, 'D': 3}
  assert plan[1][8] == {'-': 1}
  assert plan[1][0] == {}
  ##print plan

def test_plan_stones():
  plan = Plan(plan_list)
  ###print plan
  plan[0][0] = {'#': 3}
  assert plan[0][0] == {'#': 3}
  plan[0][0] = {'#': 0}   # stones can not be replaced
  assert plan[0][0] == {'#': 3}
  plan[1][8] = {'#': 3}
  assert plan[1][8] == {'#': 3}
  plan[1][0] = {'#': 3}
  assert plan[1][0] == {'#': 3}
  ###print plan

def main():
  game = Game('plan.dat')

if __name__ == '__main__':
  ###main()
  test_start_init()
  test_plan_init()
  test_plan_stones()

# I expect this should work like this:
#
# load all data
# execute all competing strategies with command line params: <strategy_id> <datafile_path> <potion>
# evaluate putting stones on the map
# evaluate extending routes
# count potions completed by each strategy in this round
#   for each strategy:
#     what ingredient do I have access to?
#     how many potions can I create?
#   compete for resources we have to compete for
# update (remove ingredients used for potions in this round...) and dump map
