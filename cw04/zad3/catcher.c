#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>

int signals_sent = 0;
int signals_received = 0;
char * mode;

int freeze = 1;
int sender_pid;


void handler_sigrtmin(int sig_num, siginfo_t * info){
    sender_pid = info->si_pid;
    signals_received = signals_received+1;
    kill(sender_pid, SIGUSR1);
}

void handler_sigrtmax(int sig_num, siginfo_t * info){
    freeze = 0;
}


void handler_sigusr1(int sig_num, siginfo_t * info){
    sender_pid = info->si_pid;
    signals_received = signals_received+1;
    kill(sender_pid, SIGUSR1);
}

void handler_sigusr2(int sig_num, siginfo_t * info){
    freeze = 0;
}


int main(int argc, char * argv[]){

    if(argc<2){
        printf("Not enough parameters\n");
        exit(1);
    }

    mode = argv[1];

    struct sigaction actionusr1; 
    actionusr1.sa_sigaction = handler_sigusr1;
    sigemptyset(&actionusr1.sa_mask);
    actionusr1.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &actionusr1, NULL);

    struct sigaction actionusr2; 
    actionusr2.sa_sigaction = handler_sigusr2;
    sigemptyset(&actionusr2.sa_mask);
    actionusr2.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR2, &actionusr2, NULL);

    struct sigaction actionrt1; 
    actionrt1.sa_sigaction = handler_sigrtmin;
    sigemptyset(&actionrt1.sa_mask);
    actionrt1.sa_flags = SA_SIGINFO;

    sigaction(SIGRTMIN, &actionrt1, NULL);

    struct sigaction actionrt2; 
    actionrt2.sa_sigaction = handler_sigrtmax;
    sigemptyset(&actionrt2.sa_mask);
    actionrt2.sa_flags = SA_SIGINFO;

    sigaction(SIGRTMAX, &actionrt2, NULL);

    printf("Catcher's PID: %d\n", getpid());

    sigset_t sigset;
    sigfillset(&sigset);
    sigdelset(&sigset, SIGUSR1);
    sigdelset(&sigset, SIGUSR2);
    sigdelset(&sigset, SIGRTMIN);
    sigdelset(&sigset, SIGRTMAX);
    sigprocmask(SIG_SETMASK, &sigset, NULL);

    union sigval value;

    while(freeze==1);

    int signals_to_send = signals_received;
    while( signals_to_send ){
        if( strcmp(mode, "KILL")==0 ) kill(sender_pid, SIGUSR1);
        else if( strcmp(mode, "SIGQUEUE")==0 ){
            value.sival_int = signals_sent+1;
            sigqueue(sender_pid, SIGUSR1, value);
        }
        else if( strcmp(mode, "SIGRT")==0 ) kill(sender_pid, SIGRTMIN);
        else{
            printf("Unrecognized mode\n");
            exit(1);
        }
        signals_sent = signals_sent+1;
        signals_to_send = signals_to_send-1;
        //printf("Signal sent\n");
        //sleep(1);
    }

    if( strcmp(mode, "KILL")==0 ) kill(sender_pid, SIGUSR2);
    else if( strcmp(mode, "SIGQUEUE")==0 ) sigqueue(sender_pid, SIGUSR2, value);
    else if( strcmp(mode, "SIGRT")==0 ) kill(sender_pid, SIGRTMAX);

    printf("========= CATCHER =========\n");
    printf("Catcher received %d signals.\n", signals_received);

    return 0;
}