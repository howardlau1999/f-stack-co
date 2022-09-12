#include "f-stack-co/tcp_connection.h"
#include "f-stack-co/event_loop.h"
#include "f-stack-co/ip_address.h"
#include <memory>
#include <sys/types.h>

namespace fstackco {

tcp_connection::read_write_awaitable::read_write_awaitable(event_loop &loop,
                                                           socket &fd,
                                                           void *buf, size_t n,
                                                           bool write)
    : loop_(loop), fd_(fd), buf_(buf), n_(n), write_(write), rc_(-1) {
  if (write_) {
    rc_ = fd_.write(buf_, n_);
  } else {
    rc_ = fd_.read(buf_, n_);
  }
}

bool tcp_connection::read_write_awaitable::await_ready() { return rc_ > 0; }

void tcp_connection::read_write_awaitable::await_suspend(
    std::coroutine_handle<> h) {
  if (write_) {
    fd_.set_writable_callback([h](...) {
      h.resume();
    });
    loop_.register_write(fd_);
  } else {
    fd_.set_readable_callback([h](...) {
      h.resume();
    });
    loop_.register_read(fd_);
  }
}

ssize_t tcp_connection::read_write_awaitable::await_resume() {
  if (rc_ < 0) {
    if (write_) {
      rc_ = fd_.write(buf_, n_);
      fd_.set_writable_callback(nullptr);
    } else {
      rc_ = fd_.read(buf_, n_);
      fd_.set_readable_callback(nullptr);
    }
  }
  return rc_;
}

tcp_connection::tcp_connection(std::shared_ptr<event_loop> loop,
                               ip_address remote_address, socket fd)
    : loop_(loop), remote_address_(remote_address),
      fd_(std::make_shared<socket>(std::move(fd))) {}

tcp_connection::read_write_awaitable tcp_connection::read(void *buf, size_t n) {
  return read_write_awaitable(*loop_, *fd_, buf, n, false);
}

tcp_connection::read_write_awaitable tcp_connection::write(const void *buf,
                                                           size_t n) {
  return read_write_awaitable(*loop_, *fd_, const_cast<void *>(buf), n, true);
}

socket &tcp_connection::fd() { return *fd_; }

ip_address &tcp_connection::remote_address() { return remote_address_; }

} // namespace fstackco