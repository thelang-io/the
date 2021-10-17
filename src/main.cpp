/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <fstream>
#include <iostream>
#include "Codegen.hpp"
#include "Error.hpp"

int main () {
  try {
    auto reader = Reader("program.adl");
    auto ast = new AST();

    while (!reader.eof()) {
      auto stmt = parse(&reader);

      if (stmt->type == STMT_END) {
        delete stmt;
        break;
      }

      ast->add(stmt);
      delete stmt;
    }

    auto code = codegen(ast);
    auto f = std::ofstream("output.c");

    f << code;
    f.close();
    system("gcc output.c -o a.out");
    fs::remove("output.c");

    delete ast;
    return EXIT_SUCCESS;
  } catch (const SyntaxError &err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const Error &err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}
