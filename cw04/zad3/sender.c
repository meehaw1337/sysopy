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
int confirmed = 1;
int confirmations_received = 0;

void handler_sigrtmin(int sig_num){
    signals_received = signals_received+1;
}

void handler_sigrtmax(int sig_num){
    printf("========= SENDER =========\n");
    printf("Confirmations received: %d\nSignals received: %d\nSignals expected to receive: %d\n",confirmations_received ,signals_received, signals_sent);
    exit(0);
}

void handler_sigusr1(int sig_num, siginfo_t * info){
    if(confirmed==1){
        signals_received = signals_received+1;
        if( strcmp(mode, "SIGQUEUE")==0 ) printf("Catcher has sent %d signals.\n", info->si_value.sival_int);
    }
    else{
        confirmed = 1;
        confirmations_received = confirmations_received+1;
    }
}

void handler_sigusr2(int sig_num){
    printf("========= SENDER =========\n");
    printf("Confirmations received: %d\nSignals received: %d\nSignals expected to receive: %d\n", confirmations_received, signals_received, signals_sent);
    exit(0);
}


int main(int argc, char * argv[]){
    if(argc < 4){
        printf("Not enough parameters\n");
        exit(1);
    }

    pid_t catcher_pid = atoi(argv[1]);
    int signals_to_send = atoi(argv[2]);
    mode = argv[3];

    printf("Sender's PID: %d\n", getpid());

    //signal(SIGUSR1, handler_sigusr1);

    struct sigaction action; 
    action.sa_sigaction = handler_sigusr1;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;


    sigaction(SIGUSR1, &action, NULL);
    signal(SIGUSR2, handler_sigusr2);

    signal(SIGRTMIN, handler_sigrtmin);
    signal(SIGRTMAX, handler_sigrtmax);


    union sigval value;

    while( signals_to_send ){
        if( strcmp(mode, "KILL")==0 ){
            confirmed = 0;
            kill(catcher_pid, SIGUSR1);
        }
        else if ( strcmp(mode, "SIGQUEUE")==0 ){
            confirmed = 0;
            sigqueue(catcher_pid, SIGUSR1, value);
        }
        else if( strcmp(mode, "SIGRT")==0 ){
            confirmed = 0;
            kill(catcher_pid, SIGRTMIN);
        }
        else{
            printf("Unrecognized mode\n");
            exit(1);
        }
        while(confirmed==0);//printf("waiting\n");
        signals_sent = signals_sent+1;
        signals_to_send = signals_to_send-1;
        //printf("Signal sent\n");
        //sleep(1);
    }

   if( strcmp(mode, "KILL")==0 ) kill(catcher_pid, SIGUSR2);
   else if( strcmp(mode, "SIGQUEUE")==0 ) sigqueue(catcher_pid, SIGUSR2, value);
   else if( strcmp(mode, "SIGRT")==0 ) kill(catcher_pid, SIGRTMAX);

    while(1);

    return 0;
}