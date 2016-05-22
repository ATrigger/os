//
// Created by kamenev on 06.12.15.
//

#include <iostream>
#include <zconf.h>
#include <fcntl.h>
#include "io_service.h"
#include "sshd.h"
#include "epoll_error.h"
void demonize() {
//    pid_t proc = fork();
//    if(proc == -1) {
//        throw_error(errno,"fork()");
//    }
//    if(proc) {
//        exit(0);
//    } else {
//        pid_t sid = setsid();
//        if(sid == -1) {
//            throw_error(errno,"setsid()");
//        }
//
//        proc = fork();
//        if(proc == -1) {
//            throw_error(errno,"second fork()");
//        }
//
//        if(proc) {
//            std::string daemonpid = std::to_string(proc);
//            int fd = open("/tmp/rshd.pid",O_RDWR | O_CREAT, 0664);
//            if(fd == -1) {
//                throw_error(errno,"open()");
//            }
//            write(fd,daemonpid.c_str(),daemonpid.length());
//            close(fd);
//            exit(0);
//        } else {
//            return;
//        }
//    }

}
int main(int argc,char *argv[])
{
    if(argc < 2) {
        std::cout << "Usage: rshd [port]" << std::endl;
        return 1;
    }
    demonize();
    io::io_service ep;
    sshd server(ep,ipv4_endpoint((uint16_t)std::stoi(argv[1]),ipv4_address::any()));
    ep.run();
    return 0;
}