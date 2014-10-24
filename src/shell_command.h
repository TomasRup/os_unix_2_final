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
  CD

  // Process handling
  FOREGROUND,
  BACKGROUND,
  JOBS,
  KILL

} ShellCommand;

#endif
