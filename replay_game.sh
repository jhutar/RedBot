#!/bin/bash
if test $# -lt 2; then
	echo "Too few arguments, expecting playing_field and delay."
	exit
fi
PLAYING_FIELD=$1
DELAY=$2
for FIELD in $(ls ${PLAYING_FIELD}*); do
	clear
	cat ${FIELD} | sed -e ''/[abcd]/s//`printf "\033[32mO\033[0m"`/g'' \
	  -e ''/[hijk]/s//`printf "\033[31mO\033[0m"`/g'' -e ''/[opqr]/s//`printf "\033[34mO\033[0m"`/g'' \
          -e ''/[wxyz]/s//`printf "\033[36mO\033[0m"`/g''
	sleep ${DELAY}
done
