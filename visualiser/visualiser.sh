#!/bin/sh

function fix_name() {
  sed -e "s/Ústafák/Ustafak/"
}

function fix_plan_name() {
  sed -e "s/city-plan_field/city_plan_field/"
}

echo "=== $1 ==="
[ -d "$1" ] || exit 1
if [ -f "$1.avi" ]; then
  echo "Already exists $1.avi"
  exit 2
fi
rm -rf $1/*.png   # cleanup previous renders if any
bot1=$( echo $1 | fix_plan_name | cut -d '-' -f '2' | fix_name )
bot2=$( echo $1 | fix_plan_name | cut -d '-' -f '3' | fix_name )
bot3=$( echo $1 | fix_plan_name | cut -d '-' -f '4' | fix_name )
bot4=$( echo $1 | fix_plan_name | cut -d '-' -f '5' | fix_name )

# Generate PNGs
./visualiser.py "$bot1" "$bot2" "$bot3" "$bot4" `find $1 -maxdepth 1 -type f | grep -v "/_std" | sort -V` || exit 3

# Generate list of prepared PNGs
find $1 -type f -name \*.png | sort -V > $1.txt || exit 4

# Make first and last image appear 3 times in the list
t=$( mktemp )
f=$( head -n 1 $1.txt )
l=$( tail -n 1 $1.txt )
echo $f >> $t
echo $f >> $t
cat $1.txt >> $t
echo $l >> $t
echo $l >> $t
mv $t $1.txt

# Generate AVI from the list of PNGs
mencoder mf://@$1.txt -mf w=1200:h=900:fps=10:type=png -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o $1.avi || exit 5
rm $1.txt
###mplayer $1.avi

# Print info
echo "Play: $1"
echo "Result: $( tail -n 1 $1/_stdout.txt )"
