#!/bin/sh

grep 'Odpoved hrace 1:(s d:b):S D (9,zasah):B' game/playfield.txt.0004 || exit 10
grep 'Odpoved hrace 2:(s n:b):S N (9,zasah):B' game/playfield.txt.0004 || exit 11

grep '^Hrac 1: Body:0 Zakladna:\[0,0\] Rakety:\[0,0\]:\[0,0\]$' game/playfield.txt.0005 || exit 2
grep '^Hrac 2: Body:0 Zakladna:\[9,9\] Rakety:\[9,9\]:\[9,9\]$' game/playfield.txt.0005 || exit 1
grep --quiet -v '^          $' game/playfield.txt.0005 || exit 3
