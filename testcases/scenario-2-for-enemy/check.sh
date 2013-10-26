#!/bin/sh

grep '^Hrac 1: Body:2 Zakladna:\[0,0\] Rakety:\[0,0\]:\[0,0\]$' game/playfield.txt.0020 || exit 1
grep '^Hrac 2: Body:0 Zakladna:\[9,9\] Rakety:\[0,0\]:\[0,0\]$' game/playfield.txt.0020 || exit 2
grep '^          $' game/playfield.txt.0020 || exit 3
