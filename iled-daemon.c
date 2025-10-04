#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/file.h>

#define MAX_BUF 1024

#define CONTROL_LED "/sys/class/leds/tpacpi::lid_logo_dot/brightness"

void iledDaemon() {
  int fd;
  char *myfifo = "/tmp/pipe";
  char buf[MAX_BUF];

  mkfifo(myfifo, 0666);

  while (1) {
    fd = open(myfifo, O_RDONLY);
    if (fd < 0) {
        // perror("open");
        continue;
    }

    while (1) {
      fd_set set;
      FD_ZERO(&set);
      FD_SET(fd, &set);
      ssize_t n;

      if (select(fd + 1, &set, NULL, NULL, NULL)) {
        n = read(fd, buf, MAX_BUF);
        if (n > 0) {
          buf[n] = '\0';
          flock(fd, LOCK_SH);
          printf("Recieved: %s\n", buf);
          close(fd);
        } else {
          close(fd);
          break;
        }
      }
    }
  }

  exit(0);
}