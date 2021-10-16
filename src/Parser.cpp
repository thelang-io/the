/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

Expr parseExpr (Reader *reader) {
  auto start = reader->loc;
  auto tok = lex(reader);

  if (
    tok.type == tkLitChar ||
    tok.type == tkLitFloat ||
    tok.type == tkLitId ||
    tok.type == tkLitIntDec ||
    tok.type == tkLitStr
  ) {
    return {exprLiteral, start, reader->loc, ExprLiteral{tok}};
  }

  reader->seek(start);
  throw Error("TODO");
}

void parseStmtWhitespace (Reader *reader) {
  while (true) {
    auto loc = reader->loc;
    auto tok = lex(reader);

    if (tok.type != tkCommentBlock && tok.type != tkCommentLine && tok.type != tkWhitespace) {
      reader->seek(loc);
      break;
    }
  }
}

Stmt parse (Reader *reader) {
  parseStmtWhitespace(reader);
  auto start = reader->loc;

  if (reader->eof()) {
    return {stmtEnd, start, reader->loc, StmtEnd{}};
  }

  auto tok1 = lex(reader);

  if (tok1.type == tkKwMain) {
    parseStmtWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type != tkOpLBrace) {
      throw Error("TODO");
    }

    auto body = std::vector<Stmt>();

    while (true) {
      parseStmtWhitespace(reader);
      auto loc3 = reader->loc;
      auto tok3 = lex(reader);

      if (tok3.type == tkOpRBrace) {
        break;
      }

      reader->seek(loc3);
      body.emplace_back(parse(reader));
    }

    return {stmtMain, start, reader->loc, StmtMain{body}};
  } else if (tok1.type == tkKwMut) {
    parseStmtWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type == tkLitId) {
      parseStmtWhitespace(reader);
      auto tok3 = lex(reader);

      if (tok3.type == tkOpColonEq) {
        parseStmtWhitespace(reader);
        auto expr = parseExpr(reader);
        return {stmtShortVarDecl, start, reader->loc, StmtShortVarDecl{tok2, expr, true}};
      }
    }
  } else if (tok1.type == tkLitId) {
    parseStmtWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type == tkOpColonEq) {
      parseStmtWhitespace(reader);
      auto expr = parseExpr(reader);
      return {stmtShortVarDecl, start, reader->loc, StmtShortVarDecl{tok1, expr, false}};
    } else if (tok2.type == tkOpEq) {
      parseStmtWhitespace(reader);
      auto expr = parseExpr(reader);
      return {stmtAssignExpr, start, reader->loc, StmtAssignExpr{tok1, expr}};
    } else if (tok2.type == tkOpLPar) {
      parseStmtWhitespace(reader);
      auto loc3 = reader->loc;
      auto tok3 = lex(reader);
      auto args = std::vector<Expr>();

      while (tok3.type != tkOpRPar) {
        reader->seek(loc3);
        auto expr = parseExpr(reader);
        args.emplace_back(expr);

        parseStmtWhitespace(reader);
        tok3 = lex(reader);

        if (tok3.type == tkOpComma) {
          parseStmtWhitespace(reader);
          loc3 = reader->loc;
          tok3 = lex(reader);
        }
      }

      return {stmtCallExpr, start, reader->loc, StmtCallExpr{args, tok1}};
    }
  }

  throw SyntaxError(reader, start, E0100);
}
