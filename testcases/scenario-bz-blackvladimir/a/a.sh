#!/bin/sh

kolo=$( head -n 1 playfield.txt | cut -d ' ' -f 1 )

head -n $( expr $kolo + 1 ) <<LIST | tail -n 1
l d d:l p p
l p p:l d d
l p p:l d d
l p p:l d d
l p:l d
b:b
b:b
t p:t d
t p:t d
t n:t l
t n:t l
t l:t n
t l:t n
t l:t n
t l:t n
t l:t n
t l:t n
t l:t n
t l:t n
t l:t n
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
#  echo l r r:l d d
#else
#  echo b:b
#fi
