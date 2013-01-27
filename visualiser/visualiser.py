#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys
from pprint import pprint
import Image, ImageColor, ImageFont, ImageDraw

tile_size = 20

class GameMap():
  tile_type = {}
  tile_type['ice'] = ('*',)
  tile_type['flower'] = ('.',)
  tile_type['bonus'] = ('+',)
  tile_type['wall'] = ('#',)
  tile_type['free'] = (' ',)
  tile_type['worm'] = {}
  tile_type['worm'][1] = ('a', 'b', 'c', 'd')
  tile_type['worm'][2] = ('h', 'i', 'j', 'k')
  tile_type['worm'][3] = ('o', 'p', 'q', 'r')
  tile_type['worm'][4] = ('w', 'x', 'y', 'z')
  tile_type['worms'] = tile_type['worm'][1] + tile_type['worm'][2] + tile_type['worm'][3] + tile_type['worm'][4]
  tile_free = tile_type['free'] + tile_type['ice'] + tile_type['flower'] + tile_type['bonus']
  tile_blocked = tile_type['worms'] + tile_type['wall']
  def __init__(self, map_file):
    # Load map content
    fp = open(map_file)
    map_content = fp.readlines()
    fp.close()
    # Load rounds
    self.current_round = int(map_content[0].strip().split(' ')[0])
    self.max_rounds = int(map_content[0].strip().split(' ')[1])
    self.remaining_flowers = int(map_content[0].strip().split(' ')[2])
    # Load map size
    self.width = int(map_content[1].strip().split(' ')[0])
    self.height = int(map_content[1].strip().split(' ')[1])
    # Load worms
    self.worm = []
    for i in (0, 1, 2, 3):
      self.worm.append({})
      self.worm[i]['head_x'] = int(map_content[2+i].strip().split(' ')[0])
      self.worm[i]['head_y'] = int(map_content[2+i].strip().split(' ')[1])
      self.worm[i]['tail_x'] = int(map_content[2+i].strip().split(' ')[2])
      self.worm[i]['tail_y'] = int(map_content[2+i].strip().split(' ')[3])
      self.worm[i]['frozeness'] = int(map_content[2+i].strip().split(' ')[4])
      self.worm[i]['bonus'] = int(map_content[2+i].strip().split(' ')[5])
      self.worm[i]['points'] = int(map_content[2+i].strip().split(' ')[6])
    # Define empty playground
    self.playground = []
    for x in range(self.width):
      self.playground.append([])
      for y in range(self.height):
        self.playground[x].append('X')
    # Load playground
    for x in range(self.width):
      for y in range(self.height):
        #print "DEBUG", x, y, map_content[6+y][x]
        self.playground[x][y] = map_content[6+y][x]
        assert self.playground[x][y] in self.tile_free or \
               self.playground[x][y] in self.tile_blocked

  def get_type(self, x, y):
    """return string explaining type of the field"""
    if self.playground[x][y] in self.tile_type['wall']:
      return 'wall'
    if self.playground[x][y] in self.tile_type['flower']:
      return 'flower'
    if self.playground[x][y] in self.tile_type['bonus']:
      return 'bonus'
    if self.playground[x][y] in self.tile_type['ice']:
      return 'ice'
    for i in 0, 1, 2, 3:
      if self.worm[i]['head_x'] == x and self.worm[i]['head_y'] == y:
        return 'head'
    if self.playground[x][y] in self.tile_type['worms']:
      return 'worm'
    return None

  def get_worm_id(self, x, y):
    """return worm id of a worm tile on a given field"""
    if self.playground[x][y] in self.tile_type['worm'][1]:
      return 1
    if self.playground[x][y] in self.tile_type['worm'][2]:
      return 2
    if self.playground[x][y] in self.tile_type['worm'][3]:
      return 3
    if self.playground[x][y] in self.tile_type['worm'][4]:
      return 4
    return None

  def print_map(self):
    for y in range(self.height):
      for x in range(self.width):
        print self.playground[x][y],
      print
    ###self._gridmap.printme()

def load_tile(name, variant=None):
  """Return tile by a given name and its alpha mask. If wariant provided, merge it as well."""
  im = Image.open('tiles/%s.png' % name)
  # As open() is lazy operation, we have to forcibly load()
  # image so we can split() it:
  im.load()
  # If we have variant, load it and merge it
  if variant != None:
    im2 = Image.open('tiles/%s-%s.png' % (name, variant))
    im2.paste(im, im.split()[3])
    im = im2
  ###print 'DEBUG', name, variant, im.size
  return im, im.split()[3]

if __name__ == '__main__':
  # Load all the images we will use
  im_lib = {}
  im_lib_alpha = {}
  for t in 'background', 'wall', 'head', 'worm', 'head-dead', 'worm-dead', 'head-frozen', 'worm-frozen', 'ice', 'flower', 'bonus':
    if t == 'worm' or t == 'head' or t == 'worm-dead' or t == 'head-dead' or t == 'worm-frozen' or t == 'head-frozen':
      im_lib[t] = {}
      im_lib_alpha[t] = {}
      for w in 1, 2, 3, 4:
        im_lib[t][w], im_lib_alpha[t][w] = load_tile(t, w)
    else:
      im_lib[t], im_lib_alpha[t] = load_tile(t)
  ###pprint(im_lib)
  ###pprint(im_lib_alpha)

  # Prepare images
  for gm_txt in sys.argv[5:]:
    print "Working on %s" % gm_txt
    gm = GameMap(gm_txt)
    im = Image.new('RGBA', (gm.width * tile_size, gm.height * tile_size))
    for x in range(gm.width):
      for y in range(gm.height):
        im.paste(im_lib['background'], (x*tile_size, y*tile_size))
        im_type = gm.get_type(x, y)
        if im_type != None:
          # If the worm is frozen or dead, mark the tile so
          if im_type == 'worm' or im_type == 'head':
            im_variant = gm.get_worm_id(x, y) - 1
            if gm.worm[im_variant]['frozeness'] > 0 and gm.worm[im_variant]['frozeness'] < 999999999:
              im_type = "%s-frozen" % im_type
            elif gm.worm[im_variant]['frozeness'] == 999999999:
              im_type = "%s-dead" % im_type
          # Now paste tile which have variant or simple one
          if im_type == 'worm' or im_type == 'head' or im_type == 'worm-dead' or im_type == 'head-dead' or im_type == 'worm-frozen' or im_type == 'head-frozen':
            im_variant = gm.get_worm_id(x, y)
            im.paste(im_lib[im_type][im_variant], (x*tile_size, y*tile_size), im_lib_alpha[im_type][im_variant])
          else:
            im.paste(im_lib[im_type], (x*tile_size, y*tile_size), im_lib_alpha[im_type])

    # Resize, add create output image
    im = im.resize((900, 900), Image.BICUBIC)
    out = Image.new('RGBA', (1200, 900), ImageColor.getcolor('black', 'RGBA'))
    out.paste(im, (0,0))
    ###im = im.resize((600, 300), Image.BICUBIC)
    ###out = Image.new('RGBA', (800, 600), ImageColor.getcolor('black', 'RGBA'))
    ###out.paste(im, (0,150))

    # Layout
    team_x = 920
    team1_name = sys.argv[1]
    team1_y = 120
    team1_color = ImageColor.getcolor('red', 'RGBA')
    team2_name = sys.argv[2]
    team2_y = 280
    team2_color = ImageColor.getcolor('green', 'RGBA')
    team3_name = sys.argv[3]
    team3_y = 440
    team3_color = ImageColor.getcolor('blue', 'RGBA')
    team4_name = sys.argv[4]
    team4_y = 580
    team4_color = ImageColor.getcolor('yellow', 'RGBA')

    def translate_frozeness(f):
      if f == 999999999:
        return "DEAD"
      else:
        return f

    # Add additional info
    draw = ImageDraw.Draw(out)
    font = ImageFont.truetype('/usr/share/fonts/liberation/LiberationMono-Bold.ttf', 40)
    draw.text((team_x, 30), "RedBot2012", font=font, fill=ImageColor.getcolor('red', 'RGBA'))
    font = ImageFont.truetype('/usr/share/fonts/liberation/LiberationMono-Regular.ttf', 30)

    draw.text((team_x, team1_y), team1_name, font=font, fill=ImageColor.getcolor('red', 'RGBA'))
    draw.text((team_x+30, team1_y+30), "points: %s" % gm.worm[0]['points'], font=font, fill=team1_color)
    draw.text((team_x+30, team1_y+60), "bonus: %s" % gm.worm[0]['bonus'], font=font, fill=team1_color)
    draw.text((team_x+30, team1_y+90), "frozeness: %s" % translate_frozeness(gm.worm[0]['frozeness']), font=font, fill=team1_color)
    
    draw.text((team_x, team2_y), team2_name, font=font, fill=ImageColor.getcolor('green', 'RGBA'))
    draw.text((team_x+30, team2_y+30), "points: %s" % gm.worm[1]['points'], font=font, fill=team2_color)
    draw.text((team_x+30, team2_y+60), "bonus: %s" % gm.worm[1]['bonus'], font=font, fill=team2_color)
    draw.text((team_x+30, team2_y+90), "frozeness: %s" % translate_frozeness(gm.worm[1]['frozeness']), font=font, fill=team2_color)

    draw.text((team_x, team3_y), team3_name, font=font, fill=ImageColor.getcolor('blue', 'RGBA'))
    draw.text((team_x+30, team3_y+30), "points: %s" % gm.worm[2]['points'], font=font, fill=team3_color)
    draw.text((team_x+30, team3_y+60), "bonus: %s" % gm.worm[2]['bonus'], font=font, fill=team3_color)
    draw.text((team_x+30, team3_y+90), "frozeness: %s" % translate_frozeness(gm.worm[2]['frozeness']), font=font, fill=team3_color)

    draw.text((team_x, team4_y), team4_name, font=font, fill=ImageColor.getcolor('yellow', 'RGBA'))
    draw.text((team_x+30, team4_y+30), "points: %s" % gm.worm[3]['points'], font=font, fill=team4_color)
    draw.text((team_x+30, team4_y+60), "bonus: %s" % gm.worm[3]['bonus'], font=font, fill=team4_color)
    draw.text((team_x+30, team4_y+90), "frozeness: %s" % translate_frozeness(gm.worm[3]['frozeness']), font=font, fill=team4_color)

    # Save result
    out.save(gm_txt + '.png')

# How to generate:
# ./visualiser.py `find data/ -type f | sort -V`
# find data/ -type f -name \*.png | sort -V > aaa.txt
# mencoder mf://@aaa.txt -mf w=800:h=600:fps=10:type=png -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o output.avi
