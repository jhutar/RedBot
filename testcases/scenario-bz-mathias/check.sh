#!/bin/sh

grep 'Hrac 1: Body:0 Zakladna:\[0,0\] Rakety:\[0,8\]:\[1,0\]' game/playfield.txt.0002 || exit 1
grep 'Hrac 2: Body:2 Zakladna:\[9,9\] Rakety:\[9,8\]:\[9,9\]' game/playfield.txt.0002 || exit 2
grep 'Odpoved hrace 1:(s p:l p):S P (8,zasah):L P' game/playfield.txt.0002 || exit 3
grep 'Odpoved hrace 2:(t d:t d):T D (1):T D' game/playfield.txt.0002 || exit 4

grep 'Hrac 1: Body:0 Zakladna:\[0,0\] Rakety:\[0,8\]:\[1,0\]' game/playfield.txt.0005 || exit 10
grep 'Hrac 2: Body:3 Zakladna:\[9,9\] Rakety:\[9,9\]:\[8,9\]' game/playfield.txt.0005 || exit 11
grep 'Odpoved hrace 1:(b:b):B:B' game/playfield.txt.0005 || exit 12
grep 'Odpoved hrace 2:(b:b):B:B' game/playfield.txt.0005 || exit 13
grep '^          $' game/playfield.txt.0005 || exit 14
