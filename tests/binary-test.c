/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include "../src/binary.h"
#include "../src/fs.h"
#include "testing.h"

TEST(binary, append_at_size) {
  binary_t *binary0 = binary_new();
  ASSERT_EQ(binary_at(binary0, 0), 0x00);
  ASSERT_EQ(binary_size(binary0), 0);

  uint8_t data[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA };

  binary_append_data(binary0, data, sizeof(data));
  ASSERT_EQ(binary_size(binary0), 6);
  ASSERT_EQ(binary_at(binary0, 0), 0xFF);
  ASSERT_EQ(binary_at(binary0, 1), 0xFE);
  ASSERT_EQ(binary_at(binary0, 2), 0xFD);
  ASSERT_EQ(binary_at(binary0, 3), 0xFC);
  ASSERT_EQ(binary_at(binary0, 4), 0xFB);
  ASSERT_EQ(binary_at(binary0, 5), 0xFA);

  binary_t *binary = binary_new();

  binary_append_binary(binary, binary0);
  ASSERT_EQ(binary_size(binary), 6);
  ASSERT_EQ(binary_at(binary, 0), 0xFF);
  ASSERT_EQ(binary_at(binary, 1), 0xFE);
  ASSERT_EQ(binary_at(binary, 2), 0xFD);
  ASSERT_EQ(binary_at(binary, 3), 0xFC);
  ASSERT_EQ(binary_at(binary, 4), 0xFB);
  ASSERT_EQ(binary_at(binary, 5), 0xFA);
  binary_free(binary0);

  size_t offset = binary_size(binary);

  binary_append_times(binary, '_', 0);
  binary_append_times(binary, '$', 2);
  ASSERT_EQ(binary_size(binary), offset + 2);
  ASSERT_EQ(binary_at(binary, offset + 0), '$');
  ASSERT_EQ(binary_at(binary, offset + 1), '$');
  offset = binary_size(binary);

  binary_append_int8(binary, 0x01);
  binary_append_int8(binary, -0x02, 0x03);
  binary_append_int8(binary, -0x04, 0x05, -0x06);
  ASSERT_EQ(binary_size(binary), offset + 6);
  ASSERT_EQ(binary_at(binary, offset + 0), 0x01);
  ASSERT_EQ(binary_at(binary, offset + 1), 0xFE);
  ASSERT_EQ(binary_at(binary, offset + 2), 0x03);
  ASSERT_EQ(binary_at(binary, offset + 3), 0xFC);
  ASSERT_EQ(binary_at(binary, offset + 4), 0x05);
  ASSERT_EQ(binary_at(binary, offset + 5), 0xFA);
  offset = binary_size(binary);

  binary_append_uint8(binary, 0x01);
  binary_append_uint8(binary, 0x02, 0x03);
  binary_append_uint8(binary, 0x04, 0x05, 0x06);
  ASSERT_EQ(binary_size(binary), offset + 6);
  ASSERT_EQ(binary_at(binary, offset + 0), 0x01);
  ASSERT_EQ(binary_at(binary, offset + 1), 0x02);
  ASSERT_EQ(binary_at(binary, offset + 2), 0x03);
  ASSERT_EQ(binary_at(binary, offset + 3), 0x04);
  ASSERT_EQ(binary_at(binary, offset + 4), 0x05);
  ASSERT_EQ(binary_at(binary, offset + 5), 0x06);
  offset = binary_size(binary);

  binary_append_int16(binary, 0x0FF1);
  binary_append_int16(binary, -0x0FF2, 0x0FF3);
  binary_append_int16(binary, -0x0FF4, 0x0FF5, -0x0FF6);
  ASSERT_EQ(binary_size(binary), offset + 6 * sizeof(int16_t));
  ASSERT_EQ(binary_at(binary, offset + 0), 0xF1);
  ASSERT_EQ(binary_at(binary, offset + 1), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 2), 0x0E);
  ASSERT_EQ(binary_at(binary, offset + 3), 0xF0);
  ASSERT_EQ(binary_at(binary, offset + 4), 0xF3);
  ASSERT_EQ(binary_at(binary, offset + 5), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 6), 0x0C);
  ASSERT_EQ(binary_at(binary, offset + 7), 0xF0);
  ASSERT_EQ(binary_at(binary, offset + 8), 0xF5);
  ASSERT_EQ(binary_at(binary, offset + 9), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 10), 0x0A);
  ASSERT_EQ(binary_at(binary, offset + 11), 0xF0);
  offset = binary_size(binary);

  binary_append_uint16(binary, 0x0FF1);
  binary_append_uint16(binary, 0x0FF2, 0x0FF3);
  binary_append_uint16(binary, 0x0FF4, 0x0FF5, 0x0FF6);
  ASSERT_EQ(binary_size(binary), offset + 6 * sizeof(uint16_t));
  ASSERT_EQ(binary_at(binary, offset + 0), 0xF1);
  ASSERT_EQ(binary_at(binary, offset + 1), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 2), 0xF2);
  ASSERT_EQ(binary_at(binary, offset + 3), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 4), 0xF3);
  ASSERT_EQ(binary_at(binary, offset + 5), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 6), 0xF4);
  ASSERT_EQ(binary_at(binary, offset + 7), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 8), 0xF5);
  ASSERT_EQ(binary_at(binary, offset + 9), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 10), 0xF6);
  ASSERT_EQ(binary_at(binary, offset + 11), 0x0F);
  offset = binary_size(binary);

  binary_append_int32(binary, 0x0FFFFFF1);
  binary_append_int32(binary, -0x0FFFFFF2, 0x0FFFFFF3);
  binary_append_int32(binary, -0x0FFFFFF4, 0x0FFFFFF5, -0x0FFFFFF6);
  ASSERT_EQ(binary_size(binary), offset + 6 * sizeof(int32_t));
  ASSERT_EQ(binary_at(binary, offset + 0), 0xF1);
  ASSERT_EQ(binary_at(binary, offset + 1), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 2), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 3), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 4), 0x0E);
  ASSERT_EQ(binary_at(binary, offset + 5), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 6), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 7), 0xF0);
  ASSERT_EQ(binary_at(binary, offset + 8), 0xF3);
  ASSERT_EQ(binary_at(binary, offset + 9), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 10), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 11), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 12), 0x0C);
  ASSERT_EQ(binary_at(binary, offset + 13), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 14), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 15), 0xF0);
  ASSERT_EQ(binary_at(binary, offset + 16), 0xF5);
  ASSERT_EQ(binary_at(binary, offset + 17), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 18), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 19), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 20), 0x0A);
  ASSERT_EQ(binary_at(binary, offset + 21), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 22), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 23), 0xF0);
  offset = binary_size(binary);

  binary_append_uint32(binary, 0x0FFFFFF1);
  binary_append_uint32(binary, 0x0FFFFFF2, 0x0FFFFFF3);
  binary_append_uint32(binary, 0x0FFFFFF4, 0x0FFFFFF5, 0x0FFFFFF6);
  ASSERT_EQ(binary_size(binary), offset + 6 * sizeof(uint32_t));
  ASSERT_EQ(binary_at(binary, offset + 0), 0xF1);
  ASSERT_EQ(binary_at(binary, offset + 1), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 2), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 3), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 4), 0xF2);
  ASSERT_EQ(binary_at(binary, offset + 5), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 6), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 7), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 8), 0xF3);
  ASSERT_EQ(binary_at(binary, offset + 9), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 10), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 11), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 12), 0xF4);
  ASSERT_EQ(binary_at(binary, offset + 13), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 14), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 15), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 16), 0xF5);
  ASSERT_EQ(binary_at(binary, offset + 17), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 18), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 19), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 20), 0xF6);
  ASSERT_EQ(binary_at(binary, offset + 21), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 22), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 23), 0x0F);
  offset = binary_size(binary);

  binary_append_int64(binary, 0x0FFFFFFFFFFFFFF1);
  binary_append_int64(binary, -0x0FFFFFFFFFFFFFF2, 0x0FFFFFFFFFFFFFF3);
  binary_append_int64(binary, -0x0FFFFFFFFFFFFFF4, 0x0FFFFFFFFFFFFFF5, -0x0FFFFFFFFFFFFFF6);
  ASSERT_EQ(binary_size(binary), offset + 6 * sizeof(int64_t));
  ASSERT_EQ(binary_at(binary, offset + 0), 0xF1);
  ASSERT_EQ(binary_at(binary, offset + 1), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 2), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 3), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 4), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 5), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 6), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 7), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 8), 0x0E);
  ASSERT_EQ(binary_at(binary, offset + 9), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 10), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 11), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 12), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 13), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 14), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 15), 0xF0);
  ASSERT_EQ(binary_at(binary, offset + 16), 0xF3);
  ASSERT_EQ(binary_at(binary, offset + 17), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 18), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 19), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 20), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 21), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 22), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 23), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 24), 0x0C);
  ASSERT_EQ(binary_at(binary, offset + 25), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 26), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 27), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 28), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 29), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 30), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 31), 0xF0);
  ASSERT_EQ(binary_at(binary, offset + 32), 0xF5);
  ASSERT_EQ(binary_at(binary, offset + 33), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 34), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 35), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 36), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 37), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 38), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 39), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 40), 0x0A);
  ASSERT_EQ(binary_at(binary, offset + 41), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 42), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 43), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 44), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 45), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 46), 0x00);
  ASSERT_EQ(binary_at(binary, offset + 47), 0xF0);
  offset = binary_size(binary);

  binary_append_uint64(binary, 0x0FFFFFFFFFFFFFF1);
  binary_append_uint64(binary, 0x0FFFFFFFFFFFFFF2, 0x0FFFFFFFFFFFFFF3);
  binary_append_uint64(binary, 0x0FFFFFFFFFFFFFF4, 0x0FFFFFFFFFFFFFF5, 0x0FFFFFFFFFFFFFF6);
  ASSERT_EQ(binary_size(binary), offset + 6 * sizeof(uint64_t));
  ASSERT_EQ(binary_at(binary, offset + 0), 0xF1);
  ASSERT_EQ(binary_at(binary, offset + 1), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 2), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 3), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 4), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 5), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 6), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 7), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 8), 0xF2);
  ASSERT_EQ(binary_at(binary, offset + 9), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 10), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 11), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 12), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 13), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 14), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 15), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 16), 0xF3);
  ASSERT_EQ(binary_at(binary, offset + 17), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 18), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 19), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 20), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 21), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 22), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 23), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 24), 0xF4);
  ASSERT_EQ(binary_at(binary, offset + 25), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 26), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 27), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 28), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 29), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 30), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 31), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 32), 0xF5);
  ASSERT_EQ(binary_at(binary, offset + 33), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 34), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 35), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 36), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 37), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 38), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 39), 0x0F);
  ASSERT_EQ(binary_at(binary, offset + 40), 0xF6);
  ASSERT_EQ(binary_at(binary, offset + 41), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 42), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 43), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 44), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 45), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 46), 0xFF);
  ASSERT_EQ(binary_at(binary, offset + 47), 0x0F);
  offset = binary_size(binary);

  const char *string = "Test";
  binary_append_string(binary, string);
  ASSERT_EQ(binary_size(binary), offset + strlen(string) + 1);
  ASSERT_EQ(binary_at(binary, offset + 0), 'T');
  ASSERT_EQ(binary_at(binary, offset + 1), 'e');
  ASSERT_EQ(binary_at(binary, offset + 2), 's');
  ASSERT_EQ(binary_at(binary, offset + 3), 't');
  ASSERT_EQ(binary_at(binary, offset + 4), '\0');
  offset = binary_size(binary);

  binary_append_times(binary, 'A', 2);
  binary_append_times(binary, '0', 4);
  ASSERT_EQ(binary_size(binary), offset + 6);
  ASSERT_EQ(binary_at(binary, offset + 0), 'A');
  ASSERT_EQ(binary_at(binary, offset + 1), 'A');
  ASSERT_EQ(binary_at(binary, offset + 2), '0');
  ASSERT_EQ(binary_at(binary, offset + 3), '0');
  ASSERT_EQ(binary_at(binary, offset + 4), '0');
  ASSERT_EQ(binary_at(binary, offset + 5), '0');

  binary_free(binary);
}

TEST(binary, data_and_empty) {
  binary_t *binary = binary_new();

  ASSERT_TRUE(binary_empty(binary));
  binary_append_string(binary, "Hello, World!");
  ASSERT_FALSE(binary_empty(binary));
  ASSERT_MEMEQ(binary_data(binary), "Hello, World!", 14);

  binary_free(binary);
}

TEST(binary, write) {
  binary_t *binary = binary_new();
  char string[] = "Hello, World!";

  binary_append_string(binary, string);
  binary_write(binary, "../test.txt");
  binary_free(binary);

  ASSERT_TRUE(file_exists("../test.txt"));
  char *content = readfile("../test.txt");

  ASSERT_STREQ(content, string);
  free(content);
  file_remove("../test.txt");
}

int main () {
  TEST_RUN(binary, append_at_size);
  TEST_RUN(binary, data_and_empty);
  TEST_RUN(binary, write);
}
