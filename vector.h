#ifndef EXERCISE_II__VECTOR_H_
#define EXERCISE_II__VECTOR_H_

#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class Vector {
 public:
  struct Vector_Traits {
    using value_type = T;
    using reference = T &;
    using const_reference = T const &;
    using pointer = T *;
    using const_pointer = T const *;
    using iterator = T *;
    using const_iterator = T const *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
  };

  using traits = Vector_Traits;

  explicit Vector(size_t capacity = 50U);
  Vector(const std::initializer_list<T> &rhs);
  explicit Vector(const Vector<T> &rhs);
  Vector &operator=(const Vector<T> &rhs);
  explicit Vector(Vector<T> &&rhs) noexcept;
  Vector &operator=(Vector<T> &&rhs) noexcept;
  ~Vector();

  inline void push_back(const T &value);
  inline void push_back(T &&value);

  inline void insert(size_t index, const T &value);
  inline void insert(size_t index, T &&value);
  inline void insert(size_t index, const std::initializer_list<T> &values);
  inline void insert(size_t index, typename traits::const_iterator start,
                     typename traits::const_iterator end);
  inline void insert(typename traits::const_iterator it, const T &value);
  inline void insert(typename traits::const_iterator it, T &&value);
  inline void insert(typename traits::const_iterator it,
                     const std::initializer_list<T> &values);
  inline void insert(typename traits::const_iterator it,
                     typename traits::const_iterator start,
                     typename traits::const_iterator end);

  template<typename ...Args>
  inline void emplace_back(Args &&...args);

  inline void reserve(size_t capacity);
  inline void shrink_to_fit();
  inline void clear() noexcept;

  inline T &operator[](size_t index);
  inline T const &operator[](size_t index) const;
  bool operator==(const Vector<T> &rhs) const;
  bool operator!=(const Vector<T> &rhs) const;
  bool operator<(const Vector<T> &rhs) const;
  bool operator<=(const Vector<T> &rhs) const;
  bool operator>(const Vector<T> &rhs) const;
  bool operator>=(const Vector<T> &rhs) const;

  [[nodiscard]]
  inline bool empty() const noexcept;

  inline T &front();
  inline T const &front() const;
  inline T &back();
  inline T const &back() const;
  inline T *data() noexcept;
  inline T const *data() const noexcept;

  inline typename traits::iterator begin() noexcept;
  inline typename traits::const_iterator cbegin() const noexcept;
  inline typename traits::iterator end() noexcept;
  inline typename traits::const_iterator cend() noexcept;
  inline typename traits::reverse_iterator rbegin() noexcept;
  inline typename traits::const_reverse_iterator crbegin() const noexcept;
  inline typename traits::reverse_iterator rend() noexcept;
  inline typename traits::const_reverse_iterator crend() const noexcept;

 private:
  inline void copy_elements(T *elements, size_t elements_n);
  inline void move(Vector<T> &rhs) noexcept;
  inline void reallocate(size_t new_capacity);

 private:
  T *elements;
 public:
  size_t size;
  size_t capacity;
 private:
  Allocator allocator;
};

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_t capacity)
    : size{0U}, capacity{capacity}, allocator{} {
  elements = allocator.allocate(capacity);
}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(const std::initializer_list<T> &rhs)
    : size{0U} {
  capacity = rhs.size() << 2U;
  elements = allocator.allocate(capacity);
  for (T &&value : rhs) {
    push_back(std::move(value));
  }
}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Vector<T> &rhs) {
  capacity = rhs.capacity << 2U;
  size = rhs.size;
  elements = allocator.allocate(capacity);
  copy_elements(rhs.elements, size);
}

template<typename T, typename Allocator>
Vector<T, Allocator> &Vector<T, Allocator>::operator=(const Vector<T> &rhs) {
  if (rhs.capacity > capacity) {
    ~Vector();
    new(this) Vector<T>(rhs);
  } else {
    size = rhs.size;
    copy_elements(rhs.elements, size);
  }
}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(Vector<T> &&rhs) noexcept
    : size{0U}, capacity{0U} {
  move(rhs);
}

template<typename T, typename Allocator>
Vector<T, Allocator> &Vector<T, Allocator>::operator=(Vector<T> &&rhs)
noexcept {
  ~Vector();
  move(rhs);
}

template<typename T, typename Allocator>
Vector<T, Allocator>::~Vector() {
  allocator.deallocate(elements, capacity);
  elements = nullptr;
  size = 0U;
  capacity = 0U;
}

template<typename T, typename Allocator>
T &Vector<T, Allocator>::operator[](size_t index) {
  return elements[index];
}

template<typename T, typename Allocator>
T const &Vector<T, Allocator>::operator[](size_t index) const {
  return elements[index];
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::copy_elements(T *const elements, size_t elements_n) {
  for (size_t i = 0U; i != elements_n; ++i) {
    this->elements[i] = elements[i];
  }
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::move(Vector<T> &rhs) noexcept {
  std::swap(elements, rhs.elements);
  std::swap(size, rhs.size);
  std::swap(capacity, rhs.capacity);
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::reallocate(size_t new_capacity) {
  T *old_elements = elements;
  elements = allocator.allocate(new_capacity);
  memcpy(elements, old_elements, size * sizeof(T));
  allocator.deallocate(old_elements, capacity);
  capacity = new_capacity;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::push_back(const T &value) {
  if (size == capacity) {
    reallocate(capacity << 2U);
  }
  elements[size++] = value;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::push_back(T &&value) {
  if (size == capacity) {
    reallocate(capacity << 2U);
  }
  elements[size++] = std::move(value);
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::insert(size_t index, const T &value) {
  if (size == capacity) {
    reallocate(capacity << 2U);
  }
  memmove(&elements[index + 1U], &elements[index], (size - index) * sizeof(T));
  elements[index] = value;
  ++size;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::insert(size_t index, T &&value) {
  if (size == capacity) {
    reallocate(capacity << 2U);
  }
  memmove(&elements[index + 1U], &elements[index], (size - index) * sizeof(T));
  elements[index] = std::move(value);
  ++size;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::insert(
    size_t index,
    typename Vector::Vector_Traits::const_iterator start,
    typename Vector::Vector_Traits::const_iterator end) {
  typename Vector::Vector_Traits::difference_type values_size = end - start;
  if (!values_size) return;
  if (size + values_size > capacity) {
    reallocate((size + values_size) << 2U);
  }
  memmove(&elements[index + values_size],
          &elements[index],
          (size - index) * sizeof(T));
  for (; start != end; ++start) {
    elements[index++] = *start;
  }
  size += values_size;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::insert(size_t index,
                                  const std::initializer_list<T> &values) {
  size_t values_size = values.size();
  if (!values_size) return;
  if (size + values_size > capacity) {
    reallocate((size + values_size) << 2U);
  }
  memmove(&elements[index + values_size],
          &elements[index],
          (size - index) * sizeof(T));
  for (T &&value : values) {
    elements[index++] = std::move(value);
  }
  size += values_size;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::insert(typename traits::const_iterator it,
                                  const T &value) {
  if (size == capacity) {
    reallocate(capacity << 2U);
  }
  memmove(it + 1U, it, (size - (elements - it)) * sizeof(T));
  *it = value;
  ++size;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::insert(typename traits::const_iterator it,
                                  T &&value) {
  if (size == capacity) {
    reallocate(capacity << 2U);
  }
  memmove(it + 1U, it, (size - (elements - it)) * sizeof(T));
  *it = std::move(value);
  ++size;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::insert(
    typename Vector::Vector_Traits::const_iterator it,
    typename Vector::Vector_Traits::const_iterator start,
    typename Vector::Vector_Traits::const_iterator end) {
  typename Vector::Vector_Traits::difference_type values_size = end - start;
  if (!values_size) return;
  if (size + values_size > capacity) {
    reallocate((size + values_size) << 2U);
  }
  memmove(it + values_size, it, (size - (elements - it)) * sizeof(T));
  for (; start != end; ++start, ++it) {
    *it = *start;
  }
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::insert(typename traits::const_iterator it,
                                  const std::initializer_list<T> &values) {
  size_t values_size = values.size();
  if (!values_size) return;
  if (size + values_size > capacity) {
    reallocate((size + values_size) << 2U);
  }
  memmove(it + values_size, it, (size - (elements - it)) * sizeof(T));
  for (T &&value : values) {
    *it = std::move(value);
    ++it;
  }
  size += values_size;
}

template<typename T, typename Allocator>
template<typename... Args>
void Vector<T, Allocator>::emplace_back(Args &&...args) {
  if (size == capacity) {
    reallocate(capacity << 2U);
  }
  elements[size++] = std::move(T(std::forward<Args>(args)...));
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::reserve(size_t capacity) {
  if (capacity < this->capacity) return;
  if (elements != nullptr) {
    allocator.deallocate(elements, capacity);
  }
  elements = allocator.allocate(capacity);
  this->capacity = capacity;
  size = 0U;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::shrink_to_fit() {
  reallocate(size);
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::clear() noexcept {
  for (size_t i = 0U; i != size; ++i) {
    elements[i].~T();
  }
  size = 0U;
}

template<typename T, typename Allocator>
bool Vector<T, Allocator>::empty() const noexcept {
  return size == 0U;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::traits::iterator Vector<T, Allocator>::begin()
noexcept {
  return elements;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Vector_Traits::const_iterator
Vector<T, Allocator>::cbegin() const noexcept {
  return elements;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Vector_Traits::iterator
Vector<T, Allocator>::end() noexcept {
  return elements + size;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Vector_Traits::const_iterator
Vector<T, Allocator>::cend() noexcept {
  return elements + size;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Vector_Traits::reverse_iterator
Vector<T, Allocator>::rbegin() noexcept {
  return std::reverse_iterator<typename Vector_Traits::iterator>(elements + size);
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Vector_Traits::const_reverse_iterator
Vector<T, Allocator>::crbegin() const noexcept {
  return std::reverse_iterator<typename Vector_Traits::const_iterator>(elements + size);
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Vector_Traits::reverse_iterator
Vector<T, Allocator>::rend() noexcept {
  return std::reverse_iterator<typename Vector_Traits::iterator>(elements);
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Vector_Traits::const_reverse_iterator
Vector<T, Allocator>::crend() const noexcept {
  return std::reverse_iterator<typename Vector_Traits::const_iterator>(elements);
}

template<typename T, typename Allocator>
T &Vector<T, Allocator>::front() {
  return elements[0];
}

template<typename T, typename Allocator>
T const &Vector<T, Allocator>::front() const {
  return elements[0];
}

template<typename T, typename Allocator>
T &Vector<T, Allocator>::back() {
  return elements[size - 1U];
}

template<typename T, typename Allocator>
T const &Vector<T, Allocator>::back() const {
  return elements[size - 1U];
}

template<typename T, typename Allocator>
T *Vector<T, Allocator>::data() noexcept {
  return elements;
}

template<typename T, typename Allocator>
T const *Vector<T, Allocator>::data() const noexcept {
  return elements;
}

template<typename T, typename Allocator>
bool Vector<T, Allocator>::operator==(const Vector<T> &rhs) const {
  if (size != rhs.size) return false;
  for (size_t i = 0U; i != rhs.size; ++i) {
    if (elements[i] != rhs.elements[i]) return false;
  }
  return true;
}

template<typename T, typename Allocator>
bool Vector<T, Allocator>::operator!=(const Vector<T> &rhs) const {
  if (size != rhs.size) return true;
  for (size_t i = 0U; i != rhs.size; ++i) {
    if (elements[i] != rhs.elements[i]) return true;
  }
  return false;
}

template<typename T, typename Allocator>
bool Vector<T, Allocator>::operator<(const Vector<T> &rhs) const {
  for (size_t i = 0U; i != std::min(size, rhs.size); ++i) {
    if (elements[i] != rhs.elements[i]) return elements[i] < rhs.elements[i];
  }
  return size < rhs.size;
}

template<typename T, typename Allocator>
bool Vector<T, Allocator>::operator<=(const Vector<T> &rhs) const {
  for (size_t i = 0U; i != std::min(size, rhs.size); ++i) {
    if (elements[i] != rhs.elements[i]) return elements[i] < rhs.elements[i];
  }
  return size <= rhs.size;
}

template<typename T, typename Allocator>
bool Vector<T, Allocator>::operator>(const Vector<T> &rhs) const {
  for (size_t i = 0U; i != std::min(size, rhs.size); ++i) {
    if (elements[i] != rhs.elements[i]) return elements[i] > rhs.elements[i];
  }
  return size > rhs.size;
}

template<typename T, typename Allocator>
bool Vector<T, Allocator>::operator>=(const Vector<T> &rhs) const {
  for (size_t i = 0U; i != std::min(size, rhs.size); ++i) {
    if (elements[i] != rhs.elements[i]) return elements[i] > rhs.elements[i];
  }
  return size >= rhs.size;
}

#endif //EXERCISE_II__VECTOR_H_
