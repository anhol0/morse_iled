#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

#include "./iled-daemon.c"

#define MAX_LETTERS 256
#define MORSE_CODE_CHAR_LENGTH 16
#define MAX_BUF 1024

typedef struct {
  char letters[MAX_LETTERS];
  char codes[MAX_LETTERS][MORSE_CODE_CHAR_LENGTH];
  int count;
} morseCode;

morseCode fillStructure(FILE *dict) {
  morseCode code;
  code.count = 0;
  while (code.count < MAX_LETTERS &&
         fscanf(dict, "%1s %15s", &code.letters[code.count],
                code.codes[code.count]) == 2) {
    code.count++;
  }
  fclose(dict);
  return code;
}

char *getMorseCode(char *message, morseCode code) {
  int maxLen = strlen(message) * MORSE_CODE_CHAR_LENGTH + 1;
  char *morseCode = malloc(maxLen);
  if (!morseCode) {
    printf("Memory allocation error\n");
    exit(1);
  }
  morseCode[0] = '\0';
  for (int i = 0; i < strlen(message); i++) {
    for (int j = 0; j < code.count; j++) {
      if (message[i] == code.letters[j]) {
        strcat(morseCode, code.codes[j]);
        strcat(morseCode, " ");
      }
    }
  }
  printf("\n");

  return morseCode;
}

int main(int argc, char *argv[]) {
  int opts;
  char *msg;
  FILE *dict;
  morseCode myCodes;
  while ((opts = getopt(argc, argv, "ds:f:")) != -1) {
    switch (opts) {
    // Paring argument of the string
    case 'd': {
      iledDaemon();
      break;
    }
    case 's': {
      msg = optarg;
      printf("message to encode: %s\n", msg);
      break;
    }
    // Parses the dictionary file
    case 'f': {
      dict = fopen(optarg, "r");
      printf("dictionary file: %s\n", optarg);
      if (dict == NULL) {
        perror("ERROR opening dictionary file\n");
        exit(1);
      }
      break;
    }

    default:
      fprintf(stderr,
              "Usage: %s [-s] string to convert [-d] dictionary to use\n",
              argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  myCodes = fillStructure(dict);
  printf("Number of characters in a dictionary: %d\n", myCodes.count);
  char *end = getMorseCode(msg, myCodes);
  printf("Encoded message in morde code: \n%s\n", end);

  // Sending data to daemon from the client through pipe
  const char *pipe = "/tmp/pipe";
  mkfifo(pipe, 0666);
  int fd = open(pipe, O_WRONLY);
  if (fd < 0) {
    printf("Error opening PIPE!\n");
    exit(1);
  }
  flock(fd, LOCK_UN);
  write(fd, end, strlen(end));
  close(fd);
  sleep(1);
  unlink(pipe);

  free(end);
}