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
  printf("kill [job number]\t\tkills the given shell job\n");
  printf("jobs\t\t\t\tshows a list of jobs which are in the background of this shell\n");
  printf("[unix command]\t\t\texecutes any UNIX command\n");
  printf("[unix command] &\t\texecutes any UNIX command in background\n");
}

void printIntro() {

  printf("\n\n\t\tWelcome to the background tasks shell app!\n\t\t\t(c) Tomas Petras Rupsys\n\n");
  printHelp();
  printf("\n");
}

void exitShell() {

  printf("Bye.\n\n");
  exit(EXIT_SUCCESS);
}

void exitError(const char *message) {

  printf("%s\n\n", message);
  exit(EXIT_FAILURE);
}

void performCommand(AppCommand command, char *rawInput) {

  switch(command) {
    case HELP:
      printHelp();
      break;

    case EXIT:
      exitShell();
      break;

    default:
      executeAppCommandInUnix(command, rawInput);
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

void promptPrefix() {
  // Getting user name
  char userName[INPUT_SIZE];
  getlogin_r(userName, INPUT_SIZE);

  // Getting host name
  char hostName[INPUT_SIZE];
  gethostname(hostName, INPUT_SIZE);

  // Getting current directory
  char currentDirectory[INPUT_SIZE];
  getcwd(currentDirectory, INPUT_SIZE);

  // Printing prefix
  printf("%s@%s:%s> ", userName, hostName, currentDirectory);
}

void startShell() {

  initShell();
  printIntro();

  char initialInputChar = '\0';
  while (initialInputChar != EOF) {

    promptPrefix();
    initialInputChar = getchar();

    // User hits enter
    if (initialInputChar == '\n') {
      continue;

    // End of file symbol reached
    } else if (initialInputChar == EOF) {
      break;

    // User is typing a command
    } else {

      // Adding the 'initialInputChar' as the first character of the command
      int inputCounter = 0;
      char shellInput[INPUT_SIZE];
      shellInput[inputCounter++] = initialInputChar;

      // Waiting for more user input
      char tempChar;

      while (inputCounter < INPUT_SIZE) {
        tempChar = getchar();

        // User has finished typing command
        if (tempChar == '\n') {

          // Properly ending the input
          shellInput[inputCounter] = '\0';

          // Performing the command
          AppCommand appCommand = identifyCommand(shellInput);
          performCommand(appCommand, shellInput);
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
