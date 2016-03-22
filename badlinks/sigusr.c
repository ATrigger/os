#include <signal.h>
#include <stdio.h>
void handler(int sig, siginfo_t* info, void* vp) { 
    printf("got sigusr from %d",info->si_pid);
} 

int main(){
    struct sigaction action;
    action.sa_sigaction = handler;  
    sigaction(SIGUSR1,&action,0);
    sleep(10000); 
}