#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_BUF 1024
#define CONTROL_LED "/sys/class/leds/tpacpi::lid_logo_dot/brightness"
#define PIPE_NAME "/tmp/pipe"

void iledDaemon() {
  // file descriptor, pipe name and character buffer initialization
  int fd;
  char *pipe = PIPE_NAME;
  char buf[MAX_BUF];

  // Creating named pipe with prw-rw-rw- privileges
  mkfifo(pipe, 0666);
  // assign value to file descriptor and check if it can be opened
  fd = open(pipe, O_RDONLY);
  if (fd < 0) {
    perror("open");
    exit(1);
  }

  // Main daemon loop
  while (1) {
    // Initialize the set of file descriptors to be processed by select()
    // Zero it and set file descriptor as its max element
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);

    // Total number of bits set in a bit mask and error checking
    int ready = select(fd + 1, &set, NULL, NULL, NULL);
    if (ready == -1) {
      perror("select");
      break;
    }
    // If file descriptor is set in a bit mask - continue
    if (FD_ISSET(fd, &set)) {
      ssize_t n = read(fd, buf, MAX_BUF);
      if (n > 0) {
        buf[n] = '\0';
        printf("Recieved: %s\n", buf);
      }
    }
  }
  close(fd);
  unlink(pipe);
  exit(0);
}