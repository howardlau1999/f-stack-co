#include "f-stack-co/tcp_listener.h"
#include "f-stack-co/error.h"
#include "f-stack-co/event_loop.h"
#include "f-stack-co/ip_address.h"
#include "f-stack-co/tcp_connection.h"

#include <cassert>
#include <ff_api.h>
#include <memory>

namespace fstackco {

tcp_listener::tcp_listener(std::shared_ptr<event_loop> loop, uint16_t port,
                           int backlog)
    : loop_(loop), fd_(std::make_shared<socket>(AF_INET, SOCK_STREAM, 0)) {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  int rc = ff_bind(fd_->fd(), (struct linux_sockaddr *)&addr, sizeof(addr));
  check_errno(rc, "failed to bind");
  rc = ff_listen(fd_->fd(), backlog);
  check_errno(rc, "failed to listen");
}

tcp_listener::accept_awaitable::accept_awaitable(
    std::shared_ptr<event_loop> loop, std::shared_ptr<socket> fd)
    : loop_(loop), fd_(fd), data_(0) {}

bool tcp_listener::accept_awaitable::await_ready() { return false; }

void tcp_listener::accept_awaitable::await_suspend(std::coroutine_handle<> h) {
  fd_->set_readable_callback([h, this](int64_t data) {
    this->data_ = data;
    h.resume();
  });
}

std::vector<tcp_connection> tcp_listener::accept_awaitable::await_resume() {
  std::vector<tcp_connection> new_connections;
  ip_address remote_address;
  for (int64_t i = 0; i < data_; ++i) {
    int client_fd_ = ff_accept(
        fd_->fd(),
        reinterpret_cast<struct linux_sockaddr *>(&remote_address.ss_),
        &remote_address.len_);
    if (client_fd_ < 0) {
      continue;
    }
    new_connections.emplace_back(loop_, remote_address, socket(client_fd_));
  }
  return new_connections;
}

tcp_listener::accept_awaitable tcp_listener::accept() {
  return accept_awaitable(loop_, fd_);
}

} // namespace fstackco