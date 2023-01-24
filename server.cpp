#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
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

  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    perror("socket");
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
  return (0);
}
