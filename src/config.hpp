/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_CONFIG_HPP
#define SRC_CONFIG_HPP

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  #define EOL "\r\n"
  #define ESC_EOL "\\r\\n"
  #define OS_FILE_EXT ".exe"
  #define OS_PATH_SEP "\\"
  #define OS_WINDOWS
#else
  #if defined(__APPLE__)
    #define OS_MACOS
  #elif defined(__linux__)
    #define OS_LINUX
  #endif

  #define EOL "\n"
  #define ESC_EOL "\\n"
  #define OS_FILE_EXT ".out"
  #define OS_PATH_SEP "/"
#endif

#endif
