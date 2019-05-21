#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <string.h>
#include <signal.h>
#include <math.h>
#include <sys/stat.h>

#define BLOCK 1
#define INTERLEAVED 2

int **image_matrix;
double **filter_matrix;
int **result_matrix;
int image_width;
int image_height;
int max_pixel_value;
int filter_size;
int number_of_threads;
int * thread_number_buffer;
pthread_t * thread_ids;
int mode;
void *returned_time;
double total_time;

char * image_filename;
char * filter_filename;


int max(int a, int b){
    if(a > b) return a;
    else return b;
}

int min(int a, int b){
    if(a > b) return b;
    else return a;
}

int s(int x, int y){
    int result = 0;
    int i,j;
    
    for(i=0; i<filter_size; i++){
        for(j=0; j<filter_size; j++){
            result += image_matrix[ max(0, min(x - ceil(filter_size / 2) + i - 1, image_height-1)) ][ max(0, min(y - ceil(filter_size / 2) + j - 1, image_width-1)) ] * filter_matrix[i][j];
        }
    }

    return result;
}

void allocate_image_matrix(){
    image_matrix = malloc( sizeof(int*) * image_height);

    int row;
    for(row=0; row<image_height; row++)
        image_matrix[row] = malloc( sizeof(int) * image_width);

    printf("Image matrix allocated\n");
}

void allocate_filter_matrix(){
    filter_matrix = malloc( sizeof(double*) * filter_size);

    int row;
    for(row=0; row<filter_size; row++)
        filter_matrix[row] = malloc( sizeof(double) * filter_size);

    printf("Filter matrix allocated\n");
}

void allocate_result_matrix(){
    result_matrix = malloc( sizeof(int*) * image_height);

    int row;
    for(row=0; row<image_height; row++){
        result_matrix[row] = malloc( sizeof(int) * image_width);
    }

    printf("Result matrix allocated\n");
}


void load_image_matrix(FILE * image_file){

    struct stat st;
    stat(image_filename, &st);
    size_t image_file_size = st.st_size;

    char * input_buffer = malloc(sizeof(char) * image_file_size);

    fread(input_buffer, sizeof(char), image_file_size, image_file);

    input_buffer[image_file_size-1] = '\0';

    char delims[2];
    delims[0] = ' ';
    delims[1] = '\n';

    char * token = strtok(input_buffer, delims);

    if( strcmp( input_buffer, "P2") == 0 ) {
        printf("File format correct\n");
    }
    else {
        printf("File format incorrect\n");
        exit(-1);
    }
    
    token = strtok(NULL, "\n");

    while( token[0] == '#'){
        token = strtok(NULL, "\n");
    }

    sscanf(token, "%d %d", &image_width, &image_height);

    max_pixel_value = atoi( strtok(NULL, delims));

    if(max_pixel_value != 255){
        printf("Warning: max pixel value is not 255, but %d\n", max_pixel_value);
    }

    allocate_image_matrix();

    int row, column;
    for(row=0; row<image_height; row++){
        for(column=0; column<image_width; column++){
            image_matrix[row][column] = atoi( strtok(NULL, delims));
        }
    }

    printf("Image data loaded succesfully\n");
}

void load_filter_matrix(FILE * filter_file){
    struct stat st;
    stat(filter_filename, &st);
    size_t filter_file_size = st.st_size;

    char * input_buffer = malloc(sizeof(char) * filter_file_size);

    fread(input_buffer, sizeof(char), filter_file_size, filter_file);

    input_buffer[filter_file_size-1] = '\0';

    char delims[2];
    delims[0] = ' ';
    delims[1] = '\n';

    char * token = strtok(input_buffer, delims);

    filter_size = atoi( token );

    allocate_filter_matrix();

    int row, column;
    for(row=0; row<filter_size; row++){
        for(column=0; column<filter_size; column++){
            filter_matrix[row][column] = atof( strtok(NULL, delims));
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
            printf("%f ", filter_matrix[row][column]);
        }
        printf("\n");
    }
}

void print_result_matrix(){
    int row, column;

    printf("Printing result matrix: \n");

    for(row=0; row<image_height; row++){
        for(column=0; column<image_width; column++){
            printf("%d ", result_matrix[row][column]);
        }
        printf("\n");
    }
}
  
void segfault_handler(int signal_number){
    printf("Segmentation fault occured, this is most likely because the file is not formatted properly\n");

    exit(-1);
}


void *block_filter(void *arg) {
    int thread_number = *(int*)arg;
    int k = thread_number + 1;
     
    int row, column;

    struct timespec rt_start, rt_end;
    clock_gettime(CLOCK_REALTIME, &rt_start);

    for(row=0; row<image_height; row++){
        for(column=(k-1)*ceil(image_width/number_of_threads); column<=k*ceil(image_width/number_of_threads); column++){
            result_matrix[row][column] = s(row, column);
        }
    }

    clock_gettime(CLOCK_REALTIME, &rt_end);

    double * real_time = malloc(sizeof(double));

    *real_time = (rt_end.tv_sec - rt_start.tv_sec + (float)(rt_end.tv_nsec - rt_start.tv_nsec)/1000000000);

    pthread_exit(real_time); 
}

void *interleaved_filter(void *arg) {
    int thread_number = *(int*)arg;
    int k = thread_number + 1;
     
    int row, column;

    struct timespec rt_start, rt_end;
    clock_gettime(CLOCK_REALTIME, &rt_start);

    for(row=0; row<image_height; row++){
        for(column=(k-1); column<image_width; column+=number_of_threads){
            result_matrix[row][column] = s(row, column);
        }
    }

    clock_gettime(CLOCK_REALTIME, &rt_end);

    double * real_time = malloc(sizeof(double));

    *real_time = (rt_end.tv_sec - rt_start.tv_sec + (double)(rt_end.tv_nsec - rt_start.tv_nsec)/1000000000);

    pthread_exit(real_time); 
} 


void create_result_file(char * filename){
    FILE * result_file = fopen(filename, "wa");

    printf("Saving the result to the result file\n");

    fprintf(result_file, "P2\n%d %d\n255\n", image_width, image_height);

    int row, column;
    int column_counter = 0;

    for(row=0; row<image_height; row++){
        for(column=0; column<image_width; column++){
            column_counter = column_counter + 1;
            if(column_counter%10==0)
                fprintf(result_file, "\n");
            fprintf(result_file, "%d ", result_matrix[row][column]);
        }
        fprintf(result_file, "\n");
    }

    fclose(result_file);
}

void add_to_report(char * mode){
    FILE * report_file = fopen("Times.txt", "a");

    printf("Adding measurements to the report file\n");

    fprintf(report_file, "Image width: %d  Image height: %d  Filter size: %d  Mode: %s  Number of threads: %d  Total time: %.10f\n",
        image_width,
        image_height,
        filter_size,
        mode,
        number_of_threads,
        total_time
    );

    fclose(report_file);
}
   
int main(int argc, char *argv[]) {

    if(argc < 6) {
        printf("Not enough parameters. The program should be launched like this:\n");
        printf("./program [number of threads] [mode] [input image file] [input filter file] [output file]\n");

        exit(-1);
    }

    signal(SIGSEGV, segfault_handler);

    number_of_threads = atoi(argv[1]);
    if( strcmp(argv[2], "block")==0 ) mode = BLOCK;
    else if( strcmp(argv[2], "interleaved")==0 ) mode = INTERLEAVED;
    else{
        printf("Unrecognized mode, it has to be either block or interleaved\n");

        exit(-1);
    }
    FILE *image_file = fopen(argv[3], "r");
    FILE *filter_file = fopen(argv[4], "r");

    image_filename = argv[3];
    filter_filename = argv[4];
    

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
    allocate_result_matrix();


    struct timespec rt_start, rt_end;

    clock_gettime(CLOCK_REALTIME, &rt_start);

    int thread_number;
    thread_number_buffer = malloc(sizeof(int) * number_of_threads);
    thread_ids = malloc(sizeof(pthread_t) * number_of_threads); 

    for(thread_number=0; thread_number < number_of_threads; thread_number++){
        thread_number_buffer[thread_number] = thread_number;
        if(mode == BLOCK)
            pthread_create(&thread_ids[thread_number], NULL, block_filter,(void*) &thread_number_buffer[thread_number]);
        else
            pthread_create(&thread_ids[thread_number], NULL, interleaved_filter,(void*) &thread_number_buffer[thread_number]);
    }

    for(thread_number=0; thread_number < number_of_threads; thread_number++){
        pthread_join(thread_ids[thread_number], &returned_time);
        printf("Thread ID: %ld, filtering time: %.10fs\n", thread_ids[thread_number], *(double*)returned_time);
    }
    
    clock_gettime(CLOCK_REALTIME, &rt_end);

    total_time = (rt_end.tv_sec - rt_start.tv_sec + (double)(rt_end.tv_nsec - rt_start.tv_nsec)/1000000000);

    printf("Total filtering time: %.10f\n", total_time);

    //print_image_matrix();
    //print_filter_matrix();
    //print_result_matrix();
        

    fclose(image_file);
    fclose(filter_file);

    create_result_file(argv[5]);
    add_to_report(argv[2]);

    return 0;
}