#!/bin/sh

kolo=$( head -n 1 playfield.txt | cut -d ' ' -f 1 )

head -n $( expr $kolo + 1 ) <<LIST | tail -n 1
b:b
l p p:b
l p p:b
s d:b
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
b:b
LIST
