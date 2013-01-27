#!/bin/sh

for data in $@; do
  data=$( basename $data )
  [ -d $data ] || exit 1
  echo "===== $data ====="
  ./visualiser2.py $data || exit 2
  find $data -type f -name visualiser-\*.png | sort -n > $data/visualiser.list
  out=$( echo "$data" | sed 's/default_playfield\.txt-/RedBot2012podzim-/' )
  mencoder mf://@$data/visualiser.list -mf w=900:h=600:fps=10:type=png -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o $out.avi >$data/mencoder.log || exit 3
  echo "VIDEO: $out.avi"
done
