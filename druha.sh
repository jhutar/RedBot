#!/bin/sh

# Load info from map
myid=$1
stones=$( grep '^Kamenu: ' playfield.txt | sed "s/^Kamenu: \([0-9]\+\)$/\1/" )
width=$( expr $( tail -n 1 playfield.txt | grep -o ',' | wc -l ) + 1 )
height=$( sed '1,/^Mapa:/d' playfield.txt | wc -l )

###echo "DEBUG: I have: myid=$myid, stones=$stones, width=$width, height=$height"

function check() {
  # Check if suggested move is possible
  ###echo "DEBUG: Checking '$action' and '$stones'"
  if [ x"$action" = x'#' -a x"$stones" = x'0' ] || [ -z "$action" ]; then
    return 1
  else
    return 0
  fi
}

# Get valid random coords and valid random action
while ! check; do
  x=$( expr $RANDOM % $width )
  y=$( expr $RANDOM % $height )

  if [ $( expr $x % 2 ) -eq 0 -a $( expr $y % 2 ) -eq 0 ]; then
    action="#"
  else
    case $( expr $RANDOM % 5 ) in
      0) action="|";;
      1) action="/";;
      2) action="-";;
      3) action="\\";;
      4) action="#";;
    esac
  fi
done

# Print
echo "$action $x $y"
