// author: Michal Komar, 11.03.2019
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <dlfcn.h>







int main(int argc, char* argv[]){

    #ifdef DYNAMIC

    void *handle = dlopen("./library.so", RTLD_LAZY);

    struct block_arr * (*create)(size_t) = dlsym(handle, "create");
    int (*get_index)(struct block_arr *) = dlsym(handle, "get_index");
    void (*run_search_command)(char* ,char* , char* ) = dlsym(handle, "run_search_command");
    int (*load_file)(struct block_arr * , char * ) = dlsym(handle, "load_file");
    void (*delete_block_at_index)(struct block_arr * , int) = dlsym(handle, "delete_block_at_index");
    void (*add_to_report)(char ** , int , float , float , float ) = dlsym(handle, "add_to_report");


    #endif
    int argument_index = 1;
    struct block_arr * arr;
    int block_created = 0;
    char ** operations = calloc(10, sizeof(char *));
    int operations_counter = 0;

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
            //printf("\nCreated table with size: %d\n", atoi(argv[argument_index+1]));
            argument_index = argument_index+2;
            operations[operations_counter++] = "create_table";
            continue;
        }

        if(strcmp(argv[argument_index],"search_directory")==0){
            if(argument_index+3 >= argc){
                printf("Not enough arguments to search directory\n");
                exit(1);
            }
            if(block_created==1){
                run_search_command(argv[argument_index+1], argv[argument_index+2], argv[argument_index+3]);
                int index = load_file(arr, argv[argument_index+3]);
                printf("\nData inserted: %s at index: %d\n", arr->array[index], index);
                argument_index = argument_index+4;
                operations[operations_counter++] = "search_directory";
                continue;
            }
            else{
                run_search_command(argv[argument_index+1], argv[argument_index+2], argv[argument_index+3]);
                argument_index = argument_index+4;
                operations[operations_counter++] = "search_directory (without saving results)";
                continue;
            }

        }

        if(strcmp(argv[argument_index],"remove_block")==0){
            if(argument_index+1 >= argc || block_created==0){
                printf("Not enough arguments to delete or block not created\n");
                exit(1);
            }
            printf("\nData: %s at %d will be deleted\n ", arr->array[atoi(argv[argument_index+1])], atoi(argv[argument_index+1]));
            delete_block_at_index(arr, atoi(argv[argument_index+1]));
            //printf("After deletion: %s", arr->array[argument_index+1]);
            argument_index = argument_index+2;
            operations[operations_counter++] = "remove_block";
            continue;
        }

        argument_index = argument_index+1;

    }

    times(&user_and_system_t_end);
    clock_gettime(CLOCK_REALTIME, &rt_end);

    printf("\noperations: ");
    int i;
    for(i=0; i<operations_counter; i++){
        printf("%s, ", operations[i]); 
    }

    float system_time = user_and_system_t_end.tms_stime - user_and_system_t_start.tms_stime;
    float user_time = user_and_system_t_end.tms_utime - user_and_system_t_start.tms_utime;
    float real_time = (rt_end.tv_sec - rt_start.tv_sec + (float)(rt_end.tv_nsec - rt_start.tv_nsec)/1000000000);

    printf("\nSystem time: %f\n", system_time);
    printf("User time: %f\n", user_time);
    printf("Real time: %f\n", real_time);

    add_to_report(operations, operations_counter, system_time, user_time, real_time);
    #ifdef DYNAMIC
    dlclose(handle);    
    #endif

    return 0;
}