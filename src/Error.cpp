/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"

Error::Error (const std::string &message) : message("Error: " + message) {
}

Error::Error (const std::string &message, const fs::path &path)
  : Error(message + "\n  at " + path.string()) {
}

Error::Error (const std::string &message, const fs::path &path, size_t line)
  : Error(message + "\n  at " + path.string() + ":" + std::to_string(line)) {
}

Error::Error (
  const std::string &message,
  const fs::path &path,
  size_t line,
  size_t col
) : Error(
  message + "\n  at " +
  path.string() + ":" + std::to_string(line) + ":" + std::to_string(col)
) {
}
