#ifndef EXERCISE_II__TIMER_H_
#define EXERCISE_II__TIMER_H_

#include <sys/times.h>
struct Timer {
  Timer();

  void start();
  void stop();
  double elapsed_cpu_seconds();
  double elapsed_seconds();
 private:
  struct tms tb1;
  struct tms tb2;
  double t1;
  double t2;
  double ticks_per_sec;
};

#endif //EXERCISE_II__TIMER_H_
