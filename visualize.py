#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os
import sys
import gi
from game import Game

gi.require_version('Gtk', '3.0')

from gi.repository import Gtk, Gdk, GdkPixbuf
import cairo

class RedBotPlayer(Gtk.Grid):

  def __init__(self, file):
    super(RedBotPlayer, self).__init__()
    dwarf = GdkPixbuf.Pixbuf.new_from_file_at_scale("sprites/dwarf_%s.svg" % file, 50, 70, True)
    potion = GdkPixbuf.Pixbuf.new_from_file_at_scale("sprites/potion.svg", 30, 30, True)
    stone = GdkPixbuf.Pixbuf.new_from_file_at_scale("sprites/stone.svg", 30, 30, True)
    self.potion_count = Gtk.Label()
    self.stone_count = Gtk.Label()
    self.attach(Gtk.Image().new_from_pixbuf(dwarf), 0, 0, 1, 2)
    self.attach(Gtk.Image().new_from_pixbuf(potion), 1, 0, 1, 1)
    self.attach(self.potion_count, 2, 0, 1, 1)
    self.attach(Gtk.Image().new_from_pixbuf(stone), 1, 1, 1, 1)
    self.attach(self.stone_count, 2, 1, 1, 1)

class RedBotPlayfield(Gtk.Grid):

  def __init__(self, colors):
    super(RedBotPlayfield, self).__init__()
    self.colors = [Gdk.color_parse(c) for c in colors]
    self.colors.append(Gdk.color_parse("black"))
    self.set_row_homogeneous(True)
    self.set_column_homogeneous(True)
    self.modify_bg(Gtk.StateType.NORMAL, self.colors[-1])

  def setup(self, plan):
    def label(c, i):
      label = Gtk.Label(c)
      label.modify_fg(Gtk.StateType.NORMAL, self.colors[i])
      return label
    def parse_field(field):
      PATHS = ['|', '/', '-', '\\', '#']
      grid = Gtk.Grid()
      grid.modify_bg(Gtk.StateType.NORMAL, Gdk.color_parse("white"))
      grid.set_border_width(1)
      i = 0
      for k, v in field.iteritems():
        if k in PATHS:
          grid.attach(label(k, int(v)), 0, i, 1, 1)
        else:
          grid.attach(label("%s : %s" %(k, v), -1),  0, i, 1, 1)
        i = i + 1
      return grid
    for x in range(plan.columns_count):
      for y in range(plan.rows_count):
        # TODO: use some graphic for fields
        element = self.get_child_at(x, plan.rows_count - y -1)
        if element != None:
          element.destroy()
        self.attach(parse_field(plan[x][y]), x, plan.rows_count - y -1, 1, 1)
    self.show_all()

# TODO: Create video from screenshots - see https://trac.ffmpeg.org/wiki/Create%20a%20video%20slideshow%20from%20images
class RedBotVisualizer(Gtk.Window):

  def __init__(self, directory):
    Gtk.Window.__init__(self, title="Red Bot 2015")

    dwarfs = ["blue", "green", "red", "violet"]
    grid = Gtk.Grid()

    self.players = []
    self.playfield = RedBotPlayfield(dwarfs)
    self.current_round = 0
    self.round_label = Gtk.Label("round: " + str(self.current_round))
    self.next_round_button = Gtk.Button(label="Next Round")
    self.next_round_button.connect("clicked", self.on_button_clicked)
    self.connect("delete-event", Gtk.main_quit)
    grid.attach(self.next_round_button, 0, 0, 1, 1)
    grid.attach(self.playfield, 0, 1, 1, 4)
    grid.attach(self.round_label, 1, 0, 1, 1)
    for i in range(len(dwarfs)):
      self.players.append(RedBotPlayer(dwarfs[i]))
      grid.attach(self.players[i], 1, i + 1, 1, 1)
    self.add(grid)
    self.files = filter(lambda l: l.startswith("playfield-") and l.endswith(".txt"), os.listdir(directory))
    if self.files != []:
      self.files.sort(reverse=True)
      self.next_round()
      self.show_all()
    else:
      print "no playfield found"
      exit(1)

  def make_screenshot(self):
    gdk_win = self.get_window()
    width = gdk_win.get_width()
    height = gdk_win.get_height()
    ims = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
    cr = cairo.Context(ims)
    Gdk.cairo_set_source_window(cr, gdk_win, 0, 0)
    cr.paint()
    ims.write_to_png("screenshot-%.4d.png" % self.current_round)

  def next_round(self):
    g = Game(self.files.pop(), [])
    self.current_round = g._get_round()
    self.round_label.set_text("round: " + str(self.current_round))
    self.playfield.setup(g._get_plan())
    for i in range(4):
      potions, stones, _, _ = g._get_strat_data(i).split(",")
      self.players[i].potion_count.set_text(potions)
      self.players[i].stone_count.set_text(stones)
    return g

  def on_button_clicked(self, widget):
    self.make_screenshot()
    if self.files != []:
      game = self.next_round()
    else:
      Gtk.main_quit()

# todo add help
if __name__ == '__main__':
  directory = sys.argv[1] if len(sys.argv) > 1 else "./"
  win = RedBotVisualizer(directory)
  Gtk.main()
