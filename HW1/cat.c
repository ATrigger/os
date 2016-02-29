//
// Created by kamenev on 17.02.16.
//

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <fcntl.h>
#include <err.h>
#include <malloc.h>
#include <sys/stat.h>

int result;

void raw(int fd);

int main(int argc,char *argv[]){
    int i=1; //multiple file concatenation input
    char *path;
    FILE *fp;

    while((path= argv[i]) != NULL || i == 1){
        int fd;
        if(path == NULL)
            fd=STDIN_FILENO;
        else {
            fd = open(path,O_RDONLY);
        }
        if(fd < 0)
            result = 1;
        else {
            raw(fd);
            if(fd != STDIN_FILENO)
                close(fd);
        }
        if(path == NULL) break;
        i++;
    }
    return result;
}

void raw(int fd){
    int off, wfd;
    ssize_t nr, nw;
    static char *buf = NULL;

    wfd = fileno(stdout);
    if (buf == NULL) {
        if ((buf = malloc(1024)) == NULL)
            err(1, "buffer");
    }
    while ((nr = read(fd, buf, 1024)) > 0)
        for (off = 0; nr; nr -= nw, off += nw)
            if ((nw = write(wfd, buf + off, (size_t)nr)) < 0)
                err(1, "stdout");
    if (nr < 0) {
        result = 1;
    }
}