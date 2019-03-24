#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

char * get_file_name_from_path(char * file_path){
    char * result = calloc(strlen(file_path), sizeof(char));
    char * result_guard = calloc(strlen(file_path), sizeof(char));
    result = strtok(file_path, "/");
    strcpy(result_guard, result);
    while(result != NULL) {
        result = strtok(NULL, "/");
        if( result==NULL ) return result_guard;
        else strcpy(result_guard, result);
    }
}

int main(){
    pid_t pid;
    int tmp = 0;
    while(1){
        if(tmp==0) {pid = fork(); tmp=1;}
        printf("%d\n",pid);
    }
    if(pid==0) printf("xD");
    /*pid_t childid;
    int xD = 5;
    childid = fork();
    if(childid!=0) printf("%d %d\n", childid, xD);
    else
    printf("potomek: %d %d\n", childid, xD);*/
    return 0;
}