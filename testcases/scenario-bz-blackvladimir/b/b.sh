#!/bin/sh

kolo=$( head -n 1 playfield.txt | cut -d ' ' -f 1 )

#echo "KOLO $kolo" 1>&2

grep -v '^#' <<LIST | head -n $( expr $kolo + 1 ) | tail -n 1
l l l:l n n
l l l:l n n
l l l:l n n
l l l:l n n
l l:l n
t p:t d
t p:t d
t p:t d
t p:t d
t p:t d
t p:t d
t p:t d
t p:t d
t p:t d
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
b:b
b:b
b:b
b:b
b:b
b:b
b:b
LIST
