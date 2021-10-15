/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <iostream>
#include "Error.hpp"
#include "Reader.hpp"

int main () {
  try {
    Reader reader("program.adl");
    ReaderLocation loc = reader.loc;

    while (!reader.eof()) {
      std::cout << reader.next();
    }

    reader.seek(loc);

    while (!reader.eof()) {
      std::cout << reader.next();
    }

    return 0;
  } catch (const Error &err) {
    std::cerr << err.what() << std::endl;
    return 1;
  }
}
