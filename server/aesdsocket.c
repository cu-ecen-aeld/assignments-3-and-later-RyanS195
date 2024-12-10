#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

static void eventHandler();
static void signal_handler(int event_signal);
static bool check_args(int argc, char *argv[]);
static void DataProcessor(int FDAccept);

typedef enum 
{
  SigInt,
  SigTerm
} SignReported;
SignReported SigReportedStatus = 0;

int main(int argc, char *argv[])
{
  int FDSoc;
  int FDAccept;
  int temp_addr = 1;

  pid_t pid = 0;
  bool is_args=check_args(argc, argv);

  socklen_t add_len = sizeof(struct sockaddr_storage);

  struct addrinfo *server_data;
  struct addrinfo default_info;
  struct sockaddr_storage soc_addr_storage;

  eventHandler();

  memset(&default_info, 0, sizeof default_info);

  default_info.ai_socktype = SOCK_STREAM;
  default_info.ai_flags = AI_PASSIVE;
  default_info.ai_family = AF_INET;
 
  getaddrinfo("localhost", "9000", &default_info, &server_data);

  FDSoc = socket(server_data->ai_family, server_data->ai_socktype, server_data->ai_protocol);

  setsockopt(FDSoc, SOL_SOCKET, SO_REUSEADDR, &temp_addr, sizeof(temp_addr));

  bind(FDSoc, server_data->ai_addr, server_data->ai_addrlen);

  if (is_args)
  {
    pid = fork();
  }

  listen(FDSoc, 1);

  // we are child
  if (pid ==0)
  {
    while (1)
    {
        FDAccept = accept(FDSoc, (struct sockaddr *) &soc_addr_storage, &add_len);

        if (SigReportedStatus != 0)
        {
            remove("/var/tmp/aesdsocketdata");
            freeaddrinfo(server_data);
            exit(1);
        }
        else if (FDAccept < 0)
        {
            freeaddrinfo(server_data); 
            close(FDSoc);
            close(FDAccept);
            exit(FDAccept);
        }
        else
        {
            DataProcessor(FDAccept);
        }
    }
  close(FDSoc);
  }
  
  // stop mem leaks
  freeaddrinfo(server_data);
  return errno;
}

static void eventHandler()
{
  struct sigaction event;
  memset(&event, 0, sizeof(struct sigaction));
  event.sa_handler = signal_handler;

  if (sigaction(SIGINT, &event, NULL) != 0) 
  {
    printf("Error: SIGINT");
    exit(errno);
  }
  if (sigaction(SIGTERM, &event, NULL) != 0) 
  {
    printf("Error: SIGTERM");
    exit(errno);
  }
  
}

static bool check_args(int argc, char *argv[])
{
  if (!((argc > 1) && (0 == strcmp(argv[1], "-d"))))
  {
    return false;
  }
  else
  {
    return true;
  }
}


static void signal_handler(int event_signal)
{
  if (event_signal == SIGTERM || event_signal == SIGINT)
  {
    SigReportedStatus = 1;
  }
}

static void DataProcessor(int FDAccept)
{
  char * buffer_ptr = NULL;
  char * buffer_ptr_2 = NULL;
  char * sendBuff;

  ssize_t recieve_size;
  ssize_t combined_size = 0;
  size_t remaining_text_size;

  bool is_file_empty = false;

  int curr_factor = 1;
  int fd;

  buffer_ptr = (char *) calloc(1024, sizeof(char));

  // process the recieve
  while (false == is_file_empty)
  {
    recieve_size = recv(FDAccept, &buffer_ptr[combined_size], (1024 *  curr_factor) - combined_size - 1, 0);

    fd = open("/var/tmp/aesdsocketdata", O_RDWR | O_CREAT | O_APPEND, 0666);

    combined_size += recieve_size;
    buffer_ptr[combined_size] = '\0';

    char* file_check  = strchr(&buffer_ptr[0], '\n');
    if (file_check)
    {
      is_file_empty = true;
    }
    else
    {
      curr_factor++;
      buffer_ptr_2 = (char *)realloc(buffer_ptr, 1024 *  curr_factor);
      buffer_ptr = buffer_ptr_2;
    }
  }

  write(fd, &buffer_ptr[0], combined_size);

  lseek(fd, 0 , SEEK_SET);
  sendBuff = (char *)calloc(1024, sizeof(char));
  
  // process the send
  while ((remaining_text_size = read(fd, sendBuff, 1024)) > 0) 
  {
     send(FDAccept, &sendBuff[0], remaining_text_size, 0);
  }

  free(sendBuff);
  free(buffer_ptr);
  close(fd);
}
