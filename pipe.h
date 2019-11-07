#ifndef EXERCISE_II__PIPE_H_
#define EXERCISE_II__PIPE_H_

#include <exception>
#include <fcntl.h>
#include <zconf.h>
#include <cstdio>
#include "common.h"
#include "array.h"

struct Pipe {
  struct Pipe_Exception : public std::exception {
    explicit Pipe_Exception(const char *message) : message{message} {}

    const char *what() const noexcept override {
      return message;
    }
    const char *message;
  };

  enum class Mode {
    Read_Only = O_RDONLY,
    Write_Only = O_WRONLY,
    Read_Write = O_RDWR,
    Non_Blocking = O_NONBLOCK
  };

  explicit Pipe(const char *path, size_t buffer_size = 0U);

  ~Pipe();

  bool open(Mode mode);
  void close();

  template<typename T>
  Pipe &write(const T &data);

  Pipe &write(byte *data, size_t bytes);

  Pipe &operator<<(const char *str);

  template<typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
  Pipe &operator<<(const T &value);

  template<typename T>
  T read();

  char *read(size_t bytes);

  template<typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
  Pipe &operator>>(T &value);

  Array<byte> buffer;
  const char *path{};
  int fd{-1};
};

template<typename T>
Pipe& Pipe::write(const T &data) {
  if (::write(fd, &data, sizeof(T)) == -1) {
    throw Pipe_Exception("Error while writing to pipe");
  }
  return *this;
}

template<typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type>
Pipe &Pipe::operator<<(const T &value) {
  return write(value);
}

template<typename T>
T Pipe::read() {
  ssize_t res = ::read(fd, buffer.data, sizeof(T));
  if (res == -1) {
    perror("Pipe read");
    throw Pipe_Exception("Error while reading");
  }
  buffer.size = sizeof(T);
  return *(T*)buffer.data;
}

template<typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type>
Pipe &Pipe::operator>>(T &value) {
  read(value);
  return *this;
}

#endif //EXERCISE_II__PIPE_H_
