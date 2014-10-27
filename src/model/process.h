//
//      Tomas Petras Rupsys
//

#ifndef process_H
#define process_H

#include <sys/types.h>

#include "process_state.h"

/* Structure which stores data for a single process */
typedef struct {

  int id;		// Job number in the list of shell tasks
  char command[1024];   // Command line which is executed in the process
  pid_t pid;		// UNIX process id
  ProcessState state;	// State of the process

} Process;

#endif
