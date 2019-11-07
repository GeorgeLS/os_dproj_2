#ifndef EXERCISE_II__UTILS_H_
#define EXERCISE_II__UTILS_H_

#include "common.h"
#include "array.h"
#include "record.h"

Array<Record> load_records_from_file(const char *filename, off64_t start, off64_t end);

bool string_to_i64(char *string, i64 *out_i64);

size_t file_size_in_bytes(const char *filename);

template<typename T>
char *to_string(T value);

template<typename... Args>
char *to_string(const char *fmt, Args... args) {
  char *str;
  asprintf(&str, fmt, args...);
  return str;
}
#endif //EXERCISE_II__UTILS_H_
