/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

/** @created_at: 2018-05-08 09:55 */

#include <iostream>
#include "Codegen.hpp"

int main (int argc, char *argv[]) {
  try {
    if (argc == 1) {
      throw Error("REPL is not supported");
    }

    auto isAST = false;
    auto isCodegen = false;
    auto isLex = false;
    auto isParse = false;
    auto fileName = std::optional<std::string>{};

    for (auto i = 1; i < argc; i++) {
      auto arg = std::string(argv[i]);
      auto isOpt = arg.starts_with('-');

      if (i == 1 && arg == "ast") {
        isAST = true;
      } else if (i == 1 && arg == "codegen") {
        isCodegen = true;
      } else if (i == 1 && arg == "lex") {
        isLex = true;
      } else if (i == 1 && arg == "parse") {
        isParse = true;
      } else if (isOpt) {
        throw Error("bad option " + arg);
      } else if (fileName == std::nullopt) {
        fileName = arg;
      } else {
        throw Error("processing multiple files is not supported");
      }
    }

    if (fileName == std::nullopt) {
      throw Error("file is not specified");
    }

    auto reader = Reader(*fileName);
    auto lexer = Lexer(&reader);

    if (isLex) {
      while (true) {
        auto [_, tok] = lexer.next();

        if (tok.type == TK_EOF) {
          break;
        }

        std::cout << tok.str() << std::endl;
      }

      return EXIT_SUCCESS;
    }

    auto parser = Parser(&lexer);

    if (isParse) {
      std::cout << parser.xml();
      return EXIT_SUCCESS;
    }

    auto ast = AST(&parser);

    if (isAST) {
      std::cout << ast.xml();
      return EXIT_SUCCESS;
    }

    auto codegen = Codegen(&ast);
    auto result = codegen.gen();

    if (isCodegen) {
      std::cout << std::get<0>(result);
      return EXIT_SUCCESS;
    }

    Codegen::compile("build/a.out", result);
    return EXIT_SUCCESS;
  } catch (const Error &err) {
    std::cerr << "Error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}
