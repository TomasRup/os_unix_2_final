//
//      Tomas Petras Rupsys
//

#ifndef shell_process_H
#define shell_process_H

#include <sys/types.h>

#include "process_regime.h"

/* Structure which stores data for a shell process */
typedef struct {

  int id;
  char command[1024];
  pid_t pid;
  pid_t pgid;
  ProcessRegime regime;

} ShellProcess;

#endif
