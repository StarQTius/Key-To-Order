#pragma once

namespace k2o {
namespace detail {

//!
template<typename T>
class unique_ptr {
public:
  //!
  unique_ptr(const unique_ptr &) = delete;

  //!
  unique_ptr(unique_ptr<T> &&other) : m_ptr{other.m_ptr} { other.m_ptr = nullptr; }

  //!
  template<typename U>
  explicit unique_ptr(unique_ptr<U> &&other) : m_ptr{other.m_ptr} {
    other.m_ptr = nullptr;
  }

  //!
  explicit unique_ptr(T *ptr) : m_ptr{ptr} {}

  //!
  unique_ptr &operator=(const unique_ptr &) = delete;

  //!
  template<typename U>
  unique_ptr &operator=(unique_ptr<U> &&other) {
    delete m_ptr;
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;

    return *this;
  }

  //!
  ~unique_ptr() { delete m_ptr; }

  //!
  T &operator*() { return *m_ptr; }

  //!
  const T &operator*() const { return *m_ptr; }

private:
  T *m_ptr;
};

//!
template<typename T>
unique_ptr<T> make_unique(T *ptr) {
  return unique_ptr<T>{ptr};
}

} // namespace detail
} // namespace k2o
