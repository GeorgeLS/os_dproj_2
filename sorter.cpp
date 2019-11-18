#include <cassert>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include "common.h"
#include "utils.h"
#include "sorter_data_structures.h"
#include "sort_methods.h"
#include "pipe.h"
#include "timer.h"

struct Sorter_Options {
  const char *filename;
  size_t start_pos;
  size_t end_pos;
  const char *sort_method;
  size_t column;
  const char *pipe_name;
};

internal Sorter_Options get_sorter_options(char *args[]) {
  Sorter_Options options{};
  options.filename = args[1];
  string_to_i64(args[2], (i64 *) &options.start_pos);
  string_to_i64(args[3], (i64 *) &options.end_pos);
  options.sort_method = args[4];
  string_to_i64(args[5], (i64 *) &options.column);
  options.pipe_name = args[6];
  return options;
}

Column_Collection copy_column_data(Array<Record> records, size_t column) {
  Array<Column> columns(records.size);
  size_t offset;
  size_t field_size;
  Column_Type type;
  switch (column) {
    case 1:
      offset = offsetof(Record, id);
      field_size = sizeof(i64);
      type = Column_Type::I64;
      break;
    case 2:
      offset = offsetof(Record, first_name);
      field_size = sizeof(char[20]);
      type = Column_Type::CHAR_20;
      break;
    case 3:
      offset = offsetof(Record, surname);
      field_size = sizeof(char[20]);
      type = Column_Type::CHAR_20;
      break;
    case 4:
      offset = offsetof(Record, address);
      field_size = sizeof(char[20]);
      type = Column_Type::CHAR_20;
      break;
    case 5:
      offset = offsetof(Record, address_id);
      field_size = sizeof(i32);
      type = Column_Type::I32;
      break;
    case 6:
      offset = offsetof(Record, town);
      field_size = sizeof(char[20]);
      type = Column_Type::CHAR_20;
      break;
    case 7:
      offset = offsetof(Record, zip_code);
      field_size = sizeof(char[6]);
      type = Column_Type::CHAR_6;
      break;
    case 8:
      offset = offsetof(Record, salary);
      field_size = sizeof(f32);
      type = Column_Type::F32;
      break;
    default: assert(0);
  }

  for (size_t i = 0U; i != records.size; ++i) {
    byte *field = new byte[field_size];
    byte *record_field = (byte*)&records[i] + offset;
    memcpy(field, record_field, field_size);
    columns.push(Column{field, &records[i]});
  }

  return Column_Collection{columns, type};
}

/***
 * The sorter program that gets created by the coach process
 * @param argc The number of command line arguments including the program name
 * @param args The command line arguments. The follow this order:
 *      1) The process name (./sorter)
 *      2) The file's filename to sort
 *      3) The starting record number to sort
 *      4) The ending record number to sort
 *      5) The sort method to use
 *      6) The column to sort
 *      7) The pipe name to open in order to communicate with parent process
 * @return
 */
int main(int argc, char *args[]) {
  assert(argc == 7);
  Sorter_Options options = get_sorter_options(args);
  Pipe pipe{options.pipe_name};
  pipe.open(Pipe::Mode::Write_Only);
  Timer t{};
  t.start();
  Array<Record> records = load_records_from_file(options.filename, options.start_pos, options.end_pos);
  Column_Collection collection = copy_column_data(records, options.column);
  size_t sort_method_len = strlen(options.sort_method);
  if (!strncmp(options.sort_method, "-q", sort_method_len)) {
    quick_sort(collection);
  } else {
    heap_sort(collection);
  }
  t.stop();
  for (Column c : collection.columns) {
    pipe << *c.record;
  }
  pipe << t.elapsed_cpu_seconds();
  kill(getppid(), SIGUSR2);
  return EXIT_SUCCESS;
}