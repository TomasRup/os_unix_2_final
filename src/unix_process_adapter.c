//
//      Tomas Petras Rupsys
//

#include <stdio.h>

#include "model/shell_command.h"
#include "cmd_identifier.h"
#include "shell.h"

void doNew(char **commands) {

  // TODO
  for (int i=0 
      ; (i < sizeof(commands)) && (commands[i] != NULL) 
      ; i++) {
    printf(":%s:\n", commands[i]);
  }
}

void doInFg(int number) {

   // TODO
   printf("fg%d", number);
}

void doInBg(int number) {

  // TODO
  printf("bg%d", number);
}

void doJobs() {

  // TODO
  printf("JOBS");
}

void doKill(int number) {

  // TODO
  printf("kill%d", number);
}

void executeShellCommand(ShellCommand shellCommand, char *shellInput) {

  switch(shellCommand) {
    case FOREGROUND:
      doInFg(parseFgArgs(shellInput));
      break;
    case BACKGROUND:
      doInBg(parseBgArgs(shellInput));
      break;
    case JOBS:
      doJobs();
      break;
    case KILL:
      doKill(parseKillArgs(shellInput));
      break;
    case NEW:
      doNew(parseNewProcessesArgs(shellInput));
      break;
    default:
      exitError("Unidentified shell command was attempted to be started in UNIX!");
  }
}
