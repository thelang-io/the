#!/usr/bin/env python3

import os
import subprocess


def update(directory: str, action: str):
    files = os.listdir(directory)
    stdin_sep = "======= stdin ======="
    stdout_sep = "======= stdout ======="
    stderr_sep = "======= stderr ======="

    for file in files:
        filepath = directory + "/" + file
        is_error_file = file.startswith("throw-")

        f = open(filepath, "r")
        content = f.read()[len(stdin_sep) + 1:]
        parts = content.split(stderr_sep) if is_error_file else content.split(stdout_sep)
        f.close()

        f = open(filepath, "w")
        f.write(parts[0][0:-1])
        f.close()

        process = subprocess.Popen(
            ["build/the-core", action, filepath],
            stderr=subprocess.PIPE,
            stdout=subprocess.PIPE,
            text=True
        )

        stdout, stderr = process.communicate()

        f = open(filepath, "w")

        if is_error_file:
            result = "/test" + stderr[stderr.find(":", 10):]
            f.write(stdin_sep + "\n" + parts[0] + stderr_sep + "\n" + result)
        else:
            f.write(stdin_sep + "\n" + parts[0] + stdout_sep + "\n" + stdout)

        f.close()


def main():
    update("test/parser-test", "parse")
    update("test/ast-test", "ast")


if __name__ == "__main__":
    main()
