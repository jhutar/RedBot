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
    dwarf = GdkPixbuf.Pixbuf.new_from_file_at_scale(file, 50, 70, True)
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

  def __init__(self):
    super(RedBotPlayfield, self).__init__()

  def setup(self, plan):
    # FIXME: Parse game plan
    size = len(plan)
    for x in range(size):
      for y in range(size):
        # TODO: use some graphic for fields
        self.attach(Gtk.Button(), x, y, 1, 1)

# TODO: Create video from screenshots - see https://trac.ffmpeg.org/wiki/Create%20a%20video%20slideshow%20from%20images
class RedBotVizualizer(Gtk.Window):

  def __init__(self, directory):
    Gtk.Window.__init__(self, title="Red Bot 2015")

    self.players = []
    grid = Gtk.Grid()
    self.playfield = RedBotPlayfield()
    self.current_round = 0
    self.round_label = Gtk.Label("round: " + str(self.current_round))
    self.next_round_button = Gtk.Button(label="Next Round")
    self.next_round_button.connect("clicked", self.on_button_clicked)
    self.connect("delete-event", Gtk.main_quit)
    grid.attach(self.next_round_button, 0, 0, 1, 1)
    grid.attach(self.playfield, 0, 1, 1, 4)
    grid.attach(self.round_label, 1, 0, 1, 1)
    dwarfs = ["sprites/dwarf_blue.svg", "sprites/dwarf_green.svg", "sprites/dwarf_red.svg", "sprites/dwarf_violet.svg"]
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
    self.playfield.setup(g._get_plan_data())
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


if __name__ == '__main__':
  directory = sys.argv[1] if len(sys.argv) > 1 else "./"
  win = RedBotVizualizer(directory)
  Gtk.main()
