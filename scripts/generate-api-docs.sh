#!/bin/bash
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
    result="${result:80}"
  elif [ "$name" == "map" ]; then
    result="${result:136}"
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
