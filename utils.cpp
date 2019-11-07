#include <cinttypes>
#include <cstdlib>
#include <cassert>
#include <fcntl.h>
#include <zconf.h>
#include <sys/stat.h>
#include "utils.h"

bool string_to_i64(char *string, i64 *out_i64) {
  char *valid;
  *out_i64 = strtoll(string, &valid, 10);
  return *valid == '\0';
}

Array<Record> load_records_from_file(const char *filename, off64_t start, off64_t end) {
  off64_t records_n = end - start;
  assert(records_n > 0);
  int fd = open(filename, O_RDONLY);
  off64_t end_pos = lseek64(fd, end * sizeof(Record), SEEK_SET);
  off64_t start_pos = lseek64(fd, start * sizeof(Record), SEEK_SET);
  assert(records_n * sizeof(Record) == end_pos - start_pos);
  Array<Record> records((size_t) records_n);
  read(fd, records.data, records_n * sizeof(Record));
  records.size = records.capacity;
  close(fd);
  return records;
}

size_t file_size_in_bytes(const char *filename) {
  struct stat info{};
  lstat(filename, &info);
  return info.st_size;
}

template<>
char *to_string<>(i8 v) {
  return to_string("%" PRId8, v);
}

template<>
char *to_string<>(u8 v) {
  return to_string("%" PRIu8, v);
}

template<>
char *to_string<>(i16 v) {
  return to_string("%" PRId16, v);
}

template<>
char *to_string<>(u16 v) {
  return to_string("%" PRIu16, v);
}

template<>
char *to_string<>(i32 v) {
  return to_string("%" PRId32, v);
}

template<>
char *to_string<>(u32 v) {
  return to_string("%" PRIu32, v);
}

template<>
char *to_string<>(i64 v) {
  return to_string("%" PRId64, v);
}

template<>
char *to_string<>(u64 v) {
  return to_string("%" PRIu64, v);
}

template<>
char *to_string<>(f32 v) {
  return to_string("%f", v);
}

template<>
char *to_string(f64 v) {
  return to_string("%lf", v);
}