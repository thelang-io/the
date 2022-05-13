/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Parser.hpp"
#include "config.hpp"

Parser::Parser (Lexer *l) {
  this->lexer = l;
  this->reader = this->lexer->reader;
}

ParserStmt Parser::next () {
  auto [_0, tok0] = this->lexer->next();

  if (tok0.type == TK_EOF) {
    return this->_wrapStmt(ParserStmtEof{}, tok0.start);
  } else if (tok0.type == TK_KW_BREAK) {
    return this->_wrapStmt(ParserStmtBreak{}, tok0.start);
  } else if (tok0.type == TK_KW_CONTINUE) {
    return this->_wrapStmt(ParserStmtContinue{}, tok0.start);
  } else if (tok0.type == TK_KW_IF) {
    auto stmtIf = this->_stmtIf();
    return this->_wrapStmt(stmtIf, tok0.start);
  } else if (tok0.type == TK_KW_MAIN) {
    auto mainBody = this->_block();
    return this->_wrapStmt(ParserStmtMain{mainBody}, tok0.start);
  } else if (tok0.type == TK_KW_RETURN) {
    auto stmtExprTest = this->_stmtExpr();
    auto returnBody = stmtExprTest == std::nullopt ? std::optional<ParserStmtExpr>{} : stmtExprTest;

    return this->_wrapStmt(ParserStmtReturn{returnBody}, tok0.start);
  }

  if (tok0.type == TK_KW_FN) {
    auto [_1, tok1] = this->lexer->next();

    if (tok1.type != TK_ID) {
      throw Error(this->reader, tok1.start, E0115);
    }

    auto [_2, tok2] = this->lexer->next();

    if (tok2.type != TK_OP_LPAR) {
      throw Error(this->reader, tok2.start, E0116);
    }

    auto [_3, tok3] = this->lexer->next();
    auto fnDeclParams = std::vector<ParserStmtFnDeclParam>{};

    while (tok3.type != TK_OP_RPAR) {
      if (tok3.type != TK_ID) {
        throw Error(this->reader, tok3.start, E0117);
      }

      auto fnDeclParamType = std::optional<ParserType>{};
      auto fnDeclParamVariadic = false;
      auto fnDeclParamInit = std::optional<ParserStmtExpr>{};
      auto [_4, tok4] = this->lexer->next();

      if (tok4.type == TK_OP_COLON) {
        auto typeTest1 = this->_type();

        if (typeTest1 == std::nullopt) {
          throw Error(this->reader, this->lexer->loc, E0118);
        }

        fnDeclParamType = typeTest1;
        auto [loc5, tok5] = this->lexer->next();

        if (tok5.type == TK_OP_EQ) {
          fnDeclParamInit = this->_stmtExpr();

          if (fnDeclParamInit == std::nullopt) {
            throw Error(this->reader, this->lexer->loc, E0130);
          }
        } else if (tok5.type == TK_OP_DOT_DOT_DOT) {
          fnDeclParamVariadic = true;
          auto [loc6, tok6] = this->lexer->next();

          if (tok6.type == TK_OP_EQ) {
            this->_stmtExpr();
            throw Error(this->reader, tok6.start, E0128);
          } else {
            this->lexer->seek(loc6);
          }
        } else {
          this->lexer->seek(loc5);
        }
      } else if (tok4.type == TK_OP_COLON_EQ) {
        fnDeclParamInit = this->_stmtExpr();

        if (fnDeclParamInit == std::nullopt) {
          throw Error(this->reader, this->lexer->loc, E0130);
        }
      } else {
        throw Error(this->reader, tok4.start, E0119);
      }

      fnDeclParams.push_back(ParserStmtFnDeclParam{tok3, fnDeclParamType, fnDeclParamVariadic, fnDeclParamInit});
      std::tie(_3, tok3) = this->lexer->next();

      if (tok3.type == TK_OP_COMMA) {
        std::tie(_3, tok3) = this->lexer->next();
      }
    }

    auto typeTest2 = this->_type();
    auto fnDeclReturnType = typeTest2 == std::nullopt ? std::optional<ParserType>{} : typeTest2;
    auto fnDeclBody = this->_block();

    return this->_wrapStmt(ParserStmtFnDecl{tok1, fnDeclParams, fnDeclReturnType, fnDeclBody}, tok0.start);
  }

  if (tok0.type == TK_KW_LOOP) {
    auto [loc1, tok1] = this->lexer->next();

    if (tok1.type == TK_OP_LBRACE) {
      this->lexer->seek(loc1);
      auto loopBody = this->_block();

      return this->_wrapStmt(ParserStmtLoop{std::nullopt, std::nullopt, std::nullopt, loopBody}, tok0.start);
    } else if (tok1.type == TK_OP_SEMI) {
      auto stmtLoop = this->_stmtLoop(std::nullopt);
      return this->_wrapStmt(stmtLoop, tok0.start);
    }

    this->lexer->seek(loc1);
    auto loopInit = this->next();

    if (!std::holds_alternative<ParserStmtExpr>(*loopInit.body) && !std::holds_alternative<ParserStmtVarDecl>(*loopInit.body)) {
      throw Error(this->reader, tok1.start, E0105);
    }

    auto [loc2, tok2] = this->lexer->next();

    if (std::holds_alternative<ParserStmtVarDecl>(*loopInit.body) && tok2.type != TK_OP_SEMI) {
      throw Error(this->reader, tok2.start, E0106);
    } else if (std::holds_alternative<ParserStmtExpr>(*loopInit.body) && tok2.type != TK_OP_LBRACE && tok2.type != TK_OP_SEMI) {
      throw Error(this->reader, tok2.start, E0107);
    }

    if (std::holds_alternative<ParserStmtExpr>(*loopInit.body) && tok2.type == TK_OP_LBRACE) {
      this->lexer->seek(loc2);

      auto loopCond = std::get<ParserStmtExpr>(*loopInit.body);
      auto loopBody = this->_block();

      return this->_wrapStmt(ParserStmtLoop{std::nullopt, loopCond, std::nullopt, loopBody}, tok0.start);
    } else {
      auto stmtLoop = this->_stmtLoop(loopInit);
      return this->_wrapStmt(stmtLoop, tok0.start);
    }
  }

  if (tok0.type == TK_KW_MUT) {
    auto [loc1, tok1] = this->lexer->next();

    if (tok1.type == TK_ID) {
      auto [_2, tok2] = this->lexer->next();

      if (tok2.type == TK_OP_COLON) {
        auto varDeclType = this->_type();

        if (varDeclType == std::nullopt) {
          throw Error(this->reader, this->lexer->loc, E0102);
        }

        auto [loc3, tok3] = this->lexer->next();
        auto varDeclInit = std::optional<ParserStmtExpr>{};

        if (tok3.type == TK_OP_EQ) {
          varDeclInit = this->_stmtExpr();

          if (varDeclInit == std::nullopt) {
            throw Error(this->reader, this->lexer->loc, E0131);
          }
        } else {
          this->lexer->seek(loc3);
        }

        return this->_wrapStmt(ParserStmtVarDecl{tok1, varDeclType, varDeclInit, true}, tok0.start);
      } else if (tok2.type == TK_OP_COLON_EQ) {
        auto varDeclInit = this->_stmtExpr();

        if (varDeclInit == std::nullopt) {
          throw Error(this->reader, this->lexer->loc, E0131);
        }

        return this->_wrapStmt(ParserStmtVarDecl{tok1, std::nullopt, varDeclInit, true}, tok0.start);
      }
    }

    this->lexer->seek(loc1);
  }

  if (tok0.type == TK_KW_OBJ) {
    auto [_1, tok1] = this->lexer->next();

    if (tok1.type != TK_ID) {
      throw Error(this->reader, tok1.start, E0121);
    }

    auto [_2, tok2] = this->lexer->next();

    if (tok2.type != TK_OP_LBRACE) {
      throw Error(this->reader, tok2.start, E0122);
    }

    auto [_3, tok3] = this->lexer->next();
    auto objDeclFields = std::vector<ParserStmtObjDeclField>{};

    while (tok3.type != TK_OP_RBRACE) {
      if (tok3.type != TK_ID) {
        throw Error(this->reader, tok3.start, E0123);
      }

      auto [_4, tok4] = this->lexer->next();

      if (tok4.type != TK_OP_COLON) {
        throw Error(this->reader, tok4.start, E0124);
      }

      auto objDeclFieldType = this->_type();

      if (objDeclFieldType == std::nullopt) {
        throw Error(this->reader, this->lexer->loc, E0125);
      }

      objDeclFields.push_back(ParserStmtObjDeclField{tok3, *objDeclFieldType});
      std::tie(_3, tok3) = this->lexer->next();
    }

    if (objDeclFields.empty()) {
      throw Error(this->reader, tok0.start, E0126);
    }

    return this->_wrapStmt(ParserStmtObjDecl{tok1, objDeclFields}, tok0.start);
  }

  if (tok0.type == TK_ID) {
    auto [loc1, tok1] = this->lexer->next();

    if (tok1.type == TK_OP_COLON) {
      auto varDeclType = this->_type();

      if (varDeclType == std::nullopt) {
        throw Error(this->reader, this->lexer->loc, E0102);
      }

      auto [loc2, tok2] = this->lexer->next();
      auto varDeclInit = std::optional<ParserStmtExpr>{};

      if (tok2.type == TK_OP_EQ) {
        varDeclInit = this->_stmtExpr();

        if (varDeclInit == std::nullopt) {
          throw Error(this->reader, this->lexer->loc, E0131);
        }
      } else {
        this->lexer->seek(loc2);
      }

      return this->_wrapStmt(ParserStmtVarDecl{tok0, varDeclType, varDeclInit}, tok0.start);
    } else if (tok1.type == TK_OP_COLON_EQ) {
      auto varDeclInit = this->_stmtExpr();

      if (varDeclInit == std::nullopt) {
        throw Error(this->reader, this->lexer->loc, E0131);
      }

      return this->_wrapStmt(ParserStmtVarDecl{tok0, std::nullopt, varDeclInit}, tok0.start);
    }

    this->lexer->seek(loc1);
  }

  this->lexer->seek(tok0.start);
  auto stmtExprTest = this->_stmtExpr();

  if (stmtExprTest != std::nullopt) {
    return this->_wrapStmt(*stmtExprTest, tok0.start);
  }

  throw Error(this->reader, tok0.start, E0100);
}

std::string Parser::xml () {
  auto result = std::string();

  while (true) {
    auto stmt = this->next();

    if (std::holds_alternative<ParserStmtEof>(*stmt.body)) {
      break;
    }

    result += stmt.xml() + EOL;
  }

  return result;
}

ParserBlock Parser::_block () {
  auto [_1, tok1] = this->lexer->next();

  if (tok1.type != TK_OP_LBRACE) {
    throw Error(this->reader, tok1.start, E0103);
  }

  auto block = ParserBlock{};

  while (true) {
    auto [loc2, tok2] = this->lexer->next();

    if (tok2.type == TK_EOF) {
      throw Error(this->reader, tok2.start, E0104);
    } else if (tok2.type == TK_OP_RBRACE) {
      break;
    }

    this->lexer->seek(loc2);

    auto stmt = this->next();
    block.push_back(stmt);
  }

  return block;
}

std::optional<ParserStmtExpr> Parser::_stmtExpr (bool root) {
  auto [loc1, tok1] = this->lexer->next();

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
    auto exprLit = ParserExprLit{tok1};
    auto stmtExpr = ParserStmtExpr{std::make_shared<ParserExpr>(exprLit), false, tok1.start, this->lexer->loc};

    return root ? this->_wrapStmtExpr(stmtExpr) : stmtExpr;
  }

  if (
    tok1.type == TK_OP_EXCL ||
    tok1.type == TK_OP_MINUS ||
    tok1.type == TK_OP_MINUS_MINUS ||
    tok1.type == TK_OP_PLUS ||
    tok1.type == TK_OP_PLUS_PLUS ||
    tok1.type == TK_OP_TILDE
  ) {
    auto exprUnaryArg = this->_stmtExpr(false);

    if (exprUnaryArg == std::nullopt) {
      throw Error(this->reader, this->lexer->loc, E0132);
    }

    auto exprUnary = ParserExprUnary{*exprUnaryArg, tok1, true};
    auto stmtExpr = ParserStmtExpr{std::make_shared<ParserExpr>(exprUnary), false, tok1.start, this->lexer->loc};

    return root ? this->_wrapStmtExpr(stmtExpr) : stmtExpr;
  }

  if (tok1.type == TK_ID) {
    auto exprAccess = ParserExprAccess{std::make_shared<ParserMemberObj>(tok1)};
    auto [loc2, tok2] = this->lexer->next();

    while (true) {
      if (tok2.type != TK_OP_DOT) {
        this->lexer->seek(loc2);
        break;
      }

      auto [_3, tok3] = this->lexer->next();

      if (tok3.type != TK_ID) {
        throw Error(this->reader, tok3.start, E0110);
      }

      auto member = ParserMember{exprAccess.body, tok3};
      exprAccess = ParserExprAccess{std::make_shared<ParserMemberObj>(member)};

      std::tie(loc2, tok2) = this->lexer->next();
    }

    auto stmtExpr = ParserStmtExpr{std::make_shared<ParserExpr>(exprAccess), false, tok1.start, this->lexer->loc};
    return root ? this->_wrapStmtExpr(stmtExpr) : stmtExpr;
  }

  if (tok1.type == TK_OP_LPAR) {
    auto stmtExpr = this->_stmtExpr();

    if (stmtExpr == std::nullopt) {
      throw Error(this->reader, this->lexer->loc, E0136);
    }

    auto [_2, tok2] = this->lexer->next();

    if (tok2.type != TK_OP_RPAR) {
      throw Error(this->reader, tok2.start, E0109);
    }

    stmtExpr->parenthesized = true;
    stmtExpr->start = tok1.start;
    stmtExpr->end = this->lexer->loc;

    return root ? this->_wrapStmtExpr(*stmtExpr) : stmtExpr;
  }

  this->lexer->seek(loc1);
  return std::nullopt;
}

ParserStmtIf Parser::_stmtIf () {
  auto cond = this->next();
  auto body = this->_block();
  auto alt = std::optional<std::shared_ptr<ParserStmtIfCond>>{};
  auto [loc1, tok1] = this->lexer->next();

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

ParserStmtLoop Parser::_stmtLoop (const std::optional<ParserStmt> &init) {
  auto condTest = this->_stmtExpr();
  auto cond = condTest == std::nullopt ? std::optional<ParserStmtExpr>{} : condTest;
  auto [loc1, tok1] = this->lexer->next();

  if (tok1.type != TK_OP_SEMI) {
    throw Error(this->reader, tok1.start, E0108);
  }

  auto updTest = this->_stmtExpr();
  auto upd = updTest == std::nullopt ? std::optional<ParserStmtExpr>{} : updTest;
  auto body = this->_block();

  return ParserStmtLoop{init, cond, upd, body};
}

std::optional<ParserType> Parser::_type () {
  auto [loc1, tok1] = this->lexer->next();

  if (tok1.type == TK_OP_LPAR) {
    auto type = this->_type();

    if (type == std::nullopt) {
      this->lexer->seek(loc1);
      return std::nullopt;
    }

    auto [loc2, tok2] = this->lexer->next();

    if (tok2.type != TK_OP_RPAR) {
      this->lexer->seek(loc2);
      throw Error(this->reader, this->lexer->loc, E0127);
    }

    return ParserType{type->body, true, tok1.start, this->lexer->loc};
  } else if (tok1.type == TK_KW_FN) {
    auto [loc2, tok2] = this->lexer->next();

    if (tok2.type != TK_OP_LPAR) {
      throw Error(this->reader, tok2.start, E0129);
    }

    auto [loc3, tok3] = this->lexer->next();
    auto fnParams = std::vector<ParserTypeFnParam>{};

    while (tok3.type != TK_OP_RPAR) {
      this->lexer->seek(loc3);
      auto fnParamType = this->_type();
      auto fnParamVariadic = false;

      if (fnParamType == std::nullopt) {
        throw Error(this->reader, this->lexer->loc, E0118);
      }

      auto [loc4, tok4] = this->lexer->next();

      if (tok4.type == TK_OP_DOT_DOT_DOT) {
        fnParamVariadic = true;
      } else {
        this->lexer->seek(loc4);
      }

      fnParams.push_back(ParserTypeFnParam{*fnParamType, fnParamVariadic});
      std::tie(loc3, tok3) = this->lexer->next();

      if (tok3.type == TK_OP_COMMA) {
        std::tie(loc3, tok3) = this->lexer->next();
      }
    }

    auto fnReturnType = this->_type();

    if (fnReturnType == std::nullopt) {
      throw Error(this->reader, this->lexer->loc, E0120);
    }

    auto typeFn = ParserTypeFn{fnParams, *fnReturnType};
    return ParserType{std::make_shared<ParserTypeBody>(typeFn), false, tok1.start, this->lexer->loc};
  } else if (tok1.type == TK_ID) {
    auto typeId = ParserTypeId{tok1};
    return ParserType{std::make_shared<ParserTypeBody>(typeId), false, tok1.start, this->lexer->loc};
  }

  this->lexer->seek(loc1);
  return std::nullopt;
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprAssign (const ParserStmtExpr &stmtExpr, ReaderLocation loc, const Token &tok) {
  if (
    std::holds_alternative<ParserExprAssign>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprAssign>(*stmtExpr.body).op.precedence() <= tok.precedence()
  ) {
    auto exprAssign = std::get<ParserExprAssign>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprAssign(exprAssign.right, loc, tok);
    auto newExprAssign = ParserExprAssign{exprAssign.left, exprAssign.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprAssign), false, stmtExpr.start, newExprAssign.right.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprBinary>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprBinary>(*stmtExpr.body).op.precedence() < tok.precedence()
  ) { // todo test
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprAssign(exprBinary.right, loc, tok);
    auto newExprBinary = ParserExprBinary{exprBinary.left, exprBinary.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprBinary), false, stmtExpr.start, newExprBinary.right.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprCond>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    Token{TK_OP_QN}.precedence() < tok.precedence()
  ) { // todo test
    auto exprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprAssign(exprCond.alt, loc, tok);
    auto newExprCond = ParserExprCond{exprCond.cond, exprCond.body, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprCond), false, stmtExpr.start, newExprCond.alt.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprUnary>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprUnary>(*stmtExpr.body).prefix &&
    std::get<ParserExprUnary>(*stmtExpr.body).op.precedence() < tok.precedence()
  ) { // todo test
    auto exprUnary = std::get<ParserExprUnary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprAssign(exprUnary.arg, loc, tok);
    auto newExprUnary = ParserExprUnary{newStmtExpr, exprUnary.op, exprUnary.prefix};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprUnary), false, stmtExpr.start, newExprUnary.arg.end},
      shouldWrap
    );
  }

  auto exprAssignRight = this->_stmtExpr(false);

  if (exprAssignRight == std::nullopt) {
    throw Error(this->reader, this->lexer->loc, E0133);
  } else if (!std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) { // todo test
    throw Error(this->reader, stmtExpr.start, stmtExpr.end, E0140);
  }

  auto exprAccess = std::get<ParserExprAccess>(*stmtExpr.body);
  auto exprAssign = ParserExprAssign{exprAccess, tok, *exprAssignRight};

  return std::make_tuple(
    ParserStmtExpr{std::make_shared<ParserExpr>(exprAssign), false, stmtExpr.start, this->lexer->loc},
    true
  );
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprBinary (const ParserStmtExpr &stmtExpr, ReaderLocation loc, const Token &tok) {
  if (
    std::holds_alternative<ParserExprAssign>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprAssign>(*stmtExpr.body).op.precedence() < tok.precedence()
  ) {
    auto exprAssign = std::get<ParserExprAssign>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprBinary(exprAssign.right, loc, tok);
    auto newExprAssign = ParserExprAssign{exprAssign.left, exprAssign.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprAssign), false, stmtExpr.start, newExprAssign.right.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprBinary>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprBinary>(*stmtExpr.body).op.precedence() < tok.precedence()
  ) {
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprBinary(exprBinary.right, loc, tok);
    auto newExprBinary = ParserExprBinary{exprBinary.left, exprBinary.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprBinary), false, stmtExpr.start, newExprBinary.right.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprCond>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    Token{TK_OP_QN}.precedence() < tok.precedence()
  ) { // todo test
    auto exprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprBinary(exprCond.alt, loc, tok);
    auto newExprCond = ParserExprCond{exprCond.cond, exprCond.body, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprCond), false, stmtExpr.start, newExprCond.alt.end},
      shouldWrap
    );
  }

  auto stmtExprRight = this->_stmtExpr(false);

  if (stmtExprRight == std::nullopt) {
    throw Error(this->reader, this->lexer->loc, E0137);
  }

  if (std::holds_alternative<ParserExprBinary>(*stmtExpr.body) && !stmtExpr.parenthesized) {
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);

    if (
      tok.type == TK_OP_PLUS &&
      std::holds_alternative<ParserExprLit>(*exprBinary.right.body) &&
      std::get<ParserExprLit>(*exprBinary.right.body).body.type == TK_LIT_STR &&
      std::holds_alternative<ParserExprLit>(*stmtExprRight->body) &&
      std::get<ParserExprLit>(*stmtExprRight->body).body.type == TK_LIT_STR
    ) {
      auto newExprBinaryRight = ParserExprBinary{exprBinary.right, tok, *stmtExprRight};

      auto newStmtExprRight = ParserStmtExpr{
        std::make_shared<ParserExpr>(newExprBinaryRight),
        false,
        exprBinary.right.start,
        stmtExprRight->end
      };

      auto newExprBinary = ParserExprBinary{exprBinary.left, exprBinary.op, newStmtExprRight};

      return std::make_tuple(
        ParserStmtExpr{std::make_shared<ParserExpr>(newExprBinary), false, stmtExpr.start, this->lexer->loc},
        true
      );
    }
  }

  auto exprBinary = ParserExprBinary{stmtExpr, tok, *stmtExprRight};

  return std::make_tuple(
    ParserStmtExpr{std::make_shared<ParserExpr>(exprBinary), false, stmtExpr.start, this->lexer->loc},
    true
  );
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprCall (const ParserStmtExpr &stmtExpr, ReaderLocation loc, const Token &tok) {
  if (std::holds_alternative<ParserExprAssign>(*stmtExpr.body) && !stmtExpr.parenthesized) { // todo test
    auto exprAssign = std::get<ParserExprAssign>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprCall(exprAssign.right, loc, tok);
    auto newExprAssign = ParserExprAssign{exprAssign.left, exprAssign.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprAssign), false, stmtExpr.start, newExprAssign.right.end},
      shouldWrap
    );
  } else if (std::holds_alternative<ParserExprBinary>(*stmtExpr.body) && !stmtExpr.parenthesized) {
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprCall(exprBinary.right, loc, tok);
    auto newExprBinary = ParserExprBinary{exprBinary.left, exprBinary.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprBinary), false, stmtExpr.start, newExprBinary.right.end},
      shouldWrap
    );
  } else if (std::holds_alternative<ParserExprCond>(*stmtExpr.body) && !stmtExpr.parenthesized) { // todo test
    auto exprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprCall(exprCond.alt, loc, tok);
    auto newExprCond = ParserExprCond{exprCond.cond, exprCond.body, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprCond), false, stmtExpr.start, newExprCond.alt.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprUnary>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprUnary>(*stmtExpr.body).prefix
  ) { // todo test
    auto exprUnary = std::get<ParserExprUnary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprCall(exprUnary.arg, loc, tok);
    auto newExprUnary = ParserExprUnary{newStmtExpr, exprUnary.op, exprUnary.prefix};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprUnary), false, stmtExpr.start, newExprUnary.arg.end},
      shouldWrap
    );
  }

  auto [loc2, tok2] = this->lexer->next();
  auto exprCallArgs = std::vector<ParserExprCallArg>{};

  while (tok2.type != TK_OP_RPAR) {
    auto exprCallArgId = std::optional<Token>{};

    if (tok2.type == TK_ID) {
      auto [_3, tok3] = this->lexer->next();

      if (tok3.type == TK_OP_COLON) {
        exprCallArgId = tok2;
      }
    }

    if (exprCallArgId == std::nullopt) {
      this->lexer->seek(loc2);
    }

    auto exprCallArgExpr = this->_stmtExpr();

    if (exprCallArgExpr == std::nullopt) {
      throw Error(this->reader, this->lexer->loc, E0135);
    }

    exprCallArgs.push_back(ParserExprCallArg{exprCallArgId, *exprCallArgExpr});
    std::tie(loc2, tok2) = this->lexer->next();

    if (tok2.type == TK_OP_COMMA) {
      std::tie(loc2, tok2) = this->lexer->next();
    }
  }

  if (!std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) { // todo test
    throw Error(this->reader, stmtExpr.start, stmtExpr.end, E0141);
  }

  auto exprAccess = std::get<ParserExprAccess>(*stmtExpr.body);
  auto exprCall = ParserExprCall{exprAccess, exprCallArgs};

  return std::make_tuple(
    ParserStmtExpr{std::make_shared<ParserExpr>(exprCall), false, stmtExpr.start, this->lexer->loc},
    true
  );
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprCond (const ParserStmtExpr &stmtExpr, ReaderLocation loc, const Token &tok) {
  if (
    std::holds_alternative<ParserExprAssign>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprAssign>(*stmtExpr.body).op.precedence() < tok.precedence()
  ) {
    auto exprAssign = std::get<ParserExprAssign>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprCond(exprAssign.right, loc, tok);
    auto newExprAssign = ParserExprAssign{exprAssign.left, exprAssign.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprAssign), false, stmtExpr.start, newExprAssign.right.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprBinary>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprBinary>(*stmtExpr.body).op.precedence() < tok.precedence()
  ) { // todo test
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprCond(exprBinary.right, loc, tok);
    auto newExprBinary = ParserExprBinary{exprBinary.left, exprBinary.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprBinary), false, stmtExpr.start, newExprBinary.right.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprCond>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    Token{TK_OP_QN}.precedence() < tok.precedence()
  ) { // todo test
    auto exprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprCond(exprCond.alt, loc, tok);
    auto newExprCond = ParserExprCond{exprCond.cond, exprCond.body, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprCond), false, stmtExpr.start, newExprCond.alt.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprUnary>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprUnary>(*stmtExpr.body).prefix &&
    std::get<ParserExprUnary>(*stmtExpr.body).op.precedence() < tok.precedence()
  ) { // todo test
    auto exprUnary = std::get<ParserExprUnary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprCond(exprUnary.arg, loc, tok);
    auto newExprUnary = ParserExprUnary{newStmtExpr, exprUnary.op, exprUnary.prefix};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprUnary), false, stmtExpr.start, newExprUnary.arg.end},
      shouldWrap
    );
  }

  auto exprCondBody = this->_stmtExpr();

  if (exprCondBody == std::nullopt) {
    throw Error(this->reader, this->lexer->loc, E0138);
  }

  auto [_2, tok2] = this->lexer->next();

  if (tok2.type != TK_OP_COLON) {
    throw Error(this->reader, tok2.start, E0111);
  }

  auto exprCondAlt = this->_stmtExpr();

  if (exprCondAlt == std::nullopt) {
    throw Error(this->reader, this->lexer->loc, E0139);
  }

  auto exprCond = ParserExprCond{stmtExpr, *exprCondBody, *exprCondAlt};

  return std::make_tuple(
    ParserStmtExpr{std::make_shared<ParserExpr>(exprCond), false, stmtExpr.start, this->lexer->loc},
    true
  );
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprObj (const ParserStmtExpr &stmtExpr, ReaderLocation loc, const Token &tok) {
  if (std::holds_alternative<ParserExprAssign>(*stmtExpr.body) && !stmtExpr.parenthesized) { // todo test
    auto exprAssign = std::get<ParserExprAssign>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprObj(exprAssign.right, loc, tok);
    auto newExprAssign = ParserExprAssign{exprAssign.left, exprAssign.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprAssign), false, stmtExpr.start, newExprAssign.right.end},
      shouldWrap
    );
  } else if (std::holds_alternative<ParserExprBinary>(*stmtExpr.body) && !stmtExpr.parenthesized) {
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprObj(exprBinary.right, loc, tok);
    auto newExprBinary = ParserExprBinary{exprBinary.left, exprBinary.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprBinary), false, stmtExpr.start, newExprBinary.right.end},
      shouldWrap
    );
  } else if (std::holds_alternative<ParserExprCond>(*stmtExpr.body) && !stmtExpr.parenthesized) { // todo test
    auto exprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprObj(exprCond.alt, loc, tok);
    auto newExprCond = ParserExprCond{exprCond.cond, exprCond.body, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprCond), false, stmtExpr.start, newExprCond.alt.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprUnary>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprUnary>(*stmtExpr.body).prefix
  ) {
    auto exprUnary = std::get<ParserExprUnary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprObj(exprUnary.arg, loc, tok);
    auto newExprUnary = ParserExprUnary{newStmtExpr, exprUnary.op, exprUnary.prefix};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprUnary), false, stmtExpr.start, newExprUnary.arg.end},
      shouldWrap
    );
  }

  auto [_2, tok2] = this->lexer->next();
  auto exprObjProps = std::vector<ParserExprObjProp>{};

  while (tok2.type != TK_OP_RBRACE) {
    if (tok2.type != TK_ID && exprObjProps.empty()) {
      this->lexer->seek(loc);
      return std::make_tuple(stmtExpr, false);
    } else if (tok2.type != TK_ID) {
      throw Error(this->reader, tok2.start, E0112);
    }

    auto [_3, tok3] = this->lexer->next();

    if (tok3.type != TK_OP_COLON && exprObjProps.empty()) {
      this->lexer->seek(loc);
      return std::make_tuple(stmtExpr, false);
    } else if (tok3.type != TK_OP_COLON) {
      throw Error(this->reader, tok3.start, E0113);
    }

    auto exprObjPropInit = this->_stmtExpr();

    if (exprObjPropInit == std::nullopt) {
      throw Error(this->reader, this->lexer->loc, E0134);
    }

    exprObjProps.push_back(ParserExprObjProp{tok2, *exprObjPropInit});
    std::tie(_2, tok2) = this->lexer->next();

    if (tok2.type == TK_OP_COMMA) {
      std::tie(_2, tok2) = this->lexer->next();
    }
  }

  if (exprObjProps.empty()) {
    this->lexer->seek(loc);
    return std::make_tuple(stmtExpr, false);
  } else if (!std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) { // todo test
    throw Error(this->reader, stmtExpr.start, stmtExpr.end, E0114);
  }

  auto exprAccess = std::get<ParserExprAccess>(*stmtExpr.body);

  if (!std::holds_alternative<Token>(*exprAccess.body)) {
    throw Error(this->reader, stmtExpr.start, stmtExpr.end, E0114);
  }

  auto exprObj = ParserExprObj{std::get<Token>(*exprAccess.body), exprObjProps};

  return std::make_tuple(
    ParserStmtExpr{std::make_shared<ParserExpr>(exprObj), false, stmtExpr.start, this->lexer->loc},
    true
  );
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprUnary (const ParserStmtExpr &stmtExpr, ReaderLocation loc, const Token &tok) {
  if (std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) {
    auto exprUnary = ParserExprUnary{stmtExpr, tok, false};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(exprUnary), false, stmtExpr.start, tok.end},
      true
    );
    // todo test !parentStmtExpr.parenthesized
  } else if (
    std::holds_alternative<ParserExprAssign>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprAssign>(*stmtExpr.body).op.precedence() < tok.precedence()
  ) { // todo test
    auto exprAssign = std::get<ParserExprAssign>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprUnary(exprAssign.right, loc, tok);
    auto newExprAssign = ParserExprAssign{exprAssign.left, exprAssign.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprAssign), false, stmtExpr.start, newExprAssign.right.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprBinary>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprBinary>(*stmtExpr.body).op.precedence() < tok.precedence()
  ) {
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprUnary(exprBinary.right, loc, tok);
    auto newExprBinary = ParserExprBinary{exprBinary.left, exprBinary.op, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprBinary), false, stmtExpr.start, newExprBinary.right.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprCond>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    Token{TK_OP_QN}.precedence() < tok.precedence()
  ) { // todo test
    auto exprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprUnary(exprCond.alt, loc, tok);
    auto newExprCond = ParserExprCond{exprCond.cond, exprCond.body, newStmtExpr};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprCond), false, stmtExpr.start, newExprCond.alt.end},
      shouldWrap
    );
  } else if (
    std::holds_alternative<ParserExprUnary>(*stmtExpr.body) &&
    !stmtExpr.parenthesized &&
    std::get<ParserExprUnary>(*stmtExpr.body).prefix &&
    std::get<ParserExprUnary>(*stmtExpr.body).op.precedence() < tok.precedence()
  ) {
    auto exprUnary = std::get<ParserExprUnary>(*stmtExpr.body);
    auto [newStmtExpr, shouldWrap] = this->_wrapExprUnary(exprUnary.arg, loc, tok);
    auto newExprUnary = ParserExprUnary{newStmtExpr, exprUnary.op, exprUnary.prefix};

    return std::make_tuple(
      ParserStmtExpr{std::make_shared<ParserExpr>(newExprUnary), false, stmtExpr.start, newExprUnary.arg.end},
      shouldWrap
    );
  }

  // todo test
  throw Error(this->reader, tok.start, E0142);
}

ParserStmt Parser::_wrapStmt (const ParserStmtBody &body, ReaderLocation start) const {
  return ParserStmt{std::make_shared<ParserStmtBody>(body), start, this->lexer->loc};
}

ParserStmtExpr Parser::_wrapStmtExpr (const ParserStmtExpr &stmtExpr) {
  auto [loc, tok] = this->lexer->next();

  if (
    tok.type == TK_OP_AND ||
    tok.type == TK_OP_AND_AND ||
    tok.type == TK_OP_CARET ||
    tok.type == TK_OP_EQ_EQ ||
    tok.type == TK_OP_EXCL_EQ ||
    tok.type == TK_OP_GT ||
    tok.type == TK_OP_GT_EQ ||
    tok.type == TK_OP_LSHIFT ||
    tok.type == TK_OP_LT ||
    tok.type == TK_OP_LT_EQ ||
    tok.type == TK_OP_MINUS ||
    tok.type == TK_OP_OR ||
    tok.type == TK_OP_OR_OR ||
    tok.type == TK_OP_PERCENT ||
    tok.type == TK_OP_PLUS ||
    tok.type == TK_OP_QN_QN ||
    tok.type == TK_OP_RSHIFT ||
    tok.type == TK_OP_SLASH ||
    tok.type == TK_OP_STAR ||
    tok.type == TK_OP_STAR_STAR
  ) {
    auto [newStmtExpr, _] = this->_wrapExprBinary(stmtExpr, loc, tok);
    return this->_wrapStmtExpr(newStmtExpr);
  } else if (
    tok.type == TK_OP_AND_AND_EQ ||
    tok.type == TK_OP_AND_EQ ||
    tok.type == TK_OP_CARET_EQ ||
    tok.type == TK_OP_EQ ||
    tok.type == TK_OP_LSHIFT_EQ ||
    tok.type == TK_OP_MINUS_EQ ||
    tok.type == TK_OP_OR_EQ ||
    tok.type == TK_OP_OR_OR_EQ ||
    tok.type == TK_OP_PERCENT_EQ ||
    tok.type == TK_OP_PLUS_EQ ||
    tok.type == TK_OP_QN_QN_EQ ||
    tok.type == TK_OP_RSHIFT_EQ ||
    tok.type == TK_OP_SLASH_EQ ||
    tok.type == TK_OP_STAR_EQ ||
    tok.type == TK_OP_STAR_STAR_EQ
  ) {
    auto [newStmtExpr, _] = this->_wrapExprAssign(stmtExpr, loc, tok);
    return this->_wrapStmtExpr(newStmtExpr);
  } else if (tok.type == TK_OP_LBRACE) {
    auto [newStmtExpr, shouldWrap] = this->_wrapExprObj(stmtExpr, loc, tok);
    return shouldWrap ? this->_wrapStmtExpr(newStmtExpr) : newStmtExpr;
  } else if (tok.type == TK_OP_LPAR) {
    auto [newStmtExpr, _] = this->_wrapExprCall(stmtExpr, loc, tok);
    return this->_wrapStmtExpr(newStmtExpr);
  } else if (tok.type == TK_OP_MINUS_MINUS || tok.type == TK_OP_PLUS_PLUS) {
    auto [newStmtExpr, _] = this->_wrapExprUnary(stmtExpr, loc, tok);
    return this->_wrapStmtExpr(newStmtExpr);
  } else if (tok.type == TK_OP_QN) {
    auto [newStmtExpr, _] = this->_wrapExprCond(stmtExpr, loc, tok);
    return this->_wrapStmtExpr(newStmtExpr);
  }

  this->lexer->seek(loc);
  return stmtExpr;
}
