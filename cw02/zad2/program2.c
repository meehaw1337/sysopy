#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <string.h>
#include <time.h>
#include <stdlib.h>



void recursive_search(char * dir_name, int operation, time_t modification_time){
    DIR * directory;
    struct dirent * ep;
    struct stat f_stat;
    char filename[99];
    int is_requirement_fulfilled = 0;



    directory = opendir (dir_name);
    if(directory != NULL){
        while(ep = readdir(directory)){
            if(strcmp(ep->d_name, ".")!=0 && strcmp(ep->d_name, "..")!=0 ){

                strcpy(filename, dir_name);
                strcat(filename, "/");
                strcat(filename, ep->d_name);
                stat(filename, &f_stat);

                char file_type[10] = "unknown";

                if(S_ISREG(f_stat.st_mode)) strcpy(file_type, "file");
                if(S_ISDIR(f_stat.st_mode)) strcpy(file_type, "dir");
                if(S_ISCHR(f_stat.st_mode)) strcpy(file_type, "char dev");
                if(S_ISBLK(f_stat.st_mode)) strcpy(file_type, "block dev");
                if(S_ISFIFO(f_stat.st_mode)) strcpy(file_type, "fifo");
                if(S_ISLNK(f_stat.st_mode)) strcpy(file_type, "slink");
                if(S_ISSOCK(f_stat.st_mode)) strcpy(file_type, "sock");

                char last_access[30];
                char last_modified[30];
                struct tm last_access_tm = *localtime(&f_stat.st_atime);
                struct tm last_modified_tm = *localtime(&f_stat.st_mtime);
                strftime(last_access, 30, "%d/%m/%Y %H/%M/%S", &last_access_tm);
                strftime(last_modified, 30, "%d/%m/%Y %H/%M/%S", &last_modified_tm);
 

                switch(operation){
                    case -1: if(f_stat.st_mtime < modification_time) is_requirement_fulfilled=1; 
                                //printf("%ld, %ld\n", f_stat.st_mtime, modification_time);
                                //printf("\noperation: %d\n -1", operation); 
                                break;
                    case 0: if(f_stat.st_mtime == modification_time) is_requirement_fulfilled=1;
                                //printf("%ld, %ld\n", f_stat.st_mtime, modification_time); 
                                //printf("\noperation: %d\n 0", operation);
                                break;
                    case 1: if(f_stat.st_mtime > modification_time) is_requirement_fulfilled=1; 
                                //printf("%ld, %ld\n", f_stat.st_mtime, modification_time);
                                //printf("\noperation: %d\n 1", operation); 
                                break;
                    default: break;
                }

                //char * file_path = realpath(ep->d_name, NULL);
                if(is_requirement_fulfilled == 1)
                    printf("%s   %s   size: %ld bytes   last access: %s   last modified: %s\n", filename, file_type, f_stat.st_size, last_access, last_modified);
                if( S_ISDIR(f_stat.st_mode)==1 ){
                    recursive_search(filename, operation, modification_time); 
                }
                strcpy(filename, "");
                strcpy(file_type, "");
                is_requirement_fulfilled=0;
            }
        }
        closedir(directory);
    }
    else
        printf("Couldn't open the directory %s \n", dir_name); 
}



int main(int argc, char * argv[]){
    if(argc<5){
        printf("\nNot enough parameters");
        exit(1);
    }
    int operation;
    char modification_time_str[17];
    time_t modification_time;
    sprintf(modification_time_str, "%s %s", argv[3], argv[4]);
    struct tm modification_time_tm;

    if( strptime(modification_time_str, "%d/%m/%Y %H/%M/%S", &modification_time_tm) == NULL){
        printf("Loading date failed\n");
        exit(1);
    }

    modification_time = mktime(&modification_time_tm);

    // UWAGA: NA UBUNTU Z JAKIEGOS POWODU WCZYTANA DATA JEST O GODZINE MNIEJSZA NIZ POWINNA


    if( strcmp(argv[2], "<")==0 ) operation=-1;
    else if( strcmp(argv[2], "=")==0 ) operation=0;
    else if( strcmp(argv[2], ">")==0 ) operation=1;
    else printf("\nUnrecognized operation: %s\n", argv[2]);
    recursive_search(argv[1], operation, modification_time);
    return 0;
}
