#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

  struct timeval timeout;
  fd_set master_set, working_set;

  bool end_flag = false;

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
  fcntl(sfd, F_SETFL, O_NONBLOCK);
#endif

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

  FD_ZERO(&master_set);
  int max_sfd = sfd;
  FD_SET(sfd, &master_set);

  timeout.tv_sec = 3 * 60;
  timeout.tv_usec = 0;

  char buffer[BUFFER_SIZE];

  do {
    memcpy(&working_set, &master_set, sizeof(master_set));
    printf("wait for select\n");
    int ready_sfd_count
        = select(max_sfd + 1, &working_set, NULL, NULL, &timeout);

    if (ready_sfd_count < 0) {
      perror("select");
      break;
    }

    if (ready_sfd_count == 0) {
      printf("select timeout\n");
      break;
    }

    for (int i = 0; i < max_sfd && ready_sfd_count > 0; ++i) {
      if (FD_ISSET(i, &working_set)) {
        ready_sfd_count -= 1;
        if (i == sfd) {
          printf("readable listening socket\n");
          int cfd;
          while (1) {
            cfd = accept(sfd, &client_info, &client_info_len);
            if (cfd < 0) {
              if (errno != EWOULDBLOCK) {
                perror("accept");
                end_flag = true;
              }
              break;
            }

            printf("new connection: %d\n", cfd);
            FD_SET(cfd, &master_set);
            if (cfd > max_sfd)
              max_sfd = cfd;
          }
        } else {
          printf("readable other socket %d\n", i);

          bool close_connection = false;

          while (1) {
            ssize_t recv_len = recv(i, buffer, BUFFER_SIZE - 1, 0);
            // XXX

            if (recv_len < 0) {
              if (errno != EWOULDBLOCK) {
                perror("recv");
                close_connection = true;
              }
              break;
            }

            if (recv_len == 0) {
              printf("connection closed\n");
              close_connection = true;
              break;
            }

            printf("recieved %ld bytes\n", recv_len);
            size_t buffer_len = strlen(buffer);
            buffer[buffer_len] = '$';
            buffer[buffer_len + 1] = '\0';

            ssize_t send_len = send(i, buffer, buffer_len + 1, 0);
            if (send_len < 0) {
              perror("send");
              close_connection = true;
              break;
            }
          }  // while (1)

          if (close_connection) {
            close(i);
            FD_CLR(i, &master_set);
            if (i == max_sfd) {
              while (FD_ISSET(max_sfd, &master_set) == false)
                --max_sfd;
            }
          }
        }
      }
    }

  } while (end_flag == false);

  for (int i = 0; i <= max_sfd; ++i) {
    if (FD_ISSET(i, &master_set))
      close(i);
  }
  return (0);
}
