#pragma once

#include "f-stack-co/detail/noncopyable.h"

#include <cstdint>
#include <functional>
#include <sys/types.h>

namespace fstackco {

class socket : noncopyable {
  int fd_;
  std::function<void(int64_t)> readable_callback_;
  std::function<void(int64_t)> writable_callback_;

public:
  socket(int fd);
  socket(int domain, int type, int protocol);
  socket(socket &&other) noexcept;
  void readable_callback(int64_t data);
  void writable_callback(int64_t data);
  void set_readable_callback(std::function<void(int64_t)> &&callback);
  void set_writable_callback(std::function<void(int64_t)> &&callback);

  ssize_t read(void *buffer, size_t length) const;
  ssize_t write(void const *buffer, size_t length) const;
  int fd() const;
  ~socket();
};

} // namespace fstackco