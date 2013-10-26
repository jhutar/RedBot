#!/bin/sh

kolo=$( head -n 1 playfield.txt | cut -d ' ' -f 1 )

head -n $( expr $kolo + 1 ) <<LIST | tail -n 1
l p p:l p p
l p p:l p p
l p:l p d
b:s d
b:b
b:s d
b:b
b:b
b:b
b:b
b:b
b:b
b:b
b:b
LIST

#if [ $kolo -lt 4 ]; then
#  echo l r r:l d d
#else
#  echo b:b
#fi
