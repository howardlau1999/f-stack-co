#include "f-stack-co/socket.h"
#include "f-stack-co/error.h"
#include <cstdint>
#include <ff_api.h>

#include <cassert>
#include <sys/types.h>
#include <utility>

namespace fstackco {

socket::socket(int fd) : fd_(fd) {}

socket::socket(int domain, int type, int protocol)
    : fd_(ff_socket(domain, type, protocol)) {
  check_errno(fd_, "failed to create socket");
}

socket::socket(socket &&other) noexcept
    : fd_(std::exchange(other.fd_, -1)),
      readable_callback_(std::move(other.readable_callback_)),
      writable_callback_(std::move(other.writable_callback_)) {}

int socket::fd() const { return fd_; }

void socket::readable_callback(int64_t data) {
  if (readable_callback_) {
    readable_callback_(data);
  }
}

void socket::writable_callback(int64_t data) {
  if (writable_callback_) {
    writable_callback_(data);
  }
}

void socket::set_readable_callback(std::function<void(int64_t)> &&callback) {
  readable_callback_ = std::move(callback);
}

void socket::set_writable_callback(std::function<void(int64_t)> &&callback) {
  writable_callback_ = std::move(callback);
}

ssize_t socket::read(void *buffer, size_t length) const {
  assert(fd_ > -1);
  auto ret = ff_read(fd_, buffer, length);
  return ret;
}

ssize_t socket::write(void const *buffer, size_t length) const {
  assert(fd_ > -1);
  auto ret = ff_write(fd_, buffer, length);
  return ret;
}

int socket::connect(const struct sockaddr *addr, socklen_t addrlen) const {
  assert(fd_ > -1);
  auto ret = ff_connect(
      fd_, reinterpret_cast<struct linux_sockaddr const *>(addr), addrlen);
  return ret;
}

socket::~socket() {
  if (fd_ > 0) [[likely]] {
    ff_close(fd_);
  }
}

} // namespace fstackco