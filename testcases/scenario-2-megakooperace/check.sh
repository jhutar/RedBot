#!/bin/sh

grep '^Hrac 1: Body:0 Zakladna:\[0,0\] Rakety:\[5,5\]:\[5,5\]$' game/playfield.txt.0010 || exit 2
grep '^Hrac 2: Body:0 Zakladna:\[9,9\] Rakety:\[5,5\]:\[5,5\]$' game/playfield.txt.0010 || exit 1
grep '^          $' game/playfield.txt.0010 || exit 3
