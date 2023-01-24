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
  struct sockaddr_in server_info = {0};
  socklen_t server_info_len = sizeof(server_info);

  server_info.sin_family = AF_INET;
  server_info.sin_addr.s_addr = htonl(0x7f000001);
  server_info.sin_port = htons(1337);

  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (connect(sfd, (struct sockaddr*)&server_info, server_info_len) < 0) {
    perror("connect");
    close(sfd);
    exit(EXIT_FAILURE);
  }

  char buff_read[BUFFER_SIZE];
  char buff_write[BUFFER_SIZE];

  while (1) {
    memset(buff_read, 0, BUFFER_SIZE);
    if (scanf("%s", buff_write) != 1)
      break;
    if (strlen(buff_write) >= BUFFER_SIZE) {
      printf("input tooo long. try again\n");
      continue;
    }

    ssize_t send_len = send(sfd, buff_write, strlen(buff_write), 0);
    if (send_len < 0) {
      perror("send");
      break;
    }

    ssize_t recv_len = recv(sfd, buff_read, BUFFER_SIZE - 1, 0);
    if (recv_len < 0) {
      perror("recv");
      break;
    }
    printf("%s\n", buff_read);
  }

  close(sfd);

  return (0);
}
