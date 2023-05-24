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
#define DEFAULT_PORT 1337

class Server {
 private:
  struct sockaddr_in server_info;
  struct sockaddr client_info;
socklen_t  server_info_len;
socklen_t  client_info_len;

  bool end_server;
  bool close_conn;

  fd_set master_set, working_set;
  int sfd;
  int max_sfd;

  struct timeval timeout;

  int optVal;

  char buffer[BUFFER_SIZE];

  void
  init_socket(void) {
#if defined(__APPLE__)
    sfd = socket(AF_INET, SOCK_STREAM, 0);
#else
    sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
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

    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal))
        < 0) {
      perror("setsockopt");
      close(sfd);
      exit(EXIT_FAILURE);
    }

    if (bind(sfd, (struct sockaddr*)&server_info, server_info_len) < 0) {
      perror("bind");
      close(sfd);
      exit(EXIT_FAILURE);
    }

    if (listen(sfd, 16) < 0) {
      perror("listen");
      close(sfd);
      exit(EXIT_FAILURE);
    }

    FD_ZERO(&master_set);
    max_sfd = sfd;
    FD_SET(sfd, &master_set);
  }

  void
  run(void) {
    while (end_server == false) {
      memcpy(&working_set, &master_set, sizeof(master_set));
      printf("Waiting on select()...\n");

      int ready_fd_count
          = select(max_sfd + 1, &working_set, NULL, NULL, &timeout);

      if (ready_fd_count < 0) {
        perror("select failure");
        break;
      }

      if (ready_fd_count == 0) {
        perror("select timeout");
        break;
      }

      this->process_ready_fd(ready_fd_count);
    }
  }

  void
  process_ready_fd(int count) {
    for (int i = 0; i <= max_sfd && count > 0; ++i) {
      if (!FD_ISSET(i, &working_set))
        continue;

      count--;

      if (i == sfd) {
        printf("  Listening socket is readable\n");
        this->accept_new_connection();

      } else {
        printf("  Descriptor %d is readable\n", i);
        close_conn = false;

        this->recv_and_send(i);

        if (close_conn) {
          this->close_connection(i);
        }
      }
    }
  }

  void
  close_connection(int fd_to_close) {
    close(fd_to_close);
    FD_CLR(fd_to_close, &master_set);
    if (fd_to_close == max_sfd) {
      while (FD_ISSET(max_sfd, &master_set) == false)
        max_sfd -= 1;
    }
  }

  void
  accept_new_connection(void) {
    while (true) {
      int new_sfd = accept(sfd, NULL, NULL);  // XXX
      if (new_sfd < 0) {
        if (errno != EWOULDBLOCK) {
          perror("accept");
          end_server = true;
        }
        break;
      }

      printf("  New incoming connection - %d\n", new_sfd);
      FD_SET(new_sfd, &master_set);
      if (new_sfd > max_sfd)
        max_sfd = new_sfd;
    }
  }

  void
  recv_and_send(int read_fd) {
    while (true) {
      ssize_t received_byte = recv(read_fd, buffer, sizeof(buffer), 0);

      if (received_byte < 0) {
        if (errno != EWOULDBLOCK) {
          perror("  recv() failed");
          close_conn = true;
        }
        break;
      }

      if (received_byte == 0) {
        printf("  Connection closed\n");
        close_conn = true;
        break;
      }

      printf("  %ld bytes received\n", received_byte);

      ssize_t sent_byte = send(read_fd, buffer, received_byte, 0);
      if (sent_byte < 0) {
        perror("  send() failed");
        close_conn = true;
        break;
      }
    }
  }

 protected:
 public:
  Server(void) : server_info(), client_info() {
    server_info_len = sizeof(server_info);

    server_info.sin_family = AF_INET;
    // server_info.sin_addr = INADDR_ANY;  // XXX
    server_info.sin_port = htons(DEFAULT_PORT);

    client_info_len = sizeof(client_info);
    end_server = false;

    timeout.tv_sec = 3 * 60;
    timeout.tv_usec = 0;

    optVal = 1;
  }

  void
  start(void) {
    this->init_socket();
    this->run();
  }

  ~Server(void) {
    for (int i = 0; i <= max_sfd; ++i) {
      if (FD_ISSET(i, &master_set))
        close(i);
    }
  }
};

int
main(void) {
  Server server;
  server.start();

  return (0);
}
