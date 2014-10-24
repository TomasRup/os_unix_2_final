//
//      Tomas Petras Rupsys
//

#ifndef shell_H
#define shell_H

#include "model/shell_command.h"

/* Prints introduction text */
void printIntro();

/* Prints help text about how to use the app */
void printHelp();

/* Changes directory */
void doCd(char *rawInput);

/* Exits application */
void exitShell();

/* Performs given command */
void performCommand(ShellCommand command, char *rawInput);

/* Starts shell. Returns exit code when completed */
void startShell();

#endif
