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
  protected:
    event_loop &loop_;
    socket &fd_;
    void *buf_;
    size_t n_;
    int rc_;

  public:
    read_write_awaitable(event_loop &loop, socket &fd, void *buf, size_t n);
  };

  class write_awaitable : public read_write_awaitable {
  public:
    write_awaitable(event_loop &loop, socket &fd, void *buf, size_t n);
    bool await_ready();
    void await_suspend(std::coroutine_handle<> h);
    ssize_t await_resume();
  };

  class read_awaitable : public read_write_awaitable {
  public:
    read_awaitable(event_loop &loop, socket &fd, void *buf, size_t n);
    bool await_ready();
    void await_suspend(std::coroutine_handle<> h);
    ssize_t await_resume();
  };

  class connect_awaitable {
    std::shared_ptr<event_loop> loop_;
    socket fd_;
    ip_address remote_address_;
    int rc_;
  public:
    connect_awaitable(std::shared_ptr<event_loop> loop, ip_address remote_address);
    bool await_ready();
    void await_suspend(std::coroutine_handle<> h);
    tcp_connection await_resume();
  };

  tcp_connection(std::shared_ptr<event_loop> loop, ip_address remote_address,
                 socket fd);
  tcp_connection(tcp_connection &&) = default;
  void set_loop(std::shared_ptr<event_loop> loop);
  read_awaitable read(void *buf, size_t n);
  write_awaitable write(const void *buf, size_t n);
  socket &fd();
  ip_address &remote_address();
};

} // namespace fstackco