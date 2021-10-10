/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "lexer.h"

token_t *lexer_next (reader_t *reader) {
  token_t *tok = token_init(reader->loc);

  if (reader_eof(reader)) {
    tok->type = eof;
    return tok;
  }

  char ch = reader_next(reader);

  tok->val = realloc(tok->val, ++tok->val_len + 1);
  tok->val[tok->val_len - 1] = ch;
  tok->val[tok->val_len] = '\0';

  if (ch == '&') return lexer_op_eq2(reader, tok, '&', opAnd, opAndEq, opAndAnd, opAndAndEq);
  if (ch == '^') return lexer_op_eq(reader, tok, opCaret, opCaretEq);
  if (ch == ':') return lexer_op_eq(reader, tok, opColon, opColonEq);
  if (ch == ',') return lexer_token(reader, tok, opComma);
  if (ch == '=') return lexer_op_eq(reader, tok, opEq, opEqEq);
  if (ch == '!') return lexer_op_eq_double(reader, tok, '!', opExcl, opExclEq, opExclExcl);
  if (ch == '>') return lexer_op_eq2(reader, tok, '>', opGt, opGtEq, opRShift, opRShiftEq);
  if (ch == '{') return lexer_token(reader, tok, opLBrace);
  if (ch == '[') return lexer_token(reader, tok, opLBrack);
  if (ch == '(') return lexer_token(reader, tok, opLPar);
  if (ch == '<') return lexer_op_eq2(reader, tok, '<', opLt, opLtEq, opLShift, opLShiftEq);
  if (ch == '-') return lexer_op_eq_double(reader, tok, '-', opMinus, opMinusEq, opMinusMinus);
  if (ch == '|') return lexer_op_eq2(reader, tok, '|', opOr, opOrEq, opOrOr, opOrOrEq);
  if (ch == '%') return lexer_op_eq(reader, tok, opPercent, opPercentEq);
  if (ch == '+') return lexer_op_eq_double(reader, tok, '+', opPlus, opPlusEq, opPlusPlus);
  if (ch == '}') return lexer_token(reader, tok, opRBrace);
  if (ch == ']') return lexer_token(reader, tok, opRBrack);
  if (ch == ')') return lexer_token(reader, tok, opRPar);
  if (ch == ';') return lexer_token(reader, tok, opSemi);
  if (ch == '*') return lexer_op_eq2(reader, tok, '*', opStar, opStarEq, opStarStar, opStarStarEq);
  if (ch == '~') return lexer_token(reader, tok, opTilde);

  if (ch == '.') {
    if (reader_eof(reader)) {
      return lexer_token(reader, tok, opDot);
    }

    reader_location_t loc1 = reader->loc;
    char ch1 = reader_next(reader);

    if (ch1 == '.') {
      tok->val = realloc(tok->val, ++tok->val_len + 1);
      tok->val[tok->val_len - 1] = ch1;
      tok->val[tok->val_len] = '\0';

      if (reader_eof(reader)) {
        return lexer_token(reader, tok, opDotDot);
      }

      reader_location_t loc2 = reader->loc;
      char ch2 = reader_next(reader);

      if (ch2 == '.' || ch2 == '=') {
        tok->val = realloc(tok->val, ++tok->val_len + 1);
        tok->val[tok->val_len - 1] = ch2;
        tok->val[tok->val_len] = '\0';

        return lexer_token(reader, tok, ch2 == '.' ? opDotDotDot : opDotDotEq);
      } else {
        reader_seek(reader, loc2);
        return lexer_token(reader, tok, opDotDot);
      }
    } else {
      reader_seek(reader, loc1);
      return lexer_token(reader, tok, opDot);
    }
  } else if (ch == '?') {
    if (reader_eof(reader)) {
      return lexer_token(reader, tok, opQn);
    }

    reader_location_t loc1 = reader->loc;
    char ch1 = reader_next(reader);

    if (ch1 == '.') {
      tok->val = realloc(tok->val, ++tok->val_len + 1);
      tok->val[tok->val_len - 1] = ch1;
      tok->val[tok->val_len] = '\0';

      return lexer_token(reader, tok, opQnDot);
    } else if (ch1 == '?') {
      tok->val = realloc(tok->val, ++tok->val_len + 1);
      tok->val[tok->val_len - 1] = ch1;
      tok->val[tok->val_len] = '\0';

      if (reader_eof(reader)) {
        return lexer_token(reader, tok, opQnQn);
      }

      reader_location_t loc2 = reader->loc;
      char ch2 = reader_next(reader);

      if (ch2 == '=') {
        tok->val = realloc(tok->val, ++tok->val_len + 1);
        tok->val[tok->val_len - 1] = ch2;
        tok->val[tok->val_len] = '\0';

        return lexer_token(reader, tok, opQnQnEq);
      } else {
        reader_seek(reader, loc2);
        return lexer_token(reader, tok, opQnQn);
      }
    } else {
      reader_seek(reader, loc1);
      return lexer_token(reader, tok, opQn);
    }
  } else if (ch == '/') {
    if (reader_eof(reader)) {
      return lexer_token(reader, tok, opSlash);
    }

    reader_location_t loc1 = reader->loc;
    char ch1 = reader_next(reader);

    if (ch1 == '=') {
      tok->val = realloc(tok->val, ++tok->val_len + 1);
      tok->val[tok->val_len - 1] = ch1;
      tok->val[tok->val_len] = '\0';

      return lexer_token(reader, tok, opSlashEq);
    } else if (ch1 != '/' && ch1 != '*') {
      reader_seek(reader, loc1);
      return lexer_token(reader, tok, opSlash);
    } else {
      reader_seek(reader, loc1);
    }
  }

  if (token_is_whitespace(ch)) {
    lexer_walk(reader, tok, token_is_whitespace);
    return lexer_token(reader, tok, whitespace);
  } else if (token_is_lit_id_start(ch)) {
    lexer_walk(reader, tok, token_is_lit_id_continue);

    if (strcmp(tok->val, "as") == 0) {
      if (reader_eof(reader)) {
        return lexer_token(reader, tok, kwAs);
      }

      reader_location_t loc1 = reader->loc;
      char ch1 = reader_next(reader);

      if (ch1 == '?') {
        tok->val = realloc(tok->val, ++tok->val_len + 1);
        tok->val[tok->val_len - 1] = ch1;
        tok->val[tok->val_len] = '\0';

        return lexer_token(reader, tok, kwAsSafe);
      } else {
        reader_seek(reader, loc1);
        return lexer_token(reader, tok, kwAs);
      }
    }

    if (strcmp(tok->val, "async") == 0) return lexer_token(reader, tok, kwAsync);
    if (strcmp(tok->val, "await") == 0) return lexer_token(reader, tok, kwAwait);
    if (strcmp(tok->val, "break") == 0) return lexer_token(reader, tok, kwBreak);
    if (strcmp(tok->val, "case") == 0) return lexer_token(reader, tok, kwCase);
    if (strcmp(tok->val, "catch") == 0) return lexer_token(reader, tok, kwCatch);
    if (strcmp(tok->val, "class") == 0) return lexer_token(reader, tok, kwClass);
    if (strcmp(tok->val, "const") == 0) return lexer_token(reader, tok, kwConst);
    if (strcmp(tok->val, "continue") == 0) return lexer_token(reader, tok, kwContinue);
    if (strcmp(tok->val, "default") == 0) return lexer_token(reader, tok, kwDefault);
    if (strcmp(tok->val, "deinit") == 0) return lexer_token(reader, tok, kwDeinit);
    if (strcmp(tok->val, "elif") == 0) return lexer_token(reader, tok, kwElif);
    if (strcmp(tok->val, "else") == 0) return lexer_token(reader, tok, kwElse);
    if (strcmp(tok->val, "enum") == 0) return lexer_token(reader, tok, kwEnum);
    if (strcmp(tok->val, "export") == 0) return lexer_token(reader, tok, kwExport);
    if (strcmp(tok->val, "fallthrough") == 0) return lexer_token(reader, tok, kwFallthrough);
    if (strcmp(tok->val, "false") == 0) return lexer_token(reader, tok, kwFalse);
    if (strcmp(tok->val, "fn") == 0) return lexer_token(reader, tok, kwFn);
    if (strcmp(tok->val, "from") == 0) return lexer_token(reader, tok, kwFrom);
    if (strcmp(tok->val, "if") == 0) return lexer_token(reader, tok, kwIf);
    if (strcmp(tok->val, "import") == 0) return lexer_token(reader, tok, kwImport);
    if (strcmp(tok->val, "in") == 0) return lexer_token(reader, tok, kwIn);
    if (strcmp(tok->val, "init") == 0) return lexer_token(reader, tok, kwInit);
    if (strcmp(tok->val, "interface") == 0) return lexer_token(reader, tok, kwInterface);
    if (strcmp(tok->val, "is") == 0) return lexer_token(reader, tok, kwIs);
    if (strcmp(tok->val, "loop") == 0) return lexer_token(reader, tok, kwLoop);
    if (strcmp(tok->val, "main") == 0) return lexer_token(reader, tok, kwMain);
    if (strcmp(tok->val, "match") == 0) return lexer_token(reader, tok, kwMatch);
    if (strcmp(tok->val, "mut") == 0) return lexer_token(reader, tok, kwMut);
    if (strcmp(tok->val, "new") == 0) return lexer_token(reader, tok, kwNew);
    if (strcmp(tok->val, "nil") == 0) return lexer_token(reader, tok, kwNil);
    if (strcmp(tok->val, "obj") == 0) return lexer_token(reader, tok, kwObj);
    if (strcmp(tok->val, "op") == 0) return lexer_token(reader, tok, kwOp);
    if (strcmp(tok->val, "override") == 0) return lexer_token(reader, tok, kwOverride);
    if (strcmp(tok->val, "priv") == 0) return lexer_token(reader, tok, kwPriv);
    if (strcmp(tok->val, "prot") == 0) return lexer_token(reader, tok, kwProt);
    if (strcmp(tok->val, "pub") == 0) return lexer_token(reader, tok, kwPub);
    if (strcmp(tok->val, "return") == 0) return lexer_token(reader, tok, kwReturn);
    if (strcmp(tok->val, "static") == 0) return lexer_token(reader, tok, kwStatic);
    if (strcmp(tok->val, "super") == 0) return lexer_token(reader, tok, kwSuper);
    if (strcmp(tok->val, "this") == 0) return lexer_token(reader, tok, kwThis);
    if (strcmp(tok->val, "throw") == 0) return lexer_token(reader, tok, kwThrow);
    if (strcmp(tok->val, "true") == 0) return lexer_token(reader, tok, kwTrue);
    if (strcmp(tok->val, "try") == 0) return lexer_token(reader, tok, kwTry);
    if (strcmp(tok->val, "union") == 0) return lexer_token(reader, tok, kwUnion);

    return lexer_token(reader, tok, litId);
  } else if (token_is_digit(ch)) {
    if (ch == '0') {
      if (reader_eof(reader)) {
        return lexer_token(reader, tok, litIntDec);
      }

      reader_location_t loc1 = reader->loc;
      char ch1 = reader_next(reader);

      if (token_is_digit(ch1)) {
        lexer_walk(reader, tok, token_is_lit_id_continue);
        throw_syntax_error(reader, tok->start, E0007);
      } else if (ch1 == 'B' || ch1 == 'b' || ch1 == 'X' || ch1 == 'x' || ch1 == 'O' || ch1 == 'o') {
        tok->val = realloc(tok->val, ++tok->val_len + 1);
        tok->val[tok->val_len - 1] = ch1;
        tok->val[tok->val_len] = '\0';

        if (ch1 == 'B' || ch1 == 'b') {
          return lexer_lit_num(reader, tok, token_is_lit_int_bin, litIntBin);
        } else if (ch1 == 'X' || ch1 == 'x') {
          return lexer_lit_num(reader, tok, token_is_lit_int_hex, litIntHex);
        } else {
          return lexer_lit_num(reader, tok, token_is_lit_int_oct, litIntOct);
        }
      } else {
        reader_seek(reader, loc1);
      }

      return lexer_lit_num(reader, tok, token_is_lit_int_dec, litIntDec);
    }

    lexer_walk(reader, tok, token_is_lit_int_dec);
    return lexer_lit_num(reader, tok, token_is_lit_int_dec, litIntDec);
  } else if (ch == '"') {
    if (reader_eof(reader)) {
      throw_syntax_error(reader, tok->start, E0003);
    }

    lexer_walk_lit_str(reader, tok);
    return lexer_token(reader, tok, litStr);
  } else if (ch == '/') {
    char ch1 = reader_next(reader);

    if (ch1 == '/') {
      tok->val = realloc(tok->val, ++tok->val_len + 1);
      tok->val[tok->val_len - 1] = ch1;
      tok->val[tok->val_len] = '\0';
      lexer_walk(reader, tok, token_is_not_newline);

      return lexer_token(reader, tok, commentLine);
    } else {
      tok->val = realloc(tok->val, ++tok->val_len + 1);
      tok->val[tok->val_len - 1] = ch1;
      tok->val[tok->val_len] = '\0';

      if (reader_eof(reader)) {
        throw_syntax_error(reader, tok->start, E0001);
      }

      while (true) {
        char ch2 = reader_next(reader);

        if (reader_eof(reader)) {
          throw_syntax_error(reader, tok->start, E0001);
        } else if (ch2 == '*') {
          reader_location_t loc3 = reader->loc;
          char ch3 = reader_next(reader);

          if (ch3 == '/') {
            tok->val_len += 2;
            tok->val = realloc(tok->val, tok->val_len + 1);
            tok->val[tok->val_len -2] = ch2;
            tok->val[tok->val_len - 1] = ch3;
            tok->val[tok->val_len] = '\0';

            break;
          } else {
            reader_seek(reader, loc3);
          }
        }

        tok->val = realloc(tok->val, ++tok->val_len + 1);
        tok->val[tok->val_len - 1] = ch2;
        tok->val[tok->val_len] = '\0';
      }

      return lexer_token(reader, tok, commentBlock);
    }
  } else if (ch == '\'') {
    if (reader_eof(reader)) {
      throw_syntax_error(reader, tok->start, E0002);
    }

    reader_location_t loc1 = reader->loc;
    char ch1 = reader_next(reader);

    if (ch1 == '\'') {
      throw_syntax_error(reader, tok->start, E0004);
    } else if (reader_eof(reader)) {
      throw_syntax_error(reader, tok->start, E0002);
    } else if (ch1 == '\\') {
      char ch2 = reader_next(reader);

      if (!token_is_lit_char_escape(ch2)) {
        throw_syntax_error(reader, loc1, E0004);
      } else if (reader_eof(reader)) {
        throw_syntax_error(reader, tok->start, E0002);
      }

      tok->val_len += 2;
      tok->val = realloc(tok->val, tok->val_len + 1);
      tok->val[tok->val_len - 2] = ch1;
      tok->val[tok->val_len - 1] = ch2;
      tok->val[tok->val_len] = '\0';
    } else {
      tok->val = realloc(tok->val, ++tok->val_len + 1);
      tok->val[tok->val_len - 1] = ch1;
      tok->val[tok->val_len] = '\0';
    }

    char ch3 = reader_next(reader);

    if (ch3 != '\'') {
      while (!reader_eof(reader)) {
        if (reader_next(reader) == '\'') {
          break;
        }
      }

      throw_syntax_error(reader, tok->start, E0006);
    }

    tok->val = realloc(tok->val, ++tok->val_len + 1);
    tok->val[tok->val_len - 1] = ch3;
    tok->val[tok->val_len] = '\0';

    return lexer_token(reader, tok, litChar);
  }

  throw_syntax_error(reader, tok->start, E0000);
}

token_t *lexer_lit_float (reader_t *reader, token_t *tok, token_type_t type) {
  if (!reader_eof(reader)) {
    reader_location_t loc = reader->loc;
    char ch = reader_next(reader);

    if (token_is_lit_id_continue(ch)) {
      lexer_walk(reader, tok, token_is_lit_id_continue);
      throw_syntax_error(reader, tok->start, E0012);
    } else {
      reader_seek(reader, loc);
    }
  }

  if (type == litIntBin) {
    throw_syntax_error(reader, tok->start, E0014);
  } else if (type == litIntHex) {
    throw_syntax_error(reader, tok->start, E0015);
  } else if (type == litIntOct) {
    throw_syntax_error(reader, tok->start, E0016);
  }

  return lexer_token(reader, tok, litFloat);
}

token_t *lexer_lit_num (reader_t *reader, token_t *tok, bool (*fn) (char), token_type_t type) {
  char *err_code = type == litIntBin ? E0008 : type == litIntDec ? E0009 : type == litIntHex ? E0010 : E0011;

  if (type != litIntDec) {
    if (reader_eof(reader)) {
      throw_syntax_error(reader, tok->start, err_code);
    }

    char ch1 = reader_next(reader);

    if (!fn(ch1)) {
      lexer_walk(reader, tok, token_is_lit_id_continue);
      throw_syntax_error(reader, tok->start, err_code);
    }

    tok->val = realloc(tok->val, ++tok->val_len + 1);
    tok->val[tok->val_len - 1] = ch1;
    tok->val[tok->val_len] = '\0';
    lexer_walk(reader, tok, fn);
  }

  if (reader_eof(reader)) {
    return lexer_token(reader, tok, type);
  }

  reader_location_t loc2 = reader->loc;
  char ch2 = reader_next(reader);

  if (ch2 == 'E' || ch2 == 'e') {
    tok->val = realloc(tok->val, ++tok->val_len + 1);
    tok->val[tok->val_len - 1] = ch2;
    tok->val[tok->val_len] = '\0';
    lexer_walk_lit_float_exp(reader, tok);

    return lexer_lit_float(reader, tok, type);
  } else if (ch2 == '.') {
    if (reader_eof(reader)) {
      throw_syntax_error(reader, tok->start, E0012);
    }

    char ch3 = reader_next(reader);

    if (ch3 == '.') {
      reader_seek(reader, loc2);
    } else if (ch3 == 'E' || ch3 == 'e') {
      tok->val_len += 2;
      tok->val = realloc(tok->val, tok->val_len + 1);
      tok->val[tok->val_len - 2] = ch2;
      tok->val[tok->val_len - 1] = ch3;
      tok->val[tok->val_len] = '\0';
      lexer_walk_lit_float_exp(reader, tok);

      return lexer_lit_float(reader, tok, type);
    } else if (!token_is_lit_id_continue(ch3)) {
      if (token_is_lit_id_continue(ch3)) {
        lexer_walk(reader, tok, token_is_lit_id_continue);
      }

      throw_syntax_error(reader, tok->start, E0012);
    } else {
      bool with_exp = false;
      tok->val_len += 2;
      tok->val = realloc(tok->val, tok->val_len + 1);
      tok->val[tok->val_len - 2] = ch2;
      tok->val[tok->val_len - 1] = ch3;
      tok->val[tok->val_len] = '\0';

      while (!reader_eof(reader)) {
        reader_location_t loc4 = reader->loc;
        char ch4 = reader_next(reader);

        if (ch4 == 'E' || ch4 == 'e') {
          with_exp = true;
        } else if (!token_is_digit(ch4)) {
          reader_seek(reader, loc4);
          break;
        }

        tok->val = realloc(tok->val, ++tok->val_len + 1);
        tok->val[tok->val_len - 1] = ch4;
        tok->val[tok->val_len] = '\0';

        if (with_exp) {
          break;
        }
      }

      if (with_exp) {
        lexer_walk_lit_float_exp(reader, tok);
      }

      return lexer_lit_float(reader, tok, type);
    }
  } else if (token_is_lit_id_continue(ch2)) {
    lexer_walk(reader, tok, token_is_lit_id_continue);
    throw_syntax_error(reader, tok->start, err_code);
  } else {
    reader_seek(reader, loc2);
  }

  return lexer_token(reader, tok, type);
}

token_t *lexer_op_eq (reader_t *reader, token_t *tok, token_type_t type1, token_type_t type2) {
  if (reader_eof(reader)) {
    return lexer_token(reader, tok, type1);
  }

  reader_location_t loc = reader->loc;
  char ch = reader_next(reader);

  if (ch == '=') {
    tok->val = realloc(tok->val, ++tok->val_len + 1);
    tok->val[tok->val_len - 1] = ch;
    tok->val[tok->val_len] = '\0';

    return lexer_token(reader, tok, type2);
  } else {
    reader_seek(reader, loc);
    return lexer_token(reader, tok, type1);
  }
}

token_t *lexer_op_eq2 (
  reader_t *reader,
  token_t *tok,
  char ch,
  token_type_t type1,
  token_type_t type2,
  token_type_t type3,
  token_type_t type4
) {
  if (reader_eof(reader)) {
    return lexer_token(reader, tok, type1);
  }

  reader_location_t loc1 = reader->loc;
  char ch1 = reader_next(reader);

  if (ch1 == '=' || reader_eof(reader)) {
    tok->val = realloc(tok->val, ++tok->val_len + 1);
    tok->val[tok->val_len - 1] = ch1;
    tok->val[tok->val_len] = '\0';

    return lexer_token(reader, tok, ch1 == '=' ? type2 : type3);
  } else if (ch1 == ch) {
    reader_location_t loc2 = reader->loc;
    char ch2 = reader_next(reader);

    if (ch2 == '=') {
      tok->val_len += 2;
      tok->val = realloc(tok->val, tok->val_len + 1);
      tok->val[tok->val_len - 2] = ch1;
      tok->val[tok->val_len - 1] = ch2;
      tok->val[tok->val_len] = '\0';

      return lexer_token(reader, tok, type4);
    } else {
      tok->val = realloc(tok->val, ++tok->val_len + 1);
      tok->val[tok->val_len - 1] = ch1;
      tok->val[tok->val_len] = '\0';
      reader_seek(reader, loc2);

      return lexer_token(reader, tok, type3);
    }
  } else {
    reader_seek(reader, loc1);
    return lexer_token(reader, tok, type1);
  }
}

token_t *lexer_op_eq_double (
  reader_t *reader,
  token_t *tok,
  char ch,
  token_type_t type1,
  token_type_t type2,
  token_type_t type3
) {
  if (reader_eof(reader)) {
    return lexer_token(reader, tok, type1);
  }

  reader_location_t loc2 = reader->loc;
  char ch1 = reader_next(reader);

  if (ch1 == '=' || ch1 == ch) {
    tok->val = realloc(tok->val, ++tok->val_len + 1);
    tok->val[tok->val_len - 1] = ch1;
    tok->val[tok->val_len] = '\0';

    return lexer_token(reader, tok, ch1 == '=' ? type2 : type3);
  } else {
    reader_seek(reader, loc2);
    return lexer_token(reader, tok, type1);
  }
}

token_t *lexer_token (reader_t *reader, token_t *tok, token_type_t type) {
  tok->type = type;
  tok->end = reader->loc;
  return tok;
}

void lexer_walk (reader_t *reader, token_t *tok, bool (*fn) (char)) {
  while (!reader_eof(reader)) {
    reader_location_t loc = reader->loc;
    char ch = reader_next(reader);

    if (!fn(ch)) {
      reader_seek(reader, loc);
      break;
    }

    tok->val = realloc(tok->val, ++tok->val_len + 1);
    tok->val[tok->val_len - 1] = ch;
    tok->val[tok->val_len] = '\0';
  }
}

void lexer_walk_lit_float_exp (reader_t *reader, token_t *tok) {
  if (reader_eof(reader)) {
    throw_syntax_error(reader, tok->start, E0013);
  }

  char ch1 = reader_next(reader);

  if (ch1 == '+' || ch1 == '-') {
    if (reader_eof(reader)) {
      throw_syntax_error(reader, tok->start, E0013);
    }

    char ch2 = reader_next(reader);

    if (!token_is_digit(ch2)) {
      lexer_walk(reader, tok, token_is_lit_id_continue);
      throw_syntax_error(reader, tok->start, E0013);
    }

    tok->val_len += 2;
    tok->val = realloc(tok->val, tok->val_len + 1);
    tok->val[tok->val_len - 2] = ch1;
    tok->val[tok->val_len - 1] = ch2;
    tok->val[tok->val_len] = '\0';
  } else if (token_is_digit(ch1)) {
    tok->val = realloc(tok->val, ++tok->val_len + 1);
    tok->val[tok->val_len - 1] = ch1;
    tok->val[tok->val_len] = '\0';
  } else {
    lexer_walk(reader, tok, token_is_lit_id_continue);
    throw_syntax_error(reader, tok->start, E0013);
  }

  while (!reader_eof(reader)) {
    reader_location_t loc3 = reader->loc;
    char ch3 = reader_next(reader);

    if (!token_is_digit(ch3)) {
      reader_seek(reader, loc3);
      break;
    }

    tok->val = realloc(tok->val, ++tok->val_len + 1);
    tok->val[tok->val_len - 1] = ch3;
    tok->val[tok->val_len] = '\0';
  }
}

void lexer_walk_lit_str (reader_t *reader, token_t *tok) {
  size_t blocks = 0;
  bool inside_char = false;

  while (true) {
    reader_location_t loc1 = reader->loc;
    char ch1 = reader_next(reader);

    tok->val = realloc(tok->val, ++tok->val_len + 1);
    tok->val[tok->val_len - 1] = ch1;
    tok->val[tok->val_len] = '\0';

    if (ch1 == '\\' && blocks == 0) {
      if (reader_eof(reader)) {
        throw_syntax_error(reader, tok->start, E0003);
      }

      char ch2 = reader_next(reader);

      if (!token_is_lit_str_escape(ch2)) {
        throw_syntax_error(reader, loc1, E0005);
      }

      tok->val = realloc(tok->val, ++tok->val_len + 1);
      tok->val[tok->val_len - 1] = ch2;
      tok->val[tok->val_len] = '\0';
    } else if (ch1 == '{' && !inside_char) {
      blocks += 1;
    } else if (ch1 == '}' && blocks != 0 && !inside_char) {
      blocks -= 1;
    } else if (ch1 == '\'' && inside_char) {
      inside_char = false;
    } else if (ch1 == '\'' && blocks != 0) {
      inside_char = true;
    } else if (ch1 == '"' && blocks != 0) {
      lexer_walk_lit_str(reader, tok);
    } else if (ch1 == '"') {
      break;
    } else if (reader_eof(reader)) {
      throw_syntax_error(reader, tok->start, E0003);
    }
  }
}
