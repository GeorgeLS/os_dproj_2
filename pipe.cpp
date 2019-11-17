#include <cstdlib>
#include <cstring>
#include <zconf.h>
#include <sys/stat.h>
#include "pipe.h"

Pipe::Pipe(const char *path, size_t buffer_size)
    : path{strdup(path)} {
  if (buffer_size) {
    buffer = Array<byte>(buffer_size + 1U);
  }
}

Pipe::~Pipe() {
  close();
}

bool Pipe::open(Pipe::Mode mode) {
  mkfifo(path, 0777);
  fd = ::open(path, static_cast<int>(mode));
  if (fd == -1) {
    perror("Opening pipe:");
  }
  return fd != -1;
}

void Pipe::close() {
  if (fd != -1) {
    unlink(path);
    fd = -1;
  }
}

Pipe& Pipe::write(byte *data, size_t bytes) {
  if (::write(fd, data, bytes) == -1) {
    throw Pipe_Exception("Error while writing to pipe");
  }
  return *this;
}

Pipe& Pipe::operator<<(const char *str) {
  return write((byte*)str, strlen(str));
}

char *Pipe::read(size_t bytes) {
  ssize_t res = ::read(fd, buffer.data, bytes);
  if (res == -1) {
    throw Pipe_Exception("Error while reading");
  }
  buffer.size = bytes;
  buffer[buffer.size] = '\0';
  return (char *)buffer.data;
}
