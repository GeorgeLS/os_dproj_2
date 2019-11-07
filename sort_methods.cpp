#include <random>
#include "sort_methods.h"
#include "common.h"

internal std::default_random_engine generator;

internal void __insertion_sort(Column *data, ssize_t length, Column_Type type) {
  for (ssize_t i = 1; i != length; i++) {
    Column key = data[i];
    ssize_t j = i - 1;

    /* Move elements of arr[0..i-1], that are
    greater than key, to one position ahead
    of their current position */
    while (j >= 0 && Column::compare(data[j], key, type) > 0) {
      data[j + 1] = data[j];
      --j;
    }
    data[j + 1] = key;
  }
}

internal ssize_t __partition(Column *data, ssize_t left_index, ssize_t right_index, Column_Type type) {
  std::uniform_int_distribution<ssize_t> distribution{left_index, right_index};
  ssize_t random_index = distribution(generator);
  std::swap(data[random_index], data[right_index]);
  Column pivot = data[right_index];
  ssize_t i = left_index - 1;
  for (ssize_t j = left_index; j != right_index; ++j) {
    if (Column::compare(data[j], pivot, type) <= 0) {
      ++i;
      std::swap(data[i], data[j]);
    }
  }
  std::swap(data[i + 1], data[right_index]);
  return i + 1;
}

internal void __quick_sort(Column *data, ssize_t left_index, ssize_t right_index, Column_Type type) {
  while (left_index < right_index) {
    ssize_t length = right_index - left_index + 1;
    // For small lengths, fall back to insertion sort.
    if (length <= 16) {
      __insertion_sort(data + left_index, length, type);
      return;
    }
    ssize_t partition_index = __partition(data, left_index, right_index, type);
    // Save stack space by going into the corresponding part.
    if (partition_index - left_index < right_index - partition_index) {
      __quick_sort(data, left_index, partition_index - 1, type);
      left_index = partition_index + 1;
    } else {
      __quick_sort(data, partition_index + 1, right_index, type);
      right_index = partition_index - 1;
    }
  }
}

void quick_sort(Column_Collection collection) {
  __quick_sort(collection.columns.data, 0, collection.columns.size - 1, collection.type);
}

void heap_sort(Column_Collection collection) {

}
