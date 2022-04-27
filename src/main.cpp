/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <fstream>
#include <iostream>
#include "Codegen.hpp"

int main (int argc, char *argv[]) {
  try {
    if (argc == 1) {
      throw Error("Error: REPL is not supported");
    }

    auto isAST = false;
    auto isLex = false;
    auto isParse = false;
    auto fileName = std::optional<std::string>{};

    for (auto i = 1; i < argc; i++) {
      auto arg = std::string(argv[i]);
      auto isOpt = arg.substr(0, 1) == "-";

      if (i == 1 && arg == "ast") {
        isAST = true;
      } else if (i == 1 && arg == "lex") {
        isLex = true;
      } else if (i == 1 && arg == "parse") {
        isParse = true;
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
      while (true) {
        auto stmt = parser.next();

        if (std::holds_alternative<ParserStmtEof>(stmt.body)) {
          break;
        }

        std::cout << stmt.xml() << std::endl;
      }

      return EXIT_SUCCESS;
    }

    auto ast = AST(&parser);

    if (isAST) {
      auto nodes = ast.gen();

      for (const auto &node : nodes) {
        std::cout << node.xml() << std::endl;
      }

      return EXIT_SUCCESS;
    }

    auto codegen = Codegen(&ast);
    auto [code, flags] = codegen.gen();

    auto f = std::ofstream("build/output.c");
    f << code;
    f.close();

    auto cmd = "gcc build/output.c -o build/a.out " + flags;
    system(cmd.c_str());

    return EXIT_SUCCESS;
  } catch (const Error &err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}
