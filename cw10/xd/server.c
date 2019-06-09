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
void handle_event(struct epoll_event*);

void exit_handler(int);
int register_client(int, const char*);

 int RUNNING = 1;

 int PORT;
 char UNIX_PATH[UNIX_PATH_MAX];

 int client_fd[MAX_CLIENTS] = {0};
 int client_state[MAX_CLIENTS] = {0};
 char client_name[MAX_CLIENTS][USERNAME_MAX_LEN];

 int request = 0;
 int epoll_fd;

int main(int argc, char **argv)
{
  pthread_t listener_tid, input_tid, ping_tid;

  if(argc != 3){
    printf("ARGS [PORT] [UNIX PORT NAME]\n");
    
    return 1;
  }

  PORT = atoi(argv[1]);
  strcpy(UNIX_PATH, argv[2]);
  signal(SIGINT, exit_handler);

  if(pthread_create(&listener_tid, NULL, listener_thread, NULL) != 0){
    return 1;
  }

  if(pthread_create(&input_tid, NULL, input_thread, NULL) != 0){
    return 1;
  }

  if(pthread_create(&ping_tid, NULL, ping_thread, NULL) != 0){
      return 1;
  } 


  if(pthread_join(listener_tid, NULL) != 0){
    return 1;
  }

  if(pthread_join(input_tid, NULL) != 0){
    return 1;
  }

  if(pthread_join(ping_tid, NULL) != 0){
      return 1;
  }

  return 0;
}

void handle_event(struct epoll_event *event)
{
  message msg;

  size_t bytes_read;
  int fd;
  int index = 0;

  fd = event->data.fd;
  bytes_read = read(fd, &msg, sizeof(message));
  if(bytes_read > 0)
  {
    switch(msg.type)
    {
    case REGISTER:
      if( (index = register_client(fd, msg.text)) >= 0){

        msg.type=RESPONSE;
        if(send(fd, &msg, sizeof(msg), 0) < 0){
          printf("Couldnt send message to client\n" );
          
        }

        printf("REGISTERED CLIENT %s\n",client_name[index]);
        
      }else{

        msg.type=FAILED;
        if(send(fd, &msg, sizeof(msg), 0) < 0){
          printf("Couldnt send message to client\n" );
          
        }
      }

      break;
    case RESPONSE:
      printf("RESPONSE %d\n", msg.id);
      printf("Words: %d\n", msg.counter);


      
      for(int i = 0 ; i < MAX_WORDS ; ++i){
          if(msg.words_counter[i] == 0){
          break;
          }


        printf("%s : %d \n",msg.words[i],msg.words_counter[i] );
        
      }

      for(int i=0; i < MAX_CLIENTS; i++)
      {
        if(client_fd[i] == fd)
          {
            printf("--------------- \nFROM:%s\n",client_name[i]);
            client_state[i] = 0;
            break; }
      }

      break;
    default:
      break;
    }
  }
}


void *listener_thread(void *arg)
{
  int inet_fd, unix_fd, cli_fd;
  struct sockaddr_in inet_addr, inet_cli;
  struct sockaddr_un unix_addr, unix_cli;
  socklen_t addr_len = sizeof(struct sockaddr);

  int event_count;
  struct epoll_event events[MAX_EPOLL_EVENTS];

  if((inet_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))==-1){
    printf("[INET] CREAT FAILURE\n");
    
    exit(1);
  }

  inet_addr.sin_family = AF_INET;
  inet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  inet_addr.sin_port = htons(PORT);

  if ((bind(inet_fd, (struct sockaddr*) &inet_addr, sizeof(inet_addr))) != 0){
    printf("[INET] BIND FAILURE\n");
    
    exit(2);
  }

  unix_fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (unix_fd == -1){
      printf("[UNIX] CREAT FAILURE\n");
      
      exit(2);
  }

  unix_addr.sun_family = AF_UNIX;
  strcpy(unix_addr.sun_path, UNIX_PATH);

  if ((bind(unix_fd, (struct sockaddr*) &unix_addr, sizeof(unix_addr))) != 0){
    printf("[UNIX] BIND FAILURE\n");
    
    exit(1);
  }

  if (listen(unix_fd, MAX_CLIENTS) != 0){
    printf("[UNIX] LISTEN FAILURE\n");
    
    exit(1);
  }

  if (listen(inet_fd, MAX_CLIENTS) != 0){
    printf("[AF_INET] LISTEN FAILURE\n");
    
    exit(1);
  }


  if((epoll_fd = epoll_create1(0)) == -1){
    printf("[EPOLL] CREAT FAILURE\n");
    
    exit(1);
  }

    struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 100;



  while(RUNNING){

    cli_fd = accept(inet_fd, (struct sockaddr*) &inet_cli, &addr_len);
    if(cli_fd >= 0)
    {
        setsockopt(cli_fd, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof(tv));
      struct epoll_event event;
       event.events = EPOLLIN | EPOLLOUT;
       event.data.fd = cli_fd;

       if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cli_fd, &event) < 0)
       {
         printf("[EPOLL] ADD FAILURE\n");
         exit(1);
      }
    }

    cli_fd = accept(unix_fd, (struct sockaddr*) &unix_cli, &addr_len);
    if(cli_fd >= 0)
    {
        setsockopt(cli_fd, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof(tv));
      struct epoll_event event;
       event.events = EPOLLIN | EPOLLOUT;
       event.data.fd = cli_fd;

       if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cli_fd, &event) < 0)
       {
         printf("[EPOLL] ADD FAILURE\n");
         exit(1);
      }
    }



    event_count = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, 0);

    for(int i = 0; i < event_count; i++)
      handle_event(&events[i]);

    
  }

  close(inet_fd);
  close(unix_fd);
  close(epoll_fd);
  unlink(UNIX_PATH);

  pthread_exit(NULL);
}

void *ping_thread(void *arg) {
    message msg_ping;

    while(RUNNING){
        for(int i=0; i<MAX_CLIENTS; i++) {
            if(client_fd[i] != 0) {
                msg_ping.type = PING;

                if(send(client_fd[i], &msg_ping, sizeof(msg_ping), 0) < 0) {
                    printf("Client %s is not responding\n", client_name[i]);

                    struct epoll_event event;
                    event.data.fd = client_fd[i];
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd[i], &event);
                    
                    client_fd[i] = client_state[i] = 0;
                    strcpy(client_name[i], "");
                }
            }
        }
    }

    pthread_exit(NULL);
}

void *input_thread(void *arg) {
  message msg;
  int fp;

  msg.type = REQUEST;

  char filename[512];

  while(RUNNING) {
    scanf("%s", filename);

    fp = open(filename, O_RDONLY);

      if(fp < 0){
          printf("open error\n");
          continue;
      }

      if(read(fp, msg.text, sizeof(msg.text)) < 0){
          continue;
      }

      close(fp);

      msg.id = request++;

      int cli_idx, cli_fd;
      cli_idx = -1;

      for(int i=0; i<MAX_CLIENTS; i++){
        if(client_fd[i] == 0)
          { continue; }

        cli_idx = i;
        if(!client_state[i])
          { break; }
      }

      cli_fd = client_fd[cli_idx];

      if(cli_idx == -1 || cli_fd == 0){
         printf("No availablie clients\n");
         continue;
       }
       printf("sending: %s\n",msg.text);

      if(send(cli_fd, &msg, sizeof(msg), 0) < 0){
         printf("[SENDING] TO %d ERROR\n", cli_fd);
      }
      else{
        client_state[cli_idx] = 1;
      }

  }

  pthread_exit(NULL);
}

void exit_handler(int sig)
{
  printf("shutdown\n");
  RUNNING = 0;
  for(int i=0; i<MAX_CLIENTS; i++){
    if(client_fd[i] != 0)
      shutdown(client_fd[i], SHUT_RDWR);
  }
  sleep(1);
  exit(0);
}


int register_client(int fd, const char *hostname)
{

  for( int i = 0 ; i < MAX_CLIENTS ; ++i){
    if(strcmp(client_name[i],hostname) == 0 )
      return -1;
  }

  for( int i = 0 ; i < MAX_CLIENTS ; ++i){
    if(client_fd[i] ==0){
      client_fd[i] = fd;
      client_state[i] = 0;
      strcpy(client_name[i], hostname);
      return i;
    }
  }

  return -2;
}


