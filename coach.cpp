#include <cassert>
#include <cstdio>
#include "utils.h"
#include "common.h"
#include "array.h"
#include "process.h"
#include "pair.h"

struct Coach_Options {
  const char *filename;
  size_t records_n;
  size_t id;
  const char *sort_method;
  const char *column;
};

internal Coach_Options get_coach_options(char *args[]) {
  Coach_Options options{};
  options.filename = args[1];
  string_to_i64(args[2], (i64 *) &options.records_n);
  string_to_i64(args[3], (i64 *) &options.id);
  options.sort_method = args[4];
  options.column = args[5];
  return options;
}

global constexpr std::size_t sorters_divisors[4][8] = {
    {1},
    {2, 2},
    {2, 4, 8, 8},
    {4, 4, 8, 8, 16, 16, 16, 16}
};

internal Array<size_t> calculate_sizes_for_sorters(size_t coach_id, size_t records_n) {
  auto divisors = sorters_divisors[coach_id];
  size_t sorters_n = 1U << coach_id;
  Array<size_t> sizes(sorters_n);
  size_t current_sum = 0U;
  for (size_t i = 0U; i != sorters_n - 1U; ++i) {
    size_t rec_n = records_n / divisors[i];
    sizes.push(rec_n);
    current_sum += rec_n;
  }
  size_t rec_n = records_n / divisors[sorters_n - 1U];
  rec_n += records_n - current_sum - rec_n;
  sizes.push(rec_n);
  return sizes;
}

internal Pair<Array<Process>, Array<Pipe>>
create_sorters_and_pipes(Coach_Options options, Array<size_t> sorters_sizes) {
  assert(options.id >= 0 and options.id <= 3);
  size_t sorters_n = 1U << options.id;
  Array<Process> sorters(sorters_n);
  Array<Pipe> pipes(sorters_n);
  size_t current_start{0U};
  for (std::size_t i = 0U; i != sorters_n; ++i) {
    size_t records_n = sorters_sizes[i];
    const char *start_record_pos = to_string(current_start);
    const char *end_record_pos = to_string(current_start + records_n);
    const char *pipe_name = to_string("coach_%zu_to_sorter_%zu", options.id, i);
    pipes.push(Pipe{pipe_name, records_n});
    sorters.push(Process{
        "./sorter",
        options.filename,
        start_record_pos,
        end_record_pos,
        options.sort_method,
        options.column,
        pipe_name,
        (const char *) NULL
    });
    current_start += records_n;
  }
  return make_pair(sorters, pipes);
}

/**
 * The coach program that gets spawned by the coordinator process.
 * @param argc The number of command line arguments including the process name
 * @param args The command line arguments. They follow this order:
 *      1) The process name (./coach)
 *      2) The file's filename to sort
 *      3) The number of records the file has
 *      4) The id of the coach (0, 1, 2, 3)
 *      5) The sort method to use
 *      6) The column number to sort
 * @return A code indicating the success or failure of the process execution
 */
int main(int argc, char *args[]) {
  assert(argc == 6);
  Coach_Options options = get_coach_options(args);
  auto sorters_sizes = calculate_sizes_for_sorters(options.id, options.records_n);
  auto sorters_and_pipes = create_sorters_and_pipes(options, sorters_sizes);
  auto sorters = sorters_and_pipes.first;
  auto pipes = sorters_and_pipes.second;
  for (Process p : sorters) {
    p.spawn();
  }
  for (Pipe &p : pipes) {
    p.open(Pipe::Mode::Read_Only);
  }
  size_t sorters_n = 1U << options.id;
  Array<Array<Record>> records(sorters_n);
  for (size_t i = 0U; i != pipes.size; ++i) {
    size_t records_n = sorters_sizes[i];
    Pipe p = pipes[i];
    records.push(Array<Record>(records_n));
    for (size_t j = 0U; j < records_n; ++j) {
      Record r{};
      p >> r;
      records[i].push(r);
    }
  }

  size_t column;
  string_to_i64((char *) options.column, (i64 *) &column);
  size_t *indexes = (size_t *) alloca(sorters_n * sizeof(size_t));
  for (size_t i = 0U; i != sorters_n; ++i) {
    indexes[i] = 0U;
  }

  while (true) {
    bool finished{true};
    for (size_t i = 0U; i != sorters_n; ++i) {
      finished &= indexes[i] == records[i].size;
    }
    if (finished) break;
    size_t min_index = 0U;
    for (size_t i = 1U; i != sorters_n; ++i) {
      if (indexes[i] == records[i].size) continue;
      if (indexes[min_index] == records[min_index].size) {
        min_index = i;
      } else if (Record::compare(records[min_index][indexes[min_index]], records[i][indexes[i]], column) > 0) {
        min_index = i;
      }
    }
    records[min_index][indexes[min_index]].print();
    ++indexes[min_index];
  }

  return EXIT_SUCCESS;
}