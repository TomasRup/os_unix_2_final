//
//      Tomas Petras Rupsys
//

#ifndef shell_H
#define shell_H

#include "model/shell_command.h"

/* Prints help text about how to use the app */
void printHelp();

/* Prints introduction text */
void printIntro();

/* Exits application */
void exitShell();

/* Exits application with an error */
void exitError(const char *message);

/* Performs given command */
void performCommand(ShellCommand command, char *rawInput);

/* Starts shell. Returns exit code when completed */
void startShell();

#endif
