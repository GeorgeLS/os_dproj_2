#include <cstring>
#include <cstdlib>
#include <zconf.h>
#include "pair.h"
#include "common.h"
#include "report.h"
#include "utils.h"
#include "vector.h"
#include "process.h"

using Stats_Pair = Pair<double, Array<double>>;
using Stats = Array<Stats_Pair>;

constexpr char *INPUT_FILE_OPTION = (char *const) "-f";
constexpr char *QUICKSORT_OPTION = (char *const) "-q";
constexpr char *HEAPSORT_OPTION = (char *const) "-h";
constexpr char *USAGE_OPTION = (char *const) "--help";

[[noreturn]] internal void usage() {
  report("Usage: ./mysort [OPTIONS]\n"
         "Options:\n"
         "\t--help                      -- Displays this message\n"
         "\t-f      <input_filename>    -- The filename of the file to sort\n"
         "\t-h|q    <column_number>     -- The method to use to sort the column with number <column_number>\n"
         "\t                               q is for Quicksort and h for Heapsort.\n"
         "\t                               If omitted the file will be sorted on the first column only using Quicksort");
  exit(2);
}

struct Program_Options {
 private:
  using Column_Sort_Type = Pair<const char *, u64>;
 public:
  const char *input_file{nullptr};
  Vector<Column_Sort_Type> column_sorts{};

  void print(int fd = STDOUT_FILENO) {
    freport(fd, "Program options:\n\tinput_file = %s", input_file);
    for (const Column_Sort_Type &cs : column_sorts) {
      freport(fd, "\tcolumn_sort = %s %ld", cs.first, cs.second);
    }
  }
};

internal inline bool is_option(const char *str) {
  size_t str_len = strlen(str);
  return not strncmp(str, INPUT_FILE_OPTION, str_len) or
      not strncmp(str, QUICKSORT_OPTION, str_len) or
      not strncmp(str, HEAPSORT_OPTION, str_len);
}

internal inline void validate_option_argument(const char *option, const char *argument) {
  if (is_option(argument)) {
    error_and_usage_report(R"(Not valid argument "%s" for option "%s")", argument, option);
  }
}

internal Program_Options get_program_options(int argc, char *args[]) {
  Program_Options options{};
  options.column_sorts.reserve(argc - 1);
  for (int i = 1; i < argc; ++i) {
    char *arg = args[i];
    size_t arg_len = strlen(arg);
    char *next_arg = args[i + 1];
    if (not strncmp(arg, INPUT_FILE_OPTION, arg_len)) {
      options.input_file = next_arg;
      ++i;
    } else if (not strncmp(arg, QUICKSORT_OPTION, arg_len) or not strncmp(arg, HEAPSORT_OPTION, arg_len)) {
      validate_option_argument(arg, next_arg);
      i64 column;
      if (not string_to_i64(next_arg, &column) or column <= 0) {
        error_and_usage_report(R"(Not a valid column number "%s")", next_arg);
      }
      options.column_sorts.push_back(make_pair((const char *) std::move(arg), (u64) column));
      ++i;
    } else if (not strncmp(arg, USAGE_OPTION, arg_len)) {
      usage();
    } else {
      error_and_usage_report(R"(Unknown option "%s")", arg);
    }
  }
  options.column_sorts.shrink_to_fit();
  return options;
}

internal Pair<Array<Process>, Array<Pipe>> create_coaches_and_pipes(const Program_Options &options) {
  Array<Process> coaches{};
  Array<Pipe> pipes{};
  const char *records_n = to_string(file_size_in_bytes(options.input_file) / sizeof(Record));
  if (options.column_sorts.size != 0) {
    coaches.reserve(options.column_sorts.size);
    pipes.reserve(options.column_sorts.size);
    for (size_t i = 0U; i != options.column_sorts.size; ++i) {
      Pair<const char *, u64> column_sort = options.column_sorts[i];
      const char *pipe_name = to_string("coord_to_coach_%zu", i);

      coaches.push(Process{
          "./coach",
          options.input_file,
          records_n,
          (const char *) to_string(i),
          column_sort.first,
          (const char *) to_string(column_sort.second),
          (const char *) to_string("coord_to_coach_%zu", i),
          (const char *) NULL
      });

      pipes.push(Pipe{pipe_name, sizeof(double)});
    }
  } else {
    coaches.reserve(1);
    pipes.reserve(1);
    coaches.push(Process{
        "./coach",
        options.input_file,
        records_n,
        "1",
        "q",
        "1",
        "coord_to_coach_0",
        (const char *) NULL,
    });
    pipes.push(Pipe{"coord_to_coach_0", sizeof(double)});
  }
  return make_pair(coaches, pipes);
}

internal void print_stats(Stats stats) {
  report("========================= STATS =========================\n");
  double min_coach_secs{std::numeric_limits<double>::max()};
  double max_coach_secs{0.0};
  double avg_coach_secs{0.0};
  size_t coach_i{0U};
  for (Stats_Pair &p : stats) {
    double min_sorter_secs{std::numeric_limits<double>::max()};
    double max_sorter_secs{0.0};
    double avg_sorter_secs{0.0};

    for (double sorter_secs : p.second) {
      if (sorter_secs < min_sorter_secs) {
        min_sorter_secs = sorter_secs;
      }
      if (sorter_secs > max_sorter_secs) {
        max_sorter_secs = sorter_secs;
      }
      avg_sorter_secs += sorter_secs;
    }
    avg_sorter_secs /= p.second.size;

    report("COACH %zu:\n"
           "\tMax sorter execution time: %lf sec\n"
           "\tMin sorter execution time: %lf sec\n"
           "\tAverage sorter execution time: %lf sec",
           coach_i, max_sorter_secs, min_sorter_secs, avg_sorter_secs);

    ++coach_i;

    if (p.first < min_coach_secs) {
      min_coach_secs = p.first;
    }
    if (p.first > max_coach_secs) {
      max_coach_secs = p.first;
    }
    avg_coach_secs += p.first;
  }

  avg_coach_secs /= stats.size;
  report("\nMax coach execution time: %lf sec\n"
         "Min coach execution time: %lf sec\n"
         "Average coach execution time: %lf sec\n",
         max_coach_secs, min_coach_secs, avg_coach_secs);
  report("=========================================================");
}

int main(int argc, char *args[]) {
  if (argc < 3) usage();
  Program_Options options = get_program_options(argc, args);
  if (options.column_sorts.size > 4) {
    error_and_usage_report("You can sort at most 4 columns at once");
  }
  auto coaches_and_pipes = create_coaches_and_pipes(options);
  auto coaches = coaches_and_pipes.first;
  auto pipes = coaches_and_pipes.second;
  for (Process p : coaches) {
    p.spawn();
  }
  for (Pipe &p : pipes) {
    p.open(Pipe::Mode::Read_Only);
  }

  Stats stats(options.column_sorts.size);
  for (size_t i = 0U; i != pipes.size; ++i) {
    Pipe p = pipes[i];
    size_t sorters_n = 1U << i;
    double coach_elapsed_secs;
    p >> coach_elapsed_secs;
    Stats_Pair pair = make_pair(coach_elapsed_secs, Array<double>(sorters_n));
    for (size_t j = 0U; j != sorters_n; ++j) {
      double elapsed_secs;
      p >> elapsed_secs;
      pair.second.push(elapsed_secs);
    }
    stats.push(pair);
  }

  print_stats(stats);
  return 0;
}