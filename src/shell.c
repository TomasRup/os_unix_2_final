//
//      Tomas Petras Rupsys
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model/shell_command.h"
#include "app.h"
#include "cmd_identifier.h"
#include "unix_process_adapter.h"

void printHelp() {

  printf("\nAvailable general commands: [help | exit]\n");
  printf("Available process handling commands: [fg | bg | kill | jobs | \".. & .. &\"]\n");
}

void printIntro() {

  printf("\n\nWelcome to background tasks shell!\n");
  printHelp();
  printf("\n");
}

void exitShell() {

  printf("\n\nBye.\n");
  exit(EXIT_SUCCESS);
}

void exitError(const char *message) {

  perror(message);
  exit(EXIT_FAILURE);
}

void performCommand(ShellCommand command, char *rawInput) {

  switch(command) {
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
          // Properly ending the input string
          shellInput[inputCounter] = '\0';

          // Performing the command
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
