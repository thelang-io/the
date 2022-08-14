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

ParserStmt Parser::next (bool allowSemi) {
  auto [_0, tok0] = this->lexer->next();

  if (tok0.type == TK_EOF) {
    return this->_wrapStmt(false, ParserStmtEof{}, tok0.start);
  } else if (tok0.type == TK_KW_BREAK) {
    return this->_wrapStmt(allowSemi, ParserStmtBreak{}, tok0.start);
  } else if (tok0.type == TK_KW_CONTINUE) {
    return this->_wrapStmt(allowSemi, ParserStmtContinue{}, tok0.start);
  } else if (tok0.type == TK_KW_IF || tok0.type == TK_KW_ELIF) {
    auto ifCond = this->_stmtExpr();

    if (ifCond == std::nullopt) {
      throw Error(this->reader, this->lexer->loc, E0143);
    }

    auto ifBody = this->_block();
    auto ifAlt = std::optional<std::variant<ParserBlock, ParserStmt>>{};
    auto [loc1, tok1] = this->lexer->next();

    if (tok1.type == TK_KW_ELIF) {
      this->lexer->seek(loc1);
      ifAlt = this->next();
    } else if (tok1.type == TK_KW_ELSE) {
      ifAlt = this->_block();
    } else {
      this->lexer->seek(loc1);
    }

    return this->_wrapStmt(allowSemi, ParserStmtIf{*ifCond, ifBody, ifAlt}, tok0.start);
  } else if (tok0.type == TK_KW_MAIN) {
    auto mainBody = this->_block();
    return this->_wrapStmt(allowSemi, ParserStmtMain{mainBody}, tok0.start);
  } else if (tok0.type == TK_KW_RETURN) {
    auto stmtExprTest = this->_stmtExpr();
    auto returnBody = stmtExprTest == std::nullopt ? std::optional<ParserStmtExpr>{} : stmtExprTest;

    return this->_wrapStmt(allowSemi, ParserStmtReturn{returnBody}, tok0.start);
  } else if (tok0.type == TK_OP_SEMI && allowSemi) {
    return this->_wrapStmt(false, ParserStmtEmpty{}, tok0.start);
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
      auto fnDeclParamMut = tok3.type == TK_KW_MUT;

      if (tok3.type == TK_KW_MUT) {
        std::tie(_3, tok3) = this->lexer->next();
      }

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

      fnDeclParams.push_back(ParserStmtFnDeclParam{
        tok3,
        fnDeclParamType,
        fnDeclParamMut,
        fnDeclParamVariadic,
        fnDeclParamInit
      });

      std::tie(_3, tok3) = this->lexer->next();

      if (tok3.type == TK_OP_COMMA) {
        std::tie(_3, tok3) = this->lexer->next();
      }
    }

    auto typeTest2 = this->_type();
    auto fnDeclReturnType = typeTest2 == std::nullopt ? std::optional<ParserType>{} : typeTest2;
    auto fnDeclBody = this->_block();

    return this->_wrapStmt(allowSemi, ParserStmtFnDecl{tok1, fnDeclParams, fnDeclReturnType, fnDeclBody}, tok0.start);
  }

  if (tok0.type == TK_KW_LOOP) {
    auto [loc1, tok1] = this->lexer->next();
    auto loopInit = std::optional<ParserStmt>{};

    if (tok1.type == TK_OP_LBRACE) {
      this->lexer->seek(loc1);
      auto loopBody = this->_block();

      return this->_wrapStmt(allowSemi, ParserStmtLoop{std::nullopt, std::nullopt, std::nullopt, loopBody}, tok0.start);
    } else if (tok1.type != TK_OP_SEMI) {
      this->lexer->seek(loc1);
      loopInit = this->next(false);

      if (!std::holds_alternative<ParserStmtExpr>(*loopInit->body) && !std::holds_alternative<ParserStmtVarDecl>(*loopInit->body)) {
        throw Error(this->reader, tok1.start, E0105);
      } else if (std::holds_alternative<ParserStmtVarDecl>(*loopInit->body) && std::get<ParserStmtVarDecl>(*loopInit->body).mut) {
        throw Error(this->reader, tok1.start, E0144);
      }

      auto [loc2, tok2] = this->lexer->next();

      if (std::holds_alternative<ParserStmtExpr>(*loopInit->body) && tok2.type == TK_OP_LBRACE) {
        this->lexer->seek(loc2);

        auto loopCond = std::get<ParserStmtExpr>(*loopInit->body);
        auto loopBody = this->_block();

        return this->_wrapStmt(allowSemi, ParserStmtLoop{std::nullopt, loopCond, std::nullopt, loopBody}, tok0.start);
      } else if (tok2.type != TK_OP_SEMI) {
        throw Error(this->reader, tok2.start, E0106);
      }
    }

    auto loopCond = this->_stmtExpr();
    auto [_3, tok3] = this->lexer->next();

    if (tok3.type != TK_OP_SEMI) {
      throw Error(this->reader, tok3.start, E0108);
    }

    auto loopUpd = this->_stmtExpr();
    auto loopBody = this->_block();

    return this->_wrapStmt(allowSemi, ParserStmtLoop{loopInit, loopCond, loopUpd, loopBody}, tok0.start);
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

        return this->_wrapStmt(allowSemi, ParserStmtVarDecl{tok1, varDeclType, varDeclInit, true}, tok0.start);
      } else if (tok2.type == TK_OP_COLON_EQ) {
        auto varDeclInit = this->_stmtExpr();

        if (varDeclInit == std::nullopt) {
          throw Error(this->reader, this->lexer->loc, E0131);
        }

        return this->_wrapStmt(allowSemi, ParserStmtVarDecl{tok1, std::nullopt, varDeclInit, true}, tok0.start);
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

    return this->_wrapStmt(allowSemi, ParserStmtObjDecl{tok1, objDeclFields}, tok0.start);
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

      return this->_wrapStmt(allowSemi, ParserStmtVarDecl{tok0, varDeclType, varDeclInit}, tok0.start);
    } else if (tok1.type == TK_OP_COLON_EQ) {
      auto varDeclInit = this->_stmtExpr();

      if (varDeclInit == std::nullopt) {
        throw Error(this->reader, this->lexer->loc, E0131);
      }

      return this->_wrapStmt(allowSemi, ParserStmtVarDecl{tok0, std::nullopt, varDeclInit}, tok0.start);
    }

    this->lexer->seek(loc1);
  }

  this->lexer->seek(tok0.start);
  auto stmtExprTest = this->_stmtExpr();

  if (stmtExprTest != std::nullopt) {
    return this->_wrapStmt(allowSemi, *stmtExprTest, tok0.start);
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
    tok1.type == TK_KW_NIL ||
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

  if (tok1.type == TK_KW_REF) {
    auto exprRefExpr = this->_stmtExpr(false);

    if (exprRefExpr == std::nullopt) {
      throw Error(this->reader, this->lexer->loc, E0147);
    } else if (!std::holds_alternative<ParserExprAccess>(*exprRefExpr->body)) {
      throw Error(this->reader, exprRefExpr->start, E0148);
    }

    auto exprRef = ParserExprRef{*exprRefExpr};
    auto stmtExpr = ParserStmtExpr{std::make_shared<ParserExpr>(exprRef), false, tok1.start, this->lexer->loc};

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
    } else if (
      (tok1.type == TK_OP_MINUS_MINUS || tok1.type == TK_OP_PLUS_PLUS) &&
      !std::holds_alternative<ParserExprAccess>(*exprUnaryArg->body)
    ) {
      throw Error(this->reader, tok1.start, E0142);
    }

    auto exprUnary = ParserExprUnary{*exprUnaryArg, tok1, true};
    auto stmtExpr = ParserStmtExpr{std::make_shared<ParserExpr>(exprUnary), false, tok1.start, this->lexer->loc};

    return root ? this->_wrapStmtExpr(stmtExpr) : stmtExpr;
  }

  if (tok1.type == TK_OP_LBRACK) {
    auto [loc2, tok2] = this->lexer->next();
    auto exprArrayElements = std::vector<ParserStmtExpr>{};

    while (tok2.type != TK_OP_RBRACK) {
      this->lexer->seek(loc2);
      auto exprArrayElement = this->_stmtExpr();

      if (exprArrayElement == std::nullopt) {
        throw Error(this->reader, this->lexer->loc, E0152);
      }

      exprArrayElements.push_back(*exprArrayElement);
      std::tie(loc2, tok2) = this->lexer->next();

      if (tok2.type == TK_OP_COMMA) {
        std::tie(loc2, tok2) = this->lexer->next();
      }
    }

    auto exprArray = ParserExprArray{exprArrayElements};
    auto stmtExpr = ParserStmtExpr{std::make_shared<ParserExpr>(exprArray), false, tok1.start, this->lexer->loc};

    return root ? this->_wrapStmtExpr(stmtExpr) : stmtExpr;
  }

  if (tok1.type == TK_ID) {
    auto exprAccess = ParserExprAccess{tok1, std::nullopt, std::nullopt};
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

    auto newStmtExpr = ParserStmtExpr{stmtExpr->body, true, tok1.start, this->lexer->loc};
    return root ? this->_wrapStmtExpr(newStmtExpr) : newStmtExpr;
  }

  this->lexer->seek(loc1);
  return std::nullopt;
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

    return this->_wrapType(ParserType{type->body, true, tok1.start, this->lexer->loc});
  } else if (tok1.type == TK_ID) {
    auto typeId = ParserTypeId{tok1};
    return this->_wrapType(ParserType{std::make_shared<ParserTypeBody>(typeId), false, tok1.start, this->lexer->loc});
  } else if (tok1.type == TK_KW_FN) {
    auto [loc2, tok2] = this->lexer->next();

    if (tok2.type != TK_OP_LPAR) {
      throw Error(this->reader, tok2.start, E0129);
    }

    auto [loc3, tok3] = this->lexer->next();
    auto fnParams = std::vector<ParserTypeFnParam>{};

    while (tok3.type != TK_OP_RPAR) {
      this->lexer->seek(loc3);

      auto [loc4, tok4] = this->lexer->next();
      auto fnParamId = std::optional<Token>{};
      auto fnParamMut = tok4.type == TK_KW_MUT;

      if (tok4.type == TK_KW_MUT) {
        std::tie(loc4, tok4) = this->lexer->next();

        if (tok4.type != TK_ID) {
          throw Error(this->reader, tok4.start, E0145);
        }

        auto [_5, tok5] = this->lexer->next();

        if (tok5.type != TK_OP_COLON) {
          throw Error(this->reader, tok5.start, E0146);
        }

        fnParamId = tok4;
      } else if (tok4.type == TK_ID) {
        auto [_5, tok5] = this->lexer->next();

        if (tok5.type == TK_OP_COLON) {
          fnParamId = tok4;
        } else {
          this->lexer->seek(loc4);
        }
      } else {
        this->lexer->seek(loc4);
      }

      auto fnParamType = this->_type();
      auto fnParamVariadic = false;

      if (fnParamType == std::nullopt) {
        throw Error(this->reader, this->lexer->loc, E0118);
      }

      auto [loc6, tok6] = this->lexer->next();

      if (tok6.type == TK_OP_DOT_DOT_DOT) {
        fnParamVariadic = true;
      } else {
        this->lexer->seek(loc6);
      }

      fnParams.push_back(ParserTypeFnParam{fnParamId, *fnParamType, fnParamMut, fnParamVariadic});
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
    return this->_wrapType(ParserType{std::make_shared<ParserTypeBody>(typeFn), false, tok1.start, this->lexer->loc});
  } else if (tok1.type == TK_KW_REF) {
    auto type = this->_type();

    if (type == std::nullopt) {
      throw Error(this->reader, this->lexer->loc, E0149);
    }

    auto typeRef = ParserTypeRef{*type};
    return this->_wrapType(ParserType{std::make_shared<ParserTypeBody>(typeRef), false, tok1.start, this->lexer->loc});
  }

  this->lexer->seek(loc1);
  return std::nullopt;
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExpr (
  const ParserStmtExpr &stmtExpr,
  ReaderLocation loc,
  const Token &tok,
  int precedence,
  const std::function<std::tuple<ParserStmtExpr, bool> (const ParserStmtExpr &, ReaderLocation, const Token &)> &wrap
) {
  if (std::holds_alternative<ParserExprAssign>(*stmtExpr.body) && !stmtExpr.parenthesized) {
    auto exprAssign = std::get<ParserExprAssign>(*stmtExpr.body);

    auto shouldWrapExpr = exprAssign.op.precedence() == precedence
      ? exprAssign.op.associativity() == TK_ASSOC_RIGHT
      : exprAssign.op.precedence() < precedence;

    if (shouldWrapExpr) {
      auto [newStmtExpr, shouldWrap] = this->_wrapExpr(exprAssign.right, loc, tok, precedence, wrap);
      auto newExprAssign = ParserExprAssign{exprAssign.left, exprAssign.op, newStmtExpr};
      auto newExpr = std::make_shared<ParserExpr>(newExprAssign);

      return std::make_tuple(ParserStmtExpr{newExpr, false, stmtExpr.start, newExprAssign.right.end}, shouldWrap);
    }
  } else if (std::holds_alternative<ParserExprBinary>(*stmtExpr.body) && !stmtExpr.parenthesized) {
    auto exprBinary = std::get<ParserExprBinary>(*stmtExpr.body);

    auto shouldWrapExpr = exprBinary.op.precedence() == precedence
      ? exprBinary.op.associativity() == TK_ASSOC_RIGHT
      : exprBinary.op.precedence() < precedence;

    if (shouldWrapExpr) {
      auto [newStmtExpr, shouldWrap] = this->_wrapExpr(exprBinary.right, loc, tok, precedence, wrap);
      auto newExprBinary = ParserExprBinary{exprBinary.left, exprBinary.op, newStmtExpr};
      auto newExpr = std::make_shared<ParserExpr>(newExprBinary);

      return std::make_tuple(ParserStmtExpr{newExpr, false, stmtExpr.start, newExprBinary.right.end}, shouldWrap);
    }
  } else if (std::holds_alternative<ParserExprCond>(*stmtExpr.body) && !stmtExpr.parenthesized) {
    auto exprCond = std::get<ParserExprCond>(*stmtExpr.body);
    auto tkQn = Token{TK_OP_QN};

    auto shouldWrapExpr = tkQn.precedence() == precedence
      ? tkQn.associativity() == TK_ASSOC_RIGHT
      : tkQn.precedence() < precedence;

    if (shouldWrapExpr) {
      auto [newStmtExpr, shouldWrap] = this->_wrapExpr(exprCond.alt, loc, tok, precedence, wrap);
      auto newExprCond = ParserExprCond{exprCond.cond, exprCond.body, newStmtExpr};
      auto newExpr = std::make_shared<ParserExpr>(newExprCond);

      return std::make_tuple(ParserStmtExpr{newExpr, false, stmtExpr.start, newExprCond.alt.end}, shouldWrap);
    }
  } else if (std::holds_alternative<ParserExprRef>(*stmtExpr.body) && !stmtExpr.parenthesized) {
    auto exprRef = std::get<ParserExprRef>(*stmtExpr.body);
    auto tkRef = Token{TK_KW_REF};

    if (tkRef.precedence() < precedence) {
      auto [newStmtExpr, shouldWrap] = this->_wrapExpr(exprRef.expr, loc, tok, precedence, wrap);

      if (!std::holds_alternative<ParserExprAccess>(*newStmtExpr.body)) {
        throw Error(this->reader, exprRef.expr.start, E0148);
      }

      auto newExprRef = ParserExprRef{newStmtExpr};
      auto newExpr = std::make_shared<ParserExpr>(newExprRef);

      return std::make_tuple(ParserStmtExpr{newExpr, false, stmtExpr.start, newExprRef.expr.end}, shouldWrap);
    }
  } else if (std::holds_alternative<ParserExprUnary>(*stmtExpr.body) && !stmtExpr.parenthesized) {
    auto exprUnary = std::get<ParserExprUnary>(*stmtExpr.body);

    if (exprUnary.prefix && exprUnary.op.precedence(true) < precedence) {
      auto [newStmtExpr, shouldWrap] = this->_wrapExpr(exprUnary.arg, loc, tok, precedence, wrap);
      auto newExprUnary = ParserExprUnary{newStmtExpr, exprUnary.op, exprUnary.prefix};
      auto newExpr = std::make_shared<ParserExpr>(newExprUnary);

      return std::make_tuple(ParserStmtExpr{newExpr, false, stmtExpr.start, newExprUnary.arg.end}, shouldWrap);
    }
  }

  return wrap(stmtExpr, loc, tok);
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprAccess (const ParserStmtExpr &stmtExpr, [[maybe_unused]] ReaderLocation loc, const Token &tok) {
  auto exprAccess = ParserExprAccess{stmtExpr, std::nullopt, std::nullopt};

  if (tok.type == TK_OP_DOT) {
    auto [_2, tok2] = this->lexer->next();

    if (tok2.type != TK_ID) {
      throw Error(this->reader, tok2.start, E0110);
    }

    exprAccess.prop = tok2;
  } else if (tok.type == TK_OP_LBRACK) {
    auto elemStmtExpr = this->_stmtExpr();

    if (elemStmtExpr == std::nullopt) {
      throw Error(this->reader, this->lexer->loc, E0150);
    }

    auto [_2, tok2] = this->lexer->next();

    if (tok2.type != TK_OP_RBRACK) {
      throw Error(this->reader, tok2.start, E0151);
    }

    exprAccess.elem = elemStmtExpr;
  }

  return std::make_tuple(ParserStmtExpr{std::make_shared<ParserExpr>(exprAccess), false, stmtExpr.start, this->lexer->loc}, true);
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprAssign (const ParserStmtExpr &stmtExpr, [[maybe_unused]] ReaderLocation loc, const Token &tok) {
  auto exprAssignRight = this->_stmtExpr(false);

  if (exprAssignRight == std::nullopt) {
    throw Error(this->reader, this->lexer->loc, E0133);
  } else if (!std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) {
    throw Error(this->reader, stmtExpr.start, stmtExpr.end, E0140);
  }

  auto exprAssign = ParserExprAssign{stmtExpr, tok, *exprAssignRight};
  return std::make_tuple(ParserStmtExpr{std::make_shared<ParserExpr>(exprAssign), false, stmtExpr.start, this->lexer->loc}, true);
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprBinary (const ParserStmtExpr &stmtExpr, [[maybe_unused]] ReaderLocation loc, const Token &tok) {
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
  return std::make_tuple(ParserStmtExpr{std::make_shared<ParserExpr>(exprBinary), false, stmtExpr.start, this->lexer->loc}, true);
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprCall (const ParserStmtExpr &stmtExpr, [[maybe_unused]] ReaderLocation loc, [[maybe_unused]] const Token &tok) {
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

  auto exprCall = ParserExprCall{stmtExpr, exprCallArgs};
  return std::make_tuple(ParserStmtExpr{std::make_shared<ParserExpr>(exprCall), false, stmtExpr.start, this->lexer->loc}, true);
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprCond (const ParserStmtExpr &stmtExpr, [[maybe_unused]] ReaderLocation loc, [[maybe_unused]] const Token &tok) {
  auto exprCondBody = this->_stmtExpr();

  if (exprCondBody == std::nullopt) {
    throw Error(this->reader, this->lexer->loc, E0138);
  }

  auto [_2, tok2] = this->lexer->next();

  if (tok2.type != TK_OP_COLON) {
    throw Error(this->reader, tok2.start, E0111);
  }

  auto exprCondAlt = this->_stmtExpr(false);

  if (exprCondAlt == std::nullopt) {
    throw Error(this->reader, this->lexer->loc, E0139);
  }

  auto exprCond = ParserExprCond{stmtExpr, *exprCondBody, *exprCondAlt};
  return std::make_tuple(ParserStmtExpr{std::make_shared<ParserExpr>(exprCond), false, stmtExpr.start, this->lexer->loc}, true);
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprObj (const ParserStmtExpr &stmtExpr, ReaderLocation loc, [[maybe_unused]] const Token &tok) {
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
  } else if (!std::holds_alternative<ParserExprAccess>(*stmtExpr.body)) {
    throw Error(this->reader, stmtExpr.start, stmtExpr.end, E0114);
  }

  auto exprAccess = std::get<ParserExprAccess>(*stmtExpr.body);

  if (!std::holds_alternative<Token>(exprAccess.expr)) {
    throw Error(this->reader, stmtExpr.start, stmtExpr.end, E0114);
  }

  auto exprObj = ParserExprObj{std::get<Token>(exprAccess.expr), exprObjProps};
  return std::make_tuple(ParserStmtExpr{std::make_shared<ParserExpr>(exprObj), false, stmtExpr.start, this->lexer->loc}, true);
}

std::tuple<ParserStmtExpr, bool> Parser::_wrapExprUnary (const ParserStmtExpr &stmtExpr, [[maybe_unused]] ReaderLocation loc, const Token &tok) {
  if (
    (tok.type == TK_OP_MINUS_MINUS || tok.type == TK_OP_PLUS_PLUS) &&
    !std::holds_alternative<ParserExprAccess>(*stmtExpr.body)
  ) {
    throw Error(this->reader, stmtExpr.start, stmtExpr.end, E0142);
  }

  auto exprUnary = ParserExprUnary{stmtExpr, tok, false};
  return std::make_tuple(ParserStmtExpr{std::make_shared<ParserExpr>(exprUnary), false, stmtExpr.start, tok.end}, true);
}

ParserStmt Parser::_wrapStmt (bool allowSemi, const ParserStmtBody &body, ReaderLocation start) const {
  if (allowSemi) {
    auto [loc, tok] = this->lexer->next();

    if (tok.type == TK_OP_SEMI) {
      return ParserStmt{std::make_shared<ParserStmtBody>(body), start, this->lexer->loc};
    }

    this->lexer->seek(loc);
  }

  return ParserStmt{std::make_shared<ParserStmtBody>(body), start, this->lexer->loc};
}

ParserStmtExpr Parser::_wrapStmtExpr (const ParserStmtExpr &stmtExpr) {
  auto [loc, tok] = this->lexer->next();

  if (
    tok.type == TK_OP_AMP ||
    tok.type == TK_OP_AMP_AMP ||
    tok.type == TK_OP_CARET ||
    tok.type == TK_OP_EQ_EQ ||
    tok.type == TK_OP_EXCL_EQ ||
    tok.type == TK_OP_GT ||
    tok.type == TK_OP_GT_EQ ||
    tok.type == TK_OP_LSHIFT ||
    tok.type == TK_OP_LT ||
    tok.type == TK_OP_LT_EQ ||
    tok.type == TK_OP_MINUS ||
    tok.type == TK_OP_PERCENT ||
    tok.type == TK_OP_PIPE ||
    tok.type == TK_OP_PIPE_PIPE ||
    tok.type == TK_OP_PLUS ||
    tok.type == TK_OP_RSHIFT ||
    tok.type == TK_OP_SLASH ||
    tok.type == TK_OP_STAR
  ) {
    auto [newStmtExpr, _] = this->_wrapExpr(stmtExpr, loc, tok, tok.precedence(), [&] (auto _1, auto _2, auto _3) {
      return this->_wrapExprBinary(_1, _2, _3);
    });

    return this->_wrapStmtExpr(newStmtExpr);
  } else if (
    tok.type == TK_OP_AMP_EQ ||
    tok.type == TK_OP_AMP_AMP_EQ ||
    tok.type == TK_OP_CARET_EQ ||
    tok.type == TK_OP_EQ ||
    tok.type == TK_OP_LSHIFT_EQ ||
    tok.type == TK_OP_MINUS_EQ ||
    tok.type == TK_OP_PERCENT_EQ ||
    tok.type == TK_OP_PIPE_EQ ||
    tok.type == TK_OP_PIPE_PIPE_EQ ||
    tok.type == TK_OP_PLUS_EQ ||
    tok.type == TK_OP_RSHIFT_EQ ||
    tok.type == TK_OP_SLASH_EQ ||
    tok.type == TK_OP_STAR_EQ
  ) {
    auto [newStmtExpr, _] = this->_wrapExpr(stmtExpr, loc, tok, tok.precedence(), [&] (auto _1, auto _2, auto _3) {
      return this->_wrapExprAssign(_1, _2, _3);
    });

    return this->_wrapStmtExpr(newStmtExpr);
  } else if (tok.type == TK_OP_DOT || tok.type == TK_OP_LBRACK) {
    auto [newStmtExpr, shouldWrap] = this->_wrapExpr(stmtExpr, loc, tok, tok.precedence(), [&] (auto _1, auto _2, auto _3) {
      return this->_wrapExprAccess(_1, _2, _3);
    });

    return this->_wrapStmtExpr(newStmtExpr);
  } else if (tok.type == TK_OP_LBRACE) {
    auto tkLpar = Token{TK_OP_LPAR};

    auto [newStmtExpr, shouldWrap] = this->_wrapExpr(stmtExpr, loc, tok, tkLpar.precedence(), [&] (auto _1, auto _2, auto _3) {
      return this->_wrapExprObj(_1, _2, _3);
    });

    return shouldWrap ? this->_wrapStmtExpr(newStmtExpr) : newStmtExpr;
  } else if (tok.type == TK_OP_LPAR) {
    auto [newStmtExpr, shouldWrap] = this->_wrapExpr(stmtExpr, loc, tok, tok.precedence(), [&] (auto _1, auto _2, auto _3) {
      return this->_wrapExprCall(_1, _2, _3);
    });

    return this->_wrapStmtExpr(newStmtExpr);
  } else if (tok.type == TK_OP_MINUS_MINUS || tok.type == TK_OP_PLUS_PLUS) {
    auto [newStmtExpr, shouldWrap] = this->_wrapExpr(stmtExpr, loc, tok, tok.precedence(true), [&] (auto _1, auto _2, auto _3) {
      return this->_wrapExprUnary(_1, _2, _3);
    });

    return this->_wrapStmtExpr(newStmtExpr);
  } else if (tok.type == TK_OP_QN) {
    auto [newStmtExpr, shouldWrap] = this->_wrapExpr(stmtExpr, loc, tok, tok.precedence(), [&] (auto _1, auto _2, auto _3) {
      return this->_wrapExprCond(_1, _2, _3);
    });

    return this->_wrapStmtExpr(newStmtExpr);
  }

  this->lexer->seek(loc);
  return stmtExpr;
}

ParserType Parser::_wrapType (const ParserType &type) {
  auto [loc1, tok1] = this->lexer->next();

  if (tok1.type == TK_OP_LBRACK) {
    auto [loc2, tok2] = this->lexer->next();

    if (tok2.type != TK_OP_RBRACK) {
      this->lexer->seek(loc2);
      throw Error(this->reader, this->lexer->loc, E0151);
    }

    auto typeArray = ParserTypeArray{type};
    return this->_wrapType(ParserType{std::make_shared<ParserTypeBody>(typeArray), false, type.start, this->lexer->loc});
  } else if (tok1.type == TK_OP_QN) {
    auto typeOptional = ParserTypeOptional{type};
    return this->_wrapType(ParserType{std::make_shared<ParserTypeBody>(typeOptional), false, type.start, this->lexer->loc});
  }

  this->lexer->seek(loc1);
  return type;
}
