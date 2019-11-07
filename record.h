#ifndef EXERCISE_II_CMAKE_BUILD_DEBUG_RECORD_H_
#define EXERCISE_II_CMAKE_BUILD_DEBUG_RECORD_H_

#include <cstring>
#include "common.h"
#include "report.h"
#include "pipe.h"


struct Record {
  i64 id;
  char first_name[20];
  char surname[20];
  char address[20];
  i32 address_id;
  char town[20];
  char zip_code[6];
  f32 salary;

  void print() const {
    report("id = %ld | first_name = %s | surname = %s | address = %s | "
           "address_id = %d | town = %s | zip_code = %s | salary = %f",
           id, first_name, surname, address, address_id, town, zip_code, salary);
  }

  static int compare(Record &lhs, Record &rhs, size_t col_num) {
    switch (col_num) {
      case 1: return lhs.id < rhs.id ? -1 : lhs.id == rhs.id ? 0 : 1;
      case 2: return strncmp(lhs.first_name, rhs.first_name, 20);
      case 3: return strncmp(lhs.surname, rhs.surname, 20);
      case 4: return strncmp(lhs.address, rhs.address, 20);
      case 5: return lhs.address_id < rhs.address_id ? -1 : lhs.address_id == rhs.address_id ? 0 : 1;
      case 6: return strncmp(lhs.town, rhs.town, 20);
      case 7: return strncmp(lhs.zip_code, rhs.zip_code, 6);
      case 8: return lhs.salary < rhs.salary ? -1 : lhs.salary == rhs.salary ? 0 : 1;
      default: assert(0);
    }
  }

  friend Pipe &operator<<(Pipe &p, Record &r) {
    p.write(r);
    return p;
  }

  friend Pipe &operator>>(Pipe &p, Record &r) {
    r = p.read<Record>();
    return p;
  }
};

#endif //EXERCISE_II_CMAKE_BUILD_DEBUG_RECORD_H_
