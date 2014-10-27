//
//      Tomas Petras Rupsys
//

#ifndef process_state_H
#define process_state_H

/* Enum which defines state of the process */
typedef enum {

  BG,		// Background
  FG,		// Foreground
  WAITING	// Waiting (suspended)

} ProcessState;

#endif
