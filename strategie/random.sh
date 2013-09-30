#!/bin/sh

function get_action() {
  case $( expr $RANDOM % 6 ) in
    0) echo -n 'b';;
    1) echo -n 't';;
    2) echo -n 's';;
    [3-5]) echo -n 'l';;
  esac
}

function get_direction() {
  case $( expr $RANDOM % 4 ) in
    0) echo -n 'n';;
    1) echo -n 'p';;
    2) echo -n 'd';;
    3) echo -n 'l';;
  esac
}

function get_order() {
  action=$( get_action )
  options=''
  if [ "$action" != 'b' ]; then
    options=" $( get_direction )"
    if [ "$action" = 'l' -a $( expr $RANDOM % 2 ) -eq 0 ]; then
      options="$options $( get_direction )"
    fi
  fi
  echo -n "$action$options"
}

echo "$( get_order ):$( get_order )"
