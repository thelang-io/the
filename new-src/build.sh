#!/usr/bin/env bash

#!
# Copyright (c) Aaron Delasy
#
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#

dir_name="$(dirname -- "$0")/"
build_dir=$(readlink -f "$(dirname -- "$0")/build")
files=()

echo "Building into: $build_dir"

read_file () {
  echo "$(<"$dir_name$1")"
}

for file in "$dir_name"*; do
  file_name="${file:${#dir_name}}"
  if [ ! -f "$file" ] || [ "$file_name" == "main" ] || [[ "$file_name" == *"."* ]]; then continue; fi

  echo "Found file: $file_name"
  files+=("$file_name")
done

main_content="$(read_file "main")"
build_content="${main_content:0:148}"

for file in "${files[@]}"; do
  file_content="$(read_file "$file")"
  build_content+="${file_content:148}"
done

build_content+="${main_content:148}"

mkdir -p "$build_dir"
echo "$build_content" > "$build_dir/result"
echo "Built file: $build_dir/result"
the "$build_dir/result" "${@:1}"
