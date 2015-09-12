#!/usr/bin/env python
# -*- coding: UTF-8 -*-

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
