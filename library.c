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

    struct stat st;
    stat(filename, &st);
    int file_len = st.st_size;

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

