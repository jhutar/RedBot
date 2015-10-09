#!/bin/sh

round=$( grep '^Kolo:' playfield.txt | cut -d ' ' -f 2 )

cat <<EOF | head -n $( expr $round + 1 ) | tail -n 1
| 8 3
| 8 5
- 7 2
- 5 2
- 3 2
- 1 2

EOF
