#include <iostream>
#include <vector>
#include <signal.h>
class mybuf {
    size_t size;
    size_t capacity;
    char *buf;
public:
    mybuf(size_t cap):size(0),capacity(cap),buf(malloc(capacity)){

    }
    ~mybuf(){
        free(buf);
    }
    size_t size() {
        return size;
    }
    ssize_t getline(int source, char *dest){
        //GOT LINE IN BUF
        for(size_t i=0;i<size;i++){
            if(buf[i]=='\n'){
                for(size_t k=0;k<i;k++) //send TODO: memcpy
                   dest[k] = buf[k];
                for(size_t k=i+1,j=0;k<size;k++,j++) // move
                    buf[j]=buf[k];
                size-=i;
                return i;
            }
        }
        while(1)
        {
            size_t lastsize = size;
            do{
            ssize_t bytes = read(fd,buf+size,capacity - size);
            } while(bytes <0 && errno == EINTR);
            if(bytes <0) return -1;
            if(bytes == 0)
                break;
            size+=bytes;
            for(size_t i=lastsize;i<size;i++){
                if(buf[i]=='\n'){
                    for(size_t k=0;k<i;k++) //send TODO: memcpy
                       dest[k] = buf[k];
                    for(size_t k=i+1,j=0;k<size;k++,j++) // move
                        buf[j]=buf[k];
                    size-=i;
                    return i;
                }
            }
        }
        //BUFF OVERFLOW
        //fixme:
        return 0;
    }
    void clear(){
        size=0;
    }
};
class targetarg{
    char **argv;
    targetarg(int counter,char **input){
        argv = (char **) malloc((counter+1) * sizeof(char *));
        for(int i=0;i<counter;i++){
            argv[i]=strdup(input[i]);
        }
        argv[counter]=NULL;
    }
};

bool issplitchar(char x){
    return x=='\0'||x=='\n'||x==' ';
}

std::vector<pid_t> childs;
void handler(int sig,siginfo_t* info,void *vp){
    for(auto &x:childs){
        kill(x,sig);
    }
}

int main()
{
    struct sigaction action;
    action.sa_mask = SA_SIGINFO;
    action.sa_sigaction = handler;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask,SIGINT);
    if(sigaction(SIGINT,&action,NULL) < 0)
       return errno;
    mybuf tmp(4096);
    char targetstr[4096];
    while(true){
        write(STDOUT_FILENO,"$ ",2);
        int count = tmp.getline(STDIN_FILENO,targetstr);
        if(count == 0) return 0;
        if(count < 0){
            //TODO:
        }
        targetstr[count]='\0';
        std::vector<targetarg> args;
        int k=0;
        for(int offset = 0;;k++){
            int argc = [](char * input, int offset)->int{
                if(input[offset]=='\0') return 0;
                int counter = 0;
                for(int i=offset+1;;i++){
                    if(!issplitchar(input[i-1]) && issplitchar(input[i]))
                        counter++;
                    if(input[i]=='|'|| input[i]=='\0')
                        break;
                }
                return counter;
            }(targetstr,offset);
            if(argc == 0){
                break;
            }
            char *tempargv[argc];
            for(int i=0;i<argc;i++){
                argv[i] = [](char *input,int &offset)->char*{
                    int i=offset;
                    while(input[i]==' ') i++;
                    int argstart=i;
                    while(!issplitchar(input[i])) i++;
                    offset=i;
                    return strndup(input + argstart, i - argstart);
                }(targetstr,offset);
            }
            args.push_back({argc,tempargv});
            while(targetstr[offset] == ' ' || targetstr[offset] == '|')
                offset++;
        }
        //TODO:RUN
        [](std::vector<targetarg> &args) {
            
        }(args);
    }
    return 0;
}

