//
//      Tomas Petras Rupsys
//

#ifndef app_command_H
#define app_command_H

/* Enum which defines command of this shell that is being executed */
typedef enum {

  // Generic commands
  EXIT,			// 'exit'
  HELP,			// 'help'

  // Process handling
  FOREGROUND,		// 'fg [job number]'
  BACKGROUND,		// 'bg [job number]'
  JOBS,			// 'jobs'
  KILL,			// 'kill [job number]'
  NEW_BG,		// '[unix command] &'
  NEW_FG		// '[unix command]'

} AppCommand;

#endif
