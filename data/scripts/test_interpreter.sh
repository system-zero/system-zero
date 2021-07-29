#!/bin/sh

# shell script to test the interpreter
# it should run from the top directory of this distribution
#
# you may want to set the executable bit first as:
# chmod 0755 data/scripts/test_interpreter.sh
# or run it as:
# sh data/scripts/test_interpreter.sh
#
# to redirect syntax errors messages to /dev/null use:
# SILENT=0 data/scripts/test_interpreter.sh
#
# to run the tests with the static interpreter use:
# data/scripts/test_interpreter.sh 1
#
# to run the semantic tests under valgrind use:
# MEMCHECK_IT=1 data/scripts/test_interpreter.sh
# the ouput will be saved under /tmp/valgrind_output
#
# to run the semantic tests under gdb use:
# DEBUG_IT=1 data/scripts/test_interpreter.sh
# the ouput will be saved under /tmp/gdb_output

PRINTF=$(which printf)
NUM_FAILED=0
NUM_SUCCEEDED=0
FAILED=""
NUM=0
SILENT=${SILENT-1}
MEMCHECK_IT=${MEMCHECK_IT-0}
DEBUG_IT=${DEBUG_IT-0}
BINPATH=sys/$(uname -m)/bin
VALGRIND=$(which valgrind)
VALGRIND_ARGS="--show-leak-kinds=all --leak-check=full -v --track-origins=yes"
VALGRIND_OUTPUT="/tmp/valgrind_output"
GDB=$(which gdb)

PROG_ARGS="--run-invalid-memory-read-tests --run-fileptr-tests --run-valgrind-tests"

if [ $# = 0 ]; then
  PROG=$BINPATH/La-shared
else
  PROG=$BINPATH/La-static
fi

$PRINTF "testing $PROG interpreter\n\n"

test_semantics() {
  if [ $MEMCHECK_IT = 1 ]; then
    $PRINTF "Running under valgrind (log will be saved to $VALGRIND_OUTPUT).\n"
    $VALGRIND $VALGRIND_ARGS $PROG data/tests/la-semantics.lai \
        $PROG_ARGS 2>$VALGRIND_OUTPUT
  elif [ $DEBUG_IT = 1 ]; then
    $GDB \
      -ex "set logging file /tmp/gdb_output" \
      -ex "set logging overwrite on" \
      -ex "set logging on" -ex "run" --args \
      $PROG data/tests/la-semantics.lai $PROG_ARGS
  else
    $PROG data/tests/la-semantics.lai $PROG_ARGS
  fi
}

test_failed() {
  NUM=$(($NUM+1))
  unit=$1
  msg=$2

  $PRINTF "[$NUM] $msg - "

  if [ $SILENT = 1 ]; then
    $PROG $unit 2>/dev/null
  else
    $PROG $unit
  fi

  if [ $? = 0 ]; then
    $PRINTF "[NOTOK]\n"
    NUM_FAILED=$(($NUM_FAILED+1))
    FAILED="$FAILED [$NUM]"
  else
    $PRINTF "[OK]\n"
    NUM_SUCCEEDED=$(($NUM_SUCCEEDED+1))
  fi
}

test_semantics

$PRINTF "\r\b\r               \n"
$PRINTF "----- [ Tests that should fail ] -----\n"

test_failed data/tests/failed_map_private_field.lai "testing map field with private scope"
test_failed data/tests/failed_map_override.lai "testing overriding a method, without to set it explicitly"
$PRINTF "\n"
$PRINTF "====- FAILED TESTS SUMMARY -====\n"
$PRINTF "NUM TESTS: $NUM\n"
$PRINTF "   PASSED: $NUM_SUCCEEDED\n"
$PRINTF "   FAILED: $NUM_FAILED"
if [ $NUM_FAILED != 0 ]; then
  $PRINTF ", $FAILED"
fi

$PRINTF "\n"
