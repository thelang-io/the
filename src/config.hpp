/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_CONFIG_HPP
#define SRC_CONFIG_HPP

#ifdef _WIN32
  #define EOL "\r\n"
  #define ESC_EOL "\\r\\n"
  #define OS_WINDOWS
#else
  #define EOL "\n"
  #define ESC_EOL "\\n"
  #define OS_UNIX
#endif

#endif
