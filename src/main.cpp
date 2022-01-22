/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <iostream>
#include <optional>
#include <string>
#include "Error.hpp"
#include "Lexer.hpp"

int main (int argc, char *argv[]) {
  try {
    if (argc == 1) {
      throw Error("Error: REPL is not supported");
    }

    auto isLex = false;
    auto fileName = std::optional<std::string>{};

    for (int i = 1; i < argc; i++) {
      auto arg = std::string(argv[i]);
      auto isOpt = arg.substr(0, 1) == "-";

      if (i == 1 && arg == "lex") {
        isLex = true;
      } else if (isOpt) {
        throw Error("Error: bad option " + arg);
      } else if (fileName == std::nullopt) {
        fileName = arg;
      } else {
        throw Error("Error: processing multiple files is not supported");
      }
    }

    if (fileName == std::nullopt) {
      throw Error("Error: file is not specified");
    }

    auto reader = Reader(*fileName);
    auto lexer = Lexer(&reader);

    if (isLex) {
      while (true) {
        auto tok = lexer.next();

        if (tok.type == TK_EOF) {
          break;
        }

        std::cout << tok.str() << std::endl;
      }

      return EXIT_SUCCESS;
    }
  } catch (const Error &err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}
