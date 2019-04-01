#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

pid_t child_pid;
int freeze = 0;


void handler_sigint(int sig_num){
    printf("\nOdebrano sygnal SIGINT\n");
    
    exit(0);
}


void handler_sigtstp(int sig_num){
    if(freeze == 0){
        printf("\nOczekuje na CTRL+Z - kontynuacja, lub CTRL+C - zakonczenie programu\n");

        kill(child_pid, SIGTERM);

        freeze = 1;
    }
    else{
        child_pid = vfork();

        if( child_pid==0 )
            execl("/home/michalek/Pulpit/c/cw04/datewriter", "datewriter", NULL);

        freeze = 0;
    }
        
}

int main(){

    struct sigaction action; 
    action.sa_handler = handler_sigtstp;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGTSTP, &action, NULL);
    signal(SIGINT, handler_sigint);

    child_pid = vfork();

    if( child_pid==0 )
        execl("datewriter", "datewriter", NULL);

    while(1);

    return 0;
}