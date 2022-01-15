/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <memory>
#include "Error.hpp"
#include "Parser.hpp"

ParserStmtExpr parseStmtExpr (Reader *, bool = false);

ParserBlock parseBlock (Reader *reader) {
  auto tok1 = lex(reader);

  if (tok1.type != TK_OP_LBRACE) {
    throw Error("Expected left brace");
  }

  auto block = ParserBlock{};

  while (true) {
    auto loc2 = reader->loc;
    auto tok2 = lex(reader);

    if (tok2.type == TK_EOF) {
      throw Error("Expected right brace");
    } else if (tok2.type == TK_OP_RBRACE) {
      break;
    }

    reader->seek(loc2);

    auto stmt = parse(reader);
    block.push_back(stmt);
  }

  return block;
}

ParserStmtExpr parsePostStmtExpr (Reader *reader, ParserStmtExpr &stmtExpr) {
  auto loc1 = reader->loc;
  auto tok1 = lex(reader);

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
    auto stmtExpr2 = parseStmtExpr(reader, true);

    if (std::holds_alternative<ParserExprBinary>(*stmtExpr2.expr)) {
      auto exprBinary2 = std::get<ParserExprBinary>(*stmtExpr2.expr);

      if (tokenPrecedence(tok1) >= tokenPrecedence(exprBinary2.op) && !stmtExpr2.parenthesized) {
        auto exprBinaryLeft = std::make_shared<ParserExpr>(ParserExprBinary{stmtExpr, tok1, exprBinary2.left});
        auto stmtExprLeft = ParserStmtExpr{exprBinaryLeft};
        auto expr = std::make_shared<ParserExpr>(ParserExprBinary{stmtExprLeft, exprBinary2.op, exprBinary2.right});

        return ParserStmtExpr{expr};
      }
    }

    auto exprBinary = ParserExprBinary{stmtExpr, tok1, stmtExpr2};
    auto expr = std::make_shared<ParserExpr>(exprBinary);
    auto stmtExpr3 = ParserStmtExpr{expr};

    return parsePostStmtExpr(reader, stmtExpr3);
  } else if (tok1.type == TK_OP_QN) {
    auto stmtExpr2 = parseStmtExpr(reader);
    auto tok2 = lex(reader);

    if (tok2.type != TK_OP_COLON) {
      throw Error("Expected colon");
    }

    auto stmtExpr3 = parseStmtExpr(reader);
    auto exprCond = ParserExprCond{stmtExpr, stmtExpr2, stmtExpr3};
    auto expr = std::make_shared<ParserExpr>(exprCond);
    auto stmtExpr4 = ParserStmtExpr{expr};

    return parsePostStmtExpr(reader, stmtExpr4);
  }

  reader->seek(loc1);
  return stmtExpr;
}

ParserStmtExpr parseStmtExpr (Reader *reader, bool singleStmt) {
  auto tok1 = lex(reader);

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

    return singleStmt ? stmtExpr : parsePostStmtExpr(reader, stmtExpr);
  } else if (tok1.type == TK_ID) {
    auto id = ParserId{tok1};
    auto exprAccess = ParserExprAccess{id};

    auto loc2 = reader->loc;
    auto tok2 = lex(reader);

    while (tok2.type == TK_OP_DOT) {
      auto tok3 = lex(reader);

      if (tok3.type != TK_ID) {
        throw Error("Expected property name after dot");
      }

      auto member = ParserMember{std::make_shared<ParserExprAccess>(exprAccess), ParserId{tok3}};
      exprAccess = ParserExprAccess{member};

      loc2 = reader->loc;
      tok2 = lex(reader);
    }

    reader->seek(loc2);

    auto loc4 = reader->loc;
    auto tok4 = lex(reader);

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
      auto stmtExprRight = parseStmtExpr(reader);
      auto exprAssign = ParserExprAssign{exprAccess, tok4, stmtExprRight};
      auto expr = std::make_shared<ParserExpr>(exprAssign);
      auto stmtExpr = ParserStmtExpr{expr};

      return singleStmt ? stmtExpr : parsePostStmtExpr(reader, stmtExpr);
    } else if (tok4.type == TK_OP_LBRACE) {
      auto tok5 = lex(reader);
      auto props = std::vector<ParserExprObjProp>();

      while (tok5.type != TK_OP_RBRACE) {
        if (tok5.type != TK_ID && props.empty()) {
          reader->seek(loc4);

          auto expr = std::make_shared<ParserExpr>(exprAccess);
          auto stmtExpr = ParserStmtExpr{expr};

          return stmtExpr;
        } else if (tok5.type != TK_ID) {
          throw Error("Expected object property name");
        }

        auto tok6 = lex(reader);

        if (tok6.type != TK_OP_COLON && props.empty()) {
          reader->seek(loc4);

          auto expr = std::make_shared<ParserExpr>(exprAccess);
          auto stmtExpr = ParserStmtExpr{expr};

          return stmtExpr;
        } else if (tok6.type != TK_OP_COLON) {
          throw Error("Expected colon after object property name");
        }

        auto init = parseStmtExpr(reader);
        auto prop = ParserExprObjProp{ParserId{tok5}, init};

        props.push_back(prop);
        tok5 = lex(reader);

        if (tok5.type == TK_OP_COMMA) {
          tok5 = lex(reader);
        }
      }

      if (!std::holds_alternative<ParserId>(exprAccess)) {
        throw Error("Only identifiers accepted as object names");
      }

      auto exprObj = ParserExprObj{std::get<ParserId>(exprAccess), props};
      auto expr = std::make_shared<ParserExpr>(exprObj);
      auto stmtExpr = ParserStmtExpr{expr};

      return singleStmt ? stmtExpr : parsePostStmtExpr(reader, stmtExpr);
    } else if (tok4.type == TK_OP_LPAR) {
      auto loc5 = reader->loc;
      auto tok5 = lex(reader);
      auto args = std::vector<ParserExprCallArg>();

      while (tok5.type != TK_OP_RPAR) {
        reader->seek(loc5);

        auto argId = std::optional<ParserId>{};
        auto loc6 = reader->loc;
        auto tok6 = lex(reader);

        if (tok6.type == TK_ID) {
          auto tok7 = lex(reader);

          if (tok7.type == TK_OP_COLON) {
            argId = ParserId{tok6};
          }
        }

        if (argId == std::nullopt) {
          reader->seek(loc6);
        }

        auto argStmtExpr = parseStmtExpr(reader);
        args.push_back(ParserExprCallArg{argId, argStmtExpr});

        tok5 = lex(reader);

        if (tok5.type == TK_OP_COMMA) {
          loc5 = reader->loc;
          tok5 = lex(reader);
        }
      }

      auto exprCall = ParserExprCall{exprAccess, args};
      auto expr = std::make_shared<ParserExpr>(exprCall);
      auto stmtExpr = ParserStmtExpr{expr};

      return singleStmt ? stmtExpr : parsePostStmtExpr(reader, stmtExpr);
    } else if (tok4.type == TK_OP_MINUS_MINUS || tok4.type == TK_OP_PLUS_PLUS) {
      auto exprUnaryArgExpr = std::make_shared<ParserExpr>(exprAccess);
      auto exprUnaryArg = ParserStmtExpr{exprUnaryArgExpr};
      auto exprUnary = ParserExprUnary{exprUnaryArg, tok4};
      auto expr = std::make_shared<ParserExpr>(exprUnary);
      auto stmtExpr = ParserStmtExpr{expr};

      return singleStmt ? stmtExpr : parsePostStmtExpr(reader, stmtExpr);
    }

    reader->seek(loc4);

    auto expr = std::make_shared<ParserExpr>(exprAccess);
    auto stmtExpr = ParserStmtExpr{expr};

    return singleStmt ? stmtExpr : parsePostStmtExpr(reader, stmtExpr);
  } else if (
    tok1.type == TK_OP_EXCL ||
    tok1.type == TK_OP_EXCL_EXCL ||
    tok1.type == TK_OP_MINUS ||
    tok1.type == TK_OP_MINUS_MINUS ||
    tok1.type == TK_OP_PLUS ||
    tok1.type == TK_OP_PLUS_PLUS ||
    tok1.type == TK_OP_TILDE
  ) {
    auto exprUnaryArg = parseStmtExpr(reader, true);
    auto exprUnary = ParserExprUnary{exprUnaryArg, tok1, true};
    auto expr = std::make_shared<ParserExpr>(exprUnary);
    auto stmtExpr = ParserStmtExpr{expr};

    return singleStmt ? stmtExpr : parsePostStmtExpr(reader, stmtExpr);
  } else if (tok1.type == TK_OP_LPAR) {
    auto stmtExpr = parseStmtExpr(reader);
    auto tok2 = lex(reader);

    if (tok2.type != TK_OP_RPAR) {
      throw Error("Expected right parentheses");
    }

    stmtExpr.parenthesized = true;
    return singleStmt ? stmtExpr : parsePostStmtExpr(reader, stmtExpr);
  }

  throw Error("Unknown expression statement");
}

ParserStmt parseStmtLoopFor (Reader *reader, const std::optional<std::shared_ptr<ParserStmt>> &stmt) {
  auto stmtExpr1 = std::optional<ParserStmtExpr>{};
  auto stmtExpr2 = std::optional<ParserStmtExpr>{};
  auto loc1 = reader->loc;
  auto tok1 = lex(reader);

  if (tok1.type != TK_OP_SEMI) {
    reader->seek(loc1);

    stmtExpr1 = parseStmtExpr(reader);
    auto tok2 = lex(reader);

    if (tok2.type != TK_OP_SEMI) {
      throw Error("Expected semicolon after loop condition");
    }
  }

  auto loc3 = reader->loc;
  auto tok3 = lex(reader);

  reader->seek(loc3);

  if (tok3.type != TK_OP_LBRACE) {
    stmtExpr2 = parseStmtExpr(reader);
  }

  auto block = parseBlock(reader);
  auto stmtLoop = ParserStmtLoop{stmt, stmtExpr1, stmtExpr2, block};

  return ParserStmt{stmtLoop};
}

ParserStmt parse (Reader *reader) {
  auto loc1 = reader->loc;
  auto tok1 = lex(reader);

  if (tok1.type == TK_EOF) {
    auto stmtEnd = ParserStmtEnd{};
    return ParserStmt{stmtEnd};
  }

  if (tok1.type == TK_KW_BREAK) {
    auto stmtBreak = ParserStmtBreak{};
    return ParserStmt{stmtBreak};
  } else if (tok1.type == TK_KW_CONTINUE) {
    auto stmtContinue = ParserStmtContinue{};
    return ParserStmt{stmtContinue};
  } else if (tok1.type == TK_KW_FN) {
    auto tok2 = lex(reader);

    if (tok2.type != TK_ID) {
      throw Error("Expected function identifier");
    }

    auto tok3 = lex(reader);

    if (tok3.type != TK_OP_LPAR) {
      throw Error("Expected left parentheses");
    }

    auto tok4 = lex(reader);
    auto params = std::vector<ParserStmtFnDeclParam>();

    while (tok4.type != TK_OP_RPAR) {
      if (tok4.type != TK_ID) {
        throw Error("Expected identifier as function parameter name");
      }

      auto type = std::optional<ParserId>{};
      auto init = std::optional<ParserStmtExpr>{};
      auto tok5 = lex(reader);

      if (tok5.type == TK_OP_COLON) {
        auto tok6 = lex(reader);

        if (tok6.type != TK_ID) {
          throw Error("Expected function parameter type after colon");
        }

        type = ParserId{tok6};

        auto loc7 = reader->loc;
        auto tok7 = lex(reader);

        if (tok7.type == TK_OP_EQ) {
          init = parseStmtExpr(reader);
        } else {
          reader->seek(loc7);
        }
      } else if (tok5.type == TK_OP_COLON_EQ) {
        init = parseStmtExpr(reader);
      } else {
        throw Error("Expected function parameter type after parameter name");
      }

      auto id = ParserId{tok4};
      params.push_back(ParserStmtFnDeclParam{id, type, init});

      tok4 = lex(reader);

      if (tok4.type == TK_OP_COMMA) {
        tok4 = lex(reader);
      }
    }

    auto tok8 = lex(reader);

    if (tok8.type != TK_ID) {
      throw Error("Expected function return type after parameters list");
    }

    auto block = parseBlock(reader);
    auto id = ParserId{tok2};
    auto returnType = ParserId{tok8};
    auto stmtFnDecl = ParserStmtFnDecl{id, returnType, params, block};

    return ParserStmt{stmtFnDecl};
  } else if (tok1.type == TK_KW_IF) {
    auto stmtExpr = parseStmtExpr(reader);
    auto block = parseBlock(reader);
    auto alt = std::optional<std::shared_ptr<ParserStmtIfCond>>{};
    auto altTail = std::optional<std::shared_ptr<ParserStmtIfCond>>{};

    while (true) {
      auto loc2 = reader->loc;
      auto tok2 = lex(reader);
      auto stmtIfAlt = std::shared_ptr<ParserStmtIfCond>{};

      if (tok2.type == TK_KW_ELIF) {
        auto stmtExprElif = parseStmtExpr(reader);
        auto blockElif = parseBlock(reader);

        stmtIfAlt = std::make_shared<ParserStmtIfCond>(ParserStmtIf{stmtExprElif, blockElif, std::nullopt});
      } else if (tok2.type == TK_KW_ELSE) {
        stmtIfAlt = std::make_shared<ParserStmtIfCond>(parseBlock(reader));
      } else {
        reader->seek(loc2);
        break;
      }

      if (alt == std::nullopt) {
        alt = stmtIfAlt;
      }

      if (altTail != std::nullopt) {
        auto &stmtIf = std::get<ParserStmtIf>(**altTail);
        stmtIf.alt = stmtIfAlt;
      }

      altTail = stmtIfAlt;

      if (tok2.type == TK_KW_ELSE) {
        break;
      }
    }

    auto stmtIf = ParserStmtIf{stmtExpr, block, alt};
    return ParserStmt{stmtIf};
  } else if (tok1.type == TK_KW_LOOP) {
    auto loc2 = reader->loc;
    auto tok2 = lex(reader);

    if (tok2.type == TK_OP_LBRACE) {
      reader->seek(loc2);

      auto block = parseBlock(reader);
      auto stmtLoop = ParserStmtLoop{std::nullopt, std::nullopt, std::nullopt, block};

      return ParserStmt{stmtLoop};
    } else if (tok2.type == TK_OP_SEMI) {
      return parseStmtLoopFor(reader, std::nullopt);
    } else {
      reader->seek(loc2);
      auto stmt = parse(reader);

      if (!std::holds_alternative<ParserStmtExpr>(stmt) && !std::holds_alternative<ParserStmtVarDecl>(stmt)) {
        throw Error("Unexpected statement in loop condition");
      }

      auto loc3 = reader->loc;
      auto tok3 = lex(reader);

      if (std::holds_alternative<ParserStmtVarDecl>(stmt) && tok3.type != TK_OP_SEMI) {
        throw Error("Expected semicolon after loop initialization");
      } else if (std::holds_alternative<ParserStmtExpr>(stmt) && tok3.type != TK_OP_LBRACE && tok3.type != TK_OP_SEMI) {
        throw Error("Unexpected token after loop initialization");
      } else if (std::holds_alternative<ParserStmtExpr>(stmt) && tok3.type == TK_OP_LBRACE) {
        reader->seek(loc3);

        auto stmtExpr = std::get<ParserStmtExpr>(stmt);
        auto block = parseBlock(reader);
        auto stmtLoop = ParserStmtLoop{std::nullopt, stmtExpr, std::nullopt, block};

        return ParserStmt{stmtLoop};
      }

      return parseStmtLoopFor(reader, std::make_shared<ParserStmt>(stmt));
    }
  } else if (tok1.type == TK_KW_MAIN) {
    auto block = parseBlock(reader);
    auto stmtMain = ParserStmtMain{block};

    return ParserStmt{stmtMain};
  } else if (tok1.type == TK_KW_MUT) {
    auto tok2 = lex(reader);

    if (tok2.type == TK_ID) {
      auto tok3 = lex(reader);

      if (tok3.type == TK_OP_COLON) {
        auto tok4 = lex(reader);

        if (tok4.type != TK_ID) {
          throw Error("Expected variable type after colon");
        }

        auto loc5 = reader->loc;
        auto tok5 = lex(reader);
        auto init = std::optional<ParserStmtExpr>{};

        if (tok5.type == TK_OP_EQ) {
          init = parseStmtExpr(reader);
        } else {
          reader->seek(loc5);
        }

        auto type = ParserId{tok4};
        auto id = ParserId{tok2};
        auto stmtVarDecl = ParserStmtVarDecl{id, type, init, true};

        return ParserStmt{stmtVarDecl};
      } else if (tok3.type == TK_OP_COLON_EQ) {
        auto stmtExpr = parseStmtExpr(reader);
        auto id = ParserId{tok2};
        auto stmtVarDecl = ParserStmtVarDecl{id, std::nullopt, stmtExpr, true};

        return ParserStmt{stmtVarDecl};
      }
    }
  } else if (tok1.type == TK_KW_OBJ) {
    auto tok2 = lex(reader);

    if (tok2.type != TK_ID) {
      throw Error("Expected object identifier");
    }

    auto tok3 = lex(reader);

    if (tok3.type != TK_OP_LBRACE) {
      throw Error("Expected left brace after object identifier");
    }

    auto tok4 = lex(reader);
    auto fields = std::vector<ParserStmtObjDeclField>();

    while (tok4.type != TK_OP_RBRACE) {
      if (tok4.type != TK_ID) {
        throw Error("Expected object field name");
      }

      auto tok5 = lex(reader);

      if (tok5.type != TK_OP_COLON) {
        throw Error("Expected colon after object field name");
      }

      auto tok6 = lex(reader);

      if (tok6.type != TK_ID) {
        throw Error("Expected object field type after field name");
      }

      auto fieldId = ParserId{tok4};
      auto fieldType = ParserId{tok6};
      fields.push_back(ParserStmtObjDeclField{fieldId, fieldType});

      tok4 = lex(reader);

      if (tok4.type == TK_OP_COMMA) {
        tok4 = lex(reader);
      }
    }

    if (fields.empty()) {
      throw Error("Object declaration without fields is not allowed");
    }

    auto id = ParserId{tok2};
    auto stmtObjDecl = ParserStmtObjDecl{id, fields};

    return ParserStmt{stmtObjDecl};
  } else if (tok1.type == TK_KW_RETURN) {
    auto stmtExpr = parseStmtExpr(reader);
    auto stmtReturn = ParserStmtReturn{stmtExpr};

    return ParserStmt{stmtReturn};
  } else if (tok1.type == TK_ID) {
    auto tok2 = lex(reader);

    if (tok2.type == TK_OP_COLON) {
      auto tok3 = lex(reader);

      if (tok3.type != TK_ID) {
        throw Error("Expected variable type after colon");
      }

      auto loc4 = reader->loc;
      auto tok4 = lex(reader);
      auto init = std::optional<ParserStmtExpr>{};

      if (tok4.type == TK_OP_EQ) {
        init = parseStmtExpr(reader);
      } else {
        reader->seek(loc4);
      }

      auto type = ParserId{tok3};
      auto id = ParserId{tok1};
      auto stmtVarDecl = ParserStmtVarDecl{id, type, init};

      return ParserStmt{stmtVarDecl};
    } else if (tok2.type == TK_OP_COLON_EQ) {
      auto stmtExpr = parseStmtExpr(reader);
      auto id = ParserId{tok1};
      auto stmtVarDecl = ParserStmtVarDecl{id, std::nullopt, stmtExpr};

      return ParserStmt{stmtVarDecl};
    }
  }

  reader->seek(loc1);

  try {
    auto stmtExpr = parseStmtExpr(reader);
    return ParserStmt{stmtExpr};
  } catch (const Error &err) {
  }

  reader->seek(loc1);
  throw SyntaxError(reader, loc1, E0100);
}
