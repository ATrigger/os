//
// Created by petrovich on 22.05.16.
//

#ifndef POLL_EVENT_SSHD_H
#define POLL_EVENT_SSHD_H


#include <queue>
#include <memory>
#include <map>
#include "address.h"
#include "io_service.h"
#include "acceptor.h"
#include "outstring.h"
#include "pty_fd.h"
class sshd
{
    friend struct outbound;
    struct outbound {
    public:
        outbound(sshd* parent);
        ~outbound();
        void handleRead();
        void handleDataToPTY();
        void handleDataFromPTY();
        void handleWrite();
    private:
        void wakeUp();
        sshd* parent;
        std::unique_ptr<pty_fd> master;
        size_t duplicate;
        pid_t child;
        connection socket;

        std::queue<outstring> output,PTY;
    };
public:
    sshd(io::io_service &ep, ipv4_endpoint const &local_endpoint);
    ~sshd();
    ipv4_endpoint local_endpoint() const;
    io::io_service *ios;
private:
    void on_new_connection();
    acceptor ss;
    std::map<outbound *, std::unique_ptr<outbound>> connections;
};


#endif //POLL_EVENT_SSHD_H
