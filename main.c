// author: Michal Komar, 11.03.2019
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>
#include <sys/times.h>
#include <unistd.h>







int main(int argc, char* argv[]){
    int argument_index = 1;
    struct block_arr * arr;
    int block_created = 0;

    struct timespec rt_start, rt_end;
    struct tms user_and_system_t_start, user_and_system_t_end;

    times(&user_and_system_t_start);
    clock_gettime(CLOCK_REALTIME, &rt_start);

    while(argument_index < argc){

        if(strcmp(argv[argument_index],"create_table")==0){
            if(argument_index+1 >= argc){
                printf("Not enough arguments to create the block array\n");
                exit(1);
            }
            arr = create(atoi(argv[argument_index+1]));
            block_created = 1;
            printf("\nCreated table with size: %d\n", atoi(argv[argument_index+1]));
            argument_index = argument_index+2;
            continue;
        }

        if(strcmp(argv[argument_index],"search_directory")==0){
            if(argument_index+3 >= argc || block_created==0){
                printf("Not enough arguments to search directory or block not created\n");
                exit(1);
            }
            run_search_command(argv[argument_index+1], argv[argument_index+2], argv[argument_index+3]);
            int index = load_file(arr, argv[argument_index+3]);
            printf("\nData inserted: %s at index: %d\n", arr->array[index], index);
            //printf("Searched, dir: %s, file: %s, tmp file: %s\n", argv[argument_index+1], argv[argument_index+2], argv[argument_index+3]);
            argument_index = argument_index+4;
            continue;
        }

        if(strcmp(argv[argument_index],"remove_block")==0){
            if(argument_index+1 >= argc || block_created==0){
                printf("Not enough arguments to delete or block not created\n");
                exit(1);
            }
            printf("\nData: %s at %d will be deleted\n ", arr->array[atoi(argv[argument_index+1])], atoi(argv[argument_index+1]));
            delete_block_at_index(arr, atoi(argv[argument_index+1]));
            printf("After deletion: %s", arr->array[argument_index+1]);
            argument_index = argument_index+2; 
        }

        argument_index = argument_index+1;

    }

    times(&user_and_system_t_end);
    clock_gettime(CLOCK_REALTIME, &rt_end);

    float system_time = user_and_system_t_end.tms_stime - user_and_system_t_start.tms_stime;
    float user_time = user_and_system_t_end.tms_utime - user_and_system_t_start.tms_utime;
    float real_time = rt_end.tv_sec - rt_start.tv_sec + (rt_end.tv_nsec - rt_start.tv_nsec)/1000000000;

    printf("\nSystem time: %f\n", system_time);
    printf("User time: %f\n", user_time);
    printf("Real time: %f\n", real_time);



    return 0;
}