/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef ANON_MAP_HPP
#define ANON_MAP_HPP

#include <string>
#include <vector>

struct AnonMapItem {
  std::string name;
};

class AnonMap {
 public:
  std::string add (const std::string &);

 private:
  std::vector<AnonMapItem> _items;
};

#endif
