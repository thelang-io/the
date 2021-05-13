/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <cstdlib>
#include <iostream>
#include "Error.hpp"
#include "Reader.hpp"

int main (int argc, const char *argv[]) {
  try {
    if (argc == 1) {
      throw Error("Action is not set");
    } else if (argv[1] != std::string("lex")) {
      throw Error("Unknown action '" + std::string(argv[1]) + "'");
    } else if (argc < 3) {
      throw Error("File path is not set");
    }

    Reader reader(argv[2]);

    while (!reader.eof()) {
      std::cout << reader.next();
    }
  } catch (const Error &err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
