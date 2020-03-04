#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "message.h"

#define UNIX_PATH_MAX    108
#define MAX_CLIENTS      16
#define MAX_EPOLL_EVENTS 128
#define USERNAME_MAX_LEN 30


void *listener_thread(void *);
void *input_thread(void *);
void *ping_thread(void *);
void handle_event(struct epoll_event);

void exit_handler(int);
int register_client(int, const char*);

int PORT;
char UNIX_PATH[UNIX_PATH_MAX];

int client_fd[MAX_CLIENTS] = {0};
int client_state[MAX_CLIENTS] = {0};
char client_name[MAX_CLIENTS][USERNAME_MAX_LEN];

int request = 0;
int epoll_fd, unix_fd, inet_fd;

int main(int argc, char **argv) {
    pthread_t listener_tid, input_tid, ping_tid;

    if(argc != 3){
        printf("Incorrect number of arguments. The program should be launched like this: \n");
        printf("./server [PORT] [UNIX file name]");
        
        exit(1);
    }

    PORT = atoi(argv[1]);
    strcpy(UNIX_PATH, argv[2]);
    signal(SIGINT, exit_handler);

    if(pthread_create(&listener_tid, NULL, listener_thread, NULL) != 0){
        exit(1);
    }

    if(pthread_create(&input_tid, NULL, input_thread, NULL) != 0){
        exit(1);
    }

    if(pthread_create(&ping_tid, NULL, ping_thread, NULL) != 0){
        exit(1);
    } 


    if(pthread_join(listener_tid, NULL) != 0){
        exit(1);
    }

    if(pthread_join(input_tid, NULL) != 0){
        exit(1);
    }

    if(pthread_join(ping_tid, NULL) != 0){
        exit(1);
    }

    return 0;
}

void handle_event(struct epoll_event event) {
    message msg;

    size_t bytes_read;
    int fd;
    int index = 0;

    fd = event.data.fd;
    bytes_read = read(fd, &msg, sizeof(message));

    if(bytes_read > 0) {
        switch(msg.type) {
            case REGISTER:
                if( (index = register_client(fd, msg.text)) >= 0) {

                    msg.type=RESPONSE;
                    if(send(fd, &msg, sizeof(msg), 0) < 0){
                        printf("Couldn't send the message to the client\n" );
                    }
                    else
                        printf("Client: %s was successfully registered\n",client_name[index]);
                    
                }
                else {

                    msg.type=FAILED;
                    if(send(fd, &msg, sizeof(msg), 0) < 0) {
                        printf("Couldn't send the message to the client\n" );
                    
                    }
                }

                break;
            case RESPONSE:
                printf("Response number: %d\n", msg.id);
                printf("Words: %d\n", msg.counter);

                for(int i = 0 ; i < MAX_WORDS ; i++){
                    if(msg.words_counter[i] == 0){
                        break;
                    }


                    printf("%s : %d \n",msg.words[i],msg.words_counter[i] );
                }

                for(int i=0; i < MAX_CLIENTS; i++) {
                    if(client_fd[i] == fd) {
                        printf("--------------- \n From: %s\n\n",client_name[i]);
                        client_state[i] = 0;
                        break; 
                    }
                }

                break;
            default:
                break;
        }
    }
}


void *listener_thread(void *arg) {
    int cli_fd;
    struct sockaddr_in inet_addr, inet_cli;
    struct sockaddr_un unix_addr, unix_cli;
    socklen_t addr_len = sizeof(struct sockaddr);

    int event_count;
    struct epoll_event events[MAX_EPOLL_EVENTS];

    if((inet_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))==-1){
        printf("INET socket creation failed\n");
        
        exit(1);
    }

    inet_addr.sin_family = AF_INET;
    inet_addr.sin_addr.s_addr = INADDR_ANY;
    inet_addr.sin_port = htons(PORT);

    if ((bind(inet_fd, (struct sockaddr*) &inet_addr, sizeof(inet_addr))) != 0){
        printf("INET bind failed\n");
        
        exit(2);
    }

    unix_fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (unix_fd == -1){
        printf("UNIX socket creation failed\n");
        
        exit(2);
    }

    unix_addr.sun_family = AF_UNIX;
    strcpy(unix_addr.sun_path, UNIX_PATH);

    if ((bind(unix_fd, (struct sockaddr*) &unix_addr, sizeof(unix_addr))) != 0){
        printf("UNIX bind failed\n");
        
        exit(1);
    }

    if (listen(inet_fd, MAX_CLIENTS) != 0){
        printf("INET listen failed\n");
        
        exit(1);
    }

    if (listen(unix_fd, MAX_CLIENTS) != 0){
        printf("UNIX listen failed\n");
        
        exit(1);
    }


    if((epoll_fd = epoll_create1(0)) == -1){
        printf("Epoll descriptor creation failed\n");
        
        exit(1);
    }

    struct epoll_event ev_unix;
    ev_unix.events = EPOLLIN;
    ev_unix.data.fd = unix_fd;

    if ( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, unix_fd, &ev_unix) == -1) {
        printf("Epoll_ctl failedn");
        exit(1);
    }

    struct epoll_event ev_inet;
    ev_inet.events = EPOLLIN;
    ev_inet.data.fd = inet_fd;

    if ( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, inet_fd, &ev_inet) == -1) {
        printf("Epoll_ctl failed\n");
        exit(1);
    }

    while(1){

        event_count = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, 0);

        for(int i = 0; i < event_count; i++) {
            if(events[i].data.fd == inet_fd) {
                cli_fd = accept(inet_fd, (struct sockaddr*) &inet_cli, &addr_len);
                if(cli_fd >= 0) {

                    struct epoll_event event;
                    event.events = EPOLLIN;
                    event.data.fd = cli_fd;

                    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cli_fd, &event) < 0) {
                        printf("Epoll_ctl failed\n");
                        exit(1);
                    }
                }
            }
            else if(events[i].data.fd == unix_fd) {
                cli_fd = accept(unix_fd, (struct sockaddr*) &unix_cli, &addr_len);
                if(cli_fd >= 0) {

                    struct epoll_event event;
                    event.events = EPOLLIN;
                    event.data.fd = cli_fd;

                    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cli_fd, &event) < 0) {
                        printf("Epoll_ctl failed\n");
                        exit(1);
                    }
                }
            }
            else
                handle_event(events[i]);

        }

        
    }

    pthread_exit(NULL);
}

void *ping_thread(void *arg) {
    message msg_ping;

    while(1){
        for(int i=0; i<MAX_CLIENTS; i++) {
            if(client_fd[i] != 0) {
                msg_ping.type = PING;

                if(send(client_fd[i], &msg_ping, sizeof(msg_ping), MSG_NOSIGNAL) < 0) {
                    printf("Client %s is not responding\n", client_name[i]);

                    struct epoll_event event;
                    event.data.fd = client_fd[i];
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd[i], &event);
                    
                    client_fd[i] = 0;
                    client_state[i] = 0;
                    strcpy(client_name[i], "");
                }
            }
        }
        sleep(1);
    }

    pthread_exit(NULL);
}

void *input_thread(void *arg) {
    message msg;
    int fd;

    msg.type = REQUEST;

    char filename[512];

    while(1) {
        scanf("%s", filename);

        fd = open(filename, O_RDONLY);

        if(fd < 0){
            printf("File not found\n");
            continue;
        }

        if(read(fd, msg.text, sizeof(msg.text)) < 0){
            continue;
        }

        close(fd);

        msg.id = request++;

        int cli_id, cli_fd;
        cli_id = -1;

        for(int i=0; i<MAX_CLIENTS; i++){
            if(client_fd[i] != 0 && client_state[i] == 0){
                cli_id = i;
                break;
            }
            
        }

        if(cli_id == -1){
            printf("No availablie clients\n");
            continue;
        }

        cli_fd = client_fd[cli_id];

        printf("Sending: %s\n",msg.text);

        if(send(cli_fd, &msg, sizeof(msg), 0) < 0){
            printf("Send failed\n");
        }
        else{
            client_state[cli_id] = 1;
        }

    }

    pthread_exit(NULL);
}

void exit_handler(int sig) {
    printf("Shutdown\n");
    for(int i=0; i<MAX_CLIENTS; i++){
        if(client_fd[i] != 0)
            shutdown(client_fd[i], SHUT_RDWR);
    }
    close(inet_fd);
    close(unix_fd);
    close(epoll_fd);
    unlink(UNIX_PATH);

    exit(0);
}


int register_client(int fd, const char *hostname){

    for( int i = 0 ; i < MAX_CLIENTS ; i++){
        if(strcmp(client_name[i],hostname) == 0 )
        return -1;
    }

    for( int i = 0 ; i < MAX_CLIENTS ; i++){
        if(client_fd[i] ==0){
        client_fd[i] = fd;
        client_state[i] = 0;
        strcpy(client_name[i], hostname);
        return i;
        }
    }

    return -2;
}


