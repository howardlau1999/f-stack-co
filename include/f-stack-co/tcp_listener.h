#pragma once

#include "f-stack-co/event_loop.h"
#include "f-stack-co/ip_address.h"
#include "f-stack-co/socket.h"
#include "f-stack-co/tcp_connection.h"
#include <coroutine>
#include <cstdint>
#include <memory>

namespace fstackco {

class tcp_listener : public noncopyable {
  std::shared_ptr<event_loop> loop_;
  std::shared_ptr<socket> fd_;

public:
  struct accept_awaitable {
    std::shared_ptr<event_loop> loop_;
    std::shared_ptr<socket> fd_;
    int64_t data_;

  public:
    accept_awaitable(std::shared_ptr<event_loop> loop, std::shared_ptr<socket> fd);
    bool await_ready();
    void await_suspend(std::coroutine_handle<> h);
    std::vector<tcp_connection> await_resume();
  };

  tcp_listener(std::shared_ptr<event_loop> loop, uint16_t port,
               int backlog = 128);
  accept_awaitable accept();
};

} // namespace fstackco