#include <cstdio>
#include <cstdarg>
#include <zconf.h>
#include <cstdlib>
#include "common.h"
#include "report.h"

#define GET_ARGS() \
va_list args; \
va_start(args, fmt);

internal void report_base(const char *tag, const char *fmt, va_list args, int fd = STDERR_FILENO) {
  if (tag != nullptr) {
    dprintf(fd, "[%s]: ", tag);
  }
  vdprintf(fd, fmt, args);
  dprintf(fd, "\n");
  va_end(args);
}

void report_error(const char *fmt, ...) {
  GET_ARGS();
  report_base("ERROR", fmt, args);
}

void report(const char *fmt, ...) {
  GET_ARGS();
  report_base(nullptr, fmt, args);
}

void freport(int fd, const char *fmt, ...) {
  GET_ARGS();
  report_base(nullptr, fmt, args, fd);
}

void error_and_usage_report(const char *fmt, ...) {
  GET_ARGS();
  report_base(nullptr, fmt, args);
  report("Execute ./mysort --help for help");
  exit(EXIT_FAILURE);
}
