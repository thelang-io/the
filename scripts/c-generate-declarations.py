#!/usr/bin/env python3

import enum
import sys


@enum.unique
class ASTType(enum.IntEnum):
    UNKNOWN = 0
    COMMENT_BLOCK = 1
    COMMENT_LINE = 2
    # ENUM = 3
    # FN_DECL = 4
    # FN_DEF = 5
    MACRO_DEFINE = 6
    MACRO_IF = 7
    MACRO_IFDEF = 8
    MACRO_IFNDEF = 9
    MACRO_INCLUDE = 10
    MACRO_ELIF = 11
    MACRO_ELSE = 12
    MACRO_ENDIF = 13
    MACRO_ERROR = 14
    MACRO_PRAGMA = 15
    MACRO_UNDEF = 16
    STRUCT = 17
    TYPEDEF = 18
    WHITESPACE = 19

    def is_comment(self):
        return ASTType.COMMENT_BLOCK == self or ASTType.COMMENT_LINE == self

    def is_macro(self):
        return ASTType.MACRO_DEFINE <= self <= ASTType.MACRO_UNDEF


class Stmt:
    type = ASTType.UNKNOWN
    start = 0
    end = 0
    val = ""

    def __init__(self, t, c, s, e):
        self.type = t
        self.start = s
        self.end = e
        self.val = c[s:e]


def parse_word(s, i):
    while i + 1 < len(s) and (s[i].isalnum() or s[i] == "_"):
        i += 1

    return i


def parse_whitespace(s, i):
    while i + 1 < len(s) and s[i].isspace():
        i += 1

    return i


def parse_stmt_end(s):
    i = 0

    while True:
        if s[i].isalnum() or s[i] == "_":
            j = parse_word(s, i)
            yield s[i:j]
            i = parse_whitespace(s, j)

            if s[i] == ";":
                break

        i += 1


def parse_stmt(s):
    i = 0

    while i < len(s):
        if s[i] == "#":
            stmt_start = i
            i = parse_whitespace(s, i + 1)

            if s[i:i + 6] == "define":
                stmt_type = ASTType.MACRO_DEFINE
            elif s[i:i + 2] == "if":
                stmt_type = ASTType.MACRO_IF
            elif s[i:i + 5] == "ifdef":
                stmt_type = ASTType.MACRO_IFDEF
            elif s[i:i + 6] == "ifndef":
                stmt_type = ASTType.MACRO_IFNDEF
            elif s[i:i + 7] == "include":
                stmt_type = ASTType.MACRO_INCLUDE
            elif s[i:i + 4] == "elif":
                stmt_type = ASTType.MACRO_ELIF
            elif s[i:i + 4] == "else":
                stmt_type = ASTType.MACRO_ELSE
            elif s[i:i + 5] == "endif":
                stmt_type = ASTType.MACRO_ENDIF
            elif s[i:i + 5] == "error":
                stmt_type = ASTType.MACRO_ERROR
            elif s[i:i + 6] == "pragma":
                stmt_type = ASTType.MACRO_PRAGMA
            elif s[i:i + 5] == "undef":
                stmt_type = ASTType.MACRO_UNDEF
            else:
                stmt_type = ASTType.UNKNOWN

            while True:
                while s[i + 1] != "\n":
                    i += 1

                if s[i] != "\\":
                    break

                i += 1

            yield Stmt(stmt_type, s, stmt_start, i + 1)
        elif i + 2 < len(s) and s[i:i + 2] == "/*":
            stmt_start = i
            i += 2

            while s[i:i + 2] != "*/":
                i += 1

            i += 1
            yield Stmt(ASTType.COMMENT_BLOCK, s, stmt_start, i + 1)
        elif i + 2 < len(s) and s[i:i + 2] == "//":
            stmt_start = i
            i += 1

            while s[i + 1] != "\n":
                i += 1

            yield Stmt(ASTType.COMMENT_LINE, s, stmt_start, i + 1)
        elif i + 7 < len(s) and s[i:i + 7] == "struct ":
            stmt_start = i

            i = parse_whitespace(s, i + 7)
            i = parse_word(s, i)
            i = parse_whitespace(s, i)

            if s[i] == ";":
                stmt_type = ASTType.STRUCT
            elif s[i] == "{":
                stmt_type = ASTType.STRUCT
                blocks = 0

                while True:
                    if s[i] == ";" and blocks == 0:
                        break
                    elif s[i] == "{":
                        blocks += 1
                    elif s[i] == "}":
                        blocks -= 1

                    i += 1
            else:
                stmt_type = ASTType.UNKNOWN
                i = parse_word(s, i)

            yield Stmt(stmt_type, s, stmt_start, i + 1)
        elif i + 8 < len(s) and s[i:i + 8] == "typedef ":
            stmt_start = i
            blocks = 0
            i += 8

            while True:
                if s[i] == ";" and blocks == 0:
                    break
                elif s[i] == "{":
                    blocks += 1
                elif s[i] == "}":
                    blocks -= 1

                i += 1

            yield Stmt(ASTType.TYPEDEF, s, stmt_start, i + 1)
        elif s[i].isspace():
            stmt_start = i

            while i + 1 < len(s) and s[i + 1].isspace():
                i += 1

            yield Stmt(ASTType.WHITESPACE, s, stmt_start, i + 1)
        else:
            stmt_start = i

            while i + 1 < len(s) and (s[i + 1].isalnum() or s[i + 1] == "_"):
                i += 1

            yield Stmt(ASTType.UNKNOWN, s, stmt_start, i + 1)

        i += 1


def main():
    if len(sys.argv) < 3:
        print("Usage: c-generate-declarations.py [path] [output-path]", file=sys.stderr)
        exit(1)

    with open(sys.argv[1]) as f:
        s = f.read()

    unknown_content = ""
    result = []

    for stmt in parse_stmt(s):
        if stmt.type == ASTType.UNKNOWN or stmt.type == ASTType.WHITESPACE:
            unknown_content += stmt.val
        elif stmt.type == ASTType.MACRO_DEFINE:
            i = parse_whitespace(stmt.val, 1) + 7
            j = i

            while j < len(stmt.val) and (stmt.val[j].isalnum() or stmt.val[j] == "_"):
                j += 1

            result.append(stmt.val[i:j])
        elif stmt.type == ASTType.STRUCT:
            i = parse_whitespace(stmt.val, 6)
            j = i

            while j < len(stmt.val) and (stmt.val[j].isalnum() or stmt.val[j] == "_"):
                j += 1

            result.append(stmt.val[i:j])
        elif stmt.type == ASTType.TYPEDEF:
            i = parse_whitespace(stmt.val, 7)
            j = parse_word(stmt.val, i)

            if stmt.val[i:j] == "enum":
                i = parse_whitespace(stmt.val, j)

                if stmt.val[i] != "{":
                    j = parse_word(stmt.val, i)
                    result.append("enum " + stmt.val[i:j])
                    i = parse_whitespace(stmt.val, j)

                i += 1

                while True:
                    i = parse_whitespace(stmt.val, i)
                    j = parse_word(stmt.val, i)
                    result.append(stmt.val[i:j])

                    i = j

                    while stmt.val[i] != "}" and stmt.val[i] != ",":
                        i += 1

                    if stmt.val[i] == "}":
                        break

                    i += 1

                i = parse_whitespace(stmt.val, i + 1)

                for expr in parse_stmt_end(stmt.val[i:]):
                    result.append(expr)
            elif stmt.val[i:j] == "struct":
                i = parse_whitespace(stmt.val, j)

                if stmt.val[i] != "{":
                    j = parse_word(stmt.val, i)
                    result.append("struct " + stmt.val[i:j])
                    i = parse_whitespace(stmt.val, j)

                if stmt.val[i] == "{":
                    i += 1
                    blocks = 0

                    while True:
                        if stmt.val[i] == "{":
                            blocks += 1
                        elif stmt.val[i] == "}" and blocks == 0:
                            i += 1
                            break
                        elif stmt.val[i] == "}":
                            blocks -= 1

                        i += 1

                i = parse_whitespace(stmt.val, i)

                for expr in parse_stmt_end(stmt.val[i:]):
                    result.append(expr)
            elif stmt.val[i:j] == "union":
                i = parse_whitespace(stmt.val, j)

                if stmt.val[i] != "{":
                    j = parse_word(stmt.val, i)
                    result.append("union " + stmt.val[i:j])
                    i = parse_whitespace(stmt.val, j)

                i += 1
                blocks = 0

                while True:
                    if stmt.val[i] == "{":
                        blocks += 1
                    elif stmt.val[i] == "}" and blocks == 0:
                        i += 1
                        break
                    elif stmt.val[i] == "}":
                        blocks -= 1

                    i += 1

                i = parse_whitespace(stmt.val, i)

                for expr in parse_stmt_end(stmt.val[i:]):
                    result.append(expr)
            elif "(" in stmt.val:
                i = parse_whitespace(stmt.val, i)

                while True:
                    j = parse_word(stmt.val, i)
                    last_word = stmt.val[i:j]
                    j = parse_whitespace(stmt.val, j)
                    i = j

                    if stmt.val[i] == "(":
                        break


                while True:
                    if stmt.val[i] == ")":
                        break

                    i += 1

                j = parse_whitespace(stmt.val, i + 1)

                if stmt.val[j] == ";":
                    result.append(last_word)
                    continue

                j = i

                while stmt.val[i - 1].isalnum() or stmt.val[i - 1] == "_":
                    i -= 1

                result.append(stmt.val[i:j])
            else:
                i = len(stmt.val) - 2

                while True:
                    while stmt.val[i].isspace():
                        i -= 1

                    j = i

                    while stmt.val[i].isalnum() or stmt.val[i] == "_":
                        i -= 1

                    result.append(stmt.val[i + 1:j + 1])

                    while stmt.val[i].isspace() or stmt.val[i] == "*":
                        i -= 1

                    if stmt.val[i] != ",":
                        break

                    i -= 1

    result = filter(lambda it: it != "" and "__" not in it, result)
    result = list(set(result))
    result.sort(key=lambda it: (" " not in it, it.upper() != it, it))

    with open(sys.argv[2], "w") as f:
        f.write(unknown_content)

    print("\n".join(result))


if __name__ == "__main__":
    main()
