//
//      Tomas Petras Rupsys
//

#include "shell.h"

int processesSize;
pid_t commonGroupId;

void printHelp() {

  printf("\nCOMMAND\t\t\t\tDESCRIPTION\n");
  printf("help\t\t\t\tprints this information\n");
  printf("exit\t\t\t\texit from the application\n");
  printf("fg [job number]\t\t\tmoves the given job to execute in shell foreground\n");
  printf("bg [job number]\t\t\tmoves the given job to execute in shell background\n");
  printf("jobs\t\t\t\tshows a list of jobs which are in the background of this shell\n");
  printf("[unix command]\t\t\texecutes any UNIX command\n");
  printf("[unix command] &\t\texecutes any UNIX command in background\n");
}

void printIntro() {

  printf("\n\nWelcome to background tasks shell!\n(c) Tomas Petras Rupsys\n");
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
      printHelp();
      break;
    case EXIT:
      exitShell();
      break;
    default:
      executeShellCommand(command, rawInput);
  }
}

void initShell() {

  // Nulling the size of processes created
  processesSize = 0;

  // Setting a single group ID to be used for all shell processes
  commonGroupId = getpgrp();

  // Initializes signal parameters to ignore actions in comments
  signal(SIGTTOU, SIG_IGN);	// ttyout
  signal(SIGTTIN, SIG_IGN);	// ttyin
  signal(SIGTSTP, SIG_IGN);	// CTRL+Z
  signal(SIGINT,  SIG_IGN);	// CTRL+C

  // Adding a handler for child signals
  signal(SIGCHLD, &childSignalHandler);
}

void startShell() {

  initShell();
  printIntro();

  char initialInputChar = '\0';
  while (initialInputChar != EOF) {

    printf(">> ");
    initialInputChar = getchar();

    // User hits enter
    if (initialInputChar == '\n') {
      continue;

    // End of file symbol reached
    } else if (initialInputChar == EOF) {
      break;

    // User is typing a command
    } else {
      int inputCounter = 0;
      char shellInput[INPUT_SIZE];
      shellInput[inputCounter++] = initialInputChar;

      char tempChar;
      while (inputCounter < INPUT_SIZE) {
        tempChar = getchar();

        // User has finished typing command
        if (tempChar == '\n') {
          // Properly ending the input
          shellInput[inputCounter] = '\0';

          // Performing the command
          ShellCommand shellCommand = identifyCommand(shellInput);
          performCommand(shellCommand, shellInput);
          break;

        // Forming command
        } else {
          shellInput[inputCounter++] = tempChar;
        }
      }

      // Freeing memory for the inputted string
      memset(shellInput, 0, INPUT_SIZE);
    }
  }
}
