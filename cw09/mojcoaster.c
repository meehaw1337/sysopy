#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h>

int number_of_carts;
int number_of_passangers;
volatile int cart_capacity;
volatile int current_cart_capacity;
int number_of_rides = 0;
volatile int current_cart = 0;

pthread_mutex_t time_to_load;
pthread_mutex_t time_to_unload;
pthread_mutex_t *cart_mutex;
pthread_mutex_t hop_in_mutex;

pthread_t *cart_thread_ids;
int *cart_thread_buffer;
pthread_t *passenger_thread_ids;
int *passenger_thread_buffer;

pthread_cond_t time_to_unload_cond;
pthread_cond_t *time_to_load_cond;

void allocate(){
    cart_mutex = (pthread_mutex_t*)malloc( sizeof(pthread_mutex_t) * number_of_carts );

    cart_thread_ids = (pthread_t*)malloc( sizeof(pthread_t) * number_of_carts );

    cart_thread_buffer = (int*)malloc( sizeof(int) * number_of_carts );

    passenger_thread_ids = (pthread_t*)malloc( sizeof(pthread_t) * number_of_passangers );

    passenger_thread_buffer = (int*)malloc( sizeof(int) * number_of_passangers );

    time_to_load_cond = (pthread_cond_t*)malloc( sizeof(pthread_cond_t) * number_of_carts );

    int cart_number;
    for(cart_number=0; cart_number<number_of_carts; cart_number++){
        pthread_mutex_init(&cart_mutex[cart_number], NULL);
        pthread_cond_init(&time_to_load_cond[cart_number], NULL);
    }


    printf("Memory allocated\n");
}

void allocate_bak(){
  if((cart_mutex = (pthread_mutex_t*)malloc(number_of_carts*sizeof(pthread_mutex_t))) == NULL){
    exit(1);
  }

    for( int i = 0 ; i < number_of_carts ; ++i){
      pthread_mutex_init(&cart_mutex[i],NULL);
    }

    if((cart_thread_ids = (pthread_t *)malloc(number_of_carts*sizeof(pthread_t))) == NULL){
      exit(1);
    }

    if((passenger_thread_ids = (pthread_t*)malloc(number_of_passangers*sizeof(pthread_t))) == NULL){
      exit(1);
    }

    if((cart_thread_buffer = (int *)malloc(number_of_carts*sizeof(int))) == NULL){
      exit(1);
    }

    if((passenger_thread_buffer = (int *)malloc(number_of_passangers*sizeof(int))) == NULL){
      exit(1);
    }

    if((time_to_load_cond = (pthread_cond_t *)malloc(number_of_carts * sizeof(pthread_cond_t))) == NULL){
      exit(1);
    }
}

void *passenger_thread_function(void *arg){
    int passenger_number = *(int*) arg;

    while(1){
        pthread_mutex_lock(&hop_in_mutex);

        current_cart_capacity = current_cart_capacity + 1;

        if( current_cart_capacity == cart_capacity ){
            pthread_cond_signal(&time_to_unload_cond);
            pthread_mutex_unlock(&time_to_unload);
            printf("Passenger %d hopped in to cart %d as the last passenger\n", passenger_number, current_cart);
        }
        else{
            pthread_mutex_unlock(&hop_in_mutex);
            printf("Passenger %d hopped in to cart %d\n", passenger_number, current_cart);
        }

        pthread_mutex_lock(&cart_mutex[current_cart]);

        current_cart_capacity = current_cart_capacity - 1;

        printf("Passanger %d hopped out, %d passengers left\n",passenger_number,current_cart_capacity);

        if( current_cart_capacity == 0 ){
            pthread_cond_signal(&time_to_unload_cond);
            pthread_mutex_unlock(&time_to_unload);
        }

        pthread_mutex_unlock(&cart_mutex[current_cart]);
        printf("cart_mutex[%d] unlocked\n", current_cart);

        usleep(1000);
    }

    return NULL;
}

void *cart_thread_function(void *arg){
    int cart_number = *(int*) arg;

    if(cart_number == 0) 
        pthread_mutex_lock(&hop_in_mutex);

    int ride_number;
    for(ride_number=0; ride_number<number_of_rides; ride_number++){
        pthread_mutex_lock(&time_to_load);

        if( cart_number != current_cart){
            pthread_cond_wait(&time_to_load_cond[cart_number], &time_to_load);
        }

        printf("Cart %d arrived\n", cart_number);

        if( ride_number != 0 ){
            printf("Cart should unload\n");
            current_cart_capacity = cart_capacity;
            current_cart = cart_number;

            pthread_mutex_unlock(&cart_mutex[cart_number]);
            
            pthread_cond_wait(&time_to_unload_cond, &time_to_unload);
        }

        printf("Cart %d starts letting passengers in\n", cart_number);
        printf("Checking if cart_mutex[%d] is locked\n", cart_number);
        pthread_mutex_lock(&cart_mutex[cart_number]);
        pthread_mutex_unlock(&hop_in_mutex);

        pthread_cond_wait(&time_to_unload_cond, &time_to_unload);
        printf("halo\n");

        current_cart = current_cart + 1;
        current_cart = current_cart % number_of_carts;

        current_cart_capacity = 0;

        pthread_cond_signal(&time_to_load_cond[current_cart]);
        pthread_mutex_unlock(&time_to_load);

        printf("Cart %d left \n", cart_number);
        usleep(10000);
    }

    pthread_mutex_lock(&time_to_load);

    if( cart_number != current_cart ){
        pthread_cond_wait(&time_to_load_cond[cart_number], &time_to_load);
    }

    current_cart_capacity = cart_capacity;
    current_cart = cart_number;

    pthread_mutex_unlock(&cart_mutex[cart_number]);

    pthread_cond_wait(&time_to_unload_cond, &time_to_unload);

    current_cart = current_cart + 1;
    current_cart = current_cart % number_of_carts;

    printf("Cart %d shut down\n", cart_number);

    pthread_cond_signal(&time_to_load_cond[current_cart]);
    pthread_mutex_unlock(&time_to_load);

    return NULL;
}

int main(int argc, char *argv[]){

    number_of_carts = 2;
    number_of_passangers = 10;
    cart_capacity = 5;
    number_of_rides = 13;


    allocate();


    int cart_number;
    int passenger_number;

    for(cart_number=0; cart_number<number_of_carts; cart_number++){
        cart_thread_buffer[cart_number] = cart_number;
        pthread_create(&cart_thread_ids[cart_number], NULL, cart_thread_function, (void*) &cart_thread_buffer[cart_number]);
    }

    for(passenger_number=0; passenger_number<number_of_passangers; passenger_number++){
        passenger_thread_buffer[passenger_number] = passenger_number;
        pthread_create(&passenger_thread_ids[passenger_number], NULL, passenger_thread_function, (void*) &passenger_thread_buffer[passenger_number]);
    }

    for(cart_number=0; cart_number<number_of_carts; cart_number++){
        pthread_join(cart_thread_ids[cart_number], NULL);
    }

    return 0;
}