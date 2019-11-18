This program is written in C++ which makes use of the C++11 standard.
There is some use of templates and metaprogramming just to make easier to write some pieces of
code but it's not necessary as a design decision.

In order to compile the programs (coordinator, coach, sorter) just execute make.
The main entry point of the whole process is the coordinator program which complies to the
requirements specified by the exercise.

The times measured for coordinator and coaches are the real times (wall time) because there is no
value in measuring cpu time. That's because these processes are mostly waiting for their child processes
to finish and they don't get scheduled their childs finish.
The sorters processes which do almost all the work measure their execution time in cpu time.


There's nothing particular special in the design desicions of the programs, just some handy wrappers for
creating processes and pipes in order for the processes to communicate with each other.