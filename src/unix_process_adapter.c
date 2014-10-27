//
//      Tomas Petras Rupsys
//

#include "unix_process_adapter.h"

#define TRUE 1
#define FALSE 0
#define PROCESSES_MAX 8

ShellProcess processes[PROCESSES_MAX];

int findByPgid(pid_t givenPgid) {

  int idInArray = -1;

  int i;
  for (i=0 ; i<processesSize ; i++) {
    if (processes[i].pgid == givenPgid) {
      idInArray = i;
      break;
    }
  }

  return idInArray;
}

void addToProcesses(char *command, pid_t pgid, ProcessRegime regime) {

  if (processesSize == PROCESSES_MAX) {
    perror("Maximum number of processes has been reached!");
  }

  int writeAt = processesSize;
  processes[writeAt].id = writeAt;
  strcpy(processes[writeAt].command, command);
  processes[writeAt].pgid = pgid;
  processes[writeAt].regime = regime;
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
  // '-1' means that we are waiting for any status
  // WNOHANG - returns immediately if no child has exited
  // WUNTRACED - returns if a child has stopped
  pid = waitpid(-1, &termStatus, WUNTRACED | WNOHANG);

  // If parent or invalid PID was received - stopping execution of this function
  if (pid <= 0) {
    return;
  }

  // We are interested only about processes which belong to our shell
  int processNo = findByPgid(pid);
  if (processNo == -1) {
    return;
  }

  // Query status to see if a child process has ended normally
  if (WIFEXITED(termStatus)) {
    if (processes[processNo].regime == BG) {
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
    tcsetpgrp(STDIN_FILENO, processes[processNo].pgid);
    processes[processNo].regime = WAITING;
    printf("\n[%d] '%s' - STOPPED\n", processes[processNo].id, processes[processNo].command);
    return;

  // Other
  } else {
    removeFromProcesses(processNo);
    return;
  }
}

void waitForProcess(int processNumber) {

  int termStatus;

  while(waitpid(processes[processNumber].pgid, &termStatus, WNOHANG) == 0) {
    if (processes[processNumber].regime == WAITING) {
      return;
    }
  }

  removeFromProcesses(processNumber);
}

void executeProcess(ProcessRegime regime, int processNumber, int backFromSuspension) {

  if (regime == WAITING) {
    perror("Unexpected regime");
  }

  processes[processNumber].regime = regime;

  if (regime == FG) {
    tcsetpgrp(STDIN_FILENO, processes[processNumber].pgid);
  }

  if (backFromSuspension == TRUE) {
    if (kill(processes[processNumber].pgid, SIGCONT) < 0) {
      perror("Failed to move process back from suspension!");
    }
  }

  if (regime == FG) {
    waitForProcess(processNumber);
  }

  tcsetpgrp(STDIN_FILENO, commonGroupId);
}

void createNewProcess(char *command, ProcessRegime regime) {

  // Forking off to a child process
  pid_t pid;
  pid = fork();

  // Handling failures
  if (pid == EAGAIN) {
    perror("Error EAGAIN!");
    return;

  } else if (pid == ENOMEM) {
    perror("Error ENOMEM!");
    return;

  // Child process state
  } else if (pid == 0) {

    // Setting signals to default behaviour
    signal(SIGINT,  SIG_DFL);	// CTRL+C
    signal(SIGQUIT, SIG_DFL);	// CTRL+'\'
    signal(SIGTSTP, SIG_DFL);	// CTRL+Z
    signal(SIGTTIN, SIG_DFL);	// ttyin

    // Adding handler for a child signal
    signal(SIGCHLD, &childSignalHandler);

    // Becomming a session leader
    setpgid(pid, pid);

    // Setting terminal foreground process group
    if (regime == FG) {
      tcsetpgrp(STDIN_FILENO, getpid());

    // Informing user about a new background process
    } else if (regime == BG) {;
      printf("\n[%d] PID - %d\n", processesSize, (int) getpid());
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
    cmdArgs = realloc(cmdArgs, sizeof(char*) * (tokenCounter + 1));
    cmdArgs[tokenCounter] = 0;

    // Executing the commmand
    if (execvp(cmdFile, cmdArgs) == -1) {
      char error[INPUT_SIZE];
      sprintf(error, "Command '%s' could not be executed", command);
      perror(error);
    }

    // Exiting the process
    exit(EXIT_SUCCESS);

  // Parent process state
  } else {

    // Setting the process group id to be as the forked one
    setpgid(pid, pid);

    // Adding this process to the list
    addToProcesses(command, pid, regime);
    int processNumber = findByPgid(pid);

    // Executing job
    executeProcess(regime, processNumber, FALSE);
  }
}

void doInFg(int processNumber) {

   if (processNumber == INVALID_INT_ARG) {
     return;

   } else if (processes[processNumber].regime == WAITING) {
     executeProcess(FG, processNumber, TRUE);

   } else {
     executeProcess(FG, processNumber, FALSE);
   }
}

void doInBg(int processNumber) {

  if (processNumber == INVALID_INT_ARG) {
    return;
  }

  executeProcess(BG, processNumber, TRUE);
}

void doJobs() {

  if (processesSize == 0) {
    printf("\nThere are no running jobs.\n");

  } else {
    printf("\n");

    int i;
    for (i=0 ; i<processesSize ; i++) {
      char *status;

      if (processes[i].regime == BG) {
        status = "background";
      } else if (processes[i].regime == FG) {
        status = "foreground";
      } else {
        status = "waiting";
      }

      printf("[%d] '%s' (%s)\n", (int) processes[i].id, processes[i].command, status);
    }
  }
}

void doKill(int processNumber) {

  if (processNumber == INVALID_INT_ARG) {
    return;

  } else if (kill(processes[processNumber].pgid, SIGKILL) == -1) {
    perror("Failed to kill the process!");
  }
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
