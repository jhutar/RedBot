#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys
from pprint import pprint
import Image, ImageColor, ImageFont, ImageDraw
import re
import os

GAME = sys.argv[1]
print 'INFO: Parsing', GAME

# Like default_playfield.txt-motoko_kusanagi-LampBot-Gorily-Hyperborean/
m = re.search('^.*\.txt-([^-/]+)-([^-/]+)-([^-/]+)-([^-/]+)/?$', GAME)

TEAM = []
TEAM.append(m.group(1))
TEAM.append(m.group(2))
TEAM.append(m.group(3))
TEAM.append(m.group(4))
print "INFO: Teams", TEAM

FILE = os.path.join(GAME, 'default_playfield.txt.lastplay')
FILE_FD = open(FILE, 'r')
FILE_LINES = FILE_FD.readlines()
FILE_FD.close()
print 'INFO: File', FILE

PROPS = [u'území', u'vojáci', u'vojenství', u'jídlo', u'rolníci', u'farmaření', u'tajné služby']
#		fprintf(fd," %u %u %u %u %u %u %u", kingdom[i].land, kingdom[i].food,
#			kingdom[i].soldiers, kingdom[i].peasants, kingdom[i].arms,
#			kingdom[i].farming, kingdom[i].secret_services);
PROPS_POS = [1, 3, 5, 2, 4, 6, 7]
PROPS_MAX = [1, 1, 1, 1, 1, 1, 1]   # 1 instead of 0 to workaround some ZeroDivisionErrors
PROPS_COLORS = [
    ImageColor.getcolor('#e00', 'RGBA'),
    ImageColor.getcolor('#c03', 'RGBA'),
    ImageColor.getcolor('#a06', 'RGBA'),
    ImageColor.getcolor('#0e0', 'RGBA'),
    ImageColor.getcolor('#0c3', 'RGBA'),
    ImageColor.getcolor('#0a6', 'RGBA'),
    ImageColor.getcolor('yellow', 'RGBA'),
]

# Determine maximums for each property
re_one = "([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)"
re_full = "^[0-9]+/[0-9]+: %s %s %s %s$" % (re_one, re_one, re_one, re_one)
def get_prop(line, kingnom_id, prop_id):
  m = re.search(re_full, line)
  position = PROPS_POS[prop_id]+len(PROPS)*kingnom_id
  return int(m.group(position))

for line in FILE_LINES:
  # Like 2/500: 25 6 1 1 3 5 0 25 3 2 2 3 3 0 25 6 1 1 5 3 0 25 3 1 3 3 3 0
  for p in range(len(PROPS)):
    for k in range(len(TEAM)):
      val = get_prop(line, k, p)
      if PROPS_MAX[p] <= val:
        PROPS_MAX[p] = val
print "INFO: Maximums", PROPS_MAX

# Size of image
SIZE_X = 900
SIZE_Y = 600

# Borders of properties
X_FROM = [200, 300, 400, 500, 600, 700, 800]
X_TO = [300, 400, 500, 600, 700, 800, 900]

# Borders of kingdoms
Y_FROM = [110, 235, 360, 485]
Y_TO = [215, 340, 465, 590]

COLOR = []
COLOR.append(ImageColor.getcolor('white', 'RGBA'))
COLOR.append(ImageColor.getcolor('white', 'RGBA'))
COLOR.append(ImageColor.getcolor('white', 'RGBA'))
COLOR.append(ImageColor.getcolor('white', 'RGBA'))
COLOR_BG = ImageColor.getcolor('black', 'RGBA')
COLOR_REDBOT = ImageColor.getcolor('white', 'RGBA')

FONT = ImageFont.truetype('/usr/share/fonts/liberation/LiberationMono-Bold.ttf', 30)
FONT_REDBOT = ImageFont.truetype('/usr/share/fonts/liberation/LiberationMono-Bold.ttf', 40)
FONT_HEAD = ImageFont.truetype('/usr/share/fonts/liberation/LiberationMono-Bold.ttf', 17)

for line in FILE_LINES:
  #print 'DEBUG: Line', line
  OUT = Image.new('RGBA', (SIZE_X, SIZE_Y), COLOR_BG)
  DRAW = ImageDraw.Draw(OUT)

  # Add RedBot label
  DRAW.text((20, 10), 'RedBot', font=FONT_REDBOT, fill=COLOR_REDBOT)
  DRAW.text((20, 50), '2012', font=FONT_REDBOT, fill=COLOR_REDBOT)
  DRAW.text((20, 90), 'podzim', font=FONT_REDBOT, fill=COLOR_REDBOT)

  # Add labes for each property
  for j in range(len(PROPS)):
    y_move = 0
    for t in PROPS[j].split(' '):
      DRAW.text((X_FROM[j], 70+y_move), t, font=FONT_HEAD, fill=COLOR_REDBOT)
      y_move += 15

  # Add round where we are
  DRAW.text((SIZE_X-100, 10), line.split('/')[0], font=FONT_REDBOT, fill=COLOR_REDBOT)

  # Add info about each team
  for i in range(len(TEAM)):
    # Add team name
    for j in range(len(TEAM[i])/8+1):
      #print 'DEBUG: Printing team name', len(TEAM[i]), TEAM[i][8*j:8*(j+1)]
      DRAW.text((20, Y_FROM[i]+50+j*30), TEAM[i][8*j:8*(j+1)], font=FONT, fill=COLOR[i])
    # Add properties status
    for j in range(len(PROPS)):
      width = (X_TO[j]-X_FROM[j])*float(get_prop(line, i, j))/PROPS_MAX[j]
      #print 'DEBUG: kingdom = %s, property = %s: %s (%s px of %s px)' %(i, j, get_prop(line, i, j), width, X_TO[j]-X_FROM[j])
      DRAW.rectangle((X_FROM[j], Y_FROM[i], X_FROM[j]+width, Y_TO[i]), fill=PROPS_COLORS[j])
      DRAW.text((X_TO[j]-30, Y_TO[i]-20), str(get_prop(line, i, j)), font=FONT_HEAD, fill=COLOR_REDBOT)

  n = os.path.join(GAME, 'visualiser-%05d.png' % int(line.split('/')[0]))
  #print 'DEBUG: Saving to', n
  OUT.save(n)

# Lets stay a while at the end
n = os.path.join(GAME, 'visualiser-%05d.png' % (int(line.split('/')[0])+1))
OUT.save(n)
n = os.path.join(GAME, 'visualiser-%05d.png' % (int(line.split('/')[0])+2))
OUT.save(n)
n = os.path.join(GAME, 'visualiser-%05d.png' % (int(line.split('/')[0])+3))
OUT.save(n)
n = os.path.join(GAME, 'visualiser-%05d.png' % (int(line.split('/')[0])+4))
OUT.save(n)
