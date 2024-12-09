#include <stdbool.h>

#ifndef PATH_FUNCS_H
#define PATH_FUNCS_H

void add_character_to_string(char *string, char character);

void split_line(char *line, char *words[], char delimiter);

bool find_absolute_path(char *command, char *absolute_path);

int check_for_redir_input(char *words[]);

void check_for_redir_output(char *words[]);

#endif