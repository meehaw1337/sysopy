#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> 
#include <fcntl.h>

void generate(char * filename, int number_of_records, int record_length){
    FILE * file = fopen(filename, "w");

    //char endl[2] = "\n";

    int i, j;
    for(i=0; i<number_of_records; i++){
        for(j=0; j<record_length; j++){
            putc( rand()%26 + 97 , file); 
        }
        //fwrite(endl, sizeof(char), strlen(endl), file);
    } 

    fclose(file);
}


void sort_sys(char * filename, int number_of_records, int record_length){
    int filedesc = open(filename, O_RDWR);

    char * min = calloc(record_length, sizeof(char)); 
    char * current = calloc(record_length, sizeof(char));
    read(filedesc, min, record_length);
    int current_record_no = 0;
    int minimum_record_no = 0;

    int sorted;
    for(sorted=0; sorted<number_of_records-1; sorted++){
        current_record_no = sorted+1; 
        while( read(filedesc, current, record_length)==record_length ){
            //printf("comparing %s  and  %s\n", min, current);
            if( current[0] < min[0] ){
                strcpy(min, current);
                minimum_record_no = current_record_no;
                //printf("assigned %s to min\n", current);
            }
            current_record_no++;
        }
        if(minimum_record_no!=0){
            lseek(filedesc, (sorted)*(record_length), SEEK_SET);
            read(filedesc, current, record_length);
            lseek(filedesc, (minimum_record_no)*(record_length), SEEK_SET);
            write(filedesc, current, record_length);
            //printf("written %s in %d\n", current, minimum_record_no);
        }
        minimum_record_no=0;
        
        lseek(filedesc, (sorted)*(record_length), SEEK_SET);
        write(filedesc, min, record_length); 
        read(filedesc, min, record_length); 
        //printf("going back %d\n", sorted);

    }
    close(filedesc);  
}


void sort_lib(char * filename, int number_of_records, int record_length){
    FILE * file = fopen(filename, "r+w");

    char * min = calloc(record_length, sizeof(char)); 
    char * current = calloc(record_length, sizeof(char));
    fread(min, sizeof(char), record_length, file);
    int current_record_no = 0;
    int minimum_record_no = 0;

    int sorted;
    for(sorted=0; sorted<number_of_records-1; sorted++){
        current_record_no = sorted+1; 
        while( fread(current, sizeof(char), record_length, file)==record_length ){
            //printf("comparing %s  and  %s\n", min, current);
            if( current[0] < min[0] ){
                strcpy(min, current);
                minimum_record_no = current_record_no;
                //printf("assigned %s to min\n", current);
            }
            current_record_no++;
        }
        if(minimum_record_no!=0){
            fseek(file, (sorted)*(record_length), 0);
            fread(current, sizeof(char), record_length, file);
            fseek(file, (minimum_record_no)*(record_length), 0);
            fwrite(current, sizeof(char), record_length, file);
            //printf("written %s in %d\n", current, minimum_record_no);
        }
        minimum_record_no=0;
        
        fseek(file, (sorted)*(record_length), 0);
        fwrite(min, sizeof(char), record_length, file); 
        fread(min, sizeof(char), record_length, file); 
        //printf("going back %d\n", sorted);

    } 
    fclose(file);  
}


void copy_sys(char * filename_from, char * filename_to, int number_of_records, int record_length){
    int desc_from = open(filename_from, O_RDONLY);
    int desc_to = open(filename_to, O_WRONLY | O_CREAT);

    char * buffer = calloc(record_length, sizeof(char));
    int i;

    for(i=0; i<number_of_records; i++){
        if( read(desc_from, buffer, record_length) == record_length )
            write(desc_to, buffer, record_length);
    }

    close(desc_from);
    close(desc_to);
}

void copy_lib(char * filename_from, char * filename_to, int number_of_records, int record_length){
    FILE * file_from = fopen(filename_from, "r");
    FILE * file_to = fopen(filename_to, "w");

    char * buffer = calloc(record_length, sizeof(char));
    int i;

    for(i=0; i<number_of_records; i++){
        if( fread(buffer, sizeof(char), record_length, file_from) == record_length )
            fwrite(buffer, sizeof(char), record_length, file_to);
    }

    fclose(file_from);
    fclose(file_to);
}


int main(){
    srand(time(0));
    //generate("cokolwiek", 10, 10);
    //sort_lib("cokolwiek", 10, 10);
    copy_lib("cokolwiek", "sznycel", 10, 11);
    sort_lib("sznycel", 10, 11);
    return 0;
}