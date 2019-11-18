#include <zconf.h>
#include "timer.h"

Timer::Timer()
    : ticks_per_sec{(double) (sysconf(_SC_CLK_TCK))}, tb1{}, tb2{} {}

void Timer::start() {
  t1 = (double) times(&tb1);
}

void Timer::stop() {
  t2 = (double) times(&tb2);
}

double Timer::elapsed_cpu_seconds() {
  double cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) -
      (tb1.tms_utime + tb1.tms_stime));

  return cpu_time / ticks_per_sec;
}

double Timer::elapsed_seconds() {
  return (t2 - t1) / ticks_per_sec;
}