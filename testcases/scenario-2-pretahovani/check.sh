#!/bin/sh

grep 'Odpoved hrace 1:(t n:t n):E:E' game/playfield.txt.0006 || exit 10
grep 'Odpoved hrace 2:(t d:t d):E:E' game/playfield.txt.0006 || exit 11

grep '^Hrac 1: Body:0 Zakladna:\[0,0\] Rakety:\[5,4\]:\[5,4\]$' game/playfield.txt.0010 || exit 2
grep '^Hrac 2: Body:0 Zakladna:\[9,9\] Rakety:\[5,4\]:\[5,4\]$' game/playfield.txt.0010 || exit 1
grep --quiet -v '^          $' game/playfield.txt.0010 || exit 3
