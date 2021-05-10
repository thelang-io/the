/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "../src/fs.h"
#include "testing.h"

#define FILE_F(text, body) \
  do { writefile("../test.txt", text); \
  file_t *file = file_new("../test.txt", FILE_READ); \
  body \
  file_free(file); \
  file_remove("../test.txt"); } while (0)

TEST(file, eof_and_read) {
  FILE_F("", {
    ASSERT_TRUE(file_eof(file));
  });

  FILE_F("\n", {
    ASSERT_FALSE(file_eof(file));
    char *line = file_readline(file);

    ASSERT_STREQ(line, "\n");
    ASSERT_TRUE(file_eof(file));
    free(line);
  });

  FILE_F("Test", {
    ASSERT_FALSE(file_eof(file));
    char *line = file_readline(file);

    ASSERT_STREQ(line, "Test");
    ASSERT_TRUE(file_eof(file));
    free(line);
  });

  FILE_F("Test\n", {
    ASSERT_FALSE(file_eof(file));
    char *line = file_readline(file);

    ASSERT_STREQ(line, "Test\n");
    ASSERT_TRUE(file_eof(file));
    free(line);
  });

  FILE_F("Test1\nTest2", {
    ASSERT_FALSE(file_eof(file));
    char *line1 = file_readline(file);
    ASSERT_STREQ(line1, "Test1\n");
    ASSERT_FALSE(file_eof(file));
    free(line1);

    char *line2 = file_readline(file);
    ASSERT_STREQ(line2, "Test2");
    ASSERT_TRUE(file_eof(file));
    free(line2);
  });

  FILE_F("Test1\nTest2\n", {
    ASSERT_FALSE(file_eof(file));
    char *line1 = file_readline(file);
    ASSERT_STREQ(line1, "Test1\n");
    ASSERT_FALSE(file_eof(file));
    free(line1);

    size_t pos = file_position(file);
    char *line2 = file_readline(file);
    ASSERT_STREQ(line2, "Test2\n");
    ASSERT_TRUE(file_eof(file));
    free(line2);
    file_seek(file, pos);
    ASSERT_FALSE(file_eof(file));

    char *line3 = file_readline(file);
    ASSERT_STREQ(line3, "Test2\n");
    ASSERT_TRUE(file_eof(file));
    free(line3);
  });

  FILE_F("\n", {
    ASSERT_FALSE(file_eof(file));
    ASSERT_EQ(file_readchar(file), '\n');
    ASSERT_TRUE(file_eof(file));
  });

  FILE_F("Test", {
    ASSERT_FALSE(file_eof(file));
    ASSERT_EQ(file_readchar(file), 'T');
    ASSERT_FALSE(file_eof(file));
    ASSERT_EQ(file_readchar(file), 'e');
    ASSERT_FALSE(file_eof(file));
    ASSERT_EQ(file_readchar(file), 's');
    ASSERT_FALSE(file_eof(file));

    size_t pos = file_position(file);
    ASSERT_EQ(file_readchar(file), 't');
    ASSERT_TRUE(file_eof(file));
    file_seek(file, pos);

    ASSERT_FALSE(file_eof(file));
    ASSERT_EQ(file_readchar(file), 't');
    ASSERT_TRUE(file_eof(file));
  });

  FILE_F("Test\n", {
    ASSERT_FALSE(file_eof(file));
    ASSERT_EQ(file_readchar(file), 'T');
    ASSERT_FALSE(file_eof(file));
    ASSERT_EQ(file_readchar(file), 'e');
    ASSERT_FALSE(file_eof(file));
    ASSERT_EQ(file_readchar(file), 's');
    ASSERT_FALSE(file_eof(file));
    ASSERT_EQ(file_readchar(file), 't');
    ASSERT_FALSE(file_eof(file));
    ASSERT_EQ(file_readchar(file), '\n');
    ASSERT_TRUE(file_eof(file));
  });
}

TEST(file, exists_remove_write) {
  char *filepath = "../test.txt";
  ASSERT_FALSE(file_exists(filepath));

  file_t *file = file_new(filepath, FILE_WRITE);

  file_writeline(file, "Hello, World!");
  file_writechar(file, '\n');
  file_free(file);
  ASSERT_TRUE(file_exists(filepath));

  file = file_new(filepath, FILE_READ);
  char *line = file_readline(file);

  ASSERT_STREQ(line, "Hello, World!\n");
  file_free(file);
  free(line);

  file_remove(filepath);
  ASSERT_FALSE(file_exists(filepath));
}

TEST(file, new_and_free) {
  file_t *file = file_new("../fake.txt", FILE_READ);
  ASSERT_EQ(file, NULL);

  char *filepath = "../test.txt";

  file = file_new(filepath, FILE_WRITE);
  ASSERT_NE(file, NULL);
  file_free(file);

  file = file_new(filepath, FILE_READ);
  ASSERT_NE(file, NULL);
  file_free(file);

  file = file_new(filepath, FILE_APPEND);
  ASSERT_NE(file, NULL);
  file_free(file);

  file_remove(filepath);
}

TEST(file, position_and_seek) {
  file_t *file = file_new("../CMakeLists.txt", FILE_READ);

  ASSERT_EQ(file_position(file), 0);
  file_seek(file, 1);
  ASSERT_EQ(file_position(file), 1);
  file_seek(file, 2);
  ASSERT_EQ(file_position(file), 2);

  file_free(file);
}

TEST(path, cwd_and_real) {
  char *cwd = path_cwd();
  char *path = path_real("../CMakeLists.txt");
  char *non_path = path_real("../fake");

  ASSERT_NE(cwd, NULL);
  ASSERT_STRNE(cwd, "");
  ASSERT_NE(path, NULL);
  ASSERT_STRNE(path, "");
  ASSERT_EQ(non_path, NULL);

  free(cwd);
  free(path);
}

TEST(read_write, file) {
  char *filepath = "../test.txt";

  ASSERT_FALSE(file_exists(filepath));
  writefile(filepath, "Hello, World!\n");
  ASSERT_TRUE(file_exists(filepath));

  char *content = readfile(filepath);
  ASSERT_STREQ(content, "Hello, World!\n");
  free(content);

  file_remove(filepath);
}

int main () {
  TEST_RUN(file, eof_and_read);
  TEST_RUN(file, exists_remove_write);
  TEST_RUN(file, new_and_free);
  TEST_RUN(file, position_and_seek);
  TEST_RUN(path, cwd_and_real);
  TEST_RUN(read_write, file);
}
