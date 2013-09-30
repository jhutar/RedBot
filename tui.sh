#!/bin/sh

for fp in $( ls game/* | sort ); do
  clear
  echo "===== $fp ====="
  echo
  cat $fp
  echo
  echo -n "Press any key to continue (or Ctrl+C to exit)"
  read
done
