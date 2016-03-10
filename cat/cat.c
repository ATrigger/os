//
// Created by kamenev on 17.02.16.
//

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

const size_t BUF_SIZE = 1024;

ssize_t read_for(int fd, void *buf, size_t size) {
    size_t count = 0;
    ssize_t ret;
    while ((ret = read(fd, buf + count, size - count)) > 0) {
        count += ret;
    }
    if(ret == -1) return -1;
    return count;
}


ssize_t write_for(int fd, void *buf, size_t size) {
    size_t written = 0;
    ssize_t ret;
    while (written < size && (ret = write(fd, buf + written, size - written)) > 0) {
        written += ret;
    }
    if(ret == -1) return -1;
    return written;
}

int main() {
    char buf[BUF_SIZE];
    while (1) {
        ssize_t count = read_for(STDIN_FILENO, buf, BUF_SIZE);
        if (count <= 0)
            return count;
        ssize_t write_rs = write_for(STDOUT_FILENO, buf, count);
        if (write_rs == -1)
            return errno;
    }
    return 0;
}