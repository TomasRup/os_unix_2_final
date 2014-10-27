//
//      Tomas Petras Rupsys
//

#ifndef shell_H
#define shell_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "model/app_command.h"
#include "app.h"
#include "cmd_identifier.h"
#include "unix_process_adapter.h"

/* Defines the size of the processes */
extern int processesSize;

/* Defines the group PID of the shell */
extern pid_t commonGroupId;

/* Prints help text about how to use the app */
void printHelp();

/* Prints introduction text */
void printIntro();

/* Exits application */
void exitShell();

/* Exits application with an error */
void exitError(const char *message);

/* Performs given command */
void performCommand(AppCommand command, char *rawInput);

/* Initializes all of the settings for shell */
void initShell();

/* Prints a sign before each line */
void promptPrefix();

/* Starts shell. Returns exit code when completed */
void startShell();

#endif
