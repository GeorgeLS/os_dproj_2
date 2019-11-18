#ifndef EXERCISE_II__PROCESS_H_sort_method
#define EXERCISE_II__PROCESS_H_

#include <zconf.h>
#include <wait.h>
#include "common.h"
#include "array.h"
#include "report.h"
#include "metaprogramming.h"
#include "pair.h"

struct Process {
  Process() = delete;

  template <typename... Params>
  explicit Process(Params... params) {
    static_assert(areT<const char *, Params...>::value, "All parameters types must be const char *");
    parameters_ = Array<const char *>{params...};
  }

  void spawn() {
    switch (pid = fork()) {
      case 0: execv(parameters_[0], (char *const*)(parameters_.data)); break;
      case -1: report_error("Couldn't create new process"); break;
    }
  }

  int wait() {
    int status;
    pid_t res = ::waitpid(pid, &status, 0);
    return res == -1 ? res : status;
  }

  pid_t pid{};
 private:
  Array<const char *> parameters_;
};

#endif //EXERCISE_II__PROCESS_H_
