#!/bin/bash

TERMINAL=$1
TYPE=$2
ROUNDS=$3
PLAY_FILENAME=$4
PLAYER1=$5
PLAYER2=$6
PLAYER3=$7
PLAYER4=$8

OFFSET=${TYPE#*-}

gnuplot -p << EOF
$TERMINAL
set key left top
set xlabel "Kolo"
set ylabel "${TYPE%-*}"
set xrange [ 0:$ROUNDS ]
set style line 1 lt 1 lw 3 pt 3 lc rgb "red"
set style line 2 lt 3 lw 3 pt 3 lc rgb "green"
set style line 3 lt 1 lw 3 pt 3 lc rgb "blue"
set style line 4 lt 3 lw 3 pt 3 lc rgb "yellow"
plot "$PLAY_FILENAME" using 1:${OFFSET}title '$PLAYER1' with lines ls 1, \
"$PLAY_FILENAME" using 1:$((7+$((OFFSET)))) title '$PLAYER2' with lines ls 2, \
"$PLAY_FILENAME" using 1:$((14+$((OFFSET)))) title '$PLAYER3' with lines ls 3, \
"$PLAY_FILENAME" using 1:$((21+$((OFFSET)))) title '$PLAYER4' with lines ls 4
EOF
