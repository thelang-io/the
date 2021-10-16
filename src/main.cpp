/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <fstream>
#include <iostream>
#include "AST.hpp"
#include "Codegen.hpp"
#include "Error.hpp"
#include "Parser.hpp"

int main () {
  try {
    auto reader = Reader("program.adl");
    auto ast = AST();

    while (!reader.eof()) {
      auto stmt = parse(&reader);

      if (stmt.type == stmtEnd) {
        break;
      }

      ast.add(stmt);
    }

    auto code = codegen(ast);
    auto f = std::ofstream("output.c");

    f << code;
    f.close();
    system("gcc output.c -o a.out");
    fs::remove("output.c");

    return EXIT_SUCCESS;
  } catch (const Error &err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}
