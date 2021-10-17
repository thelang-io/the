/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "Parser.hpp"

ExprAssign::~ExprAssign () {
  delete this->left;
  delete this->right;
}

ExprCall::~ExprCall () {
  for (auto &arg : this->args) {
    delete arg;
  }

  delete this->callee;
}

void parseWalkWhitespace (Reader *reader) {
  while (true) {
    auto loc = reader->loc;
    auto tok = lex(reader);

    if (tok->type != TK_COMMENT_BLOCK && tok->type != TK_COMMENT_LINE && tok->type != TK_WHITESPACE) {
      reader->seek(loc);
      delete tok;

      break;
    }

    delete tok;
  }
}

StmtExpr *parseStmtExpr (Reader *reader) {
  auto tok1 = lex(reader);

  if (
    tok1->type == TK_LIT_CHAR ||
    tok1->type == TK_LIT_FLOAT ||
    tok1->type == TK_LIT_INT_DEC ||
    tok1->type == TK_LIT_STR
  ) {
    auto lit = new Literal{tok1};
    return new StmtExpr{STMT_EXPR_LITERAL, lit};
  } else if (tok1->type == TK_LIT_ID) {
    parseWalkWhitespace(reader);

    auto loc2 = reader->loc;
    auto tok2 = lex(reader);

    if (tok2->type == TK_OP_EQ) {
      parseWalkWhitespace(reader);
      auto stmtExpr = parseStmtExpr(reader);
      auto exprAssign = new ExprAssign{tok1, stmtExpr};
      auto expr = new Expr{EXPR_ASSIGN, exprAssign};

      delete tok2;
      return new StmtExpr{STMT_EXPR_EXPR, expr};
    } else if (tok2->type == TK_OP_LPAR) {
      parseWalkWhitespace(reader);

      auto loc3 = reader->loc;
      auto tok3 = lex(reader);
      auto args = std::vector<StmtExpr *>();

      while (tok3->type != TK_OP_RPAR) {
        reader->seek(loc3);
        args.push_back(parseStmtExpr(reader));

        parseWalkWhitespace(reader);
        delete tok3;

        tok3 = lex(reader);

        if (tok3->type == TK_OP_COMMA) {
          parseWalkWhitespace(reader);
          delete tok3;

          loc3 = reader->loc;
          tok3 = lex(reader);
        }
      }

      auto exprCall = new ExprCall{args, tok1};
      auto expr = new Expr{EXPR_CALL, exprCall};

      delete tok2;
      delete tok3;

      return new StmtExpr{STMT_EXPR_EXPR, expr};
    }

    reader->seek(loc2);
    delete tok2;

    auto id = new Identifier{tok1};
    return new StmtExpr{STMT_EXPR_IDENTIFIER, id};
  }

  throw Error("TODO");
}

Stmt *parse (Reader *reader) {
  parseWalkWhitespace(reader);

  if (reader->eof()) {
    auto stmtEnd = new StmtEnd{};
    return new Stmt{STMT_END, stmtEnd};
  }

  auto loc1 = reader->loc;
  auto tok1 = lex(reader);

  if (tok1->type == TK_KW_MAIN) {
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2->type != TK_OP_LBRACE) {
      throw Error("TODO");
    }

    delete tok1;
    delete tok2;

    auto body = std::vector<Stmt *>();

    while (true) {
      parseWalkWhitespace(reader);

      auto loc3 = reader->loc;
      auto tok3 = lex(reader);

      if (tok3->type == TK_OP_RBRACE) {
        delete tok3;
        break;
      }

      reader->seek(loc3);
      body.push_back(parse(reader));

      delete tok3;
    }

    auto stmtMain = new StmtMain{body};
    return new Stmt{STMT_MAIN, stmtMain};
  } else if (tok1->type == TK_KW_MUT) {
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2->type == TK_LIT_ID) {
      parseWalkWhitespace(reader);
      auto tok3 = lex(reader);

      if (tok3->type == TK_OP_COLON_EQ) {
        parseWalkWhitespace(reader);
        auto stmtExpr = parseStmtExpr(reader);
        auto stmtShortVarDecl = new StmtShortVarDecl{tok2, stmtExpr, true};

        delete tok1;
        delete tok3;

        return new Stmt{STMT_SHORT_VAR_DECL, stmtShortVarDecl};
      }

      delete tok3;
    }

    delete tok2;
  } else if (tok1->type == TK_LIT_ID) {
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2->type == TK_OP_COLON_EQ) {
      parseWalkWhitespace(reader);
      delete tok2;

      auto stmtExpr = parseStmtExpr(reader);
      auto stmtShortVarDecl = new StmtShortVarDecl{tok1, stmtExpr};

      return new Stmt{STMT_SHORT_VAR_DECL, stmtShortVarDecl};
    }

    delete tok2;
  }

  delete tok1;
  reader->seek(loc1);

  try {
    auto stmtExpr = parseStmtExpr(reader);
    return new Stmt{STMT_EXPR, stmtExpr};
  } catch (const Error &err) {
    reader->seek(loc1);
  }

  throw SyntaxError(reader, loc1, E0100);
}
