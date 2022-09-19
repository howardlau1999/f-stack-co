#pragma once

#include "f-stack-co/detail/noncopyable.h"
#include "f-stack-co/socket.h"

#include <ff_api.h>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace fstackco {

class event_loop : public noncopyable {
  int kq_;
  std::vector<struct kevent> events_;
  static int ff_poll_once(void *self);

public:
  event_loop(size_t max_events = 128);
  void register_read(socket &fd);
  void register_write(socket &fd);
  void loop();
  int poll();
};

} // namespace fstackco