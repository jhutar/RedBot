#!/bin/sh

grep 'Odpoved hrace 1:(t l:b):E:B' game/playfield.txt.0006 || exit 10
grep 'Odpoved hrace 2:(t d:t d):T D (1):T D (1)' game/playfield.txt.0006 || exit 11
grep 'Hrac 1: Body:0 Zakladna:\[0,0\] Rakety:\[9,0\]:\[0,0\]' game/playfield.txt.0006 || exit 12
grep 'Hrac 2: Body:0 Zakladna:\[9,9\] Rakety:\[9,1\]:\[9,1\]' game/playfield.txt.0006 || exit 13
grep '^          $' game/playfield.txt.0006 || exit 14

grep '^Hrac 1: Body:0 Zakladna:\[0,0\] Rakety:\[9,0\]:\[0,0\]$' game/playfield.txt.0010 || exit 1
grep '^Hrac 2: Body:0 Zakladna:\[9,9\] Rakety:\[9,1\]:\[9,1\]$' game/playfield.txt.0010 || exit 2
grep '^          $' game/playfield.txt.0010 || exit 3
