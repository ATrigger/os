//
// Created by petrovich on 22.05.16.
//

#ifndef POLL_EVENT_PTY_FD_H
#define POLL_EVENT_PTY_FD_H


#include <cstring>
#include <stdio.h>
#include <functional>
#include "handle.h"
#include "io_service.h"
class pty_fd
{

public:
    pty_fd(int, io::io_service &);
    typedef std::function<void()> callback;
    void setOn_read(const callback &_on_read);
    void setOn_write(const callback &_on_write);
    void setOn_rw(const callback &_on_read, const callback &on_write);
    const handle& getFd() const;
    ~pty_fd();
    ssize_t read(void *data, size_t size);
    size_t write(void const *data, size_t size);
    size_t get_available_bytes() const;
protected:
    handle fd;
    bool *destroyed;
    void syncIO();
    callback on_read;
    callback on_write;
    // TODO: check if shared_ptrs are neccessary. DONE
    io::io_entry ioEntry;

};

#endif //POLL_EVENT_PTY_FD_H
