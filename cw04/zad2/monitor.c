#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int freeze = 0;


void handler_sigusr1(int sig_num){
    freeze = 1;
}

void handler_sigint(int sig_num){
    printf("\nOdebrano sygnal SIGINT\n");

    exit(0);
}

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
    return result_guard;
}// moze tutaj zwrocic ilocs skopiowan? xD

int main(int argc, char * argv[]){
    if(argc<2){
        printf("Not enough arguments\n");
        exit(1);
    }
    char * file_name;
    char * seconds_str;
    int seconds;
    char buffer[512];
    char command_buffer[16];
    FILE * file = fopen(argv[1], "r");
    if(file==NULL){ 
        printf("File not found");
        exit(1);
    }
    mkdir("archiwum", 0777);
    while( strlen( fgets(buffer, 512, file) ) > 1 ){
        file_name = strtok(buffer, " ");
        seconds_str = strtok(NULL, " ");
        seconds = atoi(seconds_str);

        pid_t pid = fork();

        if(pid == 0){
            printf("Process %d started to monitor: %s\n", getpid(), file_name);
            int copies_made = 0;

                FILE * file = fopen(file_name, "r");
                if(file==NULL){ printf("failed to open file: %s", file_name); exit(1);}
                struct stat f_stat;
                stat(file_name, &f_stat);
                char * file_content = calloc(f_stat.st_size, sizeof(char));
                time_t last_modified = f_stat.st_mtime;
                fread(file_content, 1, f_stat.st_size, file);
                size_t file_size = f_stat.st_size;

                char * new_file_name = calloc(30 + strlen(file_name), sizeof(char));
                char * file_name_tmp = calloc(strlen(file_name), sizeof(char));
                char last_modified_str[20];
                struct tm last_modified_tm;
                fclose(file);

                
                while( 1 ){
                    printf("monitoruje plik: %s\n", file_name);
                    stat(file_name, &f_stat);
                    if( f_stat.st_mtime > last_modified && freeze == 0){

                        // PRZYGOTOWANIE NAZWY PLIKU DO ARCHIWUM
                        printf("wykrylem zmiane!\n");
                        last_modified = f_stat.st_mtime;
                        last_modified_tm = *localtime(&f_stat.st_mtime);
                        strftime(last_modified_str, 30, "_%Y-%m-%d_%H-%M-%S", &last_modified_tm);
                        strcat(new_file_name, "archiwum/" );
                        strcpy(file_name_tmp, file_name);
                        file_name_tmp = get_file_name_from_path(file_name_tmp);
                        strcat(new_file_name, file_name_tmp);
                        strcat(new_file_name, last_modified_str);
                        // UMIESZCZENIE PLIKU W ARCHIWUM
                        FILE * new_file = fopen(new_file_name, "w");
                        if(new_file==NULL) { printf("failed to open file: %s", new_file_name); exit(1);}
                        fwrite(file_content, 1, file_size, new_file);
                        fclose(new_file);

                        copies_made++;

                        last_modified = f_stat.st_mtime;
                        strcpy(file_content, "");
                        file_content = realloc(file_content, f_stat.st_size * sizeof(char));
                        file = fopen(file_name, "r");
                        if(file==NULL) {printf("failed to open file: %s", file_name); exit(1);}
                        fread(file_content, 1, f_stat.st_size, file);
                        fclose(file);
                        file_size = f_stat.st_size;
                        strcpy(new_file_name, "");
                        strcpy(file_name_tmp, file_name);
                    }
                    //printf("\nczuwam");
                    sleep(seconds);
                }
                free(file_content);
                free(file_name_tmp);
                free(new_file_name);
            
            exit(copies_made);
        }

        
    }
    fclose(file);
    pid_t result_pid;
    int status;

    while( strcmp(command_buffer, "END") != 0){
        scanf("%15s", command_buffer);
    }

    while( (result_pid = wait(&status)) > 0){
        printf("Proces PID: %d utworzyl %d kopii plikow.\n", result_pid, WEXITSTATUS(status));
    }
    return 0;    
}