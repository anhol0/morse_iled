#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define TIME_UNIT                                                              \
  200000 // Time unit is the value of dot in morse code. Dash is three time
         // units
#define MAX_BUF 1024
#define CONTROL_LED "/sys/class/leds/tpacpi::lid_logo_dot/brightness"
#define PIPE_NAME "/tmp/pipe"
#define LOCK_FILE "/var/run/iled_daemon.lock"

// Keeping only one instance of the daemon with flock
int isLocked(char *lockFile) {
  int lockFd = open(lockFile, O_RDWR | O_CREAT, 0644);
  // Checking is lock file creation/opening was successful
  if (lockFd == -1) {
    perror("Open lock file");
    exit(1);
  }
  // Trying to acquire lock file. If failed - return 1 to tell that
  // client/daemon is already running
  if (flock(lockFd, LOCK_EX | LOCK_NB) == -1) {
    if (errno == EWOULDBLOCK) {
      close(lockFd);
      return (1);
    }
    perror("flock");
    exit(1);
  }
  // Write PID of the daemon/client to the lock file
  if (ftruncate(lockFd, 0) == -1) {
    perror("ftruncate");
    close(lockFd);
    exit(1);
  }
  dprintf(lockFd, "%d\n", getpid());
  return 0;
}

// Toggle the led according to input given
// Time to keep led on, state of the led
void toggleTheLed(int timing, char *state) {
  int ledFd = open(CONTROL_LED, O_WRONLY);
  if (ledFd == -1) {
    perror("Open LED");
    exit(1);
  }
  write(ledFd, state, 1);
  close(ledFd);
  usleep(timing);
}

// Blink the Led on the back of the laptop
// according to the pattern provided by user input,
// encoded in morse code
void morseTextToLedBlink(char buffer[], int lengthOfBuffer) {
  toggleTheLed(TIME_UNIT, "0");
  for (int i = 0; i < lengthOfBuffer; i++) {
    switch (buffer[i]) {
      // Timing for dot is one TIME UNIT
    case '.': {
      if (buffer[i + 1] == '_' || buffer[i + 1] == ' ') {
        toggleTheLed(TIME_UNIT, "1");
        break;
      }
      toggleTheLed(TIME_UNIT, "1");
      toggleTheLed(TIME_UNIT, "0");
      break;
    }
      // Timing for dot is three TIME UNITS
    case '-': {
      if (buffer[i + 1] == '_' || buffer[i + 1] == ' ') {
        toggleTheLed(3 * TIME_UNIT, "1");
        break;
      }
      toggleTheLed(3 * TIME_UNIT, "1");
      toggleTheLed(TIME_UNIT, "0");
      break;
    }

      // Timing between letters is 3 TIME UNITS
    case ' ': {
      toggleTheLed(3 * TIME_UNIT, "0");
      break;
    }

      // Timing between words is seven TIME UNITS
    case '_': {
      toggleTheLed(7 * TIME_UNIT, "0");
      break;
    }

    default:
      break;
    }
  }
  toggleTheLed(0, "1");
}

void readBlkAndBlink(int fd, char buf[], char *pipe) {
  ssize_t n;
  while ((n = read(fd, buf, MAX_BUF)) > 0) {
    buf[n] = '\0';
    printf("Number of bytes read: %zd\n", n);
    printf("Received: '%s'\n", buf);
    morseTextToLedBlink(buf, strlen(buf));
  }
}

void killTheDaemon(char *pipe) {
  int lockFileFd = open(LOCK_FILE, O_RDONLY);
  char pidBuf[6];
  if (lockFileFd == -1) {
    perror("Opening lock file");
    exit(1);
  }
  int bytesRead = read(lockFileFd, pidBuf, sizeof(pidBuf));
  if (bytesRead == -1) {
    perror("Getting PID");
    exit(1);
  }
  printf("Killing daemon with PID of %s\n", pidBuf);
  int killSig = kill(atoi(pidBuf), SIGTERM);
  if(killSig == -1) {
    printf("Error killing process: %d\n", errno);
  }
  unlink(pipe);
  remove(LOCK_FILE);
  exit(0);
}

void iledDaemon() {
  // file descriptor, pipe name and character buffer initialization
  if (isLocked(LOCK_FILE)) {
    printf("Daemon is already running!\n");
    exit(1);
  }
  printf("Daemon started\n");
  int fd;
  char *pipe = PIPE_NAME;
  char morseCodeBuffer[MAX_BUF];

  unlink(pipe);
  // Creating named pipe with prw-rw-rw- privileges
  int umaskOld = umask(0);
  mkfifo(pipe, 0666);
  umask(umaskOld);
  // assign value to file descriptor and check if it can be opened
  fd = open(pipe, O_RDONLY | O_NONBLOCK);
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
      readBlkAndBlink(fd, morseCodeBuffer, pipe);
      close(fd);
      fd = open(pipe, O_RDONLY | O_NONBLOCK);
      if (fd < 0) {
        perror("reopen");
        exit(1);
      }
    }
  }
  close(fd);
  unlink(pipe);
  exit(0);
}