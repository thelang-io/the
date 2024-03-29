/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SRC_UTILS_HPP
#define SRC_UTILS_HPP

#include <optional>
#include <string>
#include <vector>

std::string convert_path_to_namespace (const std::string &);
std::string str_trim (const std::string &);
std::vector<std::string> str_lines (const std::string &);
std::string str_replace_all (const std::string &, const std::string &, const std::string &);
std::optional<std::string> parse_package_yaml_main (const std::string &);

#endif
