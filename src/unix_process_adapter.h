//
//      Tomas Petras Rupsys
//

#ifndef unix_process_adapter_H
#define unix_process_adapter_H

#include "model/shell_command.h"

/* Creates new processes */
void doNew(char **commands);

/* Moves task execution to foreground */
void doInFg(int number);

/* Moves task execution to background */
void doInBg(int number);

/* Prints all running jobs */
void doJobs();

/* Kills process */
void doKill(int number);

/* Executes given shell command in the UNIX environment */
void executeShellCommand(ShellCommand shellCommand, char *shellInput);

#endif
