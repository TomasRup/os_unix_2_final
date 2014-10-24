//
//      Tomas Petras Rupsys
//

#ifndef cmd_identifier_H
#define cmd_identifier_H

#include "model/shell_command.h"

/* Identifier what kind of shell command is the given input */
ShellCommand identifyCommand(char *input);

#endif
