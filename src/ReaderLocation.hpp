/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_READER_LOCATION_HPP
#define SRC_READER_LOCATION_HPP

#include <cstddef>

struct ReaderLocation {
  std::size_t pos = 0;
  std::size_t line = 1;
  std::size_t col = 0;
};

#endif
