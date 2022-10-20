#!/usr/bin/env python3

import enum
import sys


@enum.unique
class ASTType(enum.Enum):
    UNKNOWN = 0
    COMMENT_BLOCK = 1
    COMMENT_LINE = 2
    FN_DECL = 3
    FN_DEF = 4
    INCLUDE = 5
    MACRO_DEFINE = 6
    MACRO_IF = 7
    MACRO_IFDEF = 8
    MACRO_IFNDEF = 9
    MACRO_ELIF = 10
    MACRO_ELSE = 11
    MACRO_ENDIF = 12
    MACRO_ERROR = 13
    MACRO_PRAGMA = 14
    MACRO_UNDEF = 15
    TYPEDEF = 16
    WHITESPACE = 17

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


def process(s):
    i = 0

    while i < len(s):
        if s[i] == "#":
            stmt_start = i

            while s[i + 1].isspace():
                i += 1

            if s[i + 1:i + 7] == "define":
                stmt_type = ASTType.MACRO_DEFINE
            elif s[i + 1:i + 3] == "if":
                stmt_type = ASTType.MACRO_IF
            elif s[i + 1:i + 6] == "ifdef":
                stmt_type = ASTType.MACRO_IFDEF
            elif s[i + 1:i + 7] == "ifndef":
                stmt_type = ASTType.MACRO_IFNDEF
            elif s[i + 1:i + 5] == "elif":
                stmt_type = ASTType.MACRO_ELIF
            elif s[i + 1:i + 5] == "else":
                stmt_type = ASTType.MACRO_ELSE
            elif s[i + 1:i + 6] == "endif":
                stmt_type = ASTType.MACRO_ENDIF
            elif s[i + 1:i + 6] == "error":
                stmt_type = ASTType.MACRO_ERROR
            elif s[i + 1:i + 7] == "pragma":
                stmt_type = ASTType.MACRO_PRAGMA
            elif s[i + 1:i + 6] == "undef":
                stmt_type = ASTType.MACRO_UNDEF
            else:
                stmt_type = ASTType.UNKNOWN

            while s[i + 1] != "\n":
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
            i += 2

            while s[i] != "\n":
                i += 1

            yield Stmt(ASTType.COMMENT_LINE, s, stmt_start, i + 1)
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
    if len(sys.argv) > 1:
        with open(sys.argv[1]) as f:
            s = f.read()

        result = []

        for stmt in process(s):
            if stmt.type == ASTType.MACRO_DEFINE:
                i = 1

                while stmt.val[i].isspace():
                    i += 1

                i += 7
                j = i

                while j < len(stmt.val) and (stmt.val[j].isalnum() or stmt.val[j] == "_"):
                    j += 1

                result.append(stmt.val[i:j])
            elif stmt.type == ASTType.TYPEDEF:
                # todo grep name from basic, function, struct, enum or union
                pass
            elif stmt.type == ASTType.UNKNOWN:
                # todo check why so many unknowns
                print(stmt.val)

        result = filter(lambda it: it != "" and not it.startswith("__"), result)
        result = list(set(result))
        result.sort(key=lambda it: (" " not in it, it.upper() != it, it))

        # todo uncomment
        # print("\n".join(result))
    else:
        print("Provide file path as argument")


if __name__ == "__main__":
    main()
