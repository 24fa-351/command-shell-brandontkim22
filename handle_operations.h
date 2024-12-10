

#ifndef HANDLE_OPERATIONS_H
#define HANDLE_OPERATIONS_H

void handle_cd(char *words[]);

void handle_pwd(char *words[]);

void handle_set(char *words[]);

void handle_unset(char *words[]);

void handle_redirection(char *words[]);

void get_env_var(char *env_var, char *value_of_env_var);

void check_for_env_var(char *words[]);

int check_for_pipe(char *words[]);

void check_for_input_redirect(char *words[]);

void check_for_output_redirect(char *words[]);

#endif