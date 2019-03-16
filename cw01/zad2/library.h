// author: Michal Komar, 11.03.2019
#ifndef library
#define library

#include <stdlib.h>

struct block_arr {
    char ** array;
    size_t arr_length;
};


struct block_arr * create(size_t arr_length);
int get_index(struct block_arr * arr);
void run_search_command(char * directory, char * filename, char * tmp_filename);
int load_file(struct block_arr * arr, char * filename);
void delete_block_at_index(struct block_arr * arr, int index);
void add_to_report(char ** operations, int operations_counter, float system_time, float user_time, float real_time);


#endif


