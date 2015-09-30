#!/usr/bin/env python
# -*- coding: UTF-8 -*-

PATHS = ['|', '/', '-', '\\']

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
    """Put some item (like stone or path) to given cell.
       If adding a stone (i.e. "{'#': startID}"), then replace whatever is in the cell
       If adding a path (e.g. "{'\': startID}"), then just add it to whatever is already in the cell"""
    ###print ">>> Setting [%s,%s] to '%s'" % (self.x, y, value)
    assert len(value) == 1   # only one item can be placed in one time (this is limitation of this method implementation only)
    k = value.keys()[0]
    v = value.values()[0]
    if k == '#':   # lets place stone
      assert 0 <= v <= 3   # value for stone is stratID
      if '#' not in self.column[y]:   # only place the stone on the fields without stone
        self.column[y] = value   # just remove whatever was on the field before
    elif k in PATHS:   # lets add some path to the field
      assert 0 <= v <= 3
      if '#' not in self.column[y]:   # only place new path on the fields without stone
        if k not in self.column[y]:   # only place new path on the field if same path is not already there
          self.column[y][k] = v
    else:
      raise Exception("Do not know how to set this value (%s, %s, %s)" % (self.x, y, value))


class Plan():
  """Object to hold data about game plan. it can be adressed by x and y coordinates and [0,0] is in bottom left corner"""
  def __init__(self, dat):
    self.dat = dat
    self.columns = {}
    self.columns_count = self.dat[0].count(',') + 1
    self.rows_count = len(self.dat)
    self.plan = []
    for line in self.dat[::-1]:
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
      self.columns[x] = PlanColumn(self.plan, x)
    return self.columns[x]

  def __str__(self):
    out = ''
    for y in range(len(self.dat)-1, -1, -1):
      for x in range(len(self.dat)):
        out += str(self[x][y])
      out += "\n"
    return out

  def __eq__(self, other):
    return self.plan == other.plan

  def __add_if_valid(self, what, to):
    """Returns 'to' with 'what' apended if 'what' are valid coords"""
    if 0 <= what[0] <= self.columns_count-1 and 0 <= what[1] <= self.rows_count-1:
      ###print ">>> __add_if_valid: Adding %s to the list as it is valid coord" % what
      to.append(what)
    else:
      ###print ">>> __add_if_valid: Not adding %s to the list as it is not valid coord" % what
      pass
    return to

  def get_connected_vrcholy(self, coords, edges_seen=[]):
    """Return list of vrchols this given coords connects and list of edges
       we have seen when looking for these vrchols. Given coords have to be
       edge. Employs recursion so even long paths are discovered."""
    # FIXME: return edges seen in same path as part of returned set othervise it is left persistent in the function
    # FIXME: This is teribly ineffective
    # FIXME: Edges seen should be `set()` which contains unique items only
    assert coords[0] % 2 == 1 or coords[1] % 2 == 1   # make sure this is edge
    ###print ">>> get_connected_vrcholy: Entering function with %s and %s" % (coords, edges_seen)
    # Find cells on the plan this coords connects (there might be more paths
    # on one coords)
    vrcholy = []
    for k, v in self[coords[0]][coords[1]].iteritems():
      assert k in PATHS
      ###print ">>> get_connected_vrcholy: Considering {%s: %s} on %s" % (k, v, coords)
      if k == '|':
        vrcholy = self.__add_if_valid([coords[0], coords[1]-1], vrcholy)
        vrcholy = self.__add_if_valid([coords[0], coords[1]+1], vrcholy)
      elif k == '/':
        vrcholy = self.__add_if_valid([coords[0]+1, coords[1]-1], vrcholy)
        vrcholy = self.__add_if_valid([coords[0]-1, coords[1]+1], vrcholy)
      elif k == '-':
        vrcholy = self.__add_if_valid([coords[0]-1, coords[1]], vrcholy)
        vrcholy = self.__add_if_valid([coords[0]+1, coords[1]], vrcholy)
      elif k == '\\':
        vrcholy = self.__add_if_valid([coords[0]-1, coords[1]+1], vrcholy)
        vrcholy = self.__add_if_valid([coords[0]+1, coords[1]-1], vrcholy)
    ###print ">>> get_connected_vrcholy: Have %s edges" % edges_seen
    edges_seen.append(coords)
    ###print ">>> get_connected_vrcholy: Have %s edges" % edges_seen
    ###print ">>> get_connected_vrcholy: Have %s vrcholy" % vrcholy
    # If some of the returned coords is edge, run this recursively on this
    # edge and only add what we do not have yet, if it is vrchol, add it
    # to the output set and find paths leading from that vrchol
    vrcholy_out = []
    for item in vrcholy:
      if item[0] % 2 == 1 or item[1] % 2 == 1:   # if this is edge
        ###print ">>> get_connected_vrcholy: %s is edge, so runnig this function recursively on it" % item
        if item not in edges_seen:
          edges_seen.append(item)
        if "#" in self[item[0]][item[1]]:
          ###print ">>> get_connected_vrcholy: Stone found on edge %s, not going to find more vrchols connected by it" % item
          continue
        vrcholy2, edges_tmp = self.get_connected_vrcholy(item, edges_seen)
        for e in edges_tmp:
          if e not in edges_seen:
            edges_seen.append(e)
        ###print ">>> get_connected_vrcholy: Got back these vrcholy %s" % vrcholy2
        for item2 in vrcholy2:
          if item2 not in vrcholy_out:
            vrcholy_out.append(item2)
      else:   # if this is vrchol
        ###print ">>> get_connected_vrcholy: %s is vrchol, so adding it to the output set" % item
        vrcholy_out.append(item)
        # Now we should check if there are more paths comming from this
        # vrchol, but only if there is no stone on it
        if "#" in self[item[0]][item[1]]:
          continue
        edges2 = self.get_connecting_edges(item)
        vrcholy2 = []
        for item2 in edges2:
          # If we have not seen that edge, discover vrcholy connected to it
          # and add it to output set
          if item2 in edges_seen:
            ###print ">>> get_connected_vrcholy: get_connecting_edges returned edge %s we know about, ignoring" % item2
            pass
          else:
            ###print ">>> get_connected_vrcholy: get_connecting_edges returned new edge %s, investigating" % item2
            vrcholy2_tmp, edges_tmp = self.get_connected_vrcholy(item2)
            vrcholy2 += vrcholy2_tmp
            for e in edges_tmp:
              if e not in edges_seen:
                edges_seen.append(e)
        for item2 in vrcholy2:
          if item2 not in vrcholy_out:
            vrcholy_out.append(item2)
    ###print ">>> get_connected_vrcholy: Returning %s (edges seen: %s)" % (vrcholy_out, edges_seen)
    return vrcholy_out, edges_seen

  def get_connecting_edges(self, coords):
    """Return paths/edges connected to this vrchol"""
    assert coords[0] % 2 == 0 and coords[1] % 2 == 0   # make sure this is vrchol
    x_modif = [0]
    if coords[0] != 0:
      x_modif.append(-1)
    if coords[0] != self.columns_count - 1:
      x_modif.append(1)
    y_modif = [0]
    if coords[1] != 0:
      y_modif.append(-1)
    if coords[1] != self.rows_count - 1:
      y_modif.append(1)
    ###print ">>> get_connecting_edges: Considering these x:%s and y:%s coordinates modificators for %s" % (x_modif, y_modif, coords)
    out = []
    for x_mod in x_modif:
      for y_mod in y_modif:
        x = coords[0] + x_mod
        y = coords[1] + y_mod
        if x_mod == 0 and y_mod == 1:   # up
          if '|' in self[x][y]:
            out.append([x, y])
        if x_mod == 1 and y_mod == 1:   # up right
          if '/' in self[x][y]:
            out.append([x, y])
        if x_mod == 1 and y_mod == 0:   # right
          if '-' in self[x][y]:
            out.append([x, y])
        if x_mod == 1 and y_mod == -1:   # down right
          if '\\' in self[x][y]:
            out.append([x, y])
        if x_mod == 0 and y_mod == -1:   # down
          if '|' in self[x][y]:
            out.append([x, y])
        if x_mod == -1 and y_mod == -1:   # down left
          if '/' in self[x][y]:
            out.append([x, y])
        if x_mod == -1 and y_mod == 0:   # left
          if '-' in self[x][y]:
            out.append([x, y])
        if x_mod == -1 and y_mod == 1:   # up left
          if '\\' in self[x][y]:
            out.append([x, y])
    ###print ">>> get_connecting_edges: Found connected edges on these coords %s" % out
    return out

  def list_paths_for_strat(self, strat):
    """Return list of coords of all paths built directly by given strategy"""
    paths = []
    for x in range(self.columns_count):
      for y in range(self.rows_count):
        for k, v in self[x][y].items():
          if k in PATHS and v == strat:
            paths.append([x, y])
    ###print ">>> list_paths_for_strat: Paths created by strategy %s: %s" % (strat, paths)
    return paths

  def get_paths_for_strat(self, strat):
    """Return lists of all vrchol connected by given strategy. It can be
       multiple lists, because one path can be divided by stone."""
    paths = []
    edges_seen = []
    # For every path/edge coords built by given strategy
    for coords in self.list_paths_for_strat(strat):
      ###print ">>> get_paths_for_strat: Considering %s for %s" % (coords, strat)
      processed = False
      # If we already have these coords in some path for this strategy
      if coords in edges_seen:
        ###print ">>> get_paths_for_strat: Coords already known"
        processed = True
        break
      # If these coords are not part of this strategy
      if processed == False:
        ###print ">>> get_paths_for_strat: Coords not in any of the paths, computing additional path"
        # Compute path these coords are part of
        new, edges_tmp = self.get_connected_vrcholy(coords)
        for e in edges_tmp:
          if e not in edges_seen:
            edges_seen.append(e)
        new.sort()
        # And if this path is not known already, add it
        if new not in paths:
          ###print ">>> get_paths_for_strat: This new path (%s) is not know yet, adding it among other paths" % new
          paths.append(new)
    ###print ">>> get_paths_for_strat: Returning:", paths
    return paths
