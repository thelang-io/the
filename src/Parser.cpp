/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Parser.hpp"

Parser::Parser (Lexer *l) {
  this->lexer = l;
}

ParserStmt Parser::next () {
  this->lexer->whitespace();

  auto loc0 = this->lexer->loc;
  auto tok0 = this->lexer->next();

  if (tok0.type == TK_EOF) {
    return this->_stmt(ParserStmtEof{}, loc0, this->lexer->loc);
  } else if (tok0.type == TK_KW_BREAK) {
    return this->_stmt(ParserStmtBreak{}, loc0, this->lexer->loc);
  } else if (tok0.type == TK_KW_CONTINUE) {
    return this->_stmt(ParserStmtContinue{}, loc0, this->lexer->loc);
  } else if (tok0.type == TK_KW_IF) {
    auto stmtIf = this->_stmtIf();
    return this->_stmt(stmtIf, loc0, this->lexer->loc);
  } else if (tok0.type == TK_KW_MAIN) {
    auto mainBody = this->_block();
    return this->_stmt(ParserStmtMain{mainBody}, loc0, this->lexer->loc);
  } else if (tok0.type == TK_KW_RETURN) {
    auto loc1 = this->lexer->loc;
    auto returnBody = std::optional<ParserStmtExpr>{};

    try {
      returnBody = this->_stmtExpr();
    } catch (const Error &err) {
      this->lexer->seek(loc1);
    }

    return this->_stmt(ParserStmtReturn{returnBody}, loc0, this->lexer->loc);
  }

  if (tok0.type == TK_KW_FN) {
    auto loc1 = this->lexer->loc;
    auto tok1 = this->lexer->next();

    if (tok1.type != TK_ID) {
      throw Error(this->lexer->reader, loc1, E0115);
    }

    auto loc2 = this->lexer->loc;
    auto tok2 = this->lexer->next();

    if (tok2.type != TK_OP_LPAR) {
      throw Error(this->lexer->reader, loc2, E0116);
    }

    auto loc3 = this->lexer->loc;
    auto tok3 = this->lexer->next();
    auto fnDeclParams = std::vector<ParserStmtFnDeclParam>();

    while (tok3.type != TK_OP_RPAR) {
      if (tok3.type != TK_ID) {
        // TODO Test change of loc3
        throw Error(this->lexer->reader, loc3, E0117);
      }

      auto fnDeclParamType = std::optional<Token>{};
      auto fnDeclParamInit = std::optional<ParserStmtExpr>{};
      auto loc4 = this->lexer->loc;
      auto tok4 = this->lexer->next();

      if (tok4.type == TK_OP_COLON) {
        fnDeclParamType = this->lexer->next();

        if (fnDeclParamType->type != TK_ID) {
          throw Error(this->lexer->reader, loc4, E0118);
        }

        auto loc5 = this->lexer->loc;
        auto tok5 = this->lexer->next();

        if (tok5.type == TK_OP_EQ) {
          fnDeclParamInit = this->_stmtExpr();
        } else {
          this->lexer->seek(loc5);
        }
      } else if (tok4.type == TK_OP_COLON_EQ) {
        fnDeclParamInit = this->_stmtExpr();
      } else {
        throw Error(this->lexer->reader, loc4, E0119);
      }

      fnDeclParams.push_back(ParserStmtFnDeclParam{tok3, fnDeclParamType, fnDeclParamInit});

      loc3 = this->lexer->loc;
      tok3 = this->lexer->next();

      if (tok3.type == TK_OP_COMMA) {
        loc3 = this->lexer->loc;
        tok3 = this->lexer->next();
      }
    }

    auto loc6 = this->lexer->loc;
    auto tok6 = this->lexer->next();

    if (tok6.type != TK_ID) {
      throw Error(this->lexer->reader, loc6, E0120);
    }

    auto fnDeclBody = this->_block();
    return this->_stmt(ParserStmtFnDecl{tok1, fnDeclParams, tok6, fnDeclBody}, loc0, this->lexer->loc);
  }

  if (tok0.type == TK_KW_LOOP) {
    auto loc1 = this->lexer->loc;
    auto tok1 = this->lexer->next();

    if (tok1.type == TK_OP_LBRACE) {
      this->lexer->seek(loc1);

      auto loopBody = this->_block();
      return this->_stmt(ParserStmtLoop{std::nullopt, std::nullopt, std::nullopt, loopBody}, loc0, this->lexer->loc);
    } else if (tok1.type == TK_OP_SEMI) {
      auto stmtLoop = this->_stmtLoop(std::nullopt);
      return this->_stmt(stmtLoop, loc0, this->lexer->loc);
    }

    this->lexer->seek(loc1);
    auto loopInit = this->next();

    if (!std::holds_alternative<ParserStmtExpr>(loopInit.body) && !std::holds_alternative<ParserStmtVarDecl>(loopInit.body)) {
      throw Error(this->lexer->reader, loc1, E0105);
    }

    auto loc2 = this->lexer->loc;
    auto tok2 = this->lexer->next();

    if (std::holds_alternative<ParserStmtVarDecl>(loopInit.body) && tok2.type != TK_OP_SEMI) {
      throw Error(this->lexer->reader, loc1, E0106);
    } else if (std::holds_alternative<ParserStmtExpr>(loopInit.body) && tok2.type != TK_OP_LBRACE && tok2.type != TK_OP_SEMI) {
      throw Error(this->lexer->reader, loc1, E0107);
    } else if (std::holds_alternative<ParserStmtExpr>(loopInit.body) && tok2.type == TK_OP_LBRACE) {
      this->lexer->seek(loc2);

      auto loopCond = std::get<ParserStmtExpr>(loopInit.body);
      auto loopBody = this->_block();

      return this->_stmt(ParserStmtLoop{std::nullopt, loopCond, std::nullopt, loopBody}, loc0, this->lexer->loc);
    }

    auto stmtLoop = this->_stmtLoop(std::make_shared<ParserStmt>(loopInit));
    return this->_stmt(stmtLoop, loc0, this->lexer->loc);
  }

  if (tok0.type == TK_KW_MUT) {
    auto loc1 = this->lexer->loc;
    auto tok1 = this->lexer->next();

    if (tok1.type == TK_ID) {
      auto tok2 = this->lexer->next();

      if (tok2.type == TK_OP_COLON) {
        auto loc3 = this->lexer->loc;
        auto tok3 = this->lexer->next();

        if (tok3.type != TK_ID) {
          throw Error(this->lexer->reader, loc3, E0102);
        }

        auto loc5 = this->lexer->loc;
        auto tok5 = this->lexer->next();
        auto varDeclInit = std::optional<ParserStmtExpr>{};

        if (tok5.type == TK_OP_EQ) {
          varDeclInit = this->_stmtExpr();
        } else {
          this->lexer->seek(loc5);
        }

        return this->_stmt(ParserStmtVarDecl{tok1, tok3, varDeclInit, true}, loc0, this->lexer->loc);
      } else if (tok2.type == TK_OP_COLON_EQ) {
        auto varDeclInit = this->_stmtExpr();
        return this->_stmt(ParserStmtVarDecl{tok1, std::nullopt, varDeclInit, true}, loc0, this->lexer->loc);
      }
    }

    this->lexer->seek(loc1);
  }

  if (tok0.type == TK_KW_OBJ) {
    auto loc1 = this->lexer->loc;
    auto tok1 = this->lexer->next();

    if (tok1.type != TK_ID) {
      throw Error(this->lexer->reader, loc1, E0121);
    }

    auto loc2 = this->lexer->loc;
    auto tok2 = this->lexer->next();

    if (tok2.type != TK_OP_LBRACE) {
      throw Error(this->lexer->reader, loc2, E0122);
    }

    auto loc3 = this->lexer->loc;
    auto tok3 = this->lexer->next();
    auto objDeclFields = std::vector<ParserStmtObjDeclField>();

    while (tok3.type != TK_OP_RBRACE) {
      if (tok3.type != TK_ID) {
        throw Error(this->lexer->reader, loc3, E0123);
      }

      auto loc4 = this->lexer->loc;
      auto tok4 = this->lexer->next();

      if (tok4.type != TK_OP_COLON) {
        throw Error(this->lexer->reader, loc4, E0124);
      }

      auto loc5 = this->lexer->loc;
      auto tok5 = this->lexer->next();

      if (tok5.type != TK_ID) {
        throw Error(this->lexer->reader, loc5, E0125);
      }

      objDeclFields.push_back(ParserStmtObjDeclField{tok3, tok5});

      loc3 = this->lexer->loc;
      tok3 = this->lexer->next();
    }

    if (objDeclFields.empty()) {
      throw Error(this->lexer->reader, loc0, E0126);
    }

    return this->_stmt(ParserStmtObjDecl{tok1, objDeclFields}, loc0, this->lexer->loc);
  }

  if (tok0.type == TK_ID) {
    auto loc1 = this->lexer->loc;
    auto tok1 = this->lexer->next();

    if (tok1.type == TK_OP_COLON) {
      auto loc2 = this->lexer->loc;
      auto tok2 = this->lexer->next();

      if (tok2.type != TK_ID) {
        throw Error(this->lexer->reader, loc2, E0102);
      }

      auto loc3 = this->lexer->loc;
      auto tok3 = this->lexer->next();
      auto varDeclInit = std::optional<ParserStmtExpr>{};

      if (tok3.type == TK_OP_EQ) {
        varDeclInit = this->_stmtExpr();
      } else {
        this->lexer->seek(loc3);
      }

      return this->_stmt(ParserStmtVarDecl{tok0, tok2, varDeclInit}, loc0, this->lexer->loc);
    } else if (tok1.type == TK_OP_COLON_EQ) {
      auto varDeclInit = this->_stmtExpr();
      return this->_stmt(ParserStmtVarDecl{tok0, std::nullopt, varDeclInit}, loc0, this->lexer->loc);
    }

    this->lexer->seek(loc1);
  }

  this->lexer->seek(loc0);

  try {
    auto stmtExpr = this->_stmtExpr();
    return this->_stmt(stmtExpr, loc0, this->lexer->loc);
  } catch (const Error &err) {
    this->lexer->seek(loc0);
  }

  throw Error(this->lexer->reader, loc0, E0100);
}

ParserBlock Parser::_block () {
  auto loc1 = this->lexer->loc;
  auto tok1 = this->lexer->next();

  if (tok1.type != TK_OP_LBRACE) {
    throw Error(this->lexer->reader, loc1, E0103);
  }

  auto block = ParserBlock{};

  while (true) {
    auto loc2 = this->lexer->loc;
    auto tok2 = this->lexer->next();

    if (tok2.type == TK_EOF) {
      throw Error(this->lexer->reader, loc2, E0104);
    } else if (tok2.type == TK_OP_RBRACE) {
      break;
    }

    this->lexer->seek(loc2);

    auto stmt = this->next();
    block.push_back(stmt);
  }

  return block;
}

ParserStmt Parser::_stmt (const ParserStmtBody &body, ReaderLocation start, ReaderLocation end) const {
  return ParserStmt{body, start, end};
}

ParserStmtExpr Parser::_stmtExpr (bool singleStmt) {
  auto loc1 = this->lexer->loc;
  auto tok1 = this->lexer->next();

  if (
    tok1.type == TK_KW_FALSE ||
    tok1.type == TK_KW_TRUE ||
    tok1.type == TK_LIT_CHAR ||
    tok1.type == TK_LIT_FLOAT ||
    tok1.type == TK_LIT_INT_BIN ||
    tok1.type == TK_LIT_INT_DEC ||
    tok1.type == TK_LIT_INT_HEX ||
    tok1.type == TK_LIT_INT_OCT ||
    tok1.type == TK_LIT_STR
  ) {
    auto exprLit = std::make_shared<ParserExpr>(ParserExprLit{tok1});
    auto stmtExpr = ParserStmtExpr{exprLit};

    return singleStmt ? stmtExpr : this->_wrapStmtExpr(stmtExpr);
  }

  if (
    tok1.type == TK_OP_EXCL ||
    tok1.type == TK_OP_EXCL_EXCL ||
    tok1.type == TK_OP_MINUS ||
    tok1.type == TK_OP_MINUS_MINUS ||
    tok1.type == TK_OP_PLUS ||
    tok1.type == TK_OP_PLUS_PLUS ||
    tok1.type == TK_OP_TILDE
  ) {
    auto exprUnaryArg = this->_stmtExpr(true);
    auto exprUnary = std::make_shared<ParserExpr>(ParserExprUnary{exprUnaryArg, tok1, true});
    auto stmtExpr = ParserStmtExpr{exprUnary};

    return singleStmt ? stmtExpr : this->_wrapStmtExpr(stmtExpr);
  }

  if (tok1.type == TK_ID) {
    auto exprAccess = ParserExprAccess{tok1};
    auto loc2 = this->lexer->loc;
    auto tok2 = this->lexer->next();

    while (tok2.type == TK_OP_DOT) {
      auto loc3 = this->lexer->loc;
      auto tok3 = this->lexer->next();

      if (tok3.type != TK_ID) {
        throw Error(this->lexer->reader, loc3, E0110);
      }

      auto member = ParserMember{std::make_shared<std::variant<Token, ParserMember>>(exprAccess.body), tok3};
      exprAccess = ParserExprAccess{member};

      loc2 = this->lexer->loc;
      tok2 = this->lexer->next();
    }

    this->lexer->seek(loc2);

    auto loc4 = this->lexer->loc;
    auto tok4 = this->lexer->next();

    if (
      tok4.type == TK_OP_AND_AND_EQ ||
      tok4.type == TK_OP_AND_EQ ||
      tok4.type == TK_OP_CARET_EQ ||
      tok4.type == TK_OP_EQ ||
      tok4.type == TK_OP_LSHIFT_EQ ||
      tok4.type == TK_OP_MINUS_EQ ||
      tok4.type == TK_OP_OR_EQ ||
      tok4.type == TK_OP_OR_OR_EQ ||
      tok4.type == TK_OP_PERCENT_EQ ||
      tok4.type == TK_OP_PLUS_EQ ||
      tok4.type == TK_OP_QN_QN_EQ ||
      tok4.type == TK_OP_RSHIFT_EQ ||
      tok4.type == TK_OP_SLASH_EQ ||
      tok4.type == TK_OP_STAR_EQ ||
      tok4.type == TK_OP_STAR_STAR_EQ
    ) {
      auto exprAssignRight = this->_stmtExpr();
      auto exprAssign = std::make_shared<ParserExpr>(ParserExprAssign{exprAccess, tok4, exprAssignRight});
      auto stmtExpr = ParserStmtExpr{exprAssign};

      return singleStmt ? stmtExpr : this->_wrapStmtExpr(stmtExpr);
    }

    if (tok4.type == TK_OP_LBRACE) {
      auto loc5 = this->lexer->loc;
      auto tok5 = this->lexer->next();
      auto exprObjProps = std::vector<ParserExprObjProp>();

      while (tok5.type != TK_OP_RBRACE) {
        if (tok5.type != TK_ID && exprObjProps.empty()) {
          this->lexer->seek(loc4);

          auto expr = std::make_shared<ParserExpr>(exprAccess);
          auto stmtExpr = ParserStmtExpr{expr};

          return singleStmt ? stmtExpr : this->_wrapStmtExpr(stmtExpr);
        } else if (tok5.type != TK_ID) {
          throw Error(this->lexer->reader, loc5, E0112);
        }

        auto loc6 = this->lexer->loc;
        auto tok6 = this->lexer->next();

        if (tok6.type != TK_OP_COLON && exprObjProps.empty()) {
          this->lexer->seek(loc4);

          auto expr = std::make_shared<ParserExpr>(exprAccess);
          auto stmtExpr = ParserStmtExpr{expr};

          return singleStmt ? stmtExpr : this->_wrapStmtExpr(stmtExpr);
        } else if (tok6.type != TK_OP_COLON) {
          throw Error(this->lexer->reader, loc6, E0113);
        }

        auto exprObjPropInit = this->_stmtExpr();
        exprObjProps.push_back(ParserExprObjProp{tok5, exprObjPropInit});

        loc5 = this->lexer->loc;
        tok5 = this->lexer->next();

        if (tok5.type == TK_OP_COMMA) {
          loc5 = this->lexer->loc;
          tok5 = this->lexer->next();
        }
      }

      if (!std::holds_alternative<Token>(exprAccess.body)) {
        throw Error(this->lexer->reader, loc1, E0114);
      }

      auto exprObj = std::make_shared<ParserExpr>(ParserExprObj{std::get<Token>(exprAccess.body), exprObjProps});
      auto stmtExpr = ParserStmtExpr{exprObj};

      return singleStmt ? stmtExpr : this->_wrapStmtExpr(stmtExpr);
    }

    if (tok4.type == TK_OP_LPAR) {
      auto loc5 = this->lexer->loc;
      auto tok5 = this->lexer->next();
      auto exprCallArgs = std::vector<ParserExprCallArg>();

      while (tok5.type != TK_OP_RPAR) {
        this->lexer->seek(loc5);

        auto exprCallArgId = std::optional<Token>{};
        auto loc6 = this->lexer->loc;
        auto tok6 = this->lexer->next();

        if (tok6.type == TK_ID) {
          auto tok7 = this->lexer->next();

          if (tok7.type == TK_OP_COLON) {
            exprCallArgId = tok6;
          }
        }

        if (exprCallArgId == std::nullopt) {
          this->lexer->seek(loc6);
        }

        auto exprCallArgExpr = this->_stmtExpr();
        exprCallArgs.push_back(ParserExprCallArg{exprCallArgId, exprCallArgExpr});

        loc5 = this->lexer->loc;
        tok5 = this->lexer->next();

        if (tok5.type == TK_OP_COMMA) {
          loc5 = this->lexer->loc;
          tok5 = this->lexer->next();
        }
      }

      auto exprCall = std::make_shared<ParserExpr>(ParserExprCall{exprAccess, exprCallArgs});
      auto stmtExpr = ParserStmtExpr{exprCall};

      return singleStmt ? stmtExpr : this->_wrapStmtExpr(stmtExpr);
    }

    if (tok4.type == TK_OP_MINUS_MINUS || tok4.type == TK_OP_PLUS_PLUS) {
      auto exprUnaryArgExpr = std::make_shared<ParserExpr>(exprAccess);
      auto exprUnaryArg = ParserStmtExpr{exprUnaryArgExpr};
      auto exprUnary = std::make_shared<ParserExpr>(ParserExprUnary{exprUnaryArg, tok4});
      auto stmtExpr = ParserStmtExpr{exprUnary};

      return singleStmt ? stmtExpr : this->_wrapStmtExpr(stmtExpr);
    }

    this->lexer->seek(loc4);

    auto expr = std::make_shared<ParserExpr>(exprAccess);
    auto stmtExpr = ParserStmtExpr{expr};

    return singleStmt ? stmtExpr : this->_wrapStmtExpr(stmtExpr);
  }

  if (tok1.type == TK_OP_LPAR) {
    auto stmtExpr = this->_stmtExpr();
    auto loc2 = this->lexer->loc;
    auto tok2 = this->lexer->next();

    if (tok2.type != TK_OP_RPAR) {
      throw Error(this->lexer->reader, loc2, E0109);
    }

    stmtExpr.parenthesized = true;
    return singleStmt ? stmtExpr : this->_wrapStmtExpr(stmtExpr);
  }

  throw Error(this->lexer->reader, loc1, E0101);
}

ParserStmtIf Parser::_stmtIf () {
  auto cond = std::make_shared<ParserStmt>(this->next());
  auto body = this->_block();
  auto alt = std::optional<std::shared_ptr<ParserStmtIfCond>>{};
  auto loc1 = this->lexer->loc;
  auto tok1 = this->lexer->next();

  if (tok1.type == TK_KW_ELIF) {
    auto stmtElifCond = this->_stmtIf();
    alt = std::make_shared<ParserStmtIfCond>(stmtElifCond);
  } else if (tok1.type == TK_KW_ELSE) {
    auto stmtElseCond = this->_block();
    alt = std::make_shared<ParserStmtIfCond>(stmtElseCond);
  } else {
    this->lexer->seek(loc1);
  }

  return ParserStmtIf{cond, body, alt};
}

ParserStmtLoop Parser::_stmtLoop (const std::optional<std::shared_ptr<ParserStmt>> &init) {
  auto cond = std::optional<ParserStmtExpr>{};
  auto upd = std::optional<ParserStmtExpr>{};
  auto loc1 = this->lexer->loc;
  auto tok1 = this->lexer->next();

  if (tok1.type != TK_OP_SEMI) {
    this->lexer->seek(loc1);
    cond = this->_stmtExpr();

    auto loc2 = this->lexer->loc;
    auto tok2 = this->lexer->next();

    if (tok2.type != TK_OP_SEMI) {
      throw Error(this->lexer->reader, loc2, E0108);
    }
  }

  auto loc3 = this->lexer->loc;
  auto tok3 = this->lexer->next();

  this->lexer->seek(loc3);

  if (tok3.type != TK_OP_LBRACE) {
    upd = this->_stmtExpr();
  }

  auto body = this->_block();
  return ParserStmtLoop{init, cond, upd, body};
}

ParserStmtExpr Parser::_wrapStmtExpr (const ParserStmtExpr &stmtExpr) {
  auto loc1 = this->lexer->loc;
  auto tok1 = this->lexer->next();

  if (
    tok1.type == TK_OP_AND ||
    tok1.type == TK_OP_AND_AND ||
    tok1.type == TK_OP_CARET ||
    tok1.type == TK_OP_EQ_EQ ||
    tok1.type == TK_OP_EXCL_EQ ||
    tok1.type == TK_OP_GT ||
    tok1.type == TK_OP_GT_EQ ||
    tok1.type == TK_OP_LSHIFT ||
    tok1.type == TK_OP_LT ||
    tok1.type == TK_OP_LT_EQ ||
    tok1.type == TK_OP_MINUS ||
    tok1.type == TK_OP_OR ||
    tok1.type == TK_OP_OR_OR ||
    tok1.type == TK_OP_PERCENT ||
    tok1.type == TK_OP_PLUS ||
    tok1.type == TK_OP_QN_QN ||
    tok1.type == TK_OP_RSHIFT ||
    tok1.type == TK_OP_SLASH ||
    tok1.type == TK_OP_STAR ||
    tok1.type == TK_OP_STAR_STAR
  ) {
    auto stmtExprRight = this->_stmtExpr(true);

    if (std::holds_alternative<ParserExprBinary>(*stmtExpr.body) && !stmtExpr.parenthesized) {
      auto exprBinaryLeft = std::get<ParserExprBinary>(*stmtExpr.body);

      if (exprBinaryLeft.op.precedence() < tok1.precedence()) {
        auto newExprBinaryRight = std::make_shared<ParserExpr>(ParserExprBinary{exprBinaryLeft.right, tok1, stmtExprRight});
        auto newStmtExprRight = ParserStmtExpr{newExprBinaryRight};
        auto exprBinary = std::make_shared<ParserExpr>(ParserExprBinary{exprBinaryLeft.left, exprBinaryLeft.op, newStmtExprRight});
        auto newStmtExpr = ParserStmtExpr{exprBinary};

        return this->_wrapStmtExpr(newStmtExpr);
      }
    }

    auto exprBinary = std::make_shared<ParserExpr>(ParserExprBinary{stmtExpr, tok1, stmtExprRight});
    auto newStmtExpr = ParserStmtExpr{exprBinary};

    return this->_wrapStmtExpr(newStmtExpr);
  }

  if (tok1.type == TK_OP_QN) {
    auto exprCondBody = this->_stmtExpr();
    auto loc2 = this->lexer->loc;
    auto tok2 = this->lexer->next();

    if (tok2.type != TK_OP_COLON) {
      throw Error(this->lexer->reader, loc2, E0111);
    }

    auto exprCondAlt = this->_stmtExpr();
    auto exprCond = std::make_shared<ParserExpr>(ParserExprCond{stmtExpr, exprCondBody, exprCondAlt});
    auto newStmtExpr = ParserStmtExpr{exprCond};

    return this->_wrapStmtExpr(newStmtExpr);
  }

  this->lexer->seek(loc1);
  return stmtExpr;
}
