//
// Created by kamenev on 29.01.16.
//


#include <unistd.h>
#include <errno.h>
#include <stdexcept>
#include "handle.h"
#include "debug.h"
#include "epoll_error.h"
handle::handle()
{}
handle::handle(int fd) : raw(fd)
{}
handle::~handle()
{
    close();
}
void handle::close()
{
    if(raw == -1) return;
    LOG("Closed (%d)",raw);
    int ret = ::close(raw);
    if(ret == -1 && errno != EINTR){
        throw_error(errno,"close()");
    }
    raw = -1;
}
int handle::get_raw() const
{
    return raw;
}
