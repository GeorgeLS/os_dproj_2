#ifndef EXERCISE_II__REPORT_H_
#define EXERCISE_II__REPORT_H_

[[gnu::format(printf, 1, 2)]]
void report_error(const char *fmt, ...);

[[gnu::format(printf, 1, 2)]]
void report(const char *fmt, ...);

[[gnu::format(printf, 2, 3)]]
void freport(int fd, const char *fmt, ...);

[[noreturn]]
[[gnu::format(printf, 1, 2)]]
void error_and_usage_report(const char *fmt, ...);

#endif //EXERCISE_II__REPORT_H_
