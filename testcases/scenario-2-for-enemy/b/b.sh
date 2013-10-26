#!/bin/sh

kolo=$( head -n 1 playfield.txt | cut -d ' ' -f 1 )

#echo "KOLO $kolo" 1>&2

grep -v '^#' <<LIST | head -n $( expr $kolo + 1 ) | tail -n 1
l l l:l n n
l l l:l n n
l n n:l l l
l n n:l l l
l n:l n
t n:t n
t n:t n
t n:t n
t n:t n
t l:t l
t l:t l
t l:t l
t l:t l
t l:t l
b:b
b:b
b:b
b:b
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
#  echo l l l:l n n
#else
#  echo b:b
#fi