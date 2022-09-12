#include "f-stack-co/tcp_connection.h"
#include "f-stack-co/event_loop.h"
#include "f-stack-co/ip_address.h"
#include <memory>
#include <sys/types.h>

namespace fstackco {

tcp_connection::read_write_awaitable::read_write_awaitable(event_loop &loop,
                                                           socket &fd,
                                                           void *buf, size_t n)
    : loop_(loop), fd_(fd), buf_(buf), n_(n), rc_(-1) {}

tcp_connection::write_awaitable::write_awaitable(event_loop &loop, socket &fd,
                                                 void *buf, size_t n)
    : read_write_awaitable(loop, fd, buf, n) {
  rc_ = fd.write(buf, n);
}

bool tcp_connection::write_awaitable::await_ready() { return rc_ > 0; }

void tcp_connection::write_awaitable::await_suspend(std::coroutine_handle<> h) {
  fd_.set_writable_callback([h, this](...) {
    rc_ = fd_.write(buf_, n_);
    h.resume();
  });
  loop_.register_write(fd_);
}

ssize_t tcp_connection::write_awaitable::await_resume() { return rc_; }

tcp_connection::read_awaitable::read_awaitable(event_loop &loop, socket &fd,
                                               void *buf, size_t n)
    : read_write_awaitable(loop, fd, buf, n) {
  rc_ = fd.read(buf, n);
}

bool tcp_connection::read_awaitable::await_ready() { return rc_ > 0; }

void tcp_connection::read_awaitable::await_suspend(std::coroutine_handle<> h) {
  fd_.set_readable_callback([h, this](...) {
    rc_ = fd_.read(buf_, n_);
    h.resume();
  });
  loop_.register_read(fd_);
}

ssize_t tcp_connection::read_awaitable::await_resume() { return rc_; }

tcp_connection::tcp_connection(std::shared_ptr<event_loop> loop,
                               ip_address remote_address, socket fd)
    : loop_(loop), remote_address_(remote_address),
      fd_(std::make_shared<socket>(std::move(fd))) {}

tcp_connection::read_awaitable tcp_connection::read(void *buf, size_t n) {
  return read_awaitable(*loop_, *fd_, buf, n);
}

tcp_connection::write_awaitable tcp_connection::write(const void *buf,
                                                      size_t n) {
  return write_awaitable(*loop_, *fd_, const_cast<void *>(buf), n);
}

socket &tcp_connection::fd() { return *fd_; }

ip_address &tcp_connection::remote_address() { return remote_address_; }

} // namespace fstackco