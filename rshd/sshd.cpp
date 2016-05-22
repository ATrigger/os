//
// Created by petrovich on 22.05.16.
//

#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <zconf.h>
#include <signal.h>
#include <sys/wait.h>
#include "sshd.h"
#include "epoll_error.h"
sshd::sshd(io::io_service &ep, ipv4_endpoint const &local_endpoint)
    :
    ss{ep, local_endpoint, std::bind(&sshd::on_new_connection, this)}
{
    ios = &ep;
}
void sshd::on_new_connection()
{
    std::unique_ptr<outbound> cc(new outbound(this));
    connections.emplace(cc.get(), std::move(cc));
}
sshd::~sshd()
{ }
sshd::outbound::~outbound(){
    kill(child,SIGKILL);
    int status;
    wait(&status);
}
sshd::outbound::outbound(sshd *parent)
    : parent(parent),
      socket(parent->ss.accept([this]()
                               {
                                   this->parent->connections.erase(this);
                               }))
{
    duplicate = 0;
    int master = posix_openpt(O_RDWR);
    if(master == -1) {
        throw_error(errno,"posix_openpt(O_RDWR");
    }
    if(grantpt(master) == -1) {
        throw_error(errno,"grantpt()");
    }
    if(unlockpt(master) == -1) {
        throw_error(errno,"unlockpt()");
    }

    char *slavestr = ptsname(master);

    this->master = std::make_unique<pty_fd>(master,*parent->ios);
    this->master->setOn_read(std::bind(&outbound::handleDataFromPTY,this));
    child = fork();
    if(child == -1) {
        throw_error(errno,"fork()");
    }
    int slavefd;
    if(child == 0) {
        if(setsid() == -1) {
            throw_error(errno,"setsid()");
        }
        slavefd = open(slavestr,O_RDWR);
        if(slavefd == -1) {
            throw_error(errno,"open()");
        }
        if(dup2(slavefd,0) == -1){
            throw_error(errno,"dup2()");
        }
        if(dup2(slavefd,1) == -1){
            throw_error(errno,"dup2()");
        }
        if(dup2(slavefd,2) == -1){
            throw_error(errno,"dup2()");
        }
        close(master);
        close(slavefd);
        exit(execlp("sh","sh"));
    } else {
        close(slavefd);
        wakeUp();
    }
}
void sshd::outbound::wakeUp()
{
    socket.setOn_rw(std::bind(&outbound::handleRead, this), std::bind(&outbound::handleWrite, this));
}
void sshd::outbound::handleRead()
{
    size_t n = socket.get_available_bytes();
    if (n < 1) {
        socket.forceDisconnect();
        return;
    }
    char buff[n + 1];
    auto res = socket.read_over_connection(buff, n);
    buff[n] = '\0';
    if (res == -1) {
        return;
    }
    PTY.push({std::string(buff,n)});
    master->setOn_write(std::bind(&outbound::handleDataToPTY,this));
}
void sshd::outbound::handleDataFromPTY() {
    size_t n = master->get_available_bytes();
    if(n == 0) {
        socket.forceDisconnect();
        return;
    }
    while(duplicate > 0 && n!=0) {
        char buff[std::min(duplicate,n)];
        ssize_t skipped = master->read(buff,std::min(duplicate,n));
        if(skipped == -1) continue;
        duplicate-=skipped;
        n-=skipped;
    }
    char buff[n + 1];
    auto res = master->read(buff,n);
    if(res == -1) return;
    buff[res] = '\0';
    output.push({std::string(buff,n)});
    socket.setOn_write(std::bind(&outbound::handleWrite,this));
}
void sshd::outbound::handleDataToPTY()
{
    if (!PTY.empty()) {
        auto string = &PTY.front();
        size_t written = master->write(string->get(), string->size());
        duplicate+=written+1;
        string->operator+=(written);
        if (*string) {
            PTY.pop();
        }
    }
    if (PTY.empty()) {
        master->setOn_write(connection::callback());
    }
}
void sshd::outbound::handleWrite()
{
    if (!output.empty()) {
        auto string = &output.front();
        size_t written = socket.write_over_connection(string->get(), string->size());
        string->operator+=(written);
        if (*string) {
            output.pop();
        }
    }
    if (output.empty()) {
        socket.setOn_write(connection::callback());
    }
}