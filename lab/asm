#!/usr/bin/env bash

set -e

entries=("0_empty" "1_hello_world")
entry="${entries[$1 + 0]}"

if [ -z "$entry" ]; then
  echo "Entry is not specified"
  exit 1
fi

as -o "$entry.o" "$entry.s"

ld \
  -arch x86_64 \
  -dead_strip \
  -macosx_version_min 10.10.0 \
  -no_function_starts \
  -no_pie \
  -S \
  -L/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib \
  -lSystem \
  -o "$entry.out" \
  -x \
  "$entry.o"

rm "$entry.o"
"./$entry.out"
echo "Return Code: $?"
