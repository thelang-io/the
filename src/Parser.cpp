/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "Parser.hpp"

StmtExpr *parseStmtExpr (Reader *reader);

Block::~Block () {
  for (auto &stmt : this->body) {
    delete stmt;
  }
}

Expr::~Expr () {
  if (this->type == EXPR_ASSIGN) {
    delete std::get<ExprAssign *>(this->body);
  } else if (this->type == EXPR_BINARY) {
    delete std::get<ExprBinary *>(this->body);
  } else if (this->type == EXPR_CALL) {
    delete std::get<ExprCall *>(this->body);
  } else if (this->type == EXPR_UNARY) {
    delete std::get<ExprUnary *>(this->body);
  }
}

ExprAssign::~ExprAssign () {
  delete this->left;
  delete this->op;
  delete this->right;
}

ExprBinary::~ExprBinary () {
  delete this->left;
  delete this->op;
  delete this->right;
}

ExprCall::~ExprCall () {
  for (auto &arg : this->args) {
    delete arg;
  }

  delete this->callee;
}

ExprUnary::~ExprUnary () {
  delete this->arg;
  delete this->op;
}

Identifier::~Identifier () {
  delete this->name;
}

Literal::~Literal () {
  delete this->val;
}

Stmt::~Stmt () {
  if (this->type == STMT_END) {
    delete std::get<StmtEnd *>(this->body);
  } else if (this->type == STMT_EXPR) {
    delete std::get<StmtExpr *>(this->body);
  } else if (this->type == STMT_IF) {
    delete std::get<StmtIf *>(this->body);
  } else if (this->type == STMT_MAIN) {
    delete std::get<StmtMain *>(this->body);
  } else if (this->type == STMT_RETURN) {
    delete std::get<StmtReturn *>(this->body);
  } else if (this->type == STMT_SHORT_VAR_DECL) {
    delete std::get<StmtShortVarDecl *>(this->body);
  }
}

StmtExpr::~StmtExpr () {
  if (this->type == STMT_EXPR_EXPR) {
    delete std::get<Expr *>(this->body);
  } else if (this->type == STMT_EXPR_IDENTIFIER) {
    delete std::get<Identifier *>(this->body);
  } else if (this->type == STMT_EXPR_LITERAL) {
    delete std::get<Literal *>(this->body);
  }
}

StmtIf::~StmtIf () {
  delete this->cond;
  delete this->body;
  delete this->alt;
}

StmtIfAlt::~StmtIfAlt () {
  if (this->type == STMT_IF_ALT_BLOCK) {
    delete std::get<Block *>(this->body);
  } else if (this->type == STMT_IF_ALT_STMT_IF) {
    delete std::get<StmtIf *>(this->body);
  }
}

StmtMain::~StmtMain () {
  delete this->body;
}

StmtReturn::~StmtReturn () {
  delete this->arg;
}

StmtShortVarDecl::~StmtShortVarDecl () {
  delete this->id;
  delete this->init;
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

Block *parseBlock (Reader *reader) {
  parseWalkWhitespace(reader);
  auto tok1 = lex(reader);

  if (tok1->type != TK_OP_LBRACE) {
    throw Error("Expected left brace");
  }

  delete tok1;
  auto body = std::vector<Stmt *>();

  while (true) {
    auto loc2 = reader->loc;
    parseWalkWhitespace(reader);

    auto tok2 = lex(reader);

    if (tok2->type == TK_EOF) {
      throw Error("Expected right brace");
    } else if (tok2->type == TK_OP_RBRACE) {
      delete tok2;
      break;
    }

    reader->seek(loc2);

    auto stmt = parse(reader);
    body.push_back(stmt);

    delete tok2;
  }

  return new Block{body};
}

StmtExpr *parsePostStmtExpr (Reader *reader, StmtExpr *stmtExpr) {
  auto loc = reader->loc;
  parseWalkWhitespace(reader);

  auto tok = lex(reader);

  if (
    tok->type == TK_OP_AND ||
    tok->type == TK_OP_AND_AND ||
    tok->type == TK_OP_CARET ||
    tok->type == TK_OP_EQ_EQ ||
    tok->type == TK_OP_EXCL_EQ ||
    tok->type == TK_OP_GT ||
    tok->type == TK_OP_GT_EQ ||
    tok->type == TK_OP_LSHIFT ||
    tok->type == TK_OP_LT ||
    tok->type == TK_OP_LT_EQ ||
    tok->type == TK_OP_OR ||
    tok->type == TK_OP_OR_OR ||
    tok->type == TK_OP_MINUS ||
    tok->type == TK_OP_PERCENT ||
    tok->type == TK_OP_PLUS ||
    tok->type == TK_OP_QN_QN ||
    tok->type == TK_OP_RSHIFT ||
    tok->type == TK_OP_SLASH ||
    tok->type == TK_OP_STAR ||
    tok->type == TK_OP_STAR_STAR
  ) {
    parseWalkWhitespace(reader);
    auto stmtExpr2 = parseStmtExpr(reader);

    if (stmtExpr2->type == STMT_EXPR_EXPR) {
      auto expr2 = std::get<Expr *>(stmtExpr2->body);

      if (expr2->type == EXPR_BINARY) {
        auto exprBinary2 = std::get<ExprBinary *>(expr2->body);

        if (tokenPrecedence(tok) >= tokenPrecedence(exprBinary2->op) && !stmtExpr2->parenthesized) {
          auto exprBinaryLeft = new ExprBinary{stmtExpr, tok, exprBinary2->left};
          auto exprLeft = new Expr{EXPR_BINARY, exprBinaryLeft};
          auto stmtExprLeft = new StmtExpr{STMT_EXPR_EXPR, exprLeft};
          auto exprBinaryRight = new ExprBinary{stmtExprLeft, exprBinary2->op, exprBinary2->right};
          auto exprRight = new Expr{EXPR_BINARY, exprBinaryRight};

          exprBinary2->left = nullptr;
          exprBinary2->op = nullptr;
          exprBinary2->right = nullptr;
          delete stmtExpr2;

          return new StmtExpr{STMT_EXPR_EXPR, exprRight};
        }
      }
    }

    auto exprBinary = new ExprBinary{stmtExpr, tok, stmtExpr2};
    auto expr = new Expr{EXPR_BINARY, exprBinary};

    return new StmtExpr{STMT_EXPR_EXPR, expr};
  }

  reader->seek(loc);
  delete tok;

  return stmtExpr;
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
    return parsePostStmtExpr(reader, new StmtExpr{STMT_EXPR_LITERAL, lit});
  } else if (tok1->type == TK_LIT_ID) {
    auto loc2 = reader->loc;
    parseWalkWhitespace(reader);

    auto tok2 = lex(reader);

    if (
      tok2->type == TK_OP_AND_AND_EQ ||
      tok2->type == TK_OP_AND_EQ ||
      tok2->type == TK_OP_CARET_EQ ||
      tok2->type == TK_OP_EQ ||
      tok2->type == TK_OP_LSHIFT_EQ ||
      tok2->type == TK_OP_MINUS_EQ ||
      tok2->type == TK_OP_OR_EQ ||
      tok2->type == TK_OP_OR_OR_EQ ||
      tok2->type == TK_OP_PERCENT_EQ ||
      tok2->type == TK_OP_PLUS_EQ ||
      tok2->type == TK_OP_QN_QN_EQ ||
      tok2->type == TK_OP_RSHIFT_EQ ||
      tok2->type == TK_OP_SLASH_EQ ||
      tok2->type == TK_OP_STAR_EQ ||
      tok2->type == TK_OP_STAR_STAR_EQ
    ) {
      parseWalkWhitespace(reader);

      auto stmtExpr = parseStmtExpr(reader);
      auto exprAssign = new ExprAssign{tok1, tok2, stmtExpr};
      auto expr = new Expr{EXPR_ASSIGN, exprAssign};

      return parsePostStmtExpr(reader, new StmtExpr{STMT_EXPR_EXPR, expr});
    } else if (tok2->type == TK_OP_LPAR) {
      auto loc3 = reader->loc;
      parseWalkWhitespace(reader);

      auto tok3 = lex(reader);
      auto args = std::vector<StmtExpr *>();

      while (tok3->type != TK_OP_RPAR) {
        reader->seek(loc3);
        parseWalkWhitespace(reader);
        args.push_back(parseStmtExpr(reader));

        parseWalkWhitespace(reader);
        delete tok3;

        tok3 = lex(reader);

        if (tok3->type == TK_OP_COMMA) {
          loc3 = reader->loc;
          parseWalkWhitespace(reader);

          delete tok3;
          tok3 = lex(reader);
        }
      }

      auto exprCall = new ExprCall{tok1, args};
      auto expr = new Expr{EXPR_CALL, exprCall};

      delete tok2;
      delete tok3;

      return parsePostStmtExpr(reader, new StmtExpr{STMT_EXPR_EXPR, expr});
    } else if (tok2->type == TK_OP_MINUS_MINUS || tok2->type == TK_OP_PLUS_PLUS) {
      auto id = new Identifier{tok1};
      auto stmtExpr = new StmtExpr{STMT_EXPR_IDENTIFIER, id};
      auto exprUnary = new ExprUnary{stmtExpr, tok2};
      auto expr = new Expr{EXPR_UNARY, exprUnary};

      return parsePostStmtExpr(reader, new StmtExpr{STMT_EXPR_EXPR, expr});
    }

    reader->seek(loc2);
    delete tok2;

    auto id = new Identifier{tok1};
    return parsePostStmtExpr(reader, new StmtExpr{STMT_EXPR_IDENTIFIER, id});
  } else if (
    tok1->type == TK_OP_EXCL ||
    tok1->type == TK_OP_EXCL_EXCL ||
    tok1->type == TK_OP_MINUS ||
    tok1->type == TK_OP_MINUS_MINUS ||
    tok1->type == TK_OP_PLUS ||
    tok1->type == TK_OP_PLUS_PLUS ||
    tok1->type == TK_OP_TILDE
  ) {
    parseWalkWhitespace(reader);

    auto stmtExpr = parseStmtExpr(reader);
    auto exprUnary = new ExprUnary{stmtExpr, tok1, true};
    auto expr = new Expr{EXPR_UNARY, exprUnary};

    return parsePostStmtExpr(reader, new StmtExpr{STMT_EXPR_EXPR, expr});
  } else if (tok1->type == TK_OP_LPAR) {
    parseWalkWhitespace(reader);
    delete tok1;

    auto stmtExpr = parseStmtExpr(reader);
    parseWalkWhitespace(reader);

    auto tok2 = lex(reader);

    if (tok2->type != TK_OP_RPAR) {
      throw Error("Expected right parentheses");
    }

    delete tok2;
    stmtExpr->parenthesized = true;

    return parsePostStmtExpr(reader, stmtExpr);
  }

  throw Error("Unknown expression statement");
}

Stmt *parse (Reader *reader) {
  parseWalkWhitespace(reader);

  if (reader->eof()) {
    auto stmtEnd = new StmtEnd{};
    return new Stmt{STMT_END, stmtEnd};
  }

  auto loc1 = reader->loc;
  auto tok1 = lex(reader);

  if (tok1->type == TK_KW_IF) {
    delete tok1;
    parseWalkWhitespace(reader);

    auto stmtExpr = parseStmtExpr(reader);
    auto block = parseBlock(reader);
    auto alt = static_cast<StmtIfAlt *>(nullptr);
    auto altTail = static_cast<StmtIfAlt *>(nullptr);

    while (true) {
      auto loc2 = reader->loc;
      parseWalkWhitespace(reader);
      auto tok2 = lex(reader);

      if (tok2->type == TK_KW_ELIF) {
        delete tok2;
        parseWalkWhitespace(reader);

        auto stmtExprElif = parseStmtExpr(reader);
        auto blockElif = parseBlock(reader);
        auto stmtElif = new StmtIf{stmtExprElif, blockElif, nullptr};
        auto stmtElifAlt = new StmtIfAlt{STMT_IF_ALT_STMT_IF, stmtElif};

        if (alt == nullptr && altTail == nullptr) {
          alt = stmtElifAlt;
          altTail = stmtElifAlt;
        } else {
          std::get<StmtIf *>(altTail->body)->alt = stmtElifAlt;
          altTail = stmtElifAlt;
        }

        continue;
      } else if (tok2->type == TK_KW_ELSE) {
        parseWalkWhitespace(reader);

        auto blockElse = parseBlock(reader);
        auto stmtElseAlt = new StmtIfAlt{STMT_IF_ALT_BLOCK, blockElse};

        if (alt == nullptr && altTail == nullptr) {
          alt = stmtElseAlt;
        } else {
          std::get<StmtIf *>(altTail->body)->alt = stmtElseAlt;
        }
      } else {
        reader->seek(loc2);
      }

      delete tok2;
      break;
    }

    auto stmtIf = new StmtIf{stmtExpr, block, alt};
    return new Stmt{STMT_IF, stmtIf};
  } else if (tok1->type == TK_KW_MAIN) {
    delete tok1;

    auto block = parseBlock(reader);
    auto stmtMain = new StmtMain{block};

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
  } else if (tok1->type == TK_KW_RETURN) {
    delete tok1;
    parseWalkWhitespace(reader);

    auto stmtExpr = parseStmtExpr(reader);
    auto stmtReturn = new StmtReturn{stmtExpr};

    return new Stmt{STMT_RETURN, stmtReturn};
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
