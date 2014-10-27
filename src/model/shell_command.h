//
//      Tomas Petras Rupsys
//

#ifndef shell_command_H
#define shell_command_H

/* Enum which defines which shell command is being executed */
typedef enum {

  // Generic commands
  EXIT,
  HELP,

  // Process handling
  FOREGROUND,
  BACKGROUND,
  JOBS,
  KILL,
  NEW_BG,
  NEW_FG

} ShellCommand;

#endif
