#!/bin/sh

grep '^Hrac 1: Body:0 Zakladna:\[0,0\] Rakety:\[5,0\]:\[0,5\]$' game/playfield.txt.0004 || exit 1
grep '^Hrac 2: Body:0 Zakladna:\[9,9\] Rakety:\[9,9\]:\[9,9\]$' game/playfield.txt.0004 || exit 2
grep '^Odpoved hrace 1:(s d:s p):S D (5,zasah):S P (5,zasah)$' game/playfield.txt.0004 || exit 3
grep '^Odpoved hrace 2:(l l n:):L L N:E$' game/playfield.txt.0004 || exit 4
grep --quiet -v '^          $' game/playfield.txt.0004 || exit 5
