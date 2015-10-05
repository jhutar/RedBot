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
    assert type(dat) is list
    assert len(dat) > 0
    self.dat = dat
    self.columns = {}
    self.columns_count = self.dat[0].count(',') + 1
    self.rows_count = len(self.dat)
    self.plan = []
    for line in self.dat[::-1]:
      # Line looks like: "A:3;B:3,,A:1,-:1,,,H:1,-:2,G:3;H:3"
      assert self.columns_count == line.count(',') + 1
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

  def __is_edge(self, coords):
    return coords[0] % 2 == 1 or coords[1] % 2 == 1

  def __is_vertex(self, coords):
    return coords[0] % 2 == 0 and coords[1] % 2 == 0

  def __add_if_valid(self, what, to):
    """Returns 'to' with 'what' apended if 'what' are valid coords"""
    if 0 <= what[0] <= self.columns_count-1 and 0 <= what[1] <= self.rows_count-1:
      ###print ">>> __add_if_valid: Adding %s to the list as it is valid coord" % what
      to.append(what)
    else:
      ###print ">>> __add_if_valid: Not adding %s to the list as it is not valid coord" % what
      pass
    return to

  def get_connected_cells(self, coords, cells):
    """Return list of cells which are one one path where given coords are
       part of. Employs recursion so even long paths are discovered.
       'cells' is a lit of cells we already know about."""
    ###print ">>> get_connected_cells: Entering function with %s and %s" % (coords, cells)
    if coords not in cells:   # add starting coords to the output set
      cells.append(coords)
    if self.__is_edge(coords):
      # Find cells on the plan this coords connects (there might be more paths
      # on one coords)
      for item in self.get_continuing_cells(coords):
        if item not in cells:
          cells.append(item)
        if "#" not in self[item[0]][item[1]]:
          ###print ">>> get_connected_cells: get_continuing_cells returned new cell %s, investigating" % item
          cells = self.get_connected_cells(item, cells)
    else:   # this is vertex
      # Now we should check if there are more paths comming from this
      # vertex, but only if there is no stone on it
      if "#" not in self[coords[0]][coords[1]]:
        for item in self.get_connecting_edges(coords):
          # If we have not seen that edge, discover vrcholy connected to it
          # and add it to output set
          if item not in cells:
            ###print ">>> get_connected_cells: get_connecting_edges returned new edge %s, investigating" % item
            cells = self.get_connected_cells(item, cells)
    ###print ">>> get_connected_cells: Returning %s" % cells
    return cells

  def get_continuing_cells(self, coords):
    """Return paths/edges or vertexes attached to given path"""
    assert self.__is_edge(coords)   # make sure this is edge
    candidates = []
    for k, v in self[coords[0]][coords[1]].iteritems():
      ###print ">>> get_continuing_cells: Considering {%s: %s} on %s" % (k, v, coords)
      if k == '|':
        candidates = self.__add_if_valid([coords[0], coords[1]-1], candidates)
        candidates = self.__add_if_valid([coords[0], coords[1]+1], candidates)
      if k == '/':
        candidates = self.__add_if_valid([coords[0]+1, coords[1]-1], candidates)
        candidates = self.__add_if_valid([coords[0]-1, coords[1]+1], candidates)
      if k == '-':
        candidates = self.__add_if_valid([coords[0]-1, coords[1]], candidates)
        candidates = self.__add_if_valid([coords[0]+1, coords[1]], candidates)
      if k == '\\':
        candidates = self.__add_if_valid([coords[0]-1, coords[1]+1], candidates)
        candidates = self.__add_if_valid([coords[0]+1, coords[1]-1], candidates)
    ###print ">>> get_continuing_cells: Found cells continuing from given edge: %s" % candidates
    return candidates

  def get_connecting_edges(self, coords):
    """Return paths/edges connected to this vrchol"""
    assert self.__is_vertex(coords)   # make sure this is vertex
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
    """Return lists of all cells connected by given strategy. There can be
       multiple lists, because one path can be divided by stone."""
    paths = []
    # For every path/edge coords built by given strategy
    for coords in self.list_paths_for_strat(strat):
      ###print ">>> get_paths_for_strat: Considering %s for %s" % (coords, strat)
      processed = False
      # If we already have these coords in some path for this strategy
      for path in paths:
        if coords in path:
          ###print ">>> get_paths_for_strat: Coords %s already known" % coords
          processed = True
          break
      # If these coords are not part of this strategy
      if not processed:
        ###print ">>> get_paths_for_strat: Coords not in any of the paths, computing additional path"
        # Compute path these coords are part of
        path = self.get_connected_cells(coords, [])
        path.sort()
        # And if this path is not known already, add it
        assert path not in paths   # we are checking using that 'processed' before
        ###print ">>> get_paths_for_strat: This new path (%s) is not know yet, adding it among other paths" % path
        paths.append(path)
    ###print ">>> get_paths_for_strat: Returning:", paths
    return paths

  def get_all_ingredients(self):
    """Return list of all ingredients which can be found on the map"""
    ingredients = []
    for x in range(self.columns_count):
      for y in range(self.rows_count):
        if self.__is_vertex([x, y]):
          for i in self[x][y]:
            if i not in PATHS and i != '#':
              if i not in ingredients:
                ingredients.append(i)
    ingredients.sort()
    ###print ">>> get_all_ingredients: Returning", ingredients
    return ingredients

