#include "./iled-daemon.c"

#define MAX_LETTERS 256
#define MORSE_CODE_CHAR_LENGTH 16
#define MAX_BUF 1024
#define LOCK_CLIENT "/tmp/iled_client.lock"

// Morse code structure
typedef struct {
  char letters[MAX_LETTERS];
  char codes[MAX_LETTERS][MORSE_CODE_CHAR_LENGTH];
  int count;
} morseCode;

// Fill morse code structure with letters and codes from given dictionary
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

// Convert input string to the morse code
char *getMorseCode(char *message, morseCode code) {
  int maxLen = strlen(message) * MORSE_CODE_CHAR_LENGTH + 1;
  char *morseCode = malloc(maxLen);
  if (!morseCode) {
    printf("Memory allocation error\n");
    exit(1);
  }

  morseCode[0] = '\0';
  for (int i = 0; i < strlen(message); i++) {
    if (message[i] == ' ') {
      strcat(morseCode, "_ ");
    }
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

// Sending the message through named pipe
// And checking whether file lock is on or off
void sendMessageThroughPipe(const char *pipeName, char *data) {
  if (isLocked(LOCK_CLIENT)) {
    printf("One client instance already exists!\n");
    exit(1);
  }
  // mkfifo(pipeName, 0666);
  int fd = open(pipeName, O_WRONLY);
  if (fd < 0) {
    printf("Error opening PIPE! %d\n", errno);
    exit(1);
  }
  write(fd, data, strlen(data));
  close(fd);
  unlink(LOCK_CLIENT);
  free(data);
}

int main(int argc, char *argv[]) {
  int opts;
  char *msg;
  FILE *dict;
  morseCode myCodes;
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [-s] string to convert [-d] dictionary to use\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }
  while ((opts = getopt(argc, argv, "ds:f:")) != -1) {
    switch (opts) {
    // Paring argument of the string
    case 'd': {
      iledDaemon();
      break;
    }
    // Parsing string to convert to the morse code
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

    // If incorrect argument is provided
    default:
      fprintf(stderr,
              "Usage: %s [-s] string to convert [-d] dictionary to use\n",
              argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  // Generating morse code
  myCodes = fillStructure(dict);
  printf("Number of characters in a dictionary: %d\n", myCodes.count);
  char *data = getMorseCode(msg, myCodes);
  printf("Encoded message in morde code: \n%s\n", data);

  // Sending data to daemon from the client through pipe
  const char *pipe = "/tmp/pipe";
  sendMessageThroughPipe(pipe, data);
  return 0;
}