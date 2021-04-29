/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/testing.h>
#include "../../src/lexer/comment-block.h"
#include "../lexer-test.h"

DUC_TEST(lex_comment_block, works) {
  LEX_FE("/**/", "", LEXER_COMMENT_BLOCK);
  LEX_FE("/***/", "*", LEXER_COMMENT_BLOCK);
  LEX_FE("/* */", " ", LEXER_COMMENT_BLOCK);
  LEX_FE("/*Test*/", "Test", LEXER_COMMENT_BLOCK);
  LEX_FE("/*Test\nTest*/", "Test\nTest", LEXER_COMMENT_BLOCK);
  LEX_FE("/*\n*/", "\n", LEXER_COMMENT_BLOCK);
  LEX_FE("/*\n\n*/", "\n\n", LEXER_COMMENT_BLOCK);
}

int main () {
  DUC_TEST_RUN(lex_comment_block, works);
}
