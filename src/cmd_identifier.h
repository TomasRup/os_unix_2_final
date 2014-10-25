//
//      Tomas Petras Rupsys
//

#ifndef cmd_identifier_H
#define cmd_identifier_H

#include "model/shell_command.h"

/* Identifier what kind of shell command is the given input */
ShellCommand identifyCommand(char *input);

/* Parses an argument required for foreground command */
int parseFgArgs(char *rawInput);

/* Parses an argument required for background command */
int parseBgArgs(char *rawInput);

/* Parses an argument required for kill command */
int parseKillArgs(char *rawInput);

/* Parses array of strings of processes to be created newly */
char **parseNewProcessesArgs(char *rawInput);

#endif
