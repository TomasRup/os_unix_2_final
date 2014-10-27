//
//      Tomas Petras Rupsys
//

#ifndef unix_process_adapter_H
#define unix_process_adapter_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "model/app_command.h"
#include "model/process.h"
#include "model/process_state.h"
#include "cmd_identifier.h"
#include "shell.h"

/* Finds the number in array of processes by given PID */
int findByPid(pid_t givenPid);

/* Adds a new process to the list */
void addToProcesses(char *command, pid_t pid, ProcessState state);

/* Removes a process from the list of them */
void removeFromProcesses(int processNumber);

/* Handles signals */
void childSignalHandler(int signal);

/* Waits for process to be suspended */
void waitForProcess(int processNumber);

/* Executes process in given 'state'. Gets the process back from suspension if 'backFromSuspension' is 1 */
void executeProcess(ProcessState state, int processNumber, int backFromSuspension);

/* Creates a new process */
void createNewProcess(char *command, ProcessState state);

/* Moves task execution to foreground */
void doInFg(int processNumber);

/* Moves task execution to background */
void doInBg(int processNumber);

/* Prints all running jobs */
void doJobs();

/* Kills process */
void doKill(int processNumber);

/* Executes given application command in the UNIX environment */
void executeAppCommandInUnix(AppCommand appCommand, char *shellInput);

#endif
