//
//      Tomas Petras Rupsys
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model/shell_command.h"
#include "cmd_identifier.h"
#include "unix_process_adapter.h"

#define INPUT_SIZE 256

void printIntro() {
  printf("intro");
  // TODO
}

void printHelp() {
  printf("help");
  // TODO
}

void doCd(char *rawInput) {
  // Forming location string
  char dir[strlen(rawInput) - 3];
  for (int i=3,j=0 ; i<strlen(rawInput) ; i++,j++) { dir[j] = rawInput[i]; }

  // TODO
  printf("Going to %s", dir);
}

void exitShell() {
  printf("\n\nBye.\n");
  exit(EXIT_SUCCESS);
}

void performCommand(ShellCommand command, char *rawInput) {
  switch(command) {
    case CD:
      doCd(rawInput);
      break;
    case HELP:
    case UNKNOWN:
      printHelp();
      break;
    case EXIT:
      exitShell();
      break;
    default:
      executeShellCommand(command, rawInput);
  }
}

void startShell() {

  printIntro();
  char shellInput[INPUT_SIZE];

  while (1) {
    printf(">");

    // Reading a line
    int inputCounter = 0;
    char readChar;

    while (inputCounter < INPUT_SIZE) {
      readChar = getchar();

      // User is closing the app
      if (readChar == EOF) {
       exitShell();

      // User hits enter
      } else if (readChar == '\n') {

        if (strlen(shellInput) > 0) {
          ShellCommand shellCommand = identifyCommand(shellInput);
          performCommand(shellCommand, shellInput);
        }

        break;
      }

      // Adding the read character to the command string
      shellInput[inputCounter++] = readChar;
    }

    // Clearing the 'shellInput'
    memset(shellInput, 0, INPUT_SIZE);
  }
}
