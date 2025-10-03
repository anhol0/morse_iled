#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#define MAX_LETTERS 256
#define MORSE_CODE_CHAR_LENGTH 16

typedef struct {
  char letters[MAX_LETTERS];
  char codes[MAX_LETTERS][MORSE_CODE_CHAR_LENGTH];
  int count;
} morseCode;

morseCode fillStructure(FILE* dict) {
  morseCode code;
  code.count = 0;
  while(code.count < MAX_LETTERS && 
        fscanf(dict, "%1s %15s", &code.letters[code.count], code.codes[code.count]) == 2) {
    code.count++;
  }
  fclose(dict);
  return code;
}

char* getMorseCode(char* message, morseCode code) {
  int maxLen = strlen(message) * MORSE_CODE_CHAR_LENGTH + 1;
  char *morseCode = malloc(maxLen);
  if(!morseCode) {
    printf("Memory allocation error\n");
    exit(1);
  }
  morseCode[0] = '\0';
  for(int i = 0; i < strlen(message); i++) {
    for(int j = 0; j < code.count; j++) {
      if(message[i] == code.letters[j]) {
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
  FILE* dict;
  morseCode myCodes;
  while ((opts = getopt(argc, argv, "s:d:")) != -1) {
    switch (opts) {
    // Paring argument of the string
    case 's': {
      msg = optarg;
      printf("message to encode: %s\n", msg);
      break;
    }
    // Parses the dictionary file
    case 'd': {
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
  printf("Number of characters in a dictionary: %d\nDictionary: \n", myCodes.count);
  for(int i = 0; i < myCodes.count; i++) {
    printf("%x -> %s\n", myCodes.letters[i], myCodes.codes[i]);
  }
  char *end = getMorseCode(msg, myCodes);
  printf("Encoded message in morde code: \n%s\n", end);
  free(end);
}