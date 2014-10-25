//
//      Tomas Petras Rupsys
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model/shell_command.h"
#include "app.h"
#include "shell.h"

ShellCommand identifyCommand(char *input) {

  if (strcmp(input, "exit") == 0) {
    return EXIT;
  } else if (strcmp(input, "help") == 0) {
    return HELP;
  } else if (strncmp(input, "fg", 2) == 0 && strlen(input) > 3) {
    return FOREGROUND;
  } else if (strncmp(input, "bg", 2) == 0 && strlen(input) > 3) {
    return BACKGROUND;
  } else if (strcmp(input, "jobs") == 0) {
    return JOBS;
  } else if (strncmp(input, "kill", 4) == 0 && strlen(input) > 5) {
    return KILL;
  } else if (strcmp(input, " &") > 0 && strlen(input) > 2) {
    return NEW;
  } else {
    return UNKNOWN;
  }
}

int parseFgArgs(char *rawInput) {

  char *arg = (char *)calloc(strlen(rawInput) - 3, sizeof(char));
  for (int i=3,j=0 ; i<strlen(rawInput) ; i++,j++) { arg[j] = rawInput[i]; }
  return atoi(arg);
}

int parseBgArgs(char *rawInput) {

  char *arg = (char *)calloc(strlen(rawInput) - 3, sizeof(char));
  for (int i=3,j=0 ; i<strlen(rawInput) ; i++,j++) { arg[j] = rawInput[i]; }
  return atoi(arg);
}

int parseKillArgs(char *rawInput) {

  char *arg = (char *)calloc(strlen(rawInput) - 5, sizeof(char));
  for (int i=5,j=0 ; i<strlen(rawInput) ; i++,j++) { arg[j] = rawInput[i]; }
  return atoi(arg);
}

char **parseNewProcessesArgs(char *rawInput) {

  // Counting occurrences of '&' sign
  int occ = 0;
  for (int j=0 ; j<strlen(rawInput) ; j++) { if (rawInput[j] == '&') { occ++; }}

  // Forming an array of strings
  char **processes = malloc(sizeof(char *) * occ * INPUT_SIZE);

  int i = 0;
  char *tempString = strtok(rawInput, " &");

  while (tempString != NULL) {
    // FIXME: improve memory allocation to processes
    if (i == sizeof(processes)) {
      exitError("Memory allocation maximum has been reached for commands!");
    }

    processes[i] = malloc(strlen(tempString));
    strcpy(processes[i++], tempString);
    tempString = strtok(NULL, " &");
  }

  // Returning an array of strings with commands only
  return processes;
}
