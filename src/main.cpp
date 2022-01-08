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

const char outputFlags[] = "-Wno-constant-logical-operand -lm";

int main () {
  try {
    auto reader = Reader("program.adl");
    auto ast = analyze(&reader);
    auto result = codegen(&ast);
    auto f = std::ofstream("build/output.c");

    f << result.output;
    f.close();

    system((std::string("gcc build/output.c -o build/a.out ") + outputFlags).c_str());

    return EXIT_SUCCESS;
  } catch (const SyntaxError &err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const Error &err) {
    std::cerr << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}
