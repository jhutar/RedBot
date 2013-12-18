#!/usr/bin/python

import sys
import re

class player:
    def __init__(self, a_player, a_rockets = [[],[]], base = [] ):
        self.player = a_player
        self.rockets = a_rockets
        self.base = base
        self.command1 = "X"
        self.command2 = "X"
        
    def get_moves(self, rocket, dest = []):
        moves = []
        if self.rockets[rocket-1][0]+2 < SIZE_X :
            dist = get_distance([self.rockets[rocket-1][0]+2,self.rockets[rocket-1][1]],dest)
            moves.append({'move':"p p", 'distance' : dist})
        if self.rockets[rocket-1][0]-2 >= 0 :
            dist = get_distance([self.rockets[rocket-1][0]-2,self.rockets[rocket-1][1]],dest)
            moves.append({'move':"l l", 'distance' : dist})
        if self.rockets[rocket-1][1]+2 < SIZE_Y :
            dist = get_distance([self.rockets[rocket-1][0],self.rockets[rocket-1][1]+2],dest)
            moves.append({'move':"d d", 'distance' : dist})
        if self.rockets[rocket-1][1]-2 >= 0 :
            dist = get_distance([self.rockets[rocket-1][0],self.rockets[rocket-1][1]-2],dest)
            moves.append({'move':"n n", 'distance' : dist})
        if self.rockets[rocket-1][0]+1 < SIZE_X and self.rockets[rocket-1][1]+1 < SIZE_Y:
            dist = get_distance([self.rockets[rocket-1][0]+1,self.rockets[rocket-1][1]+1],dest)
            moves.append({'move':"p d", 'distance' : dist})
        if self.rockets[rocket-1][0]+1 < SIZE_X and self.rockets[rocket-1][1]-1 >= 0:
            dist = get_distance([self.rockets[rocket-1][0]+1,self.rockets[rocket-1][1]-1],dest)
            moves.append({'move':"p n", 'distance' : dist})
        if self.rockets[rocket-1][0]-1 >= 0 and self.rockets[rocket-1][1]+1 < SIZE_Y :
            dist = get_distance([self.rockets[rocket-1][0]-1,self.rockets[rocket-1][1]+1],dest)
            moves.append({'move':"l d", 'distance' : dist})
        if self.rockets[rocket-1][0]-1 >= 0 and self.rockets[rocket-1][1]-1 >= 0 :
            dist = get_distance([self.rockets[rocket-1][0]-1,self.rockets[rocket-1][1]-1],dest)
            moves.append({'move':"l n", 'distance' : dist})
        if self.rockets[rocket-1][0]+1 < SIZE_X :
            dist = get_distance([self.rockets[rocket-1][0]+1,self.rockets[rocket-1][1]],dest)
            moves.append({'move':"p", 'distance' : dist}) 
        if self.rockets[rocket-1][0]-1 >= 0:
            dist = get_distance([self.rockets[rocket-1][0]-1,self.rockets[rocket-1][1]],dest)
            moves.append({'move':"l", 'distance' : dist})
        if self.rockets[rocket-1][1]+1 < SIZE_Y :
            dist = get_distance([self.rockets[rocket-1][0],self.rockets[rocket-1][1]+1],dest)
            moves.append({'move':"d", 'distance' : dist}) 
        if self.rockets[rocket-1][1]-1 >= 0 :
            dist = get_distance([self.rockets[rocket-1][0],self.rockets[rocket-1][1]-1],dest)
            moves.append({'move':"n", 'distance' : dist})
            
        return moves
                         
                 
    def get_direction(self, rocket):
        direction = []
        if self.base[0] - self.rockets[rocket-1][0] > 0 and lines[6+self.rockets[rocket-1][1]][self.rockets[rocket-1][0]+1] == " " and [self.rockets[rocket-1][0]+1,self.rockets[rocket-1][1]] != other.base:
            direction.append("p")
        if self.base[0] - self.rockets[rocket-1][0] < 0 and lines[6+self.rockets[rocket-1][1]][self.rockets[rocket-1][0]-1] == " " and [self.rockets[rocket-1][0]-1,self.rockets[rocket-1][1]] != other.base:
            direction.append("l")
        if self.base[1] - self.rockets[rocket-1][1] > 0 and lines[6+self.rockets[rocket-1][1]+1][self.rockets[rocket-1][0]] == " " and [self.rockets[rocket-1][0],self.rockets[rocket-1][1]+1] != other.base:
            direction.append("d")
        if self.base[1] - self.rockets[rocket-1][1] < 0 and lines[6+self.rockets[rocket-1][1]-1][self.rockets[rocket-1][0]] == " " and [self.rockets[rocket-1][0],self.rockets[rocket-1][1]-1] != other.base:
            direction.append("n")     
        return direction
    
    def check_collision(self, direction1, direction2):
        if direction1 == "l" :
            newpos1 = [self.rockets[0][0]-1,self.rockets[0][1]]
        elif direction1 == "p" :
            newpos1 = [self.rockets[0][0]+1,self.rockets[0][1]]
        elif direction1 == "n" :
            newpos1 = [self.rockets[0][0],self.rockets[0][1]-1]
        elif direction1 == "d" :
            newpos1 = [self.rockets[0][0],self.rockets[0][1]+1]
        
        if direction2 == "l" :
            newpos2 = [self.rockets[1][0]-1,self.rockets[1][1]]
        elif direction2 == "p" :
            newpos2 = [self.rockets[1][0]+1,self.rockets[1][1]]
        elif direction2 == "n" :
            newpos2 = [self.rockets[1][0],self.rockets[1][1]-1]
        elif direction2 == "d" :
            newpos2 = [self.rockets[1][0],self.rockets[1][1]+1]
        if (newpos1 != self.base and newpos2 != self.base) and (newpos1==newpos2  or ((newpos1[0]==newpos2[0]==self.base[0] or newpos1[1]==newpos2[1]==self.base[1]) and get_distance(newpos1,newpos2)<2)):
            return True
        else :
            return False
        
    
    def rocket_dir(self,rocket, position = []):
        cmd = []      
        i=1
        for rkt in other.rockets:
            if position[0] == rkt[0] :
                if rkt[1] > position[1] :
                    if self.rockets[rocket-1][0] == self.rockets[2-rocket][0] and self.rockets[2-rocket][1] > self.rockets[rocket-1][1] and self.rockets[2-rocket][1] <= rkt[1] :
                        cmd.append({'direction': "d", 'rocket': i, 'distance': rkt[1]-position[1], 'free': False, 'asteroid': lines[6+rkt[1]][rkt[0]] != " "})
                    else :
                        cmd.append({'direction': "d", 'rocket': i, 'distance': rkt[1]-position[1], 'free': True, 'asteroid': lines[6+rkt[1]][rkt[0]] != " "})
                elif rkt[1] < position[1] :
                    if self.rockets[rocket-1][0] == self.rockets[2-rocket][0] and self.rockets[2-rocket][1] < self.rockets[rocket-1][1]  and self.rockets[2-rocket][1] >= rkt[1]:
                        cmd.append({'direction': "n", 'rocket': i, 'distance': position[1]-rkt[1], 'free': False, 'asteroid': lines[6+rkt[1]][rkt[0]] != " "})
                    else :
                        cmd.append({'direction': "n", 'rocket': i, 'distance': position[1]-rkt[1], 'free': True, 'asteroid': lines[6+rkt[1]][rkt[0]] != " "})
            if position[1] == rkt[1] :
                if rkt[0] > position[0] :
                    if self.rockets[rocket-1][0] < self.rockets[2-rocket][0] and self.rockets[2-rocket][1] == self.rockets[rocket-1][1]  and self.rockets[2-rocket][0] <= rkt[0] :
                        cmd.append({'direction': "p", 'rocket': i, 'distance': rkt[0]-position[0], 'free': False, 'asteroid': lines[6+rkt[1]][rkt[0]] != " "})
                    else :
                        cmd.append({'direction': "p", 'rocket': i, 'distance': rkt[0]-position[0], 'free': True, 'asteroid': lines[6+rkt[1]][rkt[0]] != " "})
                elif rkt[0] < position[0] :                    
                    if self.rockets[rocket-1][0] > self.rockets[2-rocket][0] and self.rockets[2-rocket][1] == self.rockets[rocket-1][1]  and self.rockets[2-rocket][0] >= rkt[0] :
                        cmd.append({'direction': "l", 'rocket': i, 'distance': position[0]-rkt[0], 'free': False, 'asteroid': lines[6+rkt[1]][rkt[0]] != " "})
                    else :
                        cmd.append({'direction': "l", 'rocket': i, 'distance': position[0]-rkt[0], 'free': True, 'asteroid': lines[6+rkt[1]][rkt[0]] != " "})
            i += 1
        return cmd
    
    def get_state(self):
        if lines[6+self.rockets[0][1]][self.rockets[0][0]] != " ":
            if lines[6+self.rockets[1][1]][self.rockets[1][0]] != " ":
                if self.rockets[0] == self.rockets[1]:
                    if lines[6+self.rockets[0][1]][self.rockets[0][0]] == "2":
                        if asteroids[0]['value'] == 2:
                            if [asteroids[0]['x'],asteroids[0]['y']] == self.rockets[0] :
                                return 'MTB2'
                            else :
                                return 'MTA2'
                        else :
                            return 'MTA1'
                    else :
                        if asteroids[0]['value'] == 1:
                            if [asteroids[0]['x'],asteroids[0]['y']] == self.rockets[0] :
                                return 'MTB11-1'
                            elif [asteroids[1]['x'],asteroids[1]['y']] == self.rockets[1]:
                                return 'MTB11-2'
                            else :
                                return 'MTA1'
                        else :
                            return 'MTA2'
                elif lines[6+self.rockets[0][1]][self.rockets[0][0]] == "1":
                    if lines[6+self.rockets[1][1]][self.rockets[1][0]] == "1" :
                        if asteroids[0]['value'] == 2:
                            return 'MTA2'
                        else :
                            if [asteroids[0]['x'],asteroids[0]['y']] == self.rockets[0]:
                                if [asteroids[1]['x'],asteroids[1]['y']] == self.rockets[1]:
                                    return 'MTB12'
                                else:
                                    return 'MTB11-1'
                            elif [asteroids[0]['x'],asteroids[0]['y']] == self.rockets[1] :
                                if [asteroids[1]['x'],asteroids[1]['y']] == self.rockets[0]:
                                    return 'MTB12'
                                else:
                                    return 'MTB11-2'
                            else:
                                return 'MTA1'
                    else :
                        if asteroids[0]['value'] == 2:
                            return 'MTA2'
                        elif [asteroids[0]['x'],asteroids[0]['y']] == self.rockets[0]:
                            return 'MTB11-1'
                        else :
                            return 'MTA1'
                elif lines[6+self.rockets[1][1]][self.rockets[1][0]] == "1":
                    if asteroids[0]['value'] == 2 :
                        return 'MTA2'
                    else :
                        if [asteroids[0]['x'],asteroids[0]['y']] == self.rockets[1] :
                            return 'MTB11-2'
                        else:
                            return 'MTA1'
                else :
                    if asteroids[0]['value'] == 2:
                        return 'MTA2'
                    else :
                        return 'MTA1'
            else :
                if lines[6+self.rockets[0][1]][self.rockets[0][0]] == "1":
                    if asteroids[0]['distance1'] <=  asteroids[0]['distance2']:
                        if [asteroids[0]['x'],asteroids[0]['y']] == self.rockets[0]:
                            return 'MTB11-1'
                        else :
                            if asteroids[0]['value'] == 2:
                                return 'MTA2'
                            else :
                                return 'MTA1'
                    elif [asteroids[1]['x'],asteroids[1]['y']] == self.rockets[0]:
                        return 'MTB11-1'
                    else:
                        if asteroids[0]['value'] == 2:
                            return 'MTA2'
                        else :
                            return 'MTA1'  
                else:
                    if asteroids[0]['value'] == 2:
                        return 'MTA2'
                    else :
                        return 'MTA1'                 
        elif lines[6+self.rockets[1][1]][self.rockets[1][0]] != " ":
            if lines[6+self.rockets[1][1]][self.rockets[1][0]] == "1" :
                if asteroids[0]['distance2'] <  asteroids[0]['distance1']:
                    if [asteroids[0]['x'],asteroids[0]['y']] == self.rockets[1] :
                        return 'MTB11-2'
                    else: 
                        if asteroids[0]['value'] == 2:
                            return 'MTA2'
                        else :
                            return 'MTA1'
                elif [asteroids[1]['x'],asteroids[1]['y']] == self.rockets[1]:
                    return 'MTB11-2'
                else:
                    if asteroids[0]['value'] == 2:
                        return 'MTA2'
                    else :
                        return 'MTA1'
            else :
                if asteroids[0]['value'] == 2:
                    return 'MTA2'
                else :
                    return 'MTA1'
        else :
            if asteroids[0]['value'] == 2:
                return 'MTA2'
            else :
                return 'MTA1'
            
    
    def check_secure(self, rocket, cmd):
        newpos = []
        if cmd[0] == "p" :
            if cmd == "p":
                newpos = [self.rockets[rocket-1][0]+1, self.rockets[rocket-1][1]]
            elif cmd == "p p":
                newpos = [self.rockets[rocket-1][0]+2, self.rockets[rocket-1][1]]
            elif cmd == "p n":
                newpos = [self.rockets[rocket-1][0]+1, self.rockets[rocket-1][1]-1]
            elif cmd == "p d":
                newpos = [self.rockets[rocket-1][0]+1, self.rockets[rocket-1][1]+1]
        elif cmd [0] == "l":
            if cmd == "l":
                newpos = [self.rockets[rocket-1][0]-1, self.rockets[rocket-1][1]]
            elif cmd == "l l":
                newpos = [self.rockets[rocket-1][0]-2, self.rockets[rocket-1][1]]
            elif cmd == "l n":
                newpos = [self.rockets[rocket-1][0]-1, self.rockets[rocket-1][1]-1]
            elif cmd == "l d":
                newpos = [self.rockets[rocket-1][0]-1, self.rockets[rocket-1][1]+1]
        elif cmd [0] == "n":
            if cmd == "n":
                newpos = [self.rockets[rocket-1][0], self.rockets[rocket-1][1]-1]
            elif cmd == "n n":
                newpos = [self.rockets[rocket-1][0], self.rockets[rocket-1][1]-2]
        elif cmd [0] == "d":
            if cmd == "d":
                newpos = [self.rockets[rocket-1][0], self.rockets[rocket-1][1]+1]
            elif cmd == "d d":
                newpos = [self.rockets[rocket-1][0], self.rockets[rocket-1][1]+2]
        dirs = self.rocket_dir(rocket, newpos)
        if len(dirs) > 0 :
            if rocket == 1 and me.command2[0] == "s":
                dirs2 = self.rocket_dir(2, self.rockets[1])
                for smer2 in dirs2:
                    for smer1 in dirs:
                        if smer1['direction'] == me.command2[2] == smer2['direction'] and  smer1['rocket'] == smer2['rocket'] and smer2['distance'] > smer1['distance']:
                            return False
            elif rocket == 2 and me.command1[0] == "s":
                dirs1 = self.rocket_dir(1, self.rockets[0])
                for smer1 in dirs1:
                    for smer2 in dirs:
                        if smer2['direction'] == me.command1[2] == smer1['direction'] and  smer2['rocket'] == smer1['rocket'] and smer1['distance'] > smer2['distance']:
                            return False
        return True
                        
    def check_available(self, x, y):
        result = False
        if x > self.base[0] :
            result |= lines[y+6][x-1] == ' '
        if x < self.base[0]:
            result |= lines[y+6][x+1] == ' '
        if y > self.base[1]:
            result |= lines[y+5][x] == ' '
        if y < self.base[1]:
            result |= lines [y+7][x] == ' '

        return result

def get_distance(x = [], y = []):
    return abs(x[0]-y[0]) + abs(x[1]-y[1])


subor = open('playfield.txt')
lines = subor.readlines()
subor.close()

position = int(sys.argv[1])

rv=r'Hrac [12]: Body:(\d+) Zakladna:\[(\d+),(\d+)\] Rakety:\[(\d+),(\d+)\]:\[(\d+),(\d+)\]'
rvc = re.compile(rv)
p1 = rvc.match(lines[2])
p2 = rvc.match(lines[3])

base1 = [int(p1.group(2)),int(p1.group(3))]
base2 = [int(p2.group(2)),int(p2.group(3))]

SIZE_X = int(lines[1].split(' ')[0])
SIZE_Y = int(lines[1].split(' ')[1])

if lines[0][0] != '0' :
    previous=[]
    previous.append(lines[3+position].split('(')[1].split(')')[0].split(':')[0])
    previous.append(lines[3+position].split('(')[1].split(')')[0].split(':')[1])
else :
    previous = [" "," "]

if position == 1:
    me = player(1, [[int(p1.group(4)), int(p1.group(5))],[int(p1.group(6)), int(p1.group(7))]], base1)
    other = player(2, [[int(p2.group(4)), int(p2.group(5))],[int(p2.group(6)), int(p2.group(7))]], base2)
else :
    other = player(1, [[int(p1.group(4)), int(p1.group(5))],[int(p1.group(6)), int(p1.group(7))]], base1)
    me = player(2, [[int(p2.group(4)), int(p2.group(5))],[int(p2.group(6)), int(p2.group(7))]], base2)
    
asteroids = []

for i in range(SIZE_Y):
    for j in range(SIZE_X):
        if lines[i+6][j] != ' ':
            if me.check_available(j,i) :
                asteroids.append({'x': j, 'y': i, 'value': int(lines[i+6][j]), 'distance1' : get_distance(me.rockets[0], [j,i]), 'distance2': get_distance(me.rockets[1], [j,i]), 'bdistance': get_distance(me.base, [j,i])})

asteroids = sorted(asteroids, key = lambda k: k['bdistance'])
state = me.get_state()
rocket1 = True
rocket2 = True
pos_s1 = me.rocket_dir(1,me.rockets[0])
pos_s2 = me.rocket_dir(2,me.rockets[1])
    

if state == 'MTB2':
    if len(pos_s1) > 0 :
        me.command1 = "s "+pos_s1[0]['direction']
        me.command2 = "b"
        rocket1 = rocket2 = False
        if previous[0] == me.command1 and lines[3+position].split(':')[3].split('(')[1].split(')')[0].split(',')[1] != "zasah":
            rocket1 = rocket2 = True
    if rocket1 and rocket2 :
        direction = me.get_direction(1)
        me.command1 = "t "+direction[0]
        me.command2 = "t "+direction[0]
        if previous[0] == me.command1 and lines[3+position].split(':')[3][0] == "E":
            me.command1 = "b"
            me.command2 = "b"
        
elif state == 'MTB12':
    
    if len(pos_s1) == 1:       
        if me.rockets[0] == me.rockets[1] :
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False 
        elif pos_s1[0]['free'] :
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False
        else:
            if len(pos_s2) == 2:
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False
    elif len(pos_s1) == 2:
        if not pos_s1[0]['free'] :
            if pos_s1[1]['free']:
                me.command1 = "s "+pos_s1[1]['direction']
                rocket1 = False
        elif not pos_s1[1]['free'] :
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False
                
    if previous[0] == me.command1 and lines[3+position].split(':')[3].split('(')[1].split(')')[0].split(',')[1] != "zasah":
        me.command1 = ""
        rocket1 = True
        
    if len(pos_s2) == 1:       
        if me.rockets[0] != me.rockets[1] :
            if pos_s2[0]['free'] :
                me.command2 = "s "+pos_s2[0]['direction']
                rocket2 = False
            else:
                if len(pos_s1) == 2:
                    me.command2 = "s "+pos_s2[0]['direction']
                    rocket2 = False
    elif len(pos_s2) == 2:
        if not pos_s2[0]['free'] :
            if pos_s2[1]['free']:
                me.command2 = "s "+pos_s2[1]['direction']
                rocket2 = False
        elif not pos_s2[1]['free'] :
            me.command2 = "s "+pos_s2[0]['direction']
            rocket2 = False
        
    if previous[1] == me.command2 and lines[3+position].split(':')[4].split('(')[1].split(')')[0].split(',')[1] != "zasah":
        me.command2 = ""
        rocket2 = True
        
    if rocket1:
        if me.rockets[0] != other.rockets[0] and me.rockets[0] != other.rockets[1] : 
            direction = me.get_direction(1)
            if len(direction) > 0 :
                me.command1 = "t "+direction[0]
                if previous[0] == me.command1 and lines[3+position].split(':')[3][0] == "E":
                    me.command1 = "b"
            else :
                me.command1 = "b"
        else :
            me.command1 = "b"
    
    if rocket2:
        if me.rockets[1] != other.rockets[0] and me.rockets[1] != other.rockets[1] :
            direction = me.get_direction(2)
            if len(direction) > 0:
                if me.command1[0] == "t" and me.check_collision(me.command1[2],direction[0]):
                    if len(direction) > 1 :
                        me.command2 = "t "+direction[1]
                    else :
                        direction1 = me.get_direction(1)
                        if len(direction1) > 1:
                            me.command1 = "t "+direction1[1]
                            me.command2 = "t "+direction[0]
                        else :
                            me.command2 = "b"
                else :
                    me.command2 = "t "+direction[0]
            else:
                me.command2 = "b"
            if previous[1] == me.command2 and lines[3+position].split(':')[4][0] == "E":
                me.command2 = "b"
        else :
            me.command2 = "b"
            
elif state == 'MTB11-1':
    
    if len(pos_s1) == 1:       
        if me.rockets[0] == me.rockets[1] :
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False 
        elif pos_s1[0]['free'] :
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False
        else:
            if len(pos_s2) == 2:
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False
    elif len(pos_s1) == 2:
        if not pos_s1[0]['free'] :
            if pos_s1[1]['free']:
                me.command1 = "s "+pos_s1[1]['direction']
                rocket1 = False
        elif not pos_s1[1]['free'] :
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False
        else :
            if me.rockets[0] == me.rockets[1] and pos_s1[0]['direction'] == pos_s1[1]['direction'] :
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False
                
    if previous[0] == me.command1 and lines[3+position].split(':')[3].split('(')[1].split(')')[0].split(',')[1] != "zasah":
        me.command1 = ""
        rocket1 = True
        
    if len(pos_s2) == 1:       
        if me.rockets[0] != me.rockets[1] :
            if pos_s2[0]['free'] and pos_s2[0]['asteroid']:
                me.command2 = "s "+pos_s2[0]['direction']
                rocket2 = False
            elif len(pos_s1) == 2 and pos_s2[0]['asteroid']:
                me.command2 = "s "+pos_s2[0]['direction']
                rocket2 = False
    elif len(pos_s2) == 2:
        if not pos_s2[0]['free']:
            if pos_s2[1]['free'] and pos_s2[1]['asteroid']:
                me.command2 = "s "+pos_s2[1]['direction']
                rocket2 = False
        elif not pos_s2[1]['free'] and pos_s2[0]['asteroid']:
            me.command2 = "s "+pos_s2[0]['direction']
            rocket2 = False
        else :
            if me.rockets[0] == me.rockets[1] and pos_s2[0]['direction'] == pos_s2[1]['direction'] and (pos_s2[0]['asteroid'] or pos_s2[1]['asteroid']) :
                me.command2 = "s "+pos_s2[0]['direction']
                rocket2 = False
        
    if previous[1] == me.command2 and lines[3+position].split(':')[4].split('(')[1].split(')')[0].split(',')[1] != "zasah":
        me.command2 = ""
        rocket2 = True
        
       
    if rocket1:
        if me.rockets[0] != other.rockets[0] and me.rockets[0] != other.rockets[1] : 
            direction = me.get_direction(1)
            if len(direction) > 0 :
                me.command1 = "t "+direction[0]
                if previous[0] == me.command1 and lines[3+position].split(':')[3][0] == "E":
                    me.command1 = "b"
            else :
                me.command1 = "b"
        else :
            me.command1 = "b"
    
    if rocket2:
        if asteroids[0]['value'] == 2 or len(asteroids) == 1 or asteroids[0]['distance2'] <  asteroids[0]['distance1']:
            moves = sorted(me.get_moves(2,[asteroids[0]['x'],asteroids[0]['y']]), key = lambda k: k['distance'])
            if len(moves) > 0:
                for move in moves :
                    if me.check_secure(2,move['move']):
                        me.command2 = "l "+move['move']
                        break
                if me.command2 == "X":
                    me.command2 = "b"
            else :
                me.command2 = "b"
        else :
            if get_distance(me.rockets[1], [asteroids[1]['x'],asteroids[1]['y']]) != 0:
                moves = sorted(me.get_moves(2,[asteroids[1]['x'],asteroids[1]['y']]), key = lambda k: k['distance'])
                for move in moves :
                    if me.check_secure(2,move['move']):
                        me.command2 = "l "+move['move']
                        break
                if me.command2 == "X":
                    me.command2 = "b"
            else :
                me.command2 = "b"
            
            
    
elif state == 'MTB11-2':
    
    if len(pos_s1) == 1:       
        if me.rockets[0] == me.rockets[1]:
            if pos_s1[0]['asteroid']:
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False 
        elif pos_s1[0]['free'] and pos_s1[0]['asteroid']:
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False
        else:
            if len(pos_s2) == 2 and pos_s1[0]['asteroid']:
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False
    elif len(pos_s1) == 2:
        if not pos_s1[0]['free'] :
            if pos_s1[1]['free'] and pos_s1[1]['asteroid']:
                me.command1 = "s "+pos_s1[1]['direction']
                rocket1 = False
        elif not pos_s1[1]['free'] and pos_s1[0]['asteroid']:
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False
        else :
            if me.rockets[0] == me.rockets[1] and pos_s1[0]['direction'] == pos_s1[1]['direction'] and (pos_s1[0]['asteroid'] or pos_s1[1]['asteroid']):
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False
                
    if previous[0] == me.command1 and lines[3+position].split(':')[3].split('(')[1].split(')')[0].split(',')[1] != "zasah":
        me.command1 = ""
        rocket1 = True
        
    if len(pos_s2) == 1:       
        if me.rockets[0] != me.rockets[1] :
            if pos_s2[0]['free'] :
                me.command2 = "s "+pos_s2[0]['direction']
                rocket2 = False
            else:
                if len(pos_s1) == 2:
                    me.command2 = "s "+pos_s2[0]['direction']
                    rocket2 = False
    elif len(pos_s2) == 2:
        if not pos_s2[0]['free'] :
            if pos_s2[1]['free']:
                me.command2 = "s "+pos_s2[1]['direction']
                rocket2 = False
        elif not pos_s2[1]['free'] :
            me.command2 = "s "+pos_s2[0]['direction']
            rocket2 = False
        else :
            if me.rockets[0] == me.rockets[1] and pos_s2[0]['direction'] == pos_s2[1]['direction'] :
                me.command2 = "s "+pos_s2[0]['direction']
                rocket2 = False
        
    if previous[1] == me.command2 and lines[3+position].split(':')[4].split('(')[1].split(')')[0].split(',')[1] != "zasah":
        me.command2 = ""
        rocket2 = True
        
    if rocket2:
        if me.rockets[1] != other.rockets[0] and me.rockets[1] != other.rockets[1] : 
            direction = me.get_direction(2)
            if len(direction) > 0 :
                me.command2 = "t "+direction[0]
                if previous[1] == me.command2 and lines[3+position].split(':')[4][0] == "E":
                    me.command2 = "b"
            else :
                me.command2 = "b"
        else :
            me.command2 = "b"
            
    if rocket1:
        if asteroids[0]['value'] == 2 or len(asteroids) == 1 or asteroids[0]['distance1'] <=  asteroids[0]['distance2']:
            moves = sorted(me.get_moves(1,[asteroids[0]['x'],asteroids[0]['y']]), key = lambda k: k['distance'])
            if len(moves) > 0:
                for move in moves :
                    if me.check_secure(1,move['move']):
                        me.command1 = "l "+move['move']
                        break
                if me.command1 == "X":
                    me.command1 = "b"
            else :
                me.command1 = "b"
        else :
            if get_distance(me.rockets[0], [asteroids[1]['x'],asteroids[1]['y']]) != 0:
                moves = sorted(me.get_moves(1,[asteroids[1]['x'],asteroids[1]['y']]), key = lambda k: k['distance'])
                for move in moves :
                    if me.check_secure(1,move['move']):
                        me.command1 = "l "+move['move']
                        break
                if me.command1 == "X":
                    me.command1 = "b"
            else :
                me.command1 = "b"
    
    
elif state == 'MTA2':
    
    if len(pos_s1) == 1:       
        if me.rockets[0] == me.rockets[1] :
            if pos_s1[0]['asteroid']:
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False 
        elif pos_s1[0]['free'] and pos_s1[0]['asteroid']:
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False
        else:
            if len(pos_s2) == 2 and pos_s1[0]['asteroid']:
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False
    elif len(pos_s1) == 2:
        if not pos_s1[0]['free'] :
            if pos_s1[1]['free'] and pos_s1[1]['asteroid']:
                me.command1 = "s "+pos_s1[1]['direction']
                rocket1 = False
        elif not pos_s1[1]['free'] and pos_s1[0]['asteroid']:
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False
        else :
            if me.rockets[0] == me.rockets[1] and pos_s1[0]['direction'] == pos_s1[1]['direction'] and (pos_s1[0]['asteroid'] or pos_s1[1]['asteroid']):
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False
                
    if previous[0] == me.command1 and lines[3+position].split(':')[3].split('(')[1].split(')')[0].split(',')[1] != "zasah":
        me.command1 = ""
        rocket1 = True
        
    if len(pos_s2) == 1:       
        if me.rockets[0] != me.rockets[1] :
            if pos_s2[0]['free'] and pos_s2[0]['asteroid']:
                me.command2 = "s "+pos_s2[0]['direction']
                rocket2 = False
            else:
                if len(pos_s1) == 2 and pos_s2[0]['asteroid']:
                    me.command2 = "s "+pos_s2[0]['direction']
                    rocket2 = False
    elif len(pos_s2) == 2:
        if not pos_s2[0]['free'] :
            if pos_s2[1]['free'] and pos_s2[1]['asteroid']:
                me.command2 = "s "+pos_s2[1]['direction']
                rocket2 = False
        elif not pos_s2[1]['free'] and pos_s2[0]['asteroid']:
            me.command2 = "s "+pos_s2[0]['direction']
            rocket2 = False
        else :
            if me.rockets[0] == me.rockets[1] and pos_s2[0]['direction'] == pos_s2[1]['direction'] and (pos_s2[0]['asteroid'] or pos_s2[1]['asteroid']):
                me.command2 = "s "+pos_s2[0]['direction']
                rocket2 = False
        
    if previous[1] == me.command2 and lines[3+position].split(':')[4].split('(')[1].split(')')[0].split(',')[1] != "zasah":
        me.command2 = ""
        rocket2 = True
        
    if rocket1:
        if get_distance(me.rockets[0], [asteroids[0]['x'],asteroids[0]['y']]) != 0:
            moves = sorted(me.get_moves(1,[asteroids[0]['x'],asteroids[0]['y']]), key = lambda k: k['distance'])
            for move in moves :
                if me.check_secure(1,move['move']) :
                    me.command1 = "l "+move['move']
                    break
            if me.command1 == "X":
                me.command1 = "b"
        else :
            me.command1 = "b"
    
    if rocket2:
        if get_distance(me.rockets[1], [asteroids[0]['x'],asteroids[0]['y']]) != 0:
            moves = sorted(me.get_moves(2,[asteroids[0]['x'],asteroids[0]['y']]), key = lambda k: k['distance'])
            if len(moves) > 0 :
                for move in moves :
                    if me.check_secure(2,move['move']) :
                        me.command2 = "l "+move['move']
                        break
                if me.command2 == "X":
                    me.command2 = "b"
        else :
            me.command2 = "b"
    
elif state == 'MTA1':
    
    if len(pos_s1) == 1:       
        if me.rockets[0] == me.rockets[1] :
            if pos_s1[0]['asteroid']:
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False 
        elif pos_s1[0]['free'] and pos_s1[0]['asteroid']:
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False
        else:
            if len(pos_s2) == 2 and pos_s1[0]['asteroid']:
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False
    elif len(pos_s1) == 2:
        if not pos_s1[0]['free'] :
            if pos_s1[1]['free'] and pos_s1[1]['asteroid']:
                me.command1 = "s "+pos_s1[1]['direction']
                rocket1 = False
        elif not pos_s1[1]['free'] and pos_s1[0]['asteroid']:
            me.command1 = "s "+pos_s1[0]['direction']
            rocket1 = False
        else :
            if me.rockets[0] == me.rockets[1] and pos_s1[0]['direction'] == pos_s1[1]['direction'] and (pos_s1[0]['asteroid'] or pos_s1[1]['asteroid']):
                me.command1 = "s "+pos_s1[0]['direction']
                rocket1 = False
            
    if previous[0] == me.command1 and lines[3+position].split(':')[3].split('(')[1].split(')')[0].split(',')[1] != "zasah":
        me.command1 = ""
        rocket1 = True
        
    if len(pos_s2) == 1:       
        if me.rockets[0] != me.rockets[1] :
            if pos_s2[0]['free'] and pos_s2[0]['asteroid']:
                me.command2 = "s "+pos_s2[0]['direction']
                rocket2 = False
            else:
                if len(pos_s1) == 2 and pos_s2[0]['asteroid']:
                    me.command2 = "s "+pos_s2[0]['direction']
                    rocket2 = False
    elif len(pos_s2) == 2:
        if not pos_s2[0]['free'] :
            if pos_s2[1]['free'] and pos_s2[1]['asteroid']:
                me.command2 = "s "+pos_s2[1]['direction']
                rocket2 = False
        elif not pos_s2[1]['free'] and pos_s2[0]['asteroid']:
            me.command2 = "s "+pos_s2[0]['direction']
            rocket2 = False
        else :
            if me.rockets[0] == me.rockets[1] and pos_s2[0]['direction'] == pos_s2[1]['direction'] and (pos_s2[0]['asteroid'] or pos_s2[1]['asteroid']):
                me.command2 = "s "+pos_s2[0]['direction']
                rocket2 = False
        
    if previous[1] == me.command2 and lines[3+position].split(':')[4].split('(')[1].split(')')[0].split(',')[1] != "zasah":
        me.command2 = ""
        rocket2 = True
        
    if rocket1:
        if asteroids[0]['distance1'] <= asteroids[0]['distance2'] or len(asteroids) == 1 :
            moves = sorted(me.get_moves(1,[asteroids[0]['x'],asteroids[0]['y']]), key = lambda k: k['distance'])
            for move in moves :
                if me.check_secure(1,move['move']) :
                    me.command1 = "l "+move['move']
                    break
            if me.command1 == "X":
                me.command1 = "b"
        else:
            if get_distance(me.rockets[0], [asteroids[1]['x'],asteroids[1]['y']]) != 0:
                moves = sorted(me.get_moves(1,[asteroids[1]['x'],asteroids[1]['y']]), key = lambda k: k['distance'])
                for move in moves :
                    if me.check_secure(1,move['move']):
                        me.command1 = "l "+move['move']
                        break
                if me.command1 == "X":
                    me.command1 = "b"
            else :
                me.command1 = "b"
    
    if rocket2:
        if asteroids[0]['distance2'] < asteroids[0]['distance1'] or len(asteroids) == 1:
            moves = sorted(me.get_moves(2,[asteroids[0]['x'],asteroids[0]['y']]), key = lambda k: k['distance'])
            for move in moves :
                if me.check_secure(2,move['move']) :
                    me.command2 = "l "+move['move']
                    break
            if me.command2 == "X":
                me.command2 = "b"
        else :
            if get_distance(me.rockets[1], [asteroids[1]['x'],asteroids[1]['y']]) != 0:
                moves = sorted(me.get_moves(2,[asteroids[1]['x'],asteroids[1]['y']]), key = lambda k: k['distance'])
                for move in moves :
                    if me.check_secure(2,move['move']):
                        me.command2 = "l "+move['move']
                        break
                if me.command2 == "X":
                    me.command2 = "b"
            else :
                me.command2 = "b"
        
print(me.command1+":"+me.command2)