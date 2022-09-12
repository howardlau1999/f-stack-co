#pragma once

#include "f-stack-co/detail/noncopyable.h"
#include "f-stack-co/event_loop.h"
#include "f-stack-co/ip_address.h"
#include "f-stack-co/socket.h"
#include <coroutine>
#include <memory>

namespace fstackco {

class tcp_connection : public noncopyable {
  std::shared_ptr<event_loop> loop_;
  ip_address remote_address_;
  std::shared_ptr<socket> fd_;

public:
  class read_write_awaitable {
    event_loop &loop_;
    socket& fd_;
    void *buf_;
    size_t n_;
    bool write_;

  public:
    read_write_awaitable(event_loop &loop, socket& fd, void *buf, size_t n, bool write);
    bool await_ready();
    void await_suspend(std::coroutine_handle<> h);
    ssize_t await_resume();
  };
  tcp_connection(std::shared_ptr<event_loop> loop, ip_address remote_address, socket fd);
  tcp_connection(tcp_connection &&) = default;
  read_write_awaitable read(void *buf, size_t n);
  read_write_awaitable write(const void *buf, size_t n);
  socket& fd();
  ip_address& remote_address();
};

} // namespace fstackco