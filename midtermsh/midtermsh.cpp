#include <iostream>
#include <vector>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

class mybuf {
    size_t _size;
    size_t capacity;
    char *buf;
public:
    mybuf(size_t cap):_size(0),capacity(cap),buf((char *)malloc(capacity)){

    }
    ~mybuf(){
        free(buf);
    }
    size_t size() {
        return _size;
    }
    ssize_t getline(int source, char *dest){
        //GOT LINE IN BUF
        for(size_t i=0;i<_size;i++){
            if(buf[i]=='\n'){
                for(size_t k=0;k<i;k++) //send TODO: memcpy
                   dest[k] = buf[k];
                for(size_t k=i+1,j=0;k<_size;k++,j++) // move
                    buf[j]=buf[k];
                _size-=i;
                return i;
            }
        }
        _size=0;
        while(1)
        {
            size_t lastsize = _size;
            ssize_t bytes=0;
            bytes = read(source,buf+_size,capacity - _size);
            if(bytes <0) return -1;
            if(bytes == 0)
                break;
            _size+=bytes;
            for(size_t i=lastsize;i<_size;i++){
                if(buf[i]=='\n'){
                    for(size_t k=0;k<i;k++) //send TODO: memcpy
                       dest[k] = buf[k];
                    for(size_t k=i+1,j=0;k<_size;k++,j++) // move
                        buf[j]=buf[k];
                    _size-=i;
                    return i;
                }
            }
        }
        //BUFF OVERFLOW
        //fixme:
        return 0;
    }
    void clear(){
        _size=0;
    }
};
class targetarg{
public:
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
void handler(int sig,siginfo_t*,void*){
    for(auto &x:childs){
        kill(x,sig);
    }
}

int main()
{
    struct sigaction action;
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
                tempargv[i] = [](char *input,int &offset)->char*{
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
        std::cout << args.size() << std::endl;
        [](std::vector<targetarg> &args) -> ssize_t{
            if(args.size() == 0) return 0;
            int pipes[args.size()-1][2];
            for(int i=0;i<args.size()-1;i++)
                pipe2(pipes[i],O_CLOEXEC);
            for(int i=0;i<args.size();i++){
                int whoami = fork();
                if(whoami == 0) {
                    //child
                    if( i + 1 < args.size())
                        dup2(pipes[i][1],STDOUT_FILENO);

                    if( i > 0)
                        dup2(pipes[i-1][0],STDIN_FILENO);

                    _exit(execvp(args[i].argv[0],args[i].argv));
                }
                if(whoami < 0) return -1;
                childs.push_back(whoami);
            }
            for(int i=0;i<args.size()-1;i++){
                close(pipes[i][1]);
                close(pipes[i][0]);
            }
            for(int i=0,stat; i<childs.size();i++){
                waitpid(-1,&stat,0);
            }
            childs.clear();
            return 0;
        }(args);
    }
    return 0;
}

