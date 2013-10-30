#!/bin/sh

kolo=$( head -n 1 playfield.txt | cut -d ' ' -f 1 )

head -n $( expr $kolo + 1 ) <<LIST | tail -n 1
b:s n
s p:l p
t n:s n
b:b
b:b
b:b
LIST
