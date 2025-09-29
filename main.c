#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void i_blink(int state, int delay) {
  if (state != 1 && state != 0) {
    perror("State can be only 0 and 1\n");
    exit(1);
  }
  for (int i = 0; i < 3; i++) {
    FILE *fd = fopen("/sys/class/leds/tpacpi::lid_logo_dot/brightness", "w");
    if (fd == NULL) {
      printf("Error opening file. Run program with sudo or check system "
             "configuration\n");
      exit(1);
    }
    fprintf(fd, "%d", (i == 1) ? (state) : (!state));
    fclose(fd);
    i == 0 ? usleep(500000) : usleep(50000);
  }
  usleep(delay * 1000);
}

int main(int argc, char *argv[]) {
  int opts;
  while ((opts = getopt(argc, argv, ":s:d:")) != -1) {
    switch (opts) {
    case 's': {
      char *msg = optarg;
      printf("message to encode: %s\n", msg);
      break;
    }
    case 'd': {
      FILE *dict = fopen(optarg, "r");
      printf("dictionary file: %s\n", optarg);
      if (dict == NULL) {
        perror("ERROR opening dictionary file\n");
        exit(1);
      }
      int c;
      printf("New character is being read\n");
      while ((c = fgetc(dict)) != EOF) {
        if (c == '\n') {
          printf("\nNew character is being read");
        }
        printf("%c", c);
      }
      fclose(dict);
      break;
    }
    default:
      fprintf(stderr,
              "Usage: %s [-s] string to convert [-d] dictionary to use\n",
              argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  i_blink(0, 1500);
  i_blink(0, 1500);
}