/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_ERROR_HPP
#define SRC_ERROR_HPP

#include <filesystem>

namespace fs = std::filesystem;

class Error : private std::exception {
 public:
  const std::string message;

  Error (const std::string &msg);
  Error (const std::string &msg, const fs::path &path);
  Error (const std::string &msg, const fs::path &path, size_t line);

  Error (
    const std::string &msg,
    const fs::path &path,
    size_t line,
    size_t col
  );
};

#endif
