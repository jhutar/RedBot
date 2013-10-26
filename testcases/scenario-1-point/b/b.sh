#!/bin/sh

kolo=$( head -n 1 playfield.txt | cut -d ' ' -f 1 )

#echo "KOLO $kolo" 1>&2

grep -v '^#' <<LIST | head -n $( expr $kolo + 1 ) | tail -n 1
l l l:b
l l l:b
l l l:b
l l l:b
l l:b
t p:b
t p:b
t p:b
t p:b
t p:b
t p:b
t p:b
t p:b
t p:b
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
