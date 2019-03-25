#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

int main(int argc, char * argv[]){
    srand(time(NULL));
    if(argc<5){
        printf("Not enough arguments.\n");
        exit(1);
    }
    FILE * file = fopen(argv[1], "a+");
    if(file==NULL){
        printf("Failed to open file: %s\n", argv[1]);
        exit(1);
    }
    time_t t= time(NULL);
    char * time_str;
    pid_t pid = getpid();
    char time_to_sleep_buffer[8];
    char pid_buffer[8];
    snprintf(pid_buffer, 8, "%d", pid);
    int pmin = atoi(argv[2]);
    int pmax = atoi(argv[3]);
    int bytes = atoi(argv[4]);
    int time_to_sleep = rand()%(pmax-pmin) + pmin;
    fclose(file);

    while(1){
        sleep(time_to_sleep);
        file = fopen(argv[1], "a+");

        fputs(pid_buffer, file);
        snprintf(time_to_sleep_buffer, 8, "%d", time_to_sleep);
        fputc(' ', file);
        fputs(time_to_sleep_buffer, file);
        t = time(NULL);
        time_str = ctime(&t);
        fputc(' ', file);
        fputs(time_str, file);
        fputc(' ', file);

        int i;
        for(i=0; i<bytes; i++) fputc( rand()%(122 - 97) + 97, file);
        fputc(10, file);

        time_to_sleep = rand()%(pmax-pmin) + pmin;
        fclose(file);
    }


    return 0;
}