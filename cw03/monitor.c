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
    char buffer[100];
    FILE * file = fopen("pliczek.txt", "r");
    if(file==NULL){ 
        printf("File not found");
        exit(1);
    }
    char * file_name;
    char * seconds_str;
    char * mode = "cpy";//calloc(3, sizeof(char));
    //strcpy(mode, "mem");
    int seconds;
    int seconds_total = 15;
    int i =1;
    while( strlen( fgets(buffer, 100, file) ) > 1 ){
        printf("buffer: %s  length: %ld\n", buffer, strlen(buffer));
        printf("\n %d \n", i);
        file_name = strtok(buffer, " ");
        seconds_str = strtok(NULL, " ");
        seconds = atoi(seconds_str);

        i++;
        printf("starting to monitor: %s\n", file_name);

        pid_t pid = fork();

        if(pid == 0){
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
                        last_modified = f_stat.st_mtime;
                        printf("wykrylem zmiane!\n");
                        last_modified_tm = *localtime(&f_stat.st_mtime);
                        strftime(last_modified_str, 30, "_%Y-%m-%d_%H-%M-%S", &last_modified_tm);
                        strcat(new_file_name, "archiwum/" );
                        strcpy(file_name_tmp, file_name);
                        printf("xD\n");
                        file_name_tmp = get_file_name_from_path(file_name_tmp);
                        printf("%s\n", file_name_tmp);
                        strcat(new_file_name, file_name_tmp);
                        strcat(new_file_name, last_modified_str);
                        printf("nowa nazwa pliku: %s\n", new_file_name);
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
                        printf("nowa zawartosc: %s\n", file_content);
                        strcpy(new_file_name, "");
                        strcpy(file_name_tmp, file_name);
                    }
                    //printf("\nczuwam");
                    clock_gettime(CLOCK_REALTIME, &end);
                    sleep(seconds);
                }
            }
            else if( strcmp(mode, "cpy")==0 ){
                struct stat f_stat;
                stat(file_name, &f_stat);
                time_t last_modified = f_stat.st_mtime;

                char * new_file_name = calloc(30 + strlen(file_name), sizeof(char));
                char * file_name_tmp = calloc(strlen(file_name), sizeof(char));
                char last_modified_str[20];
                struct tm last_modified_tm;

                while( (unsigned int)(end.tv_sec - start.tv_sec) < seconds_total ){
                    printf("monitoruje plik: %s\n", file_name);
                    stat(file_name, &f_stat);
                    if( f_stat.st_mtime > last_modified){
                        last_modified = f_stat.st_mtime;
                        printf("wykrylem zmiane!\n");
                        last_modified_tm = *localtime(&f_stat.st_mtime);
                        strftime(last_modified_str, 30, "_%Y-%m-%d_%H-%M-%S", &last_modified_tm);
                        strcpy(file_name_tmp, file_name);
                        printf("xD\n");
                        file_name_tmp = get_file_name_from_path(file_name_tmp);
                        printf("%s\n", file_name_tmp);
                        strcat(new_file_name, file_name_tmp);
                        strcat(new_file_name, last_modified_str);
                        printf("nowa nazwa pliku: %s\n", new_file_name);

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
            }
            else printf("\nUnrecognized mode");
            exit(copies_made); // moze tutaj zwrocic ilocs skopiowan? xD
        }
        else
        {
            printf("proces mama");
        }
        
    }
    fclose(file);
    printf("\n O CO CHODZI");
    return 0;    
}