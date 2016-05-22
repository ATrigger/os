//
// Created by kamenev on 05.12.15.
//

#include <sys/epoll.h>
#include <sys/ioctl.h>
#include "io_service.h"
#include "pty_fd.h"
#include "posix_sockets.h"
#include "debug.h"
pty_fd::pty_fd(int _fd, io::io_service &ep)
    : fd(_fd), destroyed(nullptr),
      ioEntry(ep, fd, 0, [this](uint32_t events)
      {
          bool is_destroyed = false;
          destroyed = &is_destroyed;
          try {
              if (events & EPOLLIN) {
                  on_read();
                  if (is_destroyed) return;
              }
              if (events & EPOLLOUT) {
                  on_write();
                  if (is_destroyed) return;
              }
          }
          catch (...) {
              destroyed = nullptr;
              INFO("EPOLL execution failed");
              __throw_exception_again;
          }
          destroyed = nullptr;
      })
{

}
void pty_fd::syncIO()
{
    uint32_t flags = EPOLLERR | EPOLLHUP | EPOLLRDHUP;
    if (on_read) flags |= EPOLLIN;
    if (on_write) flags |= EPOLLOUT;

    this->ioEntry.modify(flags);
}
ssize_t pty_fd::read(void *data, size_t size)
{
    return read_some(fd, data, size);
}
size_t pty_fd::write(void const *data, size_t size)
{
    return write_some(fd, data, size);
}
size_t pty_fd::get_available_bytes() const
{
    int n = -1;
    if (ioctl(fd.get_raw(), FIONREAD, &n) < 0) {
        LOG("IOCTL failed: %d. No bytes available. Returning 0", errno);
        return 0;
    }
    return static_cast<size_t>(n);
}
const handle &pty_fd::getFd() const
{
    return fd;
}
void pty_fd::setOn_write(const callback &_on_write)
{
    on_write = _on_write;
    syncIO();
}
void pty_fd::setOn_read(const callback &_on_read)
{
    on_read = _on_read;
    syncIO();
}
pty_fd::~pty_fd()
{
    if (destroyed) *destroyed = true;
}
void pty_fd::setOn_rw(const pty_fd::callback &on_read, const pty_fd::callback &on_write)
{
    this->on_read = on_read;
    this->on_write = on_write;
    syncIO();
}
