#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int
main(void) {
  struct sockaddr_in server_info;
  socklen_t server_info_len = sizeof(server_info);

  server_info.sin_family = AF_INET;
  server_info.sin_port = htons(1337);

  struct sockaddr client_info;
  socklen_t client_info_len = sizeof(client_info);

#if defined(__APPLE__)
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
#else
  int sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
#endif

  if (sfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

#if defined(__APPLE__)
  if (fcntl(sfd, F_SETFL, O_NONBLOCK) < 0) {
    perror("fcntl");
    close(sfd);
    exit(EXIT_FAILURE);
  }
#endif

  int on = 1;

  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    perror("setsockopt");
    close(sfd);
    exit(EXIT_FAILURE);
  }

  if (bind(sfd, (struct sockaddr*)&server_info, server_info_len) < 0) {
    perror("bind");
    close(sfd);
    exit(EXIT_FAILURE);
  }

  if (listen(sfd, 0) < 0) {
    perror("listen");
    close(sfd);
    exit(EXIT_FAILURE);
  }

  fd_set master_set, working_set;
  int max_sfd;

  FD_ZERO(&master_set);
  max_sfd = sfd;
  FD_SET(sfd, &master_set);

  (void)working_set;

  struct timeval timeout;

  timeout.tv_sec = 3 * 60;
  timeout.tv_usec = 0;

  int cfd = accept(sfd, &client_info, &client_info_len);
  if (cfd < 0) {
    perror("accept");
    close(sfd);
    exit(EXIT_FAILURE);
  }

  char buffer[BUFFER_SIZE];

  while (1) {
    memset(buffer, 0, BUFFER_SIZE);

    ssize_t recv_len = recv(cfd, buffer, BUFFER_SIZE - 1, 0);
    if (recv_len < 0) {
      perror("recv");
      break;
    }
    printf("recieved: %s\n", buffer);

    size_t buffer_len = strlen(buffer);
    buffer[buffer_len] = '$';
    buffer[buffer_len + 1] = '\0';

    ssize_t send_len = send(cfd, buffer, buffer_len + 1, 0);
    if (send_len < 0) {
      perror("send");
      break;
    }
  }
  close(cfd);
  close(sfd);
  return (0);
}
