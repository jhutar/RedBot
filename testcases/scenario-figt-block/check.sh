#!/bin/sh

grep '^Odpoved hrace 1:(b:s d):B:S D (7,obrana)$' game/playfield.txt.0004 || exit 1
grep '^Odpoved hrace 2:(b:b):B:B$' game/playfield.txt.0004 || exit 2
grep --quiet -v '^          $' game/playfield.txt.0004 || exit 3

grep '^Odpoved hrace 1:(b:s d):B:S D (7,zasah)$' game/playfield.txt.0006 || exit 10
grep '^Odpoved hrace 2:(b:):B:E$' game/playfield.txt.0006 || exit 11
grep --quiet -v '^          $' game/playfield.txt.0006 || exit 12

grep '^Hrac 1: Body:0 Zakladna:\[0,0\] Rakety:\[5,0\]:\[5,1\]$' game/playfield.txt.0010 || exit 20
grep '^Hrac 2: Body:0 Zakladna:\[9,9\] Rakety:\[5,9\]:\[9,9\]$' game/playfield.txt.0010 || exit 21
grep --quiet -v '^          $' game/playfield.txt.0010 || exit 22
