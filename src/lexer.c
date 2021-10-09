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

lexer_t *lexer_init (reader_t *reader) {
  lexer_t *this = malloc(sizeof(lexer_t));

  if (this == NULL) {
    throw_error("Unable to allocate memory for lexer");
  }

  this->val_len = 0;
  this->val = malloc(this->val_len + 1);

  if (this->val == NULL) {
    throw_error("Unable to allocate memory for lexer value");
  }

  this->reader = reader;
  this->start = reader_location_init();
  this->end = reader_location_init();
  this->val[0] = '\0';

  return this;
}

void lexer_free (lexer_t *this) {
  free(this->val);
  free(this);
}

token_t *lexer_next (lexer_t *this) {
  this->start = this->reader->loc;

  if (reader_eof(this->reader)) {
    return token_init(eof, "", this->start, this->start);
  }

  char ch = reader_next(this->reader);

  this->val_len = 1;
  this->val = realloc(this->val, this->val_len + 1);
  this->val[this->val_len - 1] = ch;
  this->val[this->val_len] = '\0';

  if (ch == '&') return lexer_op_eq2(this, '&', opAnd, opAndEq, opAndAnd, opAndAndEq);
  if (ch == '^') return lexer_op_eq(this, opCaret, opCaretEq);
  if (ch == ':') return lexer_op_eq(this, opColon, opColonEq);
  if (ch == ',') return lexer_token(this, opComma);
  if (ch == '=') return lexer_op_eq(this, opEq, opEqEq);
  if (ch == '!') return lexer_op_eq_double(this, '!', opExcl, opExclEq, opExclExcl);
  if (ch == '>') return lexer_op_eq2(this, '>', opGt, opGtEq, opRShift, opRShiftEq);
  if (ch == '{') return lexer_token(this, opLBrace);
  if (ch == '[') return lexer_token(this, opLBrack);
  if (ch == '(') return lexer_token(this, opLPar);
  if (ch == '<') return lexer_op_eq2(this, '<', opLt, opLtEq, opLShift, opLShiftEq);
  if (ch == '-') return lexer_op_eq_double(this, '-', opMinus, opMinusEq, opMinusMinus);
  if (ch == '|') return lexer_op_eq2(this, '|', opOr, opOrEq, opOrOr, opOrOrEq);
  if (ch == '%') return lexer_op_eq(this, opPercent, opPercentEq);
  if (ch == '+') return lexer_op_eq_double(this, '+', opPlus, opPlusEq, opPlusPlus);
  if (ch == '}') return lexer_token(this, opRBrace);
  if (ch == ']') return lexer_token(this, opRBrack);
  if (ch == ')') return lexer_token(this, opRPar);
  if (ch == ';') return lexer_token(this, opSemi);
  if (ch == '*') return lexer_op_eq2(this, '*', opStar, opStarEq, opStarStar, opStarStarEq);
  if (ch == '~') return lexer_token(this, opTilde);

  if (ch == '.') {
    if (reader_eof(this->reader)) {
      return lexer_token(this, opDot);
    }

    reader_location_t loc1 = this->reader->loc;
    char ch1 = reader_next(this->reader);

    if (ch1 == '.') {
      this->val = realloc(this->val, ++this->val_len + 1);
      this->val[this->val_len - 1] = ch1;
      this->val[this->val_len] = '\0';

      if (reader_eof(this->reader)) {
        return lexer_token(this, opDotDot);
      }

      reader_location_t loc2 = this->reader->loc;
      char ch2 = reader_next(this->reader);

      if (ch2 == '.' || ch2 == '=') {
        this->val = realloc(this->val, ++this->val_len + 1);
        this->val[this->val_len - 1] = ch2;
        this->val[this->val_len] = '\0';
        return lexer_token(this, ch2 == '.' ? opDotDotDot : opDotDotEq);
      } else {
        reader_seek(this->reader, loc2);
        return lexer_token(this, opDotDot);
      }
    } else {
      reader_seek(this->reader, loc1);
      return lexer_token(this, opDot);
    }
  } else if (ch == '?') {
    if (reader_eof(this->reader)) {
      return lexer_token(this, opQn);
    }

    reader_location_t loc1 = this->reader->loc;
    char ch1 = reader_next(this->reader);

    if (ch1 == '.') {
      this->val = realloc(this->val, ++this->val_len + 1);
      this->val[this->val_len - 1] = ch1;
      this->val[this->val_len] = '\0';

      return lexer_token(this, opQnDot);
    } else if (ch1 == '?') {
      this->val = realloc(this->val, ++this->val_len + 1);
      this->val[this->val_len - 1] = ch1;
      this->val[this->val_len] = '\0';

      if (reader_eof(this->reader)) {
        return lexer_token(this, opQnQn);
      }

      reader_location_t loc2 = this->reader->loc;
      char ch2 = reader_next(this->reader);

      if (ch2 == '=') {
        this->val = realloc(this->val, ++this->val_len + 1);
        this->val[this->val_len - 1] = ch2;
        this->val[this->val_len] = '\0';

        return lexer_token(this, opQnQnEq);
      } else {
        reader_seek(this->reader, loc2);
        return lexer_token(this, opQnQn);
      }
    } else {
      reader_seek(this->reader, loc1);
      return lexer_token(this, opQn);
    }
  } else if (ch == '/') {
    if (reader_eof(this->reader)) {
      return lexer_token(this, opSlash);
    }

    reader_location_t loc1 = this->reader->loc;
    char ch1 = reader_next(this->reader);

    if (ch1 == '=') {
      this->val = realloc(this->val, ++this->val_len + 1);
      this->val[this->val_len - 1] = ch1;
      this->val[this->val_len] = '\0';
      return lexer_token(this, opSlashEq);
    } else if (ch1 != '/' && ch1 != '*') {
      reader_seek(this->reader, loc1);
      return lexer_token(this, opSlash);
    } else {
      reader_seek(this->reader, loc1);
    }
  }

  if (token_is_whitespace(ch)) {
    lexer_walk(this, token_is_whitespace);
    return lexer_token(this, whitespace);
  } else if (token_is_lit_id_start(ch)) {
    lexer_walk(this, token_is_lit_id_continue);

    if (strcmp(this->val, "as") == 0) {
      if (reader_eof(this->reader)) {
        return lexer_token(this, kwAs);
      }

      reader_location_t loc1 = this->reader->loc;
      char ch1 = reader_next(this->reader);

      if (ch1 == '?') {
        this->val = realloc(this->val, ++this->val_len + 1);
        this->val[this->val_len - 1] = ch1;
        this->val[this->val_len] = '\0';
        return lexer_token(this, kwAsSafe);
      } else {
        reader_seek(this->reader, loc1);
        return lexer_token(this, kwAs);
      }
    }

    if (strcmp(this->val, "async") == 0) return lexer_token(this, kwAsync);
    if (strcmp(this->val, "await") == 0) return lexer_token(this, kwAwait);
    if (strcmp(this->val, "break") == 0) return lexer_token(this, kwBreak);
    if (strcmp(this->val, "case") == 0) return lexer_token(this, kwCase);
    if (strcmp(this->val, "catch") == 0) return lexer_token(this, kwCatch);
    if (strcmp(this->val, "class") == 0) return lexer_token(this, kwClass);
    if (strcmp(this->val, "const") == 0) return lexer_token(this, kwConst);
    if (strcmp(this->val, "continue") == 0) return lexer_token(this, kwContinue);
    if (strcmp(this->val, "default") == 0) return lexer_token(this, kwDefault);
    if (strcmp(this->val, "deinit") == 0) return lexer_token(this, kwDeinit);
    if (strcmp(this->val, "elif") == 0) return lexer_token(this, kwElif);
    if (strcmp(this->val, "else") == 0) return lexer_token(this, kwElse);
    if (strcmp(this->val, "enum") == 0) return lexer_token(this, kwEnum);
    if (strcmp(this->val, "export") == 0) return lexer_token(this, kwExport);
    if (strcmp(this->val, "fallthrough") == 0) return lexer_token(this, kwFallthrough);
    if (strcmp(this->val, "false") == 0) return lexer_token(this, kwFalse);
    if (strcmp(this->val, "fn") == 0) return lexer_token(this, kwFn);
    if (strcmp(this->val, "from") == 0) return lexer_token(this, kwFrom);
    if (strcmp(this->val, "if") == 0) return lexer_token(this, kwIf);
    if (strcmp(this->val, "import") == 0) return lexer_token(this, kwImport);
    if (strcmp(this->val, "in") == 0) return lexer_token(this, kwIn);
    if (strcmp(this->val, "init") == 0) return lexer_token(this, kwInit);
    if (strcmp(this->val, "interface") == 0) return lexer_token(this, kwInterface);
    if (strcmp(this->val, "is") == 0) return lexer_token(this, kwIs);
    if (strcmp(this->val, "loop") == 0) return lexer_token(this, kwLoop);
    if (strcmp(this->val, "main") == 0) return lexer_token(this, kwMain);
    if (strcmp(this->val, "match") == 0) return lexer_token(this, kwMatch);
    if (strcmp(this->val, "mut") == 0) return lexer_token(this, kwMut);
    if (strcmp(this->val, "new") == 0) return lexer_token(this, kwNew);
    if (strcmp(this->val, "nil") == 0) return lexer_token(this, kwNil);
    if (strcmp(this->val, "obj") == 0) return lexer_token(this, kwObj);
    if (strcmp(this->val, "op") == 0) return lexer_token(this, kwOp);
    if (strcmp(this->val, "override") == 0) return lexer_token(this, kwOverride);
    if (strcmp(this->val, "priv") == 0) return lexer_token(this, kwPriv);
    if (strcmp(this->val, "prot") == 0) return lexer_token(this, kwProt);
    if (strcmp(this->val, "pub") == 0) return lexer_token(this, kwPub);
    if (strcmp(this->val, "return") == 0) return lexer_token(this, kwReturn);
    if (strcmp(this->val, "static") == 0) return lexer_token(this, kwStatic);
    if (strcmp(this->val, "super") == 0) return lexer_token(this, kwSuper);
    if (strcmp(this->val, "this") == 0) return lexer_token(this, kwThis);
    if (strcmp(this->val, "throw") == 0) return lexer_token(this, kwThrow);
    if (strcmp(this->val, "true") == 0) return lexer_token(this, kwTrue);
    if (strcmp(this->val, "try") == 0) return lexer_token(this, kwTry);
    if (strcmp(this->val, "union") == 0) return lexer_token(this, kwUnion);

    return lexer_token(this, litId);
  } else if (token_is_digit(ch)) {
    if (ch == '0') {
      if (reader_eof(this->reader)) {
        return lexer_token(this, litIntDec);
      }

      reader_location_t loc1 = this->reader->loc;
      char ch1 = reader_next(this->reader);

      if (token_is_digit(ch1)) {
        lexer_walk(this, token_is_lit_id_continue);
        throw_syntax_error(this->reader, this->start, E0007);
      } else if (ch1 == 'B' || ch1 == 'b' || ch1 == 'X' || ch1 == 'x' || ch1 == 'O' || ch1 == 'o') {
        this->val = realloc(this->val, ++this->val_len + 1);
        this->val[this->val_len - 1] = ch1;
        this->val[this->val_len] = '\0';

        if (ch1 == 'B' || ch1 == 'b') {
          return lexer_lit_num(this, token_is_lit_int_bin, litIntBin);
        } else if (ch1 == 'X' || ch1 == 'x') {
          return lexer_lit_num(this, token_is_lit_int_hex, litIntHex);
        } else {
          return lexer_lit_num(this, token_is_lit_int_oct, litIntOct);
        }
      } else {
        reader_seek(this->reader, loc1);
      }

      return lexer_lit_num(this, token_is_lit_int_dec, litIntDec);
    }

    lexer_walk(this, token_is_lit_int_dec);
    return lexer_lit_num(this, token_is_lit_int_dec, litIntDec);
  } else if (ch == '"') {
    if (reader_eof(this->reader)) {
      throw_syntax_error(this->reader, this->start, E0003);
    }

    lexer_walk_lit_str(this);
    return lexer_token(this, litStr);
  } else if (ch == '/') {
    char ch1 = reader_next(this->reader);

    if (ch1 == '/') {
      this->val = realloc(this->val, ++this->val_len + 1);
      this->val[this->val_len - 1] = ch1;
      this->val[this->val_len] = '\0';
      lexer_walk(this, token_is_not_newline);

      return lexer_token(this, commentLine);
    } else {
      this->val = realloc(this->val, ++this->val_len + 1);
      this->val[this->val_len - 1] = ch1;
      this->val[this->val_len] = '\0';

      if (reader_eof(this->reader)) {
        throw_syntax_error(this->reader, this->start, E0001);
      }

      while (true) {
        char ch2 = reader_next(this->reader);

        if (reader_eof(this->reader)) {
          throw_syntax_error(this->reader, this->start, E0001);
        } else if (ch2 == '*') {
          reader_location_t loc3 = this->reader->loc;
          char ch3 = reader_next(this->reader);

          if (ch3 == '/') {
            this->val_len += 2;
            this->val = realloc(this->val, this->val_len + 1);
            this->val[this->val_len -2] = ch2;
            this->val[this->val_len - 1] = ch3;
            this->val[this->val_len] = '\0';

            break;
          } else {
            reader_seek(this->reader, loc3);
          }
        }

        this->val = realloc(this->val, ++this->val_len + 1);
        this->val[this->val_len - 1] = ch2;
        this->val[this->val_len] = '\0';
      }

      return lexer_token(this, commentBlock);
    }
  } else if (ch == '\'') {
    if (reader_eof(this->reader)) {
      throw_syntax_error(this->reader, this->start, E0002);
    }

    reader_location_t loc1 = this->reader->loc;
    char ch1 = reader_next(this->reader);

    if (ch1 == '\'') {
      throw_syntax_error(this->reader, this->start, E0004);
    } else if (reader_eof(this->reader)) {
      throw_syntax_error(this->reader, this->start, E0002);
    } else if (ch1 == '\\') {
      char ch2 = reader_next(this->reader);

      if (!token_is_lit_char_escape(ch2)) {
        throw_syntax_error(this->reader, loc1, E0004);
      } else if (reader_eof(this->reader)) {
        throw_syntax_error(this->reader, this->start, E0002);
      }

      this->val_len += 2;
      this->val = realloc(this->val, this->val_len + 1);
      this->val[this->val_len - 2] = ch1;
      this->val[this->val_len - 1] = ch2;
      this->val[this->val_len] = '\0';
    } else {
      this->val = realloc(this->val, ++this->val_len + 1);
      this->val[this->val_len - 1] = ch1;
      this->val[this->val_len] = '\0';
    }

    char ch3 = reader_next(this->reader);

    if (ch3 != '\'') {
      while (!reader_eof(this->reader)) {
        if (reader_next(this->reader) == '\'') {
          break;
        }
      }

      throw_syntax_error(this->reader, this->start, E0006);
    }

    this->val = realloc(this->val, ++this->val_len + 1);
    this->val[this->val_len - 1] = ch3;
    this->val[this->val_len] = '\0';
    return lexer_token(this, litChar);
  }

  throw_syntax_error(this->reader, this->start, E0000);
}

token_t *lexer_lit_float (lexer_t *this, token_type_t type) {
  if (!reader_eof(this->reader)) {
    reader_location_t loc = this->reader->loc;
    char ch = reader_next(this->reader);

    if (token_is_lit_id_continue(ch)) {
      lexer_walk(this, token_is_lit_id_continue);
      throw_syntax_error(this->reader, this->start, E0012);
    } else {
      reader_seek(this->reader, loc);
    }
  }

  if (type == litIntBin) {
    throw_syntax_error(this->reader, this->start, E0014);
  } else if (type == litIntHex) {
    throw_syntax_error(this->reader, this->start, E0015);
  } else if (type == litIntOct) {
    throw_syntax_error(this->reader, this->start, E0016);
  }

  return lexer_token(this, litFloat);
}

token_t *lexer_lit_num (lexer_t *this, bool (*fn) (char), token_type_t type) {
  char *err_code = type == litIntBin ? E0008 : type == litIntDec ? E0009 : type == litIntHex ? E0010 : E0011;

  if (type != litIntDec) {
    if (reader_eof(this->reader)) {
      throw_syntax_error(this->reader, this->start, err_code);
    }

    char ch1 = reader_next(this->reader);

    if (!fn(ch1)) {
      lexer_walk(this, token_is_lit_id_continue);
      throw_syntax_error(this->reader, this->start, err_code);
    }

    this->val = realloc(this->val, ++this->val_len + 1);
    this->val[this->val_len - 1] = ch1;
    this->val[this->val_len] = '\0';
    lexer_walk(this, fn);
  }

  if (reader_eof(this->reader)) {
    return lexer_token(this, type);
  }

  reader_location_t loc2 = this->reader->loc;
  char ch2 = reader_next(this->reader);

  if (ch2 == 'E' || ch2 == 'e') {
    this->val = realloc(this->val, ++this->val_len + 1);
    this->val[this->val_len - 1] = ch2;
    this->val[this->val_len] = '\0';
    lexer_walk_lit_float_exp(this);

    return lexer_lit_float(this, type);
  } else if (ch2 == '.') {
    if (reader_eof(this->reader)) {
      throw_syntax_error(this->reader, this->start, E0012);
    }

    char ch3 = reader_next(this->reader);

    if (ch3 == '.') {
      reader_seek(this->reader, loc2);
    } else if (ch3 == 'E' || ch3 == 'e') {
      this->val_len += 2;
      this->val = realloc(this->val, this->val_len + 1);
      this->val[this->val_len - 2] = ch2;
      this->val[this->val_len - 1] = ch3;
      this->val[this->val_len] = '\0';
      lexer_walk_lit_float_exp(this);

      return lexer_lit_float(this, type);
    } else if (!token_is_lit_id_continue(ch3)) {
      if (token_is_lit_id_continue(ch3)) {
        lexer_walk(this, token_is_lit_id_continue);
      }

      throw_syntax_error(this->reader, this->start, E0012);
    } else {
      bool with_exp = false;
      this->val_len += 2;
      this->val = realloc(this->val, this->val_len + 1);
      this->val[this->val_len - 2] = ch2;
      this->val[this->val_len - 1] = ch3;
      this->val[this->val_len] = '\0';

      while (!reader_eof(this->reader)) {
        reader_location_t loc4 = this->reader->loc;
        char ch4 = reader_next(this->reader);

        if (ch4 == 'E' || ch4 == 'e') {
          with_exp = true;
        } else if (!token_is_digit(ch4)) {
          reader_seek(this->reader, loc4);
          break;
        }

        this->val = realloc(this->val, ++this->val_len + 1);
        this->val[this->val_len - 1] = ch4;
        this->val[this->val_len] = '\0';

        if (with_exp) {
          break;
        }
      }

      if (with_exp) {
        lexer_walk_lit_float_exp(this);
      }

      return lexer_lit_float(this, type);
    }
  } else if (token_is_lit_id_continue(ch2)) {
    lexer_walk(this, token_is_lit_id_continue);
    throw_syntax_error(this->reader, this->start, err_code);
  } else {
    reader_seek(this->reader, loc2);
  }

  return lexer_token(this, type);
}

token_t *lexer_op_eq (lexer_t *this, token_type_t type1, token_type_t type2) {
  if (reader_eof(this->reader)) {
    return lexer_token(this, type1);
  }

  reader_location_t loc = this->reader->loc;
  char ch = reader_next(this->reader);

  if (ch == '=') {
    this->val = realloc(this->val, ++this->val_len + 1);
    this->val[this->val_len - 1] = ch;
    this->val[this->val_len] = '\0';

    return lexer_token(this, type2);
  } else {
    reader_seek(this->reader, loc);
    return lexer_token(this, type1);
  }
}

token_t *lexer_op_eq2 (lexer_t *this, char ch, token_type_t type1, token_type_t type2, token_type_t type3, token_type_t type4) {
  if (reader_eof(this->reader)) {
    return lexer_token(this, type1);
  }

  reader_location_t loc1 = this->reader->loc;
  char ch1 = reader_next(this->reader);

  if (ch1 == '=' || reader_eof(this->reader)) {
    this->val = realloc(this->val, ++this->val_len + 1);
    this->val[this->val_len - 1] = ch1;
    this->val[this->val_len] = '\0';

    return lexer_token(this, ch1 == '=' ? type2 : type3);
  } else if (ch1 == ch) {
    reader_location_t loc2 = this->reader->loc;
    char ch2 = reader_next(this->reader);

    if (ch2 == '=') {
      this->val_len += 2;
      this->val = realloc(this->val, this->val_len + 1);
      this->val[this->val_len - 2] = ch1;
      this->val[this->val_len - 1] = ch2;
      this->val[this->val_len] = '\0';

      return lexer_token(this, type4);
    } else {
      this->val = realloc(this->val, ++this->val_len + 1);
      this->val[this->val_len - 1] = ch1;
      this->val[this->val_len] = '\0';
      reader_seek(this->reader, loc2);

      return lexer_token(this, type3);
    }
  } else {
    reader_seek(this->reader, loc1);
    return lexer_token(this, type1);
  }
}

token_t *lexer_op_eq_double (lexer_t *this, char ch, token_type_t type1, token_type_t type2, token_type_t type3) {
  if (reader_eof(this->reader)) {
    return lexer_token(this, type1);
  }

  reader_location_t loc2 = this->reader->loc;
  char ch1 = reader_next(this->reader);

  if (ch1 == '=' || ch1 == ch) {
    this->val = realloc(this->val, ++this->val_len + 1);
    this->val[this->val_len - 1] = ch1;
    this->val[this->val_len] = '\0';

    return lexer_token(this, ch1 == '=' ? type2 : type3);
  } else {
    reader_seek(this->reader, loc2);
    return lexer_token(this, type1);
  }
}

token_t *lexer_token (lexer_t *this, token_type_t type) {
  this->end = this->reader->loc;
  return token_init(type, this->val, this->start, this->end);
}

lexer_t *lexer_walk (lexer_t *this, bool (*fn) (char)) {
  while (!reader_eof(this->reader)) {
    reader_location_t loc = this->reader->loc;
    char ch = reader_next(this->reader);

    if (!fn(ch)) {
      reader_seek(this->reader, loc);
      break;
    }

    this->val = realloc(this->val, ++this->val_len + 1);
    this->val[this->val_len - 1] = ch;
    this->val[this->val_len] = '\0';
  }

  return this;
}

lexer_t *lexer_walk_lit_float_exp (lexer_t *this) {
  if (reader_eof(this->reader)) {
    throw_syntax_error(this->reader, this->start, E0013);
  }

  char ch1 = reader_next(this->reader);

  if (ch1 == '+' || ch1 == '-') {
    if (reader_eof(this->reader)) {
      throw_syntax_error(this->reader, this->start, E0013);
    }

    char ch2 = reader_next(this->reader);

    if (!token_is_digit(ch2)) {
      lexer_walk(this, token_is_lit_id_continue);
      throw_syntax_error(this->reader, this->start, E0013);
    }

    this->val_len += 2;
    this->val = realloc(this->val, this->val_len + 1);
    this->val[this->val_len - 2] = ch1;
    this->val[this->val_len - 1] = ch2;
    this->val[this->val_len] = '\0';
  } else if (token_is_digit(ch1)) {
    this->val = realloc(this->val, ++this->val_len + 1);
    this->val[this->val_len - 1] = ch1;
    this->val[this->val_len] = '\0';
  } else {
    lexer_walk(this, token_is_lit_id_continue);
    throw_syntax_error(this->reader, this->start, E0013);
  }

  while (!reader_eof(this->reader)) {
    reader_location_t loc3 = this->reader->loc;
    char ch3 = reader_next(this->reader);

    if (!token_is_digit(ch3)) {
      reader_seek(this->reader, loc3);
      break;
    }

    this->val = realloc(this->val, ++this->val_len + 1);
    this->val[this->val_len - 1] = ch3;
    this->val[this->val_len] = '\0';
  }

  return this;
}

lexer_t *lexer_walk_lit_str (lexer_t *this) {
  size_t blocks = 0;
  bool inside_char = false;

  while (true) {
    reader_location_t loc1 = this->reader->loc;
    char ch1 = reader_next(this->reader);

    this->val = realloc(this->val, ++this->val_len + 1);
    this->val[this->val_len - 1] = ch1;
    this->val[this->val_len] = '\0';

    if (ch1 == '\\' && blocks == 0) {
      if (reader_eof(this->reader)) {
        throw_syntax_error(this->reader, this->start, E0003);
      }

      char ch2 = reader_next(this->reader);

      if (!token_is_lit_str_escape(ch2)) {
        throw_syntax_error(this->reader, loc1, E0005);
      }

      this->val = realloc(this->val, ++this->val_len + 1);
      this->val[this->val_len - 1] = ch2;
      this->val[this->val_len] = '\0';
    } else if (ch1 == '{' && !inside_char) {
      blocks += 1;
    } else if (ch1 == '}' && blocks != 0 && !inside_char) {
      blocks -= 1;
    } else if (ch1 == '\'' && inside_char) {
      inside_char = false;
    } else if (ch1 == '\'' && blocks != 0) {
      inside_char = true;
    } else if (ch1 == '"' && blocks != 0) {
      lexer_walk_lit_str(this);
    } else if (ch1 == '"') {
      break;
    } else if (reader_eof(this->reader)) {
      throw_syntax_error(this->reader, this->start, E0003);
    }
  }

  return this;
}
