//
//      Tomas Petras Rupsys
//

#include "unix_process_adapter.h"

#define TRUE 1
#define FALSE 0
#define PROCESSES_MAX 8
#define PROCESS_NOT_FOUND -1

Process processes[PROCESSES_MAX];

int findByPid(pid_t givenPid) {

  int idInArray = PROCESS_NOT_FOUND;
  int i;
  for (i=0 ; i<processesSize ; i++) {
    if (processes[i].pid == givenPid) {
      idInArray = i;
      break;
    }
  }

  return idInArray;
}

void addToProcesses(char *command, pid_t pid, ProcessState state) {

  if (processesSize == PROCESSES_MAX) {
    exitError("Maximum number of processes has been reached!");
  }

  // Finding the id of list where to write the new process at
  int writeAt = processesSize;

  // Initializing parameters
  processes[writeAt].id = writeAt;
  strcpy(processes[writeAt].command, command);
  processes[writeAt].pid = pid;
  processes[writeAt].state = state;

  // Incrementing the total amount
  processesSize++;
}

void removeFromProcesses(int processNumber) {

  int removed = FALSE;

  int i;
  for (i=0 ; i<processesSize ; i++) {
    if (processes[i].id == processNumber) {
      while (i < processesSize) {
        processes[i] = processes[i+1];
        processes[i].id = processes[i].id - 1;
        i++;
      }

      removed = TRUE;
      break;
    }
  }

  if (removed == TRUE) {
    processesSize--;
  }
}

void childSignalHandler(int signal) {

  if (signal != SIGCHLD) {
    perror("Signal handler has been used not appropriately!");
  }

  pid_t pid;
  int termStatus;

  // Waits for the child process
  // '-1' in the argument of 'waitpid' means that we are waiting for any status
  // WNOHANG - returns immediately if no child has exited
  // WUNTRACED - returns if a child has stopped
  pid = waitpid(-1, &termStatus, WUNTRACED | WNOHANG);

  // If parent or invalid PID was received - ignoring later execution
  if (pid <= 0) {
    return;
  }

  // We are interested only about processes which belong to our shell, otherwise it is ignored
  int processNo = findByPid(pid);
  if (processNo == PROCESS_NOT_FOUND) {
    return;
  }

  // Query status to see if a child process has ended normally
  if (WIFEXITED(termStatus)) {
    if (processes[processNo].state == BG) {
      printf("\n[%d] '%s' - DONE\n", processes[processNo].id, processes[processNo].command);
      removeFromProcesses(processNo);
      return;
    }

  // Query status to see if a child process has ended abnormally
  } else if (WIFSIGNALED(termStatus)) {
    printf("\n[%d] '%s' - KILLED\n", processes[processNo].id, processes[processNo].command);
    removeFromProcesses(processNo);
    return;

  // Returns non zero if child process has stopped
  } else if (WIFSTOPPED(termStatus)) {
    tcsetpgrp(STDIN_FILENO, processes[processNo].pid);
    processes[processNo].state = WAITING;
    printf("\n[%d] '%s' - STOPPED\n", processes[processNo].id, processes[processNo].command);
    return;

  // In other cases, child process should also be removed
  } else {
    removeFromProcesses(processNo);
    return;
  }
}

void waitForProcess(int processNumber) {

  int termStatus;

  for (;waitpid(processes[processNumber].pid, &termStatus, WNOHANG) == 0;) {
    if (processes[processNumber].state == WAITING) {
      return;
    }
  }

  removeFromProcesses(processNumber);
}

void executeProcess(ProcessState state, int processNumber, int backFromSuspension) {

  if (state == WAITING) {
    exitError("Unexpected state");
  }

  // Changing the state of the process
  processes[processNumber].state = state;

  // Making the process to be foreground process
  if (state == FG) {
    tcsetpgrp(STDIN_FILENO, processes[processNumber].pid);
  }

  // In case we are getting process back from suspension, 'SIGCONT'
  // i.e. continue signal is sent
  if (backFromSuspension == TRUE) {
    if (kill(processes[processNumber].pid, SIGCONT) < 0) {
      perror("Failed to move process back from suspension!");
    }
  }

  // If the process is executed in foreground, the parent (shell) process
  // must wait for it to complete
  if (state == FG) {
    waitForProcess(processNumber);
  }

  // Making the shell process (set as 'commonGroupId') to be the one in the foreground again
  tcsetpgrp(STDIN_FILENO, commonGroupId);
}

void createNewProcess(char *command, ProcessState state) {

  if (strcmp(command, INVALID_STR_ARG) == 0) {
    return;
  }

  // Forking off to a child process
  pid_t pid;
  pid = fork();

  // If could not allocate enough memory to copy parent's page
  if (pid == EAGAIN) {
    perror("Error EAGAIN!");
    return;

  // If could not allocate necessary kernel structures due to tight memory
  } else if (pid == ENOMEM) {
    perror("Error ENOMEM!");
    return;

  // If we are in the child process state
  } else if (pid == 0) {

    // Setting signals to default behaviour
    signal(SIGINT,  SIG_DFL);	// CTRL+C
    signal(SIGQUIT, SIG_DFL);	// CTRL+'\'
    signal(SIGTSTP, SIG_DFL);	// CTRL+Z
    signal(SIGTTIN, SIG_DFL);	// ttyin

    // Adding handler for a child signal
    signal(SIGCHLD, &childSignalHandler);

    // Joining an existing process group (or creating a new one)
    setpgid(pid, pid);

    // If foreground process is executed, moving the current process to the foreground of std in
    if (state == FG) {
      tcsetpgrp(STDIN_FILENO, getpid());

    // Informing user about a fresh background process
    } else if (state == BG) {;
      printf("\n[%d] %d\n", processesSize, (int) getpid());
    }

    // Forming the command string with it's arguments
    char *token = strtok(command, " ");

    char cmdFile[strlen(token)];
    strcpy(cmdFile, token);

    char **cmdArgs = NULL;
    int tokenCounter = 0;

    // Forming all arguments
    while (token) {
      cmdArgs = realloc(cmdArgs, sizeof(char*) * (tokenCounter + 1));
      cmdArgs[tokenCounter++] = token;
      token = strtok(NULL, " ");
    }

    // The ending of args must be NULL
    // requirement from 'http://linux.die.net/man/3/execvp'
    cmdArgs = realloc(cmdArgs, sizeof(char*) * (tokenCounter + 1));
    cmdArgs[tokenCounter] = 0;

    // Executing the commmand
    if (execvp(cmdFile, cmdArgs) == -1) {
      char error[INPUT_SIZE];
      sprintf(error, "Command '%s' could not be executed", command);
      perror(error);
      printf("\n");
    }

    // Exiting the child process
    exit(EXIT_SUCCESS);

  // Parent process state
  } else {

    // Creating a new process group (or joining an existing one)
    setpgid(pid, pid);

    // Adding the new process to the list of shell processes
    addToProcesses(command, pid, state);
    int processNumber = findByPid(pid);

    // Executing the job in provided 'state'
    executeProcess(state, processNumber, FALSE);
  }
}

void doInFg(int processNumber) {

   if (processNumber == INVALID_INT_ARG) {
     return;

   // Getting the process back from 'stopped' state
   } else if (processes[processNumber].state == WAITING) {
     executeProcess(FG, processNumber, TRUE);

   // Simply moving the process to the foreground
   } else {
     executeProcess(FG, processNumber, FALSE);
   }
}

void doInBg(int processNumber) {

  if (processNumber == INVALID_INT_ARG) {
    return;
  }

  // Moving process to background and to continue execution in there
  executeProcess(BG, processNumber, TRUE);
}

void doJobs() {

  if (processesSize == 0) {
    printf("There are no running jobs.\n\n");

  // Printing current jobs
  } else {
    int i;
    for (i=0 ; i<processesSize ; i++) {
      char *status;

      if (processes[i].state == BG
          || processes[i].state == FG) {
        status = "Running";
      } else {
        status = "Stopped";
      }

      printf("[%d] %s\t\t%s\n", (int) processes[i].id, status, processes[i].command);
    }
    printf("\n");
  }
}

void doKill(int processNumber) {

  if (processNumber == INVALID_INT_ARG) {
    return;

  // Sending kill signal to the given process
  } else if (kill(processes[processNumber].pid, SIGKILL) == -1) {
    perror("Failed to kill the process!");
  }
}

void executeAppCommandInUnix(AppCommand appCommand, char *shellInput) {

  // Checking which app command should be executed in the adapter
  switch(appCommand) {
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
    case NEW_BG:
      createNewProcess(parseNewBgProcessesArgs(shellInput), BG);
      break;
    case NEW_FG:
      createNewProcess(parseNewFgProcessesArgs(shellInput), FG);
      break;
    default:
      exitError("Invalid command in UNIX PROCESS ADAPTER!");
  }
}
