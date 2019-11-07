#ifndef EXERCISE_II__SORTER_DATA_STRUCTURES_H_
#define EXERCISE_II__SORTER_DATA_STRUCTURES_H_

#include <cstring>
#include "common.h"
#include "record.h"
#include "array.h"

enum class Column_Type {
  I64,
  I32,
  F32,
  CHAR_20,
  CHAR_6
};

template<typename T>
internal int compare_numeric(const byte *lhs, const byte *rhs) {
  T v1 = *(T*)lhs;
  T v2 = *(T*)rhs;
  return v1 < v2 ? -1 : v1 == v2 ? 0 : 1;
}

template<typename T, size_t N>
internal int compare_alphabeticaly(byte *lhs, byte *rhs) {
  T *v1 = (T*)lhs;
  T *v2 = (T*)rhs;
  int order = strncmp(v1, v2, N);
  return order < 0 ? -1 : order > 0 ? 1 : 0;
}

struct Column {
  byte *data;
  Record *record;

  static int compare(Column lhs, Column rhs, Column_Type type) {
    switch (type) {
      case Column_Type::I64: return compare_numeric<i64>(lhs.data, rhs.data);
      case Column_Type::I32: return compare_numeric<i32>(lhs.data, rhs.data);
      case Column_Type::F32: return compare_numeric<f32>(lhs.data, rhs.data);
      case Column_Type::CHAR_20: return compare_alphabeticaly<char, 20>(lhs.data, rhs.data);
      case Column_Type::CHAR_6: return compare_alphabeticaly<char, 6>(lhs.data, rhs.data);
    }
  }
};

struct Column_Collection {
  Array<Column> columns;
  Column_Type type;

  void print() {
    for (Column c : columns) {
      switch (type) {
        case Column_Type::I64:
          report("Data = %ld", *(i64*)c.data);
          break;
        case Column_Type::I32:
          report("Data = %d", *(i32*)c.data);
          break;
        case Column_Type::F32:
          report("Data = %f", *(f32*)c.data);
          break;
        case Column_Type::CHAR_20:
          report("Data = %.19s", (char*)c.data);
          break;
        case Column_Type::CHAR_6:
          report("Data = %.5s", (char*)c.data);
          break;
      }
    }
  }
};

#endif //EXERCISE_II__SORTER_DATA_STRUCTURES_H_
