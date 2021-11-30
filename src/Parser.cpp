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
  for (auto stmt : this->body) {
    delete stmt;
  }
}

Cond::~Cond () {
  if (this->type == COND_BLOCK) {
    delete std::get<Block *>(this->body);
  } else if (this->type == COND_STMT_IF) {
    delete std::get<StmtIf *>(this->body);
  }
}

Expr::~Expr () {
  if (this->type == EXPR_ACCESS) {
    delete std::get<ExprAccess *>(this->body);
  } else if (this->type == EXPR_ASSIGN) {
    delete std::get<ExprAssign *>(this->body);
  } else if (this->type == EXPR_BINARY) {
    delete std::get<ExprBinary *>(this->body);
  } else if (this->type == EXPR_CALL) {
    delete std::get<ExprCall *>(this->body);
  } else if (this->type == EXPR_COND) {
    delete std::get<ExprCond *>(this->body);
  } else if (this->type == EXPR_MEMBER) {
    delete std::get<ExprMember *>(this->body);
  } else if (this->type == EXPR_OBJ) {
    delete std::get<ExprObj *>(this->body);
  } else if (this->type == EXPR_UNARY) {
    delete std::get<ExprUnary *>(this->body);
  }
}

ExprAccess::~ExprAccess () {
  if (this->type == EXPR_ACCESS_EXPR_MEMBER) {
    delete std::get<ExprMember *>(this->body);
  } else if (this->type == EXPR_ACCESS_IDENTIFIER) {
    delete std::get<Identifier *>(this->body);
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
  for (auto arg : this->args) {
    delete arg->id;
    delete arg->expr;
    delete arg;
  }

  delete this->callee;
}

ExprCond::~ExprCond () {
  delete this->cond;
  delete this->body;
  delete this->alt;
}

ExprMember::~ExprMember () {
  delete this->obj;
  delete this->prop;
}

ExprObj::~ExprObj () {
  for (auto prop : this->props) {
    delete prop->id;
    delete prop->init;
    delete prop;
  }

  delete this->type;
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
  if (this->type == STMT_BREAK) {
    delete std::get<StmtBreak *>(this->body);
  } else if (this->type == STMT_CONTINUE) {
    delete std::get<StmtContinue *>(this->body);
  } else if (this->type == STMT_END) {
    delete std::get<StmtEnd *>(this->body);
  } else if (this->type == STMT_EXPR) {
    delete std::get<StmtExpr *>(this->body);
  } else if (this->type == STMT_FN_DECL) {
    delete std::get<StmtFnDecl *>(this->body);
  } else if (this->type == STMT_IF) {
    delete std::get<StmtIf *>(this->body);
  } else if (this->type == STMT_LOOP) {
    delete std::get<StmtLoop *>(this->body);
  } else if (this->type == STMT_MAIN) {
    delete std::get<StmtMain *>(this->body);
  } else if (this->type == STMT_OBJ_DECL) {
    delete std::get<StmtObjDecl *>(this->body);
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

StmtFnDecl::~StmtFnDecl () {
  for (auto param : this->params) {
    delete param->id;
    delete param->type;
    delete param->init;
    delete param;
  }

  delete this->id;
  delete this->returnType;
  delete this->body;
}

StmtIf::~StmtIf () {
  delete this->cond;
  delete this->body;
  delete this->alt;
}

StmtLoop::~StmtLoop () {
  delete this->init;
  delete this->cond;
  delete this->upd;
  delete this->body;
}

StmtMain::~StmtMain () {
  delete this->body;
}

StmtObjDecl::~StmtObjDecl () {
  for (auto field : this->fields) {
    delete field->id;
    delete field->type;
    delete field;
  }

  delete this->id;
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

ExprAccess *parseExprAccess (Reader *reader, ExprAccess *exprAccess) {
  auto loc1 = reader->loc;
  parseWalkWhitespace(reader);
  auto tok1 = lex(reader);

  while (tok1->type == TK_OP_DOT) {
    delete tok1;
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2->type != TK_LIT_ID) {
      throw Error("Expected property name after dot");
    }

    auto id = new Identifier{tok2};
    auto exprMember = new ExprMember{exprAccess, id};
    exprAccess = new ExprAccess{EXPR_ACCESS_EXPR_MEMBER, exprMember};

    loc1 = reader->loc;
    parseWalkWhitespace(reader);
    tok1 = lex(reader);
  }

  delete tok1;
  reader->seek(loc1);

  return exprAccess;
}

StmtExpr *parsePostStmtExpr (Reader *reader, StmtExpr *stmtExpr) {
  auto isExprAccess = stmtExpr->type == STMT_EXPR_EXPR && std::get<Expr *>(stmtExpr->body)->type == EXPR_ACCESS;
  auto loc1 = reader->loc;
  parseWalkWhitespace(reader);
  auto tok1 = lex(reader);

  if (
    tok1->type == TK_OP_AND ||
    tok1->type == TK_OP_AND_AND ||
    tok1->type == TK_OP_CARET ||
    tok1->type == TK_OP_EQ_EQ ||
    tok1->type == TK_OP_EXCL_EQ ||
    tok1->type == TK_OP_GT ||
    tok1->type == TK_OP_GT_EQ ||
    tok1->type == TK_OP_LSHIFT ||
    tok1->type == TK_OP_LT ||
    tok1->type == TK_OP_LT_EQ ||
    tok1->type == TK_OP_OR ||
    tok1->type == TK_OP_OR_OR ||
    tok1->type == TK_OP_MINUS ||
    tok1->type == TK_OP_PERCENT ||
    tok1->type == TK_OP_PLUS ||
    tok1->type == TK_OP_QN_QN ||
    tok1->type == TK_OP_RSHIFT ||
    tok1->type == TK_OP_SLASH ||
    tok1->type == TK_OP_STAR ||
    tok1->type == TK_OP_STAR_STAR
  ) {
    parseWalkWhitespace(reader);
    auto stmtExpr2 = parseStmtExpr(reader);

    if (stmtExpr2->type == STMT_EXPR_EXPR) {
      auto expr2 = std::get<Expr *>(stmtExpr2->body);

      if (expr2->type == EXPR_BINARY) {
        auto exprBinary2 = std::get<ExprBinary *>(expr2->body);

        if (tokenPrecedence(tok1) >= tokenPrecedence(exprBinary2->op) && !stmtExpr2->parenthesized) {
          auto exprBinaryLeft = new ExprBinary{stmtExpr, tok1, exprBinary2->left};
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

    auto exprBinary = new ExprBinary{stmtExpr, tok1, stmtExpr2};
    auto expr = new Expr{EXPR_BINARY, exprBinary};

    return new StmtExpr{STMT_EXPR_EXPR, expr};
  } else if (tok1->type == TK_OP_QN) {
    delete tok1;
    parseWalkWhitespace(reader);

    auto stmtExpr2 = parseStmtExpr(reader);
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2->type != TK_OP_COLON) {
      throw Error("Expected colon");
    }

    delete tok2;
    parseWalkWhitespace(reader);

    auto stmtExpr3 = parseStmtExpr(reader);
    auto exprCond = new ExprCond{stmtExpr, stmtExpr2, stmtExpr3};
    auto expr = new Expr{EXPR_COND, exprCond};

    return new StmtExpr{STMT_EXPR_EXPR, expr};
  } else if (isExprAccess && tok1->type == TK_OP_DOT) {
    reader->seek(loc1);
    delete tok1;

    auto expr = std::get<Expr *>(stmtExpr->body);
    auto exprAccess = std::get<ExprAccess *>(expr->body);
    expr->body = parseExprAccess(reader, exprAccess);
    return parsePostStmtExpr(reader, stmtExpr);
  } else if (isExprAccess && (
    tok1->type == TK_OP_AND_AND_EQ ||
    tok1->type == TK_OP_AND_EQ ||
    tok1->type == TK_OP_CARET_EQ ||
    tok1->type == TK_OP_EQ ||
    tok1->type == TK_OP_LSHIFT_EQ ||
    tok1->type == TK_OP_MINUS_EQ ||
    tok1->type == TK_OP_OR_EQ ||
    tok1->type == TK_OP_OR_OR_EQ ||
    tok1->type == TK_OP_PERCENT_EQ ||
    tok1->type == TK_OP_PLUS_EQ ||
    tok1->type == TK_OP_QN_QN_EQ ||
    tok1->type == TK_OP_RSHIFT_EQ ||
    tok1->type == TK_OP_SLASH_EQ ||
    tok1->type == TK_OP_STAR_EQ ||
    tok1->type == TK_OP_STAR_STAR_EQ
  )) {
    parseWalkWhitespace(reader);

    auto expr = std::get<Expr *>(stmtExpr->body);
    auto exprAccess = std::get<ExprAccess *>(expr->body);
    auto rightStmtExpr = parseStmtExpr(reader);
    auto exprAssign = new ExprAssign{exprAccess, tok1, rightStmtExpr};

    expr->type = EXPR_ASSIGN;
    expr->body = exprAssign;

    return parsePostStmtExpr(reader, stmtExpr);
  } else if (isExprAccess && tok1->type == TK_OP_LBRACE) {
    delete tok1;
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);
    auto props = std::vector<ExprObjProp *>();

    while (tok2->type != TK_OP_RBRACE) {
      if (tok2->type != TK_LIT_ID && props.empty()) {
        reader->seek(loc1);
        delete tok2;
        return stmtExpr;
      } else if (tok2->type != TK_LIT_ID) {
        throw Error("Expected object property name");
      }

      parseWalkWhitespace(reader);
      auto tok3 = lex(reader);

      if (tok3->type != TK_OP_COLON && props.empty()) {
        reader->seek(loc1);
        delete tok2;
        delete tok3;
        return stmtExpr;
      } else if (tok3->type != TK_OP_COLON) {
        throw Error("Expected colon after object property name");
      }

      delete tok3;
      parseWalkWhitespace(reader);

      auto init = parseStmtExpr(reader);
      auto id = new Identifier{tok2};
      auto prop = new ExprObjProp{id, init};
      props.push_back(prop);

      parseWalkWhitespace(reader);
      tok2 = lex(reader);

      if (tok2->type == TK_OP_COMMA) {
        delete tok2;
        parseWalkWhitespace(reader);
        tok2 = lex(reader);
      }
    }

    delete tok2;
    auto expr = std::get<Expr *>(stmtExpr->body);
    auto exprAccess = std::get<ExprAccess *>(expr->body);

    if (exprAccess->type != EXPR_ACCESS_IDENTIFIER) {
      throw Error("Only identifiers accepted as object names");
    }

    auto id = std::get<Identifier *>(exprAccess->body);
    auto exprObj = new ExprObj{id, props};

    expr->type = EXPR_OBJ;
    expr->body = exprObj;
    exprAccess->body = static_cast<Identifier *>(nullptr);

    delete exprAccess;
    return parsePostStmtExpr(reader, stmtExpr);
  } else if (isExprAccess && tok1->type == TK_OP_LPAR) {
    delete tok1;
    auto loc2 = reader->loc;
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);
    auto args = std::vector<ExprCallArg *>();

    while (tok2->type != TK_OP_RPAR) {
      reader->seek(loc2);

      auto argId = static_cast<Identifier *>(nullptr);
      auto loc3 = reader->loc;
      parseWalkWhitespace(reader);
      auto tok3 = lex(reader);

      if (tok3->type == TK_LIT_ID) {
        parseWalkWhitespace(reader);
        auto tok4 = lex(reader);

        if (tok4->type == TK_OP_COLON) {
          argId = new Identifier{tok3};
        }

        delete tok4;
      }

      if (argId == nullptr) {
        delete tok3;
        reader->seek(loc3);
      }

      parseWalkWhitespace(reader);
      auto argStmtExpr = parseStmtExpr(reader);

      args.push_back(new ExprCallArg{argId, argStmtExpr});
      parseWalkWhitespace(reader);

      delete tok2;
      tok2 = lex(reader);

      if (tok2->type == TK_OP_COMMA) {
        loc2 = reader->loc;
        parseWalkWhitespace(reader);

        delete tok2;
        tok2 = lex(reader);
      }
    }

    delete tok2;
    auto expr = std::get<Expr *>(stmtExpr->body);
    auto exprAccess = std::get<ExprAccess *>(expr->body);
    auto exprCall = new ExprCall{exprAccess, args};

    expr->type = EXPR_CALL;
    expr->body = exprCall;

    return parsePostStmtExpr(reader, stmtExpr);
  } else if (tok1->type == TK_OP_MINUS_MINUS || tok1->type == TK_OP_PLUS_PLUS) {
    if (stmtExpr->type != STMT_EXPR_EXPR) {
      throw Error("Only expressions can be used with increment/decrement operators");
    } else if (std::get<Expr *>(stmtExpr->body)->type != EXPR_ACCESS) {
      throw Error("Only access expressions can be used with increment/decrement operators");
    }

    auto exprUnary = new ExprUnary{stmtExpr, tok1};
    auto expr = new Expr{EXPR_UNARY, exprUnary};

    return parsePostStmtExpr(reader, new StmtExpr{STMT_EXPR_EXPR, expr});
  }

  reader->seek(loc1);
  delete tok1;

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
    auto id = new Identifier{tok1};
    auto exprAccess = new ExprAccess{EXPR_ACCESS_IDENTIFIER, id};
    auto expr = new Expr{EXPR_ACCESS, exprAccess};

    return parsePostStmtExpr(reader, new StmtExpr{STMT_EXPR_EXPR, expr});
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

Stmt *parseStmtLoopFor (Reader *reader, Stmt *stmt) {
  auto stmtExpr2 = static_cast<StmtExpr *>(nullptr);
  auto stmtExpr3 = static_cast<StmtExpr *>(nullptr);

  auto loc1 = reader->loc;
  parseWalkWhitespace(reader);
  auto tok1 = lex(reader);

  if (tok1->type != TK_OP_SEMI) {
    reader->seek(loc1);
    parseWalkWhitespace(reader);

    stmtExpr2 = parseStmtExpr(reader);
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2->type != TK_OP_SEMI) {
      throw Error("Expected semicolon after loop condition");
    }

    delete tok2;
  }

  delete tok1;

  auto loc3 = reader->loc;
  parseWalkWhitespace(reader);
  auto tok3 = lex(reader);

  reader->seek(loc3);

  if (tok3->type != TK_OP_LBRACE) {
    parseWalkWhitespace(reader);
    stmtExpr3 = parseStmtExpr(reader);
  }

  delete tok3;

  auto block = parseBlock(reader);
  auto stmtLoop = new StmtLoop{stmt, stmtExpr2, stmtExpr3, block};

  return new Stmt{STMT_LOOP, stmtLoop};
}

Stmt *parse (Reader *reader) {
  parseWalkWhitespace(reader);

  if (reader->eof()) {
    auto stmtEnd = new StmtEnd{};
    return new Stmt{STMT_END, stmtEnd};
  }

  auto loc1 = reader->loc;
  auto tok1 = lex(reader);

  if (tok1->type == TK_KW_BREAK) {
    delete tok1;
    auto stmtBreak = new StmtBreak{};

    return new Stmt{STMT_BREAK, stmtBreak};
  } else if (tok1->type == TK_KW_CONTINUE) {
    delete tok1;
    auto stmtContinue = new StmtContinue{};

    return new Stmt{STMT_CONTINUE, stmtContinue};
  } else if (tok1->type == TK_KW_FN) {
    delete tok1;
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2->type != TK_LIT_ID) {
      throw Error("Expected function identifier");
    }

    parseWalkWhitespace(reader);
    auto tok3 = lex(reader);

    if (tok3->type != TK_OP_LPAR) {
      throw Error("Expected left parentheses");
    }

    delete tok3;
    parseWalkWhitespace(reader);
    auto tok4 = lex(reader);
    auto params = std::vector<StmtFnDeclParam *>();

    while (tok4->type != TK_OP_RPAR) {
      if (tok4->type != TK_LIT_ID) {
        throw Error("Expected identifier as function parameter name");
      }

      parseWalkWhitespace(reader);
      auto tok5 = lex(reader);
      auto type = static_cast<Identifier *>(nullptr);
      auto init = static_cast<StmtExpr *>(nullptr);

      if (tok5->type == TK_OP_COLON) {
        parseWalkWhitespace(reader);
        auto tok6 = lex(reader);

        if (tok6->type != TK_LIT_ID) {
          throw Error("Expected function parameter type after colon");
        }

        type = new Identifier{tok6};

        auto loc7 = reader->loc;
        parseWalkWhitespace(reader);
        auto tok7 = lex(reader);

        if (tok7->type == TK_OP_EQ) {
          parseWalkWhitespace(reader);
          init = parseStmtExpr(reader);
        } else {
          reader->seek(loc7);
        }

        delete tok7;
      } else if (tok5->type == TK_OP_COLON_EQ) {
        parseWalkWhitespace(reader);
        init = parseStmtExpr(reader);
      } else {
        throw Error("Expected function parameter type after parameter name");
      }

      delete tok5;
      auto paramId = new Identifier{tok4};
      auto param = new StmtFnDeclParam{paramId, type, init};
      params.push_back(param);

      parseWalkWhitespace(reader);
      tok4 = lex(reader);

      if (tok4->type == TK_OP_COMMA) {
        delete tok4;
        parseWalkWhitespace(reader);
        tok4 = lex(reader);
      }
    }

    delete tok4;
    parseWalkWhitespace(reader);
    auto tok8 = lex(reader);

    if (tok8->type != TK_LIT_ID) {
      throw Error("Expected function return type after parameters list");
    }

    auto block = parseBlock(reader);
    auto id = new Identifier{tok2};
    auto returnType = new Identifier{tok8};
    auto stmtFnDecl = new StmtFnDecl{id, params, returnType, block};

    return new Stmt{STMT_FN_DECL, stmtFnDecl};
  } else if (tok1->type == TK_KW_IF) {
    delete tok1;
    parseWalkWhitespace(reader);

    auto stmtExpr = parseStmtExpr(reader);
    auto block = parseBlock(reader);
    auto alt = static_cast<Cond *>(nullptr);
    auto altTail = static_cast<Cond *>(nullptr);

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
        auto condElif = new Cond{COND_STMT_IF, stmtElif};

        if (alt == nullptr && altTail == nullptr) {
          alt = condElif;
          altTail = condElif;
        } else if (altTail != nullptr) {
          std::get<StmtIf *>(altTail->body)->alt = condElif;
          altTail = condElif;
        }

        continue;
      } else if (tok2->type == TK_KW_ELSE) {
        parseWalkWhitespace(reader);

        auto blockElse = parseBlock(reader);
        auto condElse = new Cond{COND_BLOCK, blockElse};

        if (alt == nullptr && altTail == nullptr) {
          alt = condElse;
        } else if (altTail != nullptr) {
          std::get<StmtIf *>(altTail->body)->alt = condElse;
        }
      } else {
        reader->seek(loc2);
      }

      delete tok2;
      break;
    }

    auto stmtIf = new StmtIf{stmtExpr, block, alt};
    return new Stmt{STMT_IF, stmtIf};
  } else if (tok1->type == TK_KW_LOOP) {
    delete tok1;

    auto loc2 = reader->loc;
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2->type == TK_OP_LBRACE) {
      delete tok2;
      reader->seek(loc2);

      auto block = parseBlock(reader);
      auto stmtLoop = new StmtLoop{nullptr, nullptr, nullptr, block};

      return new Stmt{STMT_LOOP, stmtLoop};
    } else if (tok2->type == TK_OP_SEMI) {
      delete tok2;
      return parseStmtLoopFor(reader, nullptr);
    } else {
      delete tok2;
      reader->seek(loc2);

      auto stmt = parse(reader);

      if (stmt->type != STMT_EXPR && stmt->type != STMT_SHORT_VAR_DECL) {
        throw Error("Unexpected statement in loop condition");
      }

      auto loc3 = reader->loc;
      parseWalkWhitespace(reader);
      auto tok3 = lex(reader);

      if (stmt->type == STMT_SHORT_VAR_DECL && tok3->type != TK_OP_SEMI) {
        throw Error("Expected semicolon after loop initialization");
      } else if (stmt->type == STMT_EXPR && tok3->type != TK_OP_LBRACE && tok3->type != TK_OP_SEMI) {
        throw Error("Unexpected token after loop initialization");
      } else if (stmt->type == STMT_EXPR && tok3->type == TK_OP_LBRACE) {
        delete tok3;
        reader->seek(loc3);

        auto stmtExpr = std::get<StmtExpr *>(stmt->body);
        auto block = parseBlock(reader);
        auto stmtLoop = new StmtLoop{nullptr, stmtExpr, nullptr, block};

        stmt->body = static_cast<StmtExpr *>(nullptr);
        delete stmt;

        return new Stmt{STMT_LOOP, stmtLoop};
      }

      delete tok3;
      return parseStmtLoopFor(reader, stmt);
    }
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
        auto id = new Identifier{tok2};
        auto stmtShortVarDecl = new StmtShortVarDecl{id, stmtExpr, true};

        delete tok1;
        delete tok3;

        return new Stmt{STMT_SHORT_VAR_DECL, stmtShortVarDecl};
      }

      delete tok3;
    }

    delete tok2;
  } else if (tok1->type == TK_KW_OBJ) {
    delete tok1;
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2->type != TK_LIT_ID) {
      throw Error("Expected object identifier");
    }

    parseWalkWhitespace(reader);
    auto tok3 = lex(reader);

    if (tok3->type != TK_OP_LBRACE) {
      throw Error("Expected left brace after object identifier");
    }

    delete tok3;
    parseWalkWhitespace(reader);
    auto tok4 = lex(reader);
    auto fields = std::vector<StmtObjDeclField *>();

    while (tok4->type != TK_OP_RBRACE) {
      if (tok4->type != TK_LIT_ID) {
        throw Error("Expected object field name");
      }

      parseWalkWhitespace(reader);
      auto tok5 = lex(reader);

      if (tok5->type != TK_OP_COLON) {
        throw Error("Expected colon after object field name");
      }

      delete tok5;
      parseWalkWhitespace(reader);
      auto tok6 = lex(reader);

      if (tok6->type != TK_LIT_ID) {
        throw Error("Expected object field type after field name");
      }

      auto fieldId = new Identifier{tok4};
      auto fieldType = new Identifier{tok6};
      auto field = new StmtObjDeclField{fieldId, fieldType};
      fields.push_back(field);

      parseWalkWhitespace(reader);
      tok4 = lex(reader);

      if (tok4->type == TK_OP_COMMA) {
        delete tok4;
        parseWalkWhitespace(reader);
        tok4 = lex(reader);
      }
    }

    delete tok4;

    if (fields.empty()) {
      throw Error("Object declaration without fields is not allowed");
    }

    auto id = new Identifier{tok2};
    auto stmtObjDecl = new StmtObjDecl{id, fields};

    return new Stmt{STMT_OBJ_DECL, stmtObjDecl};
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
      auto id = new Identifier{tok1};
      auto stmtShortVarDecl = new StmtShortVarDecl{id, stmtExpr};

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
