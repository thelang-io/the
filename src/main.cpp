/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <iostream>
#include "Error.hpp"
#include "Parser.hpp"

int main () {
  try {
    auto reader = Reader("program.adl");

    while (!reader.eof()) {
      auto stmt = parse(&reader);

      if (stmt.type == stmtEnd) {
        break;
      }

      std::cout << stmtStr(stmt) << std::endl;
    }

    return 0;
  } catch (const Error &err) {
    std::cerr << err.what() << std::endl;
    return 1;
  }
}
