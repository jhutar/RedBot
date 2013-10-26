#!/bin/sh

grep '^Hrac 1: Body:2 Zakladna:\[0,0\] Rakety:\[0,0\]:\[0,0\]$' game/playfield.txt.0020 || exit 1
grep '^Hrac 2: Body:2 Zakladna:\[9,9\] Rakety:\[9,9\]:\[9,9\]$' game/playfield.txt.0020 || exit 2
grep '^Odpoved hrace 1:(t l:t n):T L (1):T N (1)$' game/playfield.txt.0020 || exit 3
grep '^Odpoved hrace 2:(b:b):B:B$' game/playfield.txt.0020 || exit 4
grep '^          $' game/playfield.txt.0020 || exit 5
grep '^20 20$' game/playfield.txt.0020 || exit 6
grep '^10 10$' game/playfield.txt.0020 || exit 7
