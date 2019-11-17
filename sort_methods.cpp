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

[[gnu::always_inline]]
internal size_t left(size_t index) { return (index << 1U) + 1U; }

[[gnu::always_inline]]
internal size_t right(size_t index) { return (index << 1U) + 2U; }

internal void max_heapify(Column_Collection collection, size_t index) {
  auto &heap = collection.columns;
  bool restoring{true};
  while (restoring) {
    size_t max_index = index;
    size_t left_index = left(max_index);
    size_t right_index = right(max_index);
    if (left_index < heap.size and Column::compare(heap[left_index], heap[max_index], collection.type) > 0) {
      max_index = left_index;
    }
    if (right_index < heap.size and Column::compare(heap[right_index], heap[max_index], collection.type) > 0) {
      max_index = right_index;
    }
    if (max_index != index) {
      std::swap(heap[max_index], heap[index]);
      index = max_index;
    } else {
      restoring = false;
    }
  };
}

internal void build_max_heap(Column_Collection collection) {
  for (ssize_t i = (collection.columns.size >> 1U) - 1U; i >= 0; --i) {
    max_heapify(collection, i);
  }
}

void heap_sort(Column_Collection collection) {
  build_max_heap(collection);
  auto &heap = collection.columns;
  for (ssize_t i = heap.capacity - 1U; i >= 0U; --i) {
    std::swap(heap[0], heap[i]);
    --heap.size;
    max_heapify(collection, 0);
  }
  heap.size = heap.capacity;
}
