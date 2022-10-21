#!/usr/bin/env python3

import sys


def main():
    if len(sys.argv) < 3:
        print("Usage: find-duplicates.py [file] [search-file]", file=sys.stderr)
        exit(1)

    with open(sys.argv[1], "r") as f:
        c = f.read()
        lines = c.splitlines()

    with open(sys.argv[2], "r") as f:
        search_text = f.read()

    non_duplicates = []

    for line in lines:
        if line not in search_text:
            non_duplicates.append(line)

    if len(non_duplicates) > 0:
        print("\n".join(non_duplicates))


if __name__ == "__main__":
    main()
