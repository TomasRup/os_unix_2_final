//
//      Tomas Petras Rupsys
//

#include <string.h>
#include "model/shell_command.h"

ShellCommand identifyCommand(char *input) {
  if (strcmp(input, "exit") == 0) {
    return EXIT;
  } else if (strcmp(input, "help") == 0) {
    return HELP;
  } else if (strcmp(input, "fg") == 0) {
    return FOREGROUND;
  } else if (strcmp(input, "bg") == 0) {
    return BACKGROUND;
  } else if (strcmp(input, "jobs") == 0) {
    return JOBS;
  } else if (strcmp(input, "kill") == 0) {
    return KILL;
  } else if (strlen(input) > 3
      && input[0] == 'c'
      && input[1] == 'd') {
    return CD;
  } else if (input[strlen(input) - 1] == '&') {
    return NEW;
  } else {
    return UNKNOWN;
  }
}
