//
// Created by kamenev on 17.02.16.
//

#include <stdio.h>
#include <signal.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
#include <errno.h>
#include <unistd.h>

const size_t BUF_SIZE = 1024;
void sighandle(int signo)
{
    return ;
}
ssize_t read_for(int fd, void *buf, size_t size) {
    size_t count = 0;
    ssize_t ret=0;
    while(ret >= 0 || errno == EINTR){
        while ((ret = read(fd, buf + count, size - count)) > 0) {
            count += ret;
        }
        if(ret == -1) printf("error happened %d\n",errno);
        if(ret == 0) break;
    }
    if(ret == -1) return -1;
    return count;
}


ssize_t write_for(int fd, void *buf, size_t size) {
    size_t written = 0;
    ssize_t ret=0;
    while(written < size || errno == EINTR){
        while (written < size && (ret = write(fd, buf + written, size - written)) > 0) {
            written += ret;
        }
     
    }
    if(ret == -1) return -1;
    return written;
}

int main(int argc,char* argv[]) {
    char buf[BUF_SIZE];
    int target = STDIN_FILENO;
    struct sigaction int_handler;
    int_handler.sa_handler=sighandle;
    sigaction(SIGINT,&int_handler,0);
    if(argv[1]!=NULL) {
        target = open(argv[1],O_RDONLY);
        if(target == -1){ 
            target = STDIN_FILENO; 
            printf("Error in open: %d\n",errno);                    
        }
    }
    while (1) {
        ssize_t count = read_for(target, buf, BUF_SIZE);
        if (count <= 0)
            return count;
        ssize_t write_rs = write_for(STDOUT_FILENO, buf, count);
        if (write_rs == -1)
            return errno;
    }
    return 0;
}