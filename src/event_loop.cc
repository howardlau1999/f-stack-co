#include "f-stack-co/event_loop.h"
#include <cassert>
#include <ff_api.h>
#include <ff_event.h>
#include <mutex>

#include "f-stack-co/error.h"

namespace fstackco {

event_loop::event_loop(size_t max_events)
    : kq_(ff_kqueue()), events_(max_events) {
  check_errno(kq_, "failed to create kqueue");
}

int event_loop::ff_poll_once(void *self) {
  reinterpret_cast<event_loop *>(self)->poll();
  return 0;
}

void event_loop::loop() {
  ff_run(&event_loop::ff_poll_once, this);
}

int event_loop::poll() {
  int nr_events =
      ff_kevent(kq_, nullptr, 0, &events_[0], events_.size(), nullptr);
  assert(nr_events >= 0);
  for (int i = 0; i < nr_events; ++i) {
    struct kevent &event = events_[i];
    int fd = (int)event.ident;
    auto sock = [&]() {
      auto it = sockets_.find(fd);
      if (it == sockets_.end()) {
        return (socket *)nullptr;
      }
      return it->second;
    }();
    if (event.flags & EV_EOF) {
      sockets_.erase(fd);
    } else if (event.filter == EVFILT_READ) {
      if (sock) {
        sock->readable_callback(event.data);
      }
    } else if (event.filter == EVFILT_WRITE) {
      if (sock) {
        sock->writable_callback(event.data);
      }
    }
  }
  return nr_events;
}

void event_loop::register_read(socket &fd) {
  struct kevent event;
  {
    sockets_.insert_or_assign(fd.fd(), &fd);
  }
  EV_SET(&event, fd.fd(), EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
  int ret = ff_kevent(kq_, &event, 1, nullptr, 0, nullptr);
  check_errno(ret, "failed to register read event");
}

void event_loop::register_write(socket &fd) {
  struct kevent event;
  {
    sockets_.insert_or_assign(fd.fd(), &fd);
  }
  EV_SET(&event, fd.fd(), EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
  int ret = ff_kevent(kq_, &event, 1, nullptr, 0, nullptr);
  check_errno(ret, "failed to register write event");
}

} // namespace fstackco