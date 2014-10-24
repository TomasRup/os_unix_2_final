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
  CD,
  UNKNOWN,

  // Process handling
  FOREGROUND,
  BACKGROUND,
  JOBS,
  KILL,
  NEW

} ShellCommand;

#endif
