#pragma once

#include "f-stack-co/detail/noncopyable.h"
#include "f-stack-co/socket.h"

#include <shared_mutex>
#include <unordered_map>
#include <memory>
#include <vector>
#include <ff_api.h>

namespace fstackco {

class event_loop : public noncopyable {
  int kq_;
  std::shared_mutex mutex_;
  std::unordered_map<int, std::weak_ptr<socket>> sockets_;
  std::vector<struct kevent> events_;
public:
  event_loop(size_t max_events = 128);
  void register_read(std::shared_ptr<socket> fd);
  void register_write(std::shared_ptr<socket> fd);
  static int ff_run(void *self);
  int poll();
};

}