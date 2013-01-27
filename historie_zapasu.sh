#!/bin/bash

if [ $# -eq 0 ]; then
	echo "Pouziti: $0 <soubor_s_historii_hry> [<nazev_hrace1> <nazev_hrace2> <nazev_hrace3> <nazev_hrace4> [<terminal>]]"
	echo "                kde terminal je typ vystupniho souboru (jpeg, png apod., viz. gnuplot & set terminal) - vytvori prislusne soubory"
	exit
fi

PLAY_FILENAME=${1}.$RANDOM
cat $1 | sed "s/\// /g" | sed "s/://g" > $PLAY_FILENAME

if [ $# -ge 5 ]; then
	PLAYER1=$2
	PLAYER2=$3
	PLAYER3=$4
	PLAYER4=$5
else
	PLAYER1="1.kralovstvi"
	PLAYER2="2.kralovstvi"
	PLAYER3="3.kralovstvi"
	PLAYER4="4.kralovstvi"
fi

if [ $# -ge 6 ]; then
	TERMINAL="set terminal $6"
fi

ROUNDS=$(tail -n 1 $PLAY_FILENAME | cut -d' ' -f1)

for TYPE in "Území-3" "Jídlo-4" "Vojáci-5" "Rolníci-6" "Zbraně-7" "Farmaření-8" "Tajné_služby-9"; do
OFFSET=${TYPE#*-}
if [ $# -ge 6 ]; then
	filename="${TYPE%-*}.${6}"
	echo "writing to file $filename"
	./do_gnuplot.sh "$TERMINAL" $TYPE $ROUNDS $PLAY_FILENAME $PLAYER1 $PLAYER2 $PLAYER3 $PLAYER4 > ${TYPE%-*}.${6}
else
	./do_gnuplot.sh " " $TYPE $ROUNDS $PLAY_FILENAME $PLAYER1 $PLAYER2 $PLAYER3 $PLAYER4
fi

#gnuplot -p << EOF
#$TERMINAL
#set key left top
#set xlabel "Kolo"
#set ylabel "${TYPE%-*}"
#set xrange [ 0:$ROUNDS ]
#set style line 1 lt 1 lw 3 pt 3 lc rgb "red"
#set style line 2 lt 3 lw 3 pt 3 lc rgb "green"
#set style line 3 lt 1 lw 3 pt 3 lc rgb "blue"
#set style line 4 lt 3 lw 3 pt 3 lc rgb "yellow"
#plot "$PLAY_FILENAME" using 1:${OFFSET}title '$PLAYER1' with lines ls 1, \
#"$PLAY_FILENAME" using 1:$((7+$((OFFSET)))) title '$PLAYER2' with lines ls 2, \
#"$PLAY_FILENAME" using 1:$((14+$((OFFSET)))) title '$PLAYER3' with lines ls 3, \
#"$PLAY_FILENAME" using 1:$((21+$((OFFSET)))) title '$PLAYER4' with lines ls 4
#EOF

done

rm $PLAY_FILENAME
