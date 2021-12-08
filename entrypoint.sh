#!/bin/sh

VALGRIND_ARGS="--error-exitcode=1 --leak-check=full --show-leak-kinds=all --tool=memcheck --track-origins=yes"

valgrind $VALGRIND_ARGS build/the-core
printf "\n\n\n"
valgrind $VALGRIND_ARGS build/a.out 1>/dev/null
