#!/usr/bin/env bash

#!
# Copyright (c) 2018 Aaron Delasy
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
