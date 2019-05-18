#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <string.h>
#include <signal.h>

#define BLOCK 1
#define INTERLEAVED 2

int **image_matrix;
int **filter_matrix;
int image_width;
int image_height;
int max_pixel_value;
int filter_size;

void allocate_image_matrix(){
    image_matrix = malloc( sizeof(int*) * image_height);

    int row;
    for(row=0; row<image_height; row++)
        image_matrix[row] = malloc( sizeof(int) * image_width);

    printf("Image matrix allocated\n");
}

void allocate_filter_matrix(){
    filter_matrix = malloc( sizeof(int*) * filter_size);

    int row;
    for(row=0; row<filter_size; row++)
        filter_matrix[row] = malloc( sizeof(int) * filter_size);

    printf("Filter matrix allocated\n");
}


void load_image_matrix(FILE * image_file){
    char input_buffer[2048];


    if( !fgets(input_buffer, 2048, image_file) ) {
        printf("Failed to load the first line of the image file\n");

        exit(-1);
    }

    if( strcmp( input_buffer, "P2\n") == 0 ) {
        printf("File format correct\n");
    }
    else {
        printf("File format incorrect\n");
        exit(-1);
    }

    fgets(input_buffer, 2048, image_file);

    while( input_buffer[0] == '#' ){
        if( !fgets(input_buffer, 2048, image_file) ) {
            printf("End of file reached through comments\n");
            exit(-1);
        }
    }

    image_width  = atoi( strtok(input_buffer, " ") );
    image_height = atoi( strtok(NULL, " ") );

    fgets(input_buffer, 2048, image_file);

    max_pixel_value = atoi( input_buffer );

    if(max_pixel_value != 255){
        printf("Warning: max pixel value is not 255, but %d\n", max_pixel_value);
    }

    allocate_image_matrix();

    int row;
    for(row=0; row<image_height; row++){
        fgets(input_buffer, 2048, image_file);

        int pixel_value = atoi( strtok(input_buffer, " ") );
        image_matrix[row][0] = pixel_value;

        int column;
        for(column=1; column<image_width; column++){
            pixel_value = atoi( strtok(NULL, " ") );
            image_matrix[row][column] = pixel_value;
        }
    }

    printf("Image data loaded succesfully\n");
}

void load_filter_matrix(FILE * filter_file){
    char input_buffer[2048];


    if( !fgets(input_buffer, 2048, filter_file) ) {
        printf("Failed to load the first line of the fliter file\n");

        exit(-1);
    }

    filter_size = atoi( input_buffer );

    allocate_filter_matrix();

    int row;
    for(row=0; row<filter_size; row++){
        fgets(input_buffer, 2048, filter_file);

        int pixel_value = atoi( strtok(input_buffer, " ") );
        filter_matrix[row][0] = pixel_value;

        int column;
        for(column=1; column<filter_size; column++){
            pixel_value = atoi( strtok(NULL, " ") );
            filter_matrix[row][column] = pixel_value;
        }
    }

    printf("Filter data loaded succesfully\n");
}


void print_image_matrix(){
    int row, column;

    printf("Printing image matrix: \n");

    for(row=0; row<image_height; row++){
        for(column=0; column<image_width; column++){
            printf("%d ", image_matrix[row][column]);
        }
        printf("\n");
    }
}

void print_filter_matrix(){
    int row, column;

    printf("Printing filter matrix: \n");

    for(row=0; row<filter_size; row++){
        for(column=0; column<filter_size; column++){
            printf("%d ", filter_matrix[row][column]);
        }
        printf("\n");
    }
}
  
void segfault_handler(int signal_number){
    printf("Segmentation fault occured, this is most likely because the file is not formatted properly\n");

    exit(-1);
}


void *myThreadFun(void *vargp) { 
    sleep(1); 
    printf("Printing GeeksQuiz from Thread \n"); 
    return NULL; 
} 
   
int main(int argc, char *argv[]) {

    /*if(argc < 6) {
        printf("Not enough parameters. The program should be launched like this:\n");
        printf("./program [number of threads] [mode] [input image file] [input filter file] [output file]\n");

        exit(-1);
    }*/

    // [index wiersza][index kolumny]

    signal(SIGSEGV, segfault_handler);

    FILE * image_file = fopen(argv[1], "r");
    FILE * filter_file = fopen(argv[2], "r");

    if(image_file == NULL){
        printf("Image file not found\n");
        
        exit(-1);
    }

    if(filter_file == NULL){
        printf("Filter file not found\n");
        
        exit(-1);
    }

    load_image_matrix(image_file);
    load_filter_matrix(filter_file);

    print_image_matrix();
    print_filter_matrix();

    fclose(image_file);
    fclose(filter_file);

    return 0;
}