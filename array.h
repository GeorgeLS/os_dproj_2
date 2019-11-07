#ifndef EXERCISE_II__ARRAY_H_
#define EXERCISE_II__ARRAY_H_

#include "common.h"
#include <limits>
#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <initializer_list>
#include <utility>

// A simple lightweight array type
// It has constant size with which it is initialized.

template<typename T>
struct Array {
  Array() : data{nullptr}, capacity{0}, size{0} {}

  explicit Array(std::initializer_list<T> list) : Array(list.size()) {
    for (auto &v : list) {
      push(std::move(v));
    }
  }
  explicit Array(size_t n) : Array() { reserve(n); }

  inline T &operator[](size_t index) {
    assert(index < size);
    return data[index];
  }

  inline const T &operator[](size_t index) const {
    assert(index < size);
    return data[index];
  }

  void push(T value) {
    constexpr size_t size_t_max = std::numeric_limits<size_t>::max();
    assert(size < size_t_max);
    assert(size < capacity);
    data[size++] = value;
  }

  template<typename... Args>
  void emplace_back(Args&&... args) {
    data[size++] = T(std::forward<Args>(args)...);
  }

  void reserve(size_t cap) {
    assert(size == 0 && capacity == 0);
    assert(!data);
    // Get aligned memory for faster copying.
    data = (T *) aligned_alloc(32, cap * sizeof(T));
    assert(data);
    capacity = cap;
    size = 0U;
  }

  inline Array subarray(size_t start_index, size_t end_index) {
    assert(start_index >= 0 && start_index <= end_index &&
        end_index <= capacity);
    Array<T> subarr{};
    size_t elements_n = end_index - start_index;
    subarr.capacity = subarr.size = elements_n ? elements_n : 1U;
    subarr.data = &data[start_index];
    return subarr;
  }

  void clear() {
    size = 0;
    capacity = 0;
  }

  void clear_and_free() {
    clear();
    assert(data);
    free(data);
  }

  // Iterator section
  inline T *begin() { return data; }
  inline const T *begin() const { return data; }
  inline T *end() { return data + size; }
  inline const T *end() const { return data + size; }

  inline bool is_full() { return size && size == capacity; }

  void print() {
    for (size_t i = 0U; i != size; ++i) {
      data[i].print();
    }
  }

  T *data;
  size_t capacity;
  size_t size;
};

#endif //EXERCISE_II__ARRAY_H_
