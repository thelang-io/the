/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>

#include "../src/Error.hpp"
#include "../src/Lexer.hpp"
#include "ReaderMock.hpp"

#define LEX(t, c, v) \
  do { auto r = ::testing::NiceMock<MockReader>(c); \
  const auto val = std::string(v); \
  size_t line = 1; \
  size_t col = 0; \
  for (size_t i = 0, size = val.length(); i < size; i++) { \
    if (val[i] == '\n') { \
      col = 0; \
      line += 1; \
    } else { \
      col += 1; \
    } \
  } \
  EXPECT_EQ(Lexer(&r).next(), Token(t, v, {0, 1, 0}, {sizeof(v) - 1, line, col})); } while (0)

#define LEX_FLOAT(t, v) \
  LEX_WS(t, v); \
  LEX_FLOAT_EXP(t, v)

#define LEX_FLOAT_EXP(t, v) \
  LEX_WS(t, v "E0"); \
  LEX_WS(t, v "e1"); \
  LEX_WS(t, v "e308"); \
  LEX_WS(t, v "E+0"); \
  LEX_WS(t, v "e+1"); \
  LEX_WS(t, v "e-308")

#define LEX_KW(t, v) \
  LEX_WS(t, v); \
  LEX(litId, v "a", v "a")

#define LEX_NUM(t, v) \
  LEX_WS(t, v); \
  LEX(t, v "..", v)

#define LEX_STR(t, v) \
  LEX_WS(t, "\"" v "\""); \
  LEX_WS(t, "\"text" v "\""); \
  LEX_WS(t, "\"" v "text\""); \
  LEX_WS(t, "\"text" v "text\""); \
  LEX_WS(t, "\"text" v "text" v "text\"")

#define LEX_THROW(c, m) \
  do { auto r = ::testing::NiceMock<MockReader>(c); \
  EXPECT_THROW({ \
    try { \
      Lexer(&r).next(); \
    } catch (const Error &err) { \
      const auto w = std::string(err.what()); \
      const auto p = w.find(':', w.find(':', w.find(':') + 1) + 1) + 2; \
      EXPECT_EQ(w.substr(p, w.find('\n') - p), m); \
      throw; \
    } \
  }, Error); } while (0)

#define LEX_WS(t, v) \
  LEX(t, v, v); \
  LEX(t, v "\n", v)

TEST(LexerTest, Misc) {
  LEX(eof, "", "");
  LEX(whitespace, "\n", "\n");
  LEX(whitespace, "\r", "\r");
  LEX(whitespace, "\t", "\t");
  LEX(whitespace, " ", " ");
  LEX(whitespace, "  ", "  ");
  LEX(whitespace, "\n\r\t ", "\n\r\t ");
}

TEST(LexerTest, Comments) {
  LEX_WS(commentLine, "//");
  LEX_WS(commentLine, "//text");
  LEX_WS(commentBlock, "/**/");
  LEX_WS(commentBlock, "/***/");
  LEX_WS(commentBlock, "/*!*/");
  LEX_WS(commentBlock, "/*text*/");
  LEX_WS(commentBlock, "/*\n*/");
  LEX_WS(commentBlock, "/*text\ntext*/");
  LEX_WS(commentBlock, "/*\n\n*/");
  LEX_WS(commentBlock, "/*text\ntext\ntext*/");
}

TEST(LexerTest, Keywords) {
  LEX_KW(kwAs, "as");
  LEX_WS(kwAsSafe, "as?");
  LEX_KW(kwAsync, "async");
  LEX_KW(kwAwait, "await");
  LEX_KW(kwBreak, "break");
  LEX_KW(kwCase, "case");
  LEX_KW(kwCatch, "catch");
  LEX_KW(kwClass, "class");
  LEX_KW(kwConst, "const");
  LEX_KW(kwContinue, "continue");
  LEX_KW(kwDefault, "default");
  LEX_KW(kwDeinit, "deinit");
  LEX_KW(kwElif, "elif");
  LEX_KW(kwElse, "else");
  LEX_KW(kwEnum, "enum");
  LEX_KW(kwExport, "export");
  LEX_KW(kwFallthrough, "fallthrough");
  LEX_KW(kwFalse, "false");
  LEX_KW(kwFn, "fn");
  LEX_KW(kwFrom, "from");
  LEX_KW(kwIf, "if");
  LEX_KW(kwImport, "import");
  LEX_KW(kwIn, "in");
  LEX_KW(kwInit, "init");
  LEX_KW(kwInterface, "interface");
  LEX_KW(kwIs, "is");
  LEX_KW(kwLoop, "loop");
  LEX_KW(kwMain, "main");
  LEX_KW(kwMatch, "match");
  LEX_KW(kwMut, "mut");
  LEX_KW(kwNew, "new");
  LEX_KW(kwNil, "nil");
  LEX_KW(kwObj, "obj");
  LEX_KW(kwOp, "op");
  LEX_KW(kwOverride, "override");
  LEX_KW(kwPriv, "priv");
  LEX_KW(kwProt, "prot");
  LEX_KW(kwPub, "pub");
  LEX_KW(kwReturn, "return");
  LEX_KW(kwStatic, "static");
  LEX_KW(kwSuper, "super");
  LEX_KW(kwThis, "this");
  LEX_KW(kwThrow, "throw");
  LEX_KW(kwTrue, "true");
}

TEST(LexerTest, Literals) {
  LEX_WS(litChar, "' '");
  LEX_WS(litChar, "'!'");
  LEX_WS(litChar, "'A'");
  LEX_WS(litChar, "'a'");
  LEX_WS(litChar, "'0'");
  LEX_WS(litChar, "'9'");
  LEX_WS(litChar, "'\\0'");
  LEX_WS(litChar, "'\\t'");
  LEX_WS(litChar, "'\\n'");
  LEX_WS(litChar, "'\\r'");
  LEX_WS(litChar, "'\\\"'");
  LEX_WS(litChar, "'\\''");
  LEX_WS(litChar, "'\\\\'");

  LEX_FLOAT_EXP(litFloat, "0");
  LEX_FLOAT_EXP(litFloat, "1");
  LEX_FLOAT_EXP(litFloat, "9223372036854775807");

  LEX_FLOAT_EXP(litFloat, "0.");
  LEX_FLOAT_EXP(litFloat, "1.");
  LEX_FLOAT_EXP(litFloat, "9223372036854775807.");

  LEX_FLOAT(litFloat, "0.0");
  LEX_FLOAT(litFloat, "1.1");
  LEX_FLOAT(litFloat, "9223372036854775807.9223372036854775807");

  LEX_WS(litId, "_");
  LEX_WS(litId, "A");
  LEX_WS(litId, "a");
  LEX_WS(litId, "Aa_09");
  LEX_WS(litId, "_zZ12");

  LEX_NUM(litIntBin, "0B0");
  LEX_NUM(litIntBin, "0b1");
  LEX_NUM(litIntBin, "0B00000000000000000000000000000000");
  LEX_NUM(litIntBin, "0b11111111111111111111111111111111");

  LEX_NUM(litIntDec, "0");
  LEX_NUM(litIntDec, "1");
  LEX_NUM(litIntDec, "9223372036854775807");

  LEX_NUM(litIntHex, "0X0");
  LEX_NUM(litIntHex, "0x9");
  LEX_NUM(litIntHex, "0XA");
  LEX_NUM(litIntHex, "0xf");
  LEX_NUM(litIntHex, "0X9999999999999999");
  LEX_NUM(litIntHex, "0xffffffffffffffff");

  LEX_NUM(litIntOct, "0O0");
  LEX_NUM(litIntOct, "0o7");
  LEX_NUM(litIntOct, "0O000000000000000000000");
  LEX_NUM(litIntOct, "0o777777777777777777777");

  LEX_STR(litStr, R"()");
  LEX_STR(litStr, R"({var})");
  LEX_STR(litStr, R"({var.method})");
  LEX_STR(litStr, R"({"}" + var.method})");
  LEX_STR(litStr, R"({"text" + '{' + Node{} + "}"})");
  LEX_STR(litStr, R"(\\0)");
  LEX_STR(litStr, R"(\\t)");
  LEX_STR(litStr, R"(\\n)");
  LEX_STR(litStr, R"(\\r)");
  LEX_STR(litStr, R"(\")");
  LEX_STR(litStr, R"(\')");
  LEX_STR(litStr, R"(\\)");
  LEX_STR(litStr, R"(\{)");
}

TEST(LexerTest, Operators) {
  LEX_WS(opAnd, "&");
  LEX_WS(opAndAnd, "&&");
  LEX_WS(opAndAndEq, "&&=");
  LEX_WS(opAndEq, "&=");
  LEX_WS(opCaret, "^");
  LEX_WS(opCaretEq, "^=");
  LEX_WS(opColon, ":");
  LEX_WS(opColonEq, ":=");
  LEX_WS(opComma, ",");
  LEX_WS(opDot, ".");
  LEX_WS(opDotDot, "..");
  LEX_WS(opDotDotDot, "...");
  LEX_WS(opDotDotEq, "..=");
  LEX_WS(opEq, "=");
  LEX_WS(opEqEq, "==");
  LEX_WS(opExcl, "!");
  LEX_WS(opExclEq, "!=");
  LEX_WS(opExclExcl, "!!");
  LEX_WS(opGt, ">");
  LEX_WS(opGtEq, ">=");
  LEX_WS(opLBrace, "{");
  LEX_WS(opLBrack, "[");
  LEX_WS(opLPar, "(");
  LEX_WS(opLShift, "<<");
  LEX_WS(opLShiftEq, "<<=");
  LEX_WS(opLt, "<");
  LEX_WS(opLtEq, "<=");
  LEX_WS(opMinus, "-");
  LEX_WS(opMinusEq, "-=");
  LEX_WS(opMinusMinus, "--");
  LEX_WS(opOr, "|");
  LEX_WS(opOrEq, "|=");
  LEX_WS(opOrOr, "||");
  LEX_WS(opOrOrEq, "||=");
  LEX_WS(opPercent, "%");
  LEX_WS(opPercentEq, "%=");
  LEX_WS(opPlus, "+");
  LEX_WS(opPlusEq, "+=");
  LEX_WS(opPlusPlus, "++");
  LEX_WS(opQn, "?");
  LEX_WS(opQnDot, "?.");
  LEX_WS(opQnQn, "??");
  LEX_WS(opQnQnEq, "\?\?=");
  LEX_WS(opRBrace, "}");
  LEX_WS(opRBrack, "]");
  LEX_WS(opRPar, ")");
  LEX_WS(opRShift, ">>");
  LEX_WS(opRShiftEq, ">>=");
  LEX_WS(opSemi, ";");
  LEX_WS(opSlash, "/");
  LEX_WS(opSlashEq, "/=");
  LEX_WS(opStar, "*");
  LEX_WS(opStarEq, "*=");
  LEX_WS(opStarStar, "**");
  LEX_WS(opStarStarEq, "**=");
  LEX_WS(opTilde, "~");
}

TEST(LexerTest, Throws) {
  LEX_THROW(R"(\)", E0000);

  LEX_THROW(R"(/*)", E0001);
  LEX_THROW(R"(/*text)", E0001);

  LEX_THROW(R"(')", E0002);
  LEX_THROW(R"('')", E0004);
  LEX_THROW(R"('a)", E0002);
  LEX_THROW(R"('\)", E0002);
  LEX_THROW(R"('\n)", E0002);
  LEX_THROW(R"('\m)", E0005);
  LEX_THROW(R"('char)", E0006);
  LEX_THROW(R"('char')", E0006);
  LEX_THROW(R"(")", E0003);
  LEX_THROW(R"("text)", E0003);
  LEX_THROW(R"("text\)", E0003);
  LEX_THROW(R"("text\m")", E0005);
  LEX_THROW(R"("text{"\m"}")", E0005);

  LEX_THROW("04", E0007);
  LEX_THROW("0400", E0007);
  LEX_THROW("1234g", E0009);

  LEX_THROW("0b", E0008);
  LEX_THROW("0bG", E0008);
  LEX_THROW("0B1g", E0008);

  LEX_THROW("0x", E0010);
  LEX_THROW("0xG", E0010);
  LEX_THROW("0Xag", E0010);

  LEX_THROW("0o", E0011);
  LEX_THROW("0oG", E0011);
  LEX_THROW("0O1g", E0011);

  LEX_THROW("1234.", E0012);
  LEX_THROW("1234./", E0012);
  LEX_THROW("1234.af", E0012);
  LEX_THROW("1234.0a", E0012);
  LEX_THROW("1234.0af", E0012);
  LEX_THROW("1234e", E0013);
  LEX_THROW("1234ea", E0013);
  LEX_THROW("1234e+", E0013);
  LEX_THROW("1234e+a", E0013);
  LEX_THROW("0b1.0", E0014);
  LEX_THROW("0xa.0", E0015);
  LEX_THROW("0o1.0", E0016);
}
