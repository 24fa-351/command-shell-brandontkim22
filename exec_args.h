#include <stdbool.h>

#ifndef EXEC_ARGS_H
#define EXEC_ARGS_H

void process_command(char *words[], char *absolute_path);

void execute_piped_commands(char *words[], char *absolute_path);

#endif
