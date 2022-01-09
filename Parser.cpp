/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "Parser.hpp"

ParserStmtExpr parseStmtExpr (Reader *, bool = true);

void parseWalkWhitespace (Reader *reader) {
  while (true) {
    auto loc = reader->loc;
    auto tok = lex(reader);

    if (tok.type != TK_COMMENT_BLOCK && tok.type != TK_COMMENT_LINE && tok.type != TK_WHITESPACE) {
      reader->seek(loc);
      break;
    }
  }
}

ParserBlock parseBlock (Reader *reader) {
  parseWalkWhitespace(reader);
  auto tok1 = lex(reader);

  if (tok1.type != TK_OP_LBRACE) {
    throw Error("Expected left brace");
  }

  auto block = ParserBlock{};

  while (true) {
    auto loc2 = reader->loc;
    parseWalkWhitespace(reader);
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

ParserExprAccess parseExprAccess (Reader *reader, ParserExprAccess stmtExprAccess) {
  auto loc1 = reader->loc;
  parseWalkWhitespace(reader);
  auto tok1 = lex(reader);

  while (tok1.type == TK_OP_DOT) {
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type != TK_LIT_ID) {
      throw Error("Expected property name after dot");
    }

    auto parserId = ParserId{tok2};
    auto parserMember = ParserMember{std::make_shared<ParserExprAccess>(stmtExprAccess), parserId};
    stmtExprAccess = ParserExprAccess{parserMember};

    loc1 = reader->loc;
    parseWalkWhitespace(reader);
    tok1 = lex(reader);
  }

  reader->seek(loc1);
  return stmtExprAccess;
}

ParserStmtExpr parsePostStmtExpr (Reader *reader, ParserStmtExpr stmtExpr) {
  auto loc1 = reader->loc;
  parseWalkWhitespace(reader);
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
    parseWalkWhitespace(reader);
    auto stmtExpr2 = parseStmtExpr(reader);

    if (std::holds_alternative<ParserExprBinary>(*stmtExpr2.expr)) {
      auto stmtExprBinary2 = std::get<ParserExprBinary>(*stmtExpr2.expr);

      if (tokenPrecedence(tok1) >= tokenPrecedence(stmtExprBinary2.op) && !stmtExpr2.parenthesized) {
        auto exprBinaryLeft = std::make_shared<ParserExpr>(ParserExprBinary{stmtExpr, tok1, stmtExprBinary2.left});
        auto stmtExprLeft = ParserStmtExpr{exprBinaryLeft};
        auto exprBinaryRight = std::make_shared<ParserExpr>(ParserExprBinary{stmtExprLeft, stmtExprBinary2.op, stmtExprBinary2.right});

        return ParserStmtExpr{exprBinaryRight};
      }
    }

    auto exprBinary = std::make_shared<ParserExpr>(ParserExprBinary{stmtExpr, tok1, stmtExpr2});
    return ParserStmtExpr{exprBinary};
  } else if (tok1.type == TK_OP_QN) {
    parseWalkWhitespace(reader);

    auto stmtExpr2 = parseStmtExpr(reader);
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type != TK_OP_COLON) {
      throw Error("Expected colon");
    }

    parseWalkWhitespace(reader);

    auto stmtExpr3 = parseStmtExpr(reader);
    auto exprCond = std::make_shared<ParserExpr>(ParserExprCond{stmtExpr, stmtExpr2, stmtExpr3});

    return ParserStmtExpr{exprCond};
  } else if (std::holds_alternative<ParserExprAccess>(*stmtExpr.expr) && tok1.type == TK_OP_DOT) {
    reader->seek(loc1);

    auto stmtExprAccess = std::get<ParserExprAccess>(*stmtExpr.expr);
    auto exprAccess = std::make_shared<ParserExpr>(parseExprAccess(reader, stmtExprAccess));
    stmtExpr.expr = exprAccess;

    return parsePostStmtExpr(reader, stmtExpr);
  } else if (std::holds_alternative<ParserExprAccess>(*stmtExpr.expr) && (
    tok1.type == TK_OP_AND_AND_EQ ||
    tok1.type == TK_OP_AND_EQ ||
    tok1.type == TK_OP_CARET_EQ ||
    tok1.type == TK_OP_EQ ||
    tok1.type == TK_OP_LSHIFT_EQ ||
    tok1.type == TK_OP_MINUS_EQ ||
    tok1.type == TK_OP_OR_EQ ||
    tok1.type == TK_OP_OR_OR_EQ ||
    tok1.type == TK_OP_PERCENT_EQ ||
    tok1.type == TK_OP_PLUS_EQ ||
    tok1.type == TK_OP_QN_QN_EQ ||
    tok1.type == TK_OP_RSHIFT_EQ ||
    tok1.type == TK_OP_SLASH_EQ ||
    tok1.type == TK_OP_STAR_EQ ||
    tok1.type == TK_OP_STAR_STAR_EQ
  )) {
    parseWalkWhitespace(reader);

    auto stmtExprAccess = std::get<ParserExprAccess>(*stmtExpr.expr);
    auto rightStmtExpr = parseStmtExpr(reader);
    auto exprAssign = std::make_shared<ParserExpr>(ParserExprAssign{stmtExprAccess, tok1, rightStmtExpr});
    stmtExpr.expr = exprAssign;

    return parsePostStmtExpr(reader, stmtExpr);
  } else if (std::holds_alternative<ParserExprAccess>(*stmtExpr.expr) && tok1.type == TK_OP_LBRACE) {
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);
    auto props = std::vector<ParserExprObjProp>();

    while (tok2.type != TK_OP_RBRACE) {
      if (tok2.type != TK_LIT_ID && props.empty()) {
        reader->seek(loc1);
        return stmtExpr;
      } else if (tok2.type != TK_LIT_ID) {
        throw Error("Expected object property name");
      }

      parseWalkWhitespace(reader);
      auto tok3 = lex(reader);

      if (tok3.type != TK_OP_COLON && props.empty()) {
        reader->seek(loc1);
        return stmtExpr;
      } else if (tok3.type != TK_OP_COLON) {
        throw Error("Expected colon after object property name");
      }

      parseWalkWhitespace(reader);

      auto init = parseStmtExpr(reader);
      auto parserId = ParserId{tok2};
      auto prop = ParserExprObjProp{parserId, init};

      props.push_back(prop);

      parseWalkWhitespace(reader);
      tok2 = lex(reader);

      if (tok2.type == TK_OP_COMMA) {
        parseWalkWhitespace(reader);
        tok2 = lex(reader);
      }
    }

    auto stmtExprAccess = std::get<ParserExprAccess>(*stmtExpr.expr);

    if (!std::holds_alternative<ParserId>(stmtExprAccess)) {
      throw Error("Only identifiers accepted as object names");
    }

    auto parserId = std::get<ParserId>(stmtExprAccess);
    auto exprObj = std::make_shared<ParserExpr>(ParserExprObj{parserId, props});
    stmtExpr.expr = exprObj;

    return parsePostStmtExpr(reader, stmtExpr);
  } else if (std::holds_alternative<ParserExprAccess>(*stmtExpr.expr) && tok1.type == TK_OP_LPAR) {
    auto loc2 = reader->loc;
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);
    auto args = std::vector<ParserExprCallArg>();

    while (tok2.type != TK_OP_RPAR) {
      reader->seek(loc2);

      auto argId = std::optional<ParserId>{};
      auto loc3 = reader->loc;
      parseWalkWhitespace(reader);
      auto tok3 = lex(reader);

      if (tok3.type == TK_LIT_ID) {
        parseWalkWhitespace(reader);
        auto tok4 = lex(reader);

        if (tok4.type == TK_OP_COLON) {
          argId = ParserId{tok3};
        }
      }

      if (argId == std::nullopt) {
        reader->seek(loc3);
      }

      parseWalkWhitespace(reader);
      auto argStmtExpr = parseStmtExpr(reader);

      args.push_back(ParserExprCallArg{argId, argStmtExpr});
      parseWalkWhitespace(reader);

      tok2 = lex(reader);

      if (tok2.type == TK_OP_COMMA) {
        loc2 = reader->loc;
        parseWalkWhitespace(reader);

        tok2 = lex(reader);
      }
    }

    auto stmtExprAccess = std::get<ParserExprAccess>(*stmtExpr.expr);
    auto exprCall = std::make_shared<ParserExpr>(ParserExprCall{stmtExprAccess, args});
    return parsePostStmtExpr(reader, ParserStmtExpr{exprCall});
  } else if (tok1.type == TK_OP_MINUS_MINUS || tok1.type == TK_OP_PLUS_PLUS) {
    if (!std::holds_alternative<ParserExprAccess>(*stmtExpr.expr)) {
      throw Error("Only access expressions can be used with increment/decrement operators");
    }

    auto exprUnary = std::make_shared<ParserExpr>(ParserExprUnary{stmtExpr, tok1});
    return parsePostStmtExpr(reader, ParserStmtExpr{exprUnary});
  }

  reader->seek(loc1);
  return stmtExpr;
}

ParserStmtExpr parseStmtExpr (Reader *reader, bool shouldPostParse) {
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

    return shouldPostParse ? parsePostStmtExpr(reader, stmtExpr) : stmtExpr;
  } else if (tok1.type == TK_LIT_ID) {
    auto parserId = ParserId{tok1};
    auto exprAccess = std::make_shared<ParserExpr>(ParserExprAccess{parserId});
    auto stmtExpr = ParserStmtExpr{exprAccess};

    return shouldPostParse ? parsePostStmtExpr(reader, stmtExpr) : stmtExpr;
  } else if (
    tok1.type == TK_OP_EXCL ||
    tok1.type == TK_OP_EXCL_EXCL ||
    tok1.type == TK_OP_MINUS ||
    tok1.type == TK_OP_MINUS_MINUS ||
    tok1.type == TK_OP_PLUS ||
    tok1.type == TK_OP_PLUS_PLUS ||
    tok1.type == TK_OP_TILDE
  ) {
    parseWalkWhitespace(reader);

    auto exprUnaryArg = parseStmtExpr(reader, false);
    auto exprUnary = std::make_shared<ParserExpr>(ParserExprUnary{exprUnaryArg, tok1, true});
    auto stmtExpr = ParserStmtExpr{exprUnary};

    return shouldPostParse ? parsePostStmtExpr(reader, stmtExpr) : stmtExpr;
  } else if (tok1.type == TK_OP_LPAR) {
    parseWalkWhitespace(reader);

    auto stmtExpr = parseStmtExpr(reader);
    parseWalkWhitespace(reader);

    auto tok2 = lex(reader);

    if (tok2.type != TK_OP_RPAR) {
      throw Error("Expected right parentheses");
    }

    stmtExpr.parenthesized = true;
    return shouldPostParse ? parsePostStmtExpr(reader, stmtExpr) : stmtExpr;
  }

  throw Error("Unknown expression statement");
}

ParserStmt parseStmtLoopFor (Reader *reader, const std::optional<std::shared_ptr<ParserStmt>> &stmt) {
  auto stmtExpr2 = std::optional<ParserStmtExpr>{};
  auto stmtExpr3 = std::optional<ParserStmtExpr>{};

  auto loc1 = reader->loc;
  parseWalkWhitespace(reader);
  auto tok1 = lex(reader);

  if (tok1.type != TK_OP_SEMI) {
    reader->seek(loc1);
    parseWalkWhitespace(reader);

    stmtExpr2 = parseStmtExpr(reader);
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type != TK_OP_SEMI) {
      throw Error("Expected semicolon after loop condition");
    }
  }

  auto loc3 = reader->loc;
  parseWalkWhitespace(reader);
  auto tok3 = lex(reader);

  reader->seek(loc3);

  if (tok3.type != TK_OP_LBRACE) {
    parseWalkWhitespace(reader);
    stmtExpr3 = parseStmtExpr(reader);
  }

  auto parserBlock = parseBlock(reader);
  auto stmtLoop = ParserStmtLoop{stmt, stmtExpr2, stmtExpr3, parserBlock};
  return ParserStmt{stmtLoop};
}

ParserStmt parse (Reader *reader) {
  parseWalkWhitespace(reader);

  if (reader->eof()) {
    auto stmtEnd = ParserStmtEnd{};
    return ParserStmt{stmtEnd};
  }

  auto loc1 = reader->loc;
  auto tok1 = lex(reader);

  if (tok1.type == TK_KW_BREAK) {
    auto stmtBreak = ParserStmtBreak{};
    return ParserStmt{stmtBreak};
  } else if (tok1.type == TK_KW_CONTINUE) {
    auto stmtContinue = ParserStmtContinue{};
    return ParserStmt{stmtContinue};
  } else if (tok1.type == TK_KW_FN) {
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type != TK_LIT_ID) {
      throw Error("Expected function identifier");
    }

    parseWalkWhitespace(reader);
    auto tok3 = lex(reader);

    if (tok3.type != TK_OP_LPAR) {
      throw Error("Expected left parentheses");
    }

    parseWalkWhitespace(reader);
    auto tok4 = lex(reader);
    auto params = std::vector<ParserStmtFnDeclParam>();

    while (tok4.type != TK_OP_RPAR) {
      if (tok4.type != TK_LIT_ID) {
        throw Error("Expected identifier as function parameter name");
      }

      parseWalkWhitespace(reader);
      auto tok5 = lex(reader);
      auto type = std::optional<ParserId>{};
      auto init = std::optional<ParserStmtExpr>{};

      if (tok5.type == TK_OP_COLON) {
        parseWalkWhitespace(reader);
        auto tok6 = lex(reader);

        if (tok6.type != TK_LIT_ID) {
          throw Error("Expected function parameter type after colon");
        }

        type = ParserId{tok6};

        auto loc7 = reader->loc;
        parseWalkWhitespace(reader);
        auto tok7 = lex(reader);

        if (tok7.type == TK_OP_EQ) {
          parseWalkWhitespace(reader);
          init = parseStmtExpr(reader);
        } else {
          reader->seek(loc7);
        }
      } else if (tok5.type == TK_OP_COLON_EQ) {
        parseWalkWhitespace(reader);
        init = parseStmtExpr(reader);
      } else {
        throw Error("Expected function parameter type after parameter name");
      }

      auto paramId = ParserId{tok4};
      auto param = ParserStmtFnDeclParam{paramId, type, init};
      params.push_back(param);

      parseWalkWhitespace(reader);
      tok4 = lex(reader);

      if (tok4.type == TK_OP_COMMA) {
        parseWalkWhitespace(reader);
        tok4 = lex(reader);
      }
    }

    parseWalkWhitespace(reader);
    auto tok8 = lex(reader);

    if (tok8.type != TK_LIT_ID) {
      throw Error("Expected function return type after parameters list");
    }

    auto parserBlock = parseBlock(reader);
    auto parserId = ParserId{tok2};
    auto returnType = ParserId{tok8};
    auto stmtFnDecl = ParserStmtFnDecl{parserId, returnType, params, parserBlock};

    return ParserStmt{stmtFnDecl};
  } else if (tok1.type == TK_KW_IF) {
    parseWalkWhitespace(reader);

    auto stmtExpr = parseStmtExpr(reader);
    auto parserBlock = parseBlock(reader);
    auto alt = std::optional<std::shared_ptr<ParserStmtIfCond>>{};
    auto altTail = std::optional<std::shared_ptr<ParserStmtIfCond>>{};

    while (true) {
      auto loc2 = reader->loc;
      parseWalkWhitespace(reader);
      auto tok2 = lex(reader);
      auto stmtIfAlt = std::shared_ptr<ParserStmtIfCond>{};

      if (tok2.type == TK_KW_ELIF) {
        parseWalkWhitespace(reader);
        auto stmtExprElif = parseStmtExpr(reader);
        auto blockElif = parseBlock(reader);

        stmtIfAlt = std::make_shared<ParserStmtIfCond>(ParserStmtIf{stmtExprElif, blockElif, std::nullopt});
      } else if (tok2.type == TK_KW_ELSE) {
        parseWalkWhitespace(reader);
        stmtIfAlt = std::make_shared<ParserStmtIfCond>(parseBlock(reader));
      } else {
        reader->seek(loc2);
        break;
      }

      if (alt == std::nullopt) {
        alt = stmtIfAlt;
      }

      if (altTail != std::nullopt) {
        std::get<ParserStmtIf>(*altTail.value()).alt = stmtIfAlt;
      }

      altTail = stmtIfAlt;

      if (tok2.type == TK_KW_ELSE) {
        break;
      }
    }

    auto stmtIf = ParserStmtIf{stmtExpr, parserBlock, alt};
    return ParserStmt{stmtIf};
  } else if (tok1.type == TK_KW_LOOP) {
    auto loc2 = reader->loc;
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type == TK_OP_LBRACE) {
      reader->seek(loc2);

      auto parserBlock = parseBlock(reader);
      auto stmtLoop = ParserStmtLoop{std::nullopt, std::nullopt, std::nullopt, parserBlock};
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
      parseWalkWhitespace(reader);
      auto tok3 = lex(reader);

      if (std::holds_alternative<ParserStmtVarDecl>(stmt) && tok3.type != TK_OP_SEMI) {
        throw Error("Expected semicolon after loop initialization");
      } else if (std::holds_alternative<ParserStmtExpr>(stmt) && tok3.type != TK_OP_LBRACE && tok3.type != TK_OP_SEMI) {
        throw Error("Unexpected token after loop initialization");
      } else if (std::holds_alternative<ParserStmtExpr>(stmt) && tok3.type == TK_OP_LBRACE) {
        reader->seek(loc3);

        auto stmtExpr = std::get<ParserStmtExpr>(stmt);
        auto parserBlock = parseBlock(reader);
        auto stmtLoop = ParserStmtLoop{std::nullopt, stmtExpr, std::nullopt, parserBlock};

        return ParserStmt{stmtLoop};
      }

      return parseStmtLoopFor(reader, std::make_shared<ParserStmt>(stmt));
    }
  } else if (tok1.type == TK_KW_MAIN) {
    auto parserBlock = parseBlock(reader);
    auto stmtMain = ParserStmtMain{parserBlock};
    return ParserStmt{stmtMain};
  } else if (tok1.type == TK_KW_MUT) {
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type == TK_LIT_ID) {
      parseWalkWhitespace(reader);
      auto tok3 = lex(reader);

      if (tok3.type == TK_OP_COLON) {
        parseWalkWhitespace(reader);
        auto tok4 = lex(reader);

        if (tok4.type != TK_LIT_ID) {
          throw Error("Expected variable type after colon");
        }

        auto loc5 = reader->loc;
        parseWalkWhitespace(reader);
        auto tok5 = lex(reader);
        auto initStmtExpr = std::optional<ParserStmtExpr>{};

        if (tok5.type == TK_OP_EQ) {
          parseWalkWhitespace(reader);
          initStmtExpr = parseStmtExpr(reader);
        } else {
          reader->seek(loc5);
        }

        auto type = ParserId{tok4};
        auto parserId = ParserId{tok2};
        auto stmtVarDecl = ParserStmtVarDecl{parserId, type, initStmtExpr, true};

        return ParserStmt{stmtVarDecl};
      } else if (tok3.type == TK_OP_COLON_EQ) {
        parseWalkWhitespace(reader);

        auto stmtExpr = parseStmtExpr(reader);
        auto parserId = ParserId{tok2};
        auto stmtVarDecl = ParserStmtVarDecl{parserId, std::nullopt, stmtExpr, true};

        return ParserStmt{stmtVarDecl};
      }
    }
  } else if (tok1.type == TK_KW_OBJ) {
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type != TK_LIT_ID) {
      throw Error("Expected object identifier");
    }

    parseWalkWhitespace(reader);
    auto tok3 = lex(reader);

    if (tok3.type != TK_OP_LBRACE) {
      throw Error("Expected left brace after object identifier");
    }

    parseWalkWhitespace(reader);
    auto tok4 = lex(reader);
    auto fields = std::vector<ParserStmtObjDeclField>();

    while (tok4.type != TK_OP_RBRACE) {
      if (tok4.type != TK_LIT_ID) {
        throw Error("Expected object field name");
      }

      parseWalkWhitespace(reader);
      auto tok5 = lex(reader);

      if (tok5.type != TK_OP_COLON) {
        throw Error("Expected colon after object field name");
      }

      parseWalkWhitespace(reader);
      auto tok6 = lex(reader);

      if (tok6.type != TK_LIT_ID) {
        throw Error("Expected object field type after field name");
      }

      auto fieldId = ParserId{tok4};
      auto fieldType = ParserId{tok6};
      auto field = ParserStmtObjDeclField{fieldId, fieldType};
      fields.push_back(field);

      parseWalkWhitespace(reader);
      tok4 = lex(reader);

      if (tok4.type == TK_OP_COMMA) {
        parseWalkWhitespace(reader);
        tok4 = lex(reader);
      }
    }

    if (fields.empty()) {
      throw Error("Object declaration without fields is not allowed");
    }

    auto parserId = ParserId{tok2};
    auto stmtObjDecl = ParserStmtObjDecl{parserId, fields};
    return ParserStmt{stmtObjDecl};
  } else if (tok1.type == TK_KW_RETURN) {
    parseWalkWhitespace(reader);

    auto stmtExpr = parseStmtExpr(reader);
    auto stmtReturn = ParserStmtReturn{stmtExpr};
    return ParserStmt{stmtReturn};
  } else if (tok1.type == TK_LIT_ID) {
    parseWalkWhitespace(reader);
    auto tok2 = lex(reader);

    if (tok2.type == TK_OP_COLON) {
      parseWalkWhitespace(reader);
      auto tok3 = lex(reader);

      if (tok3.type != TK_LIT_ID) {
        throw Error("Expected variable type after colon");
      }

      auto loc4 = reader->loc;
      parseWalkWhitespace(reader);
      auto tok4 = lex(reader);
      auto initStmtExpr = std::optional<ParserStmtExpr>{};

      if (tok4.type == TK_OP_EQ) {
        parseWalkWhitespace(reader);
        initStmtExpr = parseStmtExpr(reader);
      } else {
        reader->seek(loc4);
      }

      auto type = ParserId{tok3};
      auto parserId = ParserId{tok1};
      auto stmtVarDecl = ParserStmtVarDecl{parserId, type, initStmtExpr};

      return ParserStmt{stmtVarDecl};
    } else if (tok2.type == TK_OP_COLON_EQ) {
      parseWalkWhitespace(reader);

      auto stmtExpr = parseStmtExpr(reader);
      auto parserId = ParserId{tok1};
      auto stmtVarDecl = ParserStmtVarDecl{parserId, std::nullopt, stmtExpr};

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
