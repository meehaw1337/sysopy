#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

int freeze = 0;


void handler_sigint(int sig_num){
    printf("\nOdebrano sygnal SIGINT\n");

    exit(0);
}

void handler_sigtstp(int sig_num){
    if(freeze == 0){
        printf("\nOczekuje na CTRL+Z - kontynuacja, lub CTRL+C - zakonczenie programu\n");
        freeze = 1;
    }
    else
        freeze = 0;
}

int main(){
    
    signal(SIGINT, handler_sigint);

    struct sigaction action; 
    action.sa_handler = handler_sigtstp;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGTSTP, &action, NULL);

    char * time_str;
    time_t t;

    while(1){
        t = time(NULL);
        time_str = ctime(&t);

        if(freeze == 0) printf("%s", time_str);
        
        sleep(2);
    }

    return 0;
}