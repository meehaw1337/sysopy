// author: Michal Komar, 11.03.2019

#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>


struct block_arr *create(size_t arr_length){
    struct block_arr * result = (calloc(1, sizeof(struct block_arr)));
    char ** arr = calloc(arr_length, sizeof(char *));
    result->array = arr;     
    result->arr_length = arr_length;

    return result;


}





int get_index(struct block_arr * arr){
    int index = 0;
    while(arr->array[index] != NULL && index < arr->arr_length-1){
        index = index+1;
    }
    return index;
}




void run_search_command(char * directory, char * filename, char * tmp_filename){
    char * cmd = (char*) malloc(15 + strlen(directory) + strlen(filename) + strlen(tmp_filename));
    strcat(cmd, "find ");
    strcat(cmd, directory);
    strcat(cmd, " -name ");
    strcat(cmd, filename);
    strcat(cmd, " > ");
    strcat(cmd, tmp_filename);

    system(cmd);

    free(cmd);
}




int load_file(struct block_arr * arr, char * filename){
    FILE * file = fopen(filename, "r");

    struct stat f_stat;
    stat(filename, &f_stat);
    int file_len = f_stat.st_size;

    char * cmd_result = calloc(file_len, sizeof(char));
    fread(cmd_result, 1, file_len, file);
    fclose(file);

    int index = get_index(arr);
    arr->array[index] = cmd_result;
    return index;
}




void delete_block_at_index(struct block_arr * arr, int index){
    free(arr->array[index]);
}


void add_to_report(char ** operations, int operations_counter, float system_time, float user_time, float real_time){
    FILE * report = fopen("raport3b.txt", "a+");

    int i;
    for(i=0; i<operations_counter; i++){
        fputs(operations[i], report);
        fputs(" ", report);
    }

    fputs("\nSystem time: ", report);
    char sys_str[20];
    snprintf(sys_str, 10, "%f", system_time);
    fputs(sys_str, report);

    fputs("\nUser time: ", report);
    char usr_str[20];
    snprintf(usr_str, 10, "%f", user_time);
    fputs(usr_str, report);

    fputs("\nReal time: ", report);
    char rl_str[20];
    snprintf(rl_str, 10, "%f", real_time);
    fputs(rl_str, report);

    fputs("\n", report);



    fclose(report);
}

