#!/bin/bash

#
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

set -e

dirname="$(realpath "$(dirname -- "$0")")"
root_dir="$(realpath "$dirname/..")"
api_path="$root_dir/src/api"

rm -rf "$root_dir/build/doc"
mkdir -p "$root_dir/build/doc"

for entity in "$api_path/"*; do
  name="$(basename "$entity")"
  result="$("$root_dir/build/the" doc "$entity")"

  if [ "$name" == "arr" ]; then
    result="${result:52}"
  elif [ "$name" == "map" ]; then
    result="${result:80}"
  fi

  titleRaw="$(head -n 1 "$entity")"
  title="${titleRaw:3}"

  hero=$(cat << EOF
---
layout: page
title: $title
---

# {{ page.title }}
EOF
)

  printf '%s\n\n%s\n' "$hero" "$result" > "$root_dir/build/doc/$name.md"
done
