#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>

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
}

int main(int argc, char * argv[]){
    if(argc<6){
        printf("Not enough arguments\n");
        exit(1);
    }
    char * file_name;
    char * seconds_str;
    char * mode = argv[3];
    int seconds;
    int seconds_total = atoi(argv[2]);
    int lim_vm = atoi(argv[4]);
    int lim_cpu = atoi(argv[5]);
    char buffer[512];
    FILE * file = fopen(argv[1], "r");
    if(file==NULL){ 
        printf("File not found");
        exit(1);
    }

    while( strlen( fgets(buffer, 512, file) ) > 1 ){
        file_name = strtok(buffer, " ");
        seconds_str = strtok(NULL, " ");
        seconds = atoi(seconds_str);

        printf("starting to monitor: %s\n", file_name);

        pid_t pid = fork();

        if(pid == 0){

            struct rlimit lim_new;

            getrlimit(RLIMIT_CPU, &lim_new);    
            lim_new.rlim_max = lim_cpu;
            if (lim_new.rlim_cur > lim_cpu) lim_new.rlim_cur = lim_cpu;
            setrlimit(RLIMIT_CPU,  &lim_new);

            getrlimit(RLIMIT_AS, &lim_new);
            lim_new.rlim_max = lim_vm*1000000;
            if (lim_new.rlim_cur > lim_vm*1000000) lim_new.rlim_cur = lim_vm*1000000;
            setrlimit(RLIMIT_AS,  &lim_new);

            int copies_made = 0;
            struct timespec start, end;

            clock_gettime(CLOCK_REALTIME, &start);
            clock_gettime(CLOCK_REALTIME, &end); 

            if( strcmp(mode, "mem")==0 ){
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

                //printf("\n%s\n", new_file_name);
                
                while( (unsigned int)(end.tv_sec - start.tv_sec) < seconds_total){
                    printf("monitoruje plik: %s\n", file_name);
                    stat(file_name, &f_stat);
                    if( f_stat.st_mtime > last_modified){

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
                    clock_gettime(CLOCK_REALTIME, &end);
                    sleep(seconds);
                }
                free(file_content);
                free(file_name_tmp);
                free(new_file_name);
            }
            else if( strcmp(mode, "cpy")==0 ){
                struct stat f_stat;
                stat(file_name, &f_stat);
                time_t last_modified = f_stat.st_mtime;
                int first_copy_made = 0;

                char * new_file_name = calloc(30 + strlen(file_name), sizeof(char));
                char * file_name_tmp = calloc(strlen(file_name), sizeof(char));
                char last_modified_str[20];
                struct tm last_modified_tm;

                while( (unsigned int)(end.tv_sec - start.tv_sec) < seconds_total ){
                    printf("monitoruje plik: %s\n", file_name);
                    stat(file_name, &f_stat);
                    if( f_stat.st_mtime > last_modified || first_copy_made==0){
                        first_copy_made = 1;
                        last_modified = f_stat.st_mtime;
                        printf("wykrylem zmiane!\n");
                        last_modified_tm = *localtime(&f_stat.st_mtime);
                        strftime(last_modified_str, 30, "_%Y-%m-%d_%H-%M-%S", &last_modified_tm);
                        strcpy(file_name_tmp, file_name);
                        //printf("xD\n");
                        file_name_tmp = get_file_name_from_path(file_name_tmp);
                        strcat(new_file_name, file_name_tmp);
                        strcat(new_file_name, last_modified_str);
                        //printf("nowa nazwa pliku: %s\n", new_file_name);
                        copies_made++;

                        pid_t copy_pid = fork();
                        if(copy_pid==0){
                            char* const av[] = {"cp", file_name, new_file_name, NULL};
                            execvp("cp", av);
                            exit(1);
                        }
                        else{
                            last_modified = f_stat.st_mtime;
                            strcpy(new_file_name, "");
                            strcpy(file_name_tmp, file_name);
                        }
                    }
                    clock_gettime(CLOCK_REALTIME, &end);
                    sleep(seconds);
                }
                free(new_file_name);
                free(file_name_tmp);
            }
            else{
                printf("\nUnrecognized mode");
                break;
            }

            struct rusage usage_self;
            struct rusage usage_children;
            getrusage(RUSAGE_SELF, &usage_self);
            getrusage(RUSAGE_CHILDREN, &usage_children);
            printf("Process ID: %d\n", getpid());
            printf("Self user time: %f\n", usage_self.ru_utime.tv_sec - (double)(usage_self.ru_utime.tv_usec / 1000000.0));
            printf("Children user time: %f\n", usage_children.ru_utime.tv_sec - (double)(usage_children.ru_utime.tv_usec / 1000000.0));
            printf("Self system time: %f\n", usage_self.ru_stime.tv_sec - (double)(usage_self.ru_stime.tv_usec / 1000000.0));
            printf("Children system time: %f\n", usage_children.ru_stime.tv_sec - (double)(usage_children.ru_stime.tv_usec / 1000000.0));
            
            exit(copies_made);
        }
    }
    fclose(file);
    pid_t result_pid;
    int status;

    while( (result_pid = wait(&status)) > 0){
        printf("Proces PID: %d utworzyl %d kopii plikow.\n", result_pid, WEXITSTATUS(status));
    }
    return 0;    
}