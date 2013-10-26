#!/bin/sh

# Run all test-cases we have for RedBot and report results
# Options:
#   --clean   Do not run test-cases, just remove all temporary files
#   [scenario]   Run only specified scenario

MYPWD=$( pwd )
SCORE=0
if [ -d "$1" ]; then
  SCENARIOS=$1
else
  SCENARIOS=$( ls | grep '^scenario' )
fi

for scenario in $SCENARIOS; do
  rc=0
  echo "=== START: $scenario ==="
  cd $scenario
  let rc+=$?
  mkdir -p game
  let rc+=$?
  rm -rf game/* a/playfield.txt b/playfield.txt
  let rc+=$?
  if [ "$1" != '--clean' ]; then
    echo "--- running game ---"
    ../../redbot a/a.sh b/b.sh
    let rc+=$?
    echo "--- running check ---"
    ./check.sh
    let rc+=$?
  fi
  let SCORE+=$rc
  echo "=== RESULT: $scenario: $rc ==="
  cd $MYPWD
done
echo "### SCORE: $SCORE ###"
exit $SCORE
