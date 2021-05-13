/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <cstdlib>
#include <iostream>
#include "Error.hpp"
#include "Lexer.hpp"

int main (int argc, const char *argv[]) {
  try {
    if (argc == 1) {
      throw Error("Action is not set");
    } else if (argv[1] != std::string("lex")) {
      throw Error("Unknown action '" + std::string(argv[1]) + "'");
    } else if (argc < 3) {
      throw Error("File path is not set");
    }

    auto reader = Reader(argv[2]);
    auto lexer = Lexer(&reader);

    while (true) {
      const auto tok = lexer.next();

      if (tok == eof) {
        break;
      }

      std::cout << tok.val << ": " << tok.str() << std::endl;
    }
  } catch (const Error &err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
