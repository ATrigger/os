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

void scaninput(char *argv[]);
void raw(int fd);

int main(int argc,char *argv[]){
    scaninput(argv);
    return result;
}
void scaninput(char *argv[]){
    int i=1; //multiple file concatenation input
    char *path;
    FILE *fp;

    while((path=argv[i]) != NULL || i==1){
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
}

void raw(int fd){
    int off, wfd;
    ssize_t nr, nw;
    static size_t bsize;
    static char *buf = NULL;
    struct stat sbuf;

    wfd = fileno(stdout);
    if (buf == NULL) {
        if (fstat(wfd, &sbuf))
            err(1, "%d", fd);
        bsize = ((sbuf.st_blksize> 1024)?(sbuf.st_blksize):(1024));
        if ((buf = malloc(bsize)) == NULL)
            err(1, "buffer");
    }
    while ((nr = read(fd, buf, bsize)) > 0)
        for (off = 0; nr; nr -= nw, off += nw)
            if ((nw = write(wfd, buf + off, (size_t)nr)) < 0)
                err(1, "stdout");
    if (nr < 0) {
        result = 1;
    }
}