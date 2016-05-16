#include <signal.h>
#include <stdio.h>
#include <zconf.h>
#include <bits/siginfo.h>
#include <stdlib.h>

void handler(int sig, siginfo_t* info, void* vp) {
    printf("Signal:%d from %d\n",sig,info->si_pid);
    exit(1);
}

int main(){
    struct sigaction action;
    action.sa_sigaction = handler;
    action.sa_flags=SA_SIGINFO;
    sigemptyset(&action.sa_mask);
//    sigaddset(&action.sa_mask,SIGUSR1);
//    sigaddset(&action.sa_mask,SIGUSR2);
	for(int i=1;i<28;i++){
		sigaddset(&action.sa_mask,i);
	}
//    sigaction(SIGUSR1,&action,0);
//    sigaction(SIGUSR2,&action,0);
	for(int i=1;i<28;i++){
		sigaction(i,&action,0);
	}
    sleep(10);
	action.sa_handler = SIG_IGN;
	sigemptyset(&action.sa_mask);
	for(int i=1;i<28;i++){
		sigaction(i,&action,0);
	}
    printf("No signals were caught\n");
}
