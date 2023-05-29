#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "get_next_line.h"

int
ft_max(int a, int b) {
  return a > b ? a : b;
}

void
ft_putstr_fd(const char* s, int fd) {
  write(fd, s, strlen(s));
}

void
fatal_error(void) {
  ft_putstr_fd("Fatal Error\n", STDERR_FILENO);
  exit(1);
}

void
malloc_error(void) {
  ft_putstr_fd("Fatal Error\n", STDERR_FILENO);
  exit(1);
}

char write_buffer[1 << 28];
ssize_t write_size;

void
broadcast_except(int except, int sfd, int max_fd, fd_set* master_set) {
  for (int fd = 0; fd <= max_fd; ++fd) {
    if (fd == sfd || fd == except || !FD_ISSET(fd, master_set))
      continue;
    if (send(fd, write_buffer, write_size, 0) < 0) {
      fatal_error();
    }
  }
}

int
extract_msg(char** buf, char** msg) {
  (void)buf;
  (void)msg;
  return 0;
}

char*
ft_substr(char* s, size_t start, size_t len) {
  char* rtn = malloc(sizeof(char) * (len + 1));
  if (!rtn)
    malloc_error();
  memcpy(rtn, s + start, len);
  rtn[len] = '\0';
  return rtn;
}

char*
str_join_n(char* s1, size_t len1, char* s2, size_t len2) {
  char* newstr = malloc(sizeof(char) * (len1 + len2 + 1));
  if (newstr == NULL)
    malloc_error();
  memcpy(newstr, s1, len1);
  memcpy(newstr + len1, s2, len2);
  newstr[len1 + len2] = '\0';

  return newstr;
}

int
main(int argc, char** argv) {
  if (argc != 2) {
    ft_putstr_fd("wrong arg\n", STDERR_FILENO);
    return 1;
  }

  const int binding_port = atoi(argv[1]);

  int sfd = socket(PF_INET, SOCK_STREAM, 0);

  if (sfd < 0) {
    fatal_error();
  }

  struct sockaddr_in sa;

  memset(&sa, 0, sizeof(struct sockaddr_in));

  sa.sin_family = PF_INET;
  sa.sin_addr.s_addr = htonl(2130706433);  // 127.0.0.1
  sa.sin_port = htons(binding_port);

  if (bind(sfd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
    fatal_error();
  }

  if (listen(sfd, 16) < 0) {
    fatal_error();
  }

  fd_set master_set, working_set;
  int max_fd = sfd;

  FD_ZERO(&master_set);
  FD_SET(sfd, &master_set);

  while (1) {
    struct timeval timeout;
    timeout.tv_sec = 120;
    timeout.tv_usec = 0;

    FD_COPY(&master_set, &working_set);

    int number_of_ready_fd
        = select(max_fd + 1, &working_set, NULL, NULL, &timeout);
    if (number_of_ready_fd < 0) {
      perror(NULL);
      break;
    }

    printf("number_of_ready_fd: %d\n", number_of_ready_fd);

    if (number_of_ready_fd == 0) {
      ft_putstr_fd("timeout!\n", STDOUT_FILENO);
      break;
    }

    for (int test_fd = 0; number_of_ready_fd > 0 && test_fd <= max_fd;
         ++test_fd) {
      if (!FD_ISSET(test_fd, &working_set))
        continue;

      --number_of_ready_fd;

      if (test_fd == sfd) {
        struct sockaddr_in addr;
        socklen_t len;

        int new_cli_fd = accept(sfd, (struct sockaddr*)&addr, &len);

        if (new_cli_fd < 0) {
          perror(NULL);
          fatal_error();
        }

        printf("accepted fd: %d\n", new_cli_fd);

        FD_SET(new_cli_fd, &master_set);
        max_fd = ft_max(max_fd, new_cli_fd);

        write_size = sprintf(write_buffer, "client %d connected\n", new_cli_fd);
        broadcast_except(new_cli_fd, sfd, max_fd, &master_set);

      } else {
        // FIXME

        /* char* msg = get_next_line(test_fd); */

        /* if (msg == NULL) { */
        /*   FD_CLR(test_fd, &master_set); */
        /*   close(test_fd); */
        /*   write_size */
        /*       = sprintf(write_buffer, "client %d disconnected\n", test_fd);
         */
        /* } else { */
        /*   write_size = sprintf(write_buffer, "client %d: %s", test_fd, msg);
         */
        /* } */
        /* broadcast_except(test_fd, sfd, max_fd, &master_set); */
        //

        char* msg;
        char* line;
        char* tmp;
        char read_buffer[4096];
        static const size_t read_size = 4096;

        msg = NULL;
        line = NULL;

        /* while (1) {                                                        */
        /*   if (msg) {                                                       */
        /*     char* ptr = ft_strchr(msg, '\n');                              */
        /*     if (ptr != NULL) {                                             */
        /*       line = ft_substr(msg, 0, ptr - msg + 1);                     */

        /*       write_size                                                   */
        /*           = sprintf(write_buffer, "client %d: %s", test_fd, line); */
        /*       broadcast_except(test_fd, sfd, max_fd, &master_set);         */
        /*       free(line);                                                  */
        /*       line = NULL;                                                 */

        /*       tmp = msg;                                                   */
        /*       msg = ft_substr(tmp, ptr - tmp, strlen(ptr));                */
        /*       free(tmp);                                                   */
        /*     } else {  //  no newline in msg                                */
        /*         // read                                                    */
        /*     }                                                              */

        /*   } else { // msg == NULL                                          */

        /*       ssize_t recv_rtn = recv(test_fd, read_buffer, read_size, 0); */

        /*   }                                                                */
        /* }                                                                  */

        while (1) {
          if (read_buffer == NULL) {
            read_buffer = malloc(sizeof(char) * read_size);
            if (read_buffer == NULL)
              malloc_error();
          }

          ssize_t recv_rtn = recv(test_fd, read_buffer, read_size, 0);

          if (recv_rtn < 0) {
            if (msg) {
              write_size = sprintf(write_buffer, "client %d: %s", test_fd, msg);
              broadcast_except(test_fd, sfd, max_fd, &master_set);
              free(msg);
              msg = NULL;
            }

            break;
          }

          if (recv_rtn == 0) {
            FD_CLR(test_fd, &master_set);
            close(test_fd);
            write_size
                = sprintf(write_buffer, "client %d disconnected\n", test_fd);
            broadcast_except(test_fd, sfd, max_fd, &master_set);
            break;
          }

          int flag = extract_msg(&read_buffer, recv_rtn, &msg);

          if (flag < 0)
            malloc_error();

          if (flag == 1) {
            write_size = sprintf(write_buffer, "client %d: %s", test_fd, msg);
            broadcast_except(test_fd, sfd, max_fd, &master_set);
            free(msg);
            msg = NULL;
          } else {
            char* newmsg = str_join(msg, read_buffer, recv_rtn);
            free(msg);
            msg = newmsg;
          }
        }
      }
    }
  }

  close(sfd);

  return 0;
}
