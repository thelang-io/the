/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "id.h"
#include "keyword.h"

bool lexer_keyword (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = duc_file_readchar(file);

  if (ch1 == 'f' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'n') {
      if (!duc_file_eof(file)) {
        size_t bu_pos = duc_file_position(file);
        unsigned char ch_id = duc_file_readchar(file);
        duc_file_seek(file, bu_pos);

        if (!lexer_id_is_char(ch_id)) {
          goto lexer_is_keyword_fn;
        }
      } else {
lexer_is_keyword_fn:
        lexer->raw = malloc(3);
        lexer->str = malloc(3);
        lexer->raw[0] = ch1;
        lexer->raw[1] = ch2;
        lexer->raw[2] = '\0';
        memcpy(lexer->str, lexer->raw, 3);
        lexer->token = LEXER_FN;

        return true;
      }
    }
  } else if (ch1 == 'i' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'n') {
      if (!duc_file_eof(file)) {
        size_t bu_pos = duc_file_position(file);
        unsigned char ch_id = duc_file_readchar(file);
        duc_file_seek(file, bu_pos);

        if (!lexer_id_is_char(ch_id)) {
          goto lexer_is_keyword_in;
        }
      } else {
lexer_is_keyword_in:
        lexer->raw = malloc(3);
        lexer->str = malloc(3);
        lexer->raw[0] = ch1;
        lexer->raw[1] = ch2;
        lexer->raw[2] = '\0';
        memcpy(lexer->str, lexer->raw, 3);
        lexer->token = LEXER_IN;

        return true;
      }
    }
  } else if (ch1 == 'l' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'o' && !duc_file_eof(file)) {
      unsigned char ch3 = duc_file_readchar(file);

      if (ch3 == 'o' && !duc_file_eof(file)) {
        unsigned char ch4 = duc_file_readchar(file);

        if (ch4 == 'p') {
          if (!duc_file_eof(file)) {
            size_t bu_pos = duc_file_position(file);
            unsigned char ch_id = duc_file_readchar(file);
            duc_file_seek(file, bu_pos);

            if (!lexer_id_is_char(ch_id)) {
              goto lexer_is_keyword_loop;
            }
          } else {
lexer_is_keyword_loop:
            lexer->raw = malloc(5);
            lexer->str = malloc(5);
            lexer->raw[0] = ch1;
            lexer->raw[1] = ch2;
            lexer->raw[2] = ch3;
            lexer->raw[3] = ch4;
            lexer->raw[4] = '\0';
            memcpy(lexer->str, lexer->raw, 5);
            lexer->token = LEXER_LOOP;

            return true;
          }
        }
      }
    }
  } else if (ch1 == 'm' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'a' && !duc_file_eof(file)) {
      unsigned char ch3 = duc_file_readchar(file);

      if (ch3 == 'i' && !duc_file_eof(file)) {
        unsigned char ch4 = duc_file_readchar(file);

        if (ch4 == 'n') {
          if (!duc_file_eof(file)) {
            size_t bu_pos = duc_file_position(file);
            unsigned char ch_id = duc_file_readchar(file);
            duc_file_seek(file, bu_pos);

            if (!lexer_id_is_char(ch_id)) {
              goto lexer_is_keyword_main;
            }
          } else {
lexer_is_keyword_main:
            lexer->raw = malloc(5);
            lexer->str = malloc(5);
            lexer->raw[0] = ch1;
            lexer->raw[1] = ch2;
            lexer->raw[2] = ch3;
            lexer->raw[3] = ch4;
            lexer->raw[4] = '\0';
            memcpy(lexer->str, lexer->raw, 5);
            lexer->token = LEXER_MAIN;

            return true;
          }
        }
      }
    } else if (ch2 == 'u' && !duc_file_eof(file)) {
      unsigned char ch3 = duc_file_readchar(file);

      if (ch3 == 't') {
        if (!duc_file_eof(file)) {
          size_t bu_pos = duc_file_position(file);
          unsigned char ch_id = duc_file_readchar(file);
          duc_file_seek(file, bu_pos);

          if (!lexer_id_is_char(ch_id)) {
            goto lexer_is_keyword_mut;
          }
        } else {
lexer_is_keyword_mut:
          lexer->raw = malloc(4);
          lexer->str = malloc(4);
          lexer->raw[0] = ch1;
          lexer->raw[1] = ch2;
          lexer->raw[2] = ch3;
          lexer->raw[3] = '\0';
          memcpy(lexer->str, lexer->raw, 4);
          lexer->token = LEXER_MUT;

          return true;
        }
      }
    }
  } else if (ch1 == 'r' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'e' && !duc_file_eof(file)) {
      unsigned char ch3 = duc_file_readchar(file);

      if (ch3 == 't' && !duc_file_eof(file)) {
        unsigned char ch4 = duc_file_readchar(file);

        if (ch4 == 'u' && !duc_file_eof(file)) {
          unsigned char ch5 = duc_file_readchar(file);

          if (ch5 == 'r' && !duc_file_eof(file)) {
            unsigned char ch6 = duc_file_readchar(file);

            if (ch6 == 'n') {
              if (!duc_file_eof(file)) {
                size_t bu_pos = duc_file_position(file);
                unsigned char ch_id = duc_file_readchar(file);
                duc_file_seek(file, bu_pos);

                if (!lexer_id_is_char(ch_id)) {
                  goto lexer_is_keyword_return;
                }
              } else {
lexer_is_keyword_return:
                lexer->raw = malloc(7);
                lexer->str = malloc(7);
                lexer->raw[0] = ch1;
                lexer->raw[1] = ch2;
                lexer->raw[2] = ch3;
                lexer->raw[3] = ch4;
                lexer->raw[4] = ch5;
                lexer->raw[5] = ch6;
                lexer->raw[6] = '\0';
                memcpy(lexer->str, lexer->raw, 7);
                lexer->token = LEXER_RETURN;

                return true;
              }
            }
          }
        }
      }
    }
  }

  duc_file_seek(file, pos);
  return false;
}
