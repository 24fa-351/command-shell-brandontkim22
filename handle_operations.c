#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "handle_operations.h"

#define ADD_OR_OVERWRITE_ENV_VAR 1
#define ADD_BUT_DO_NOT_OVERWRITE_VAR 0
#define MAX_LINE 1000

void handle_cd(char *words[])
{
    char cwd[MAX_LINE];
    chdir(words[1]);
}

void handle_pwd(char *words[])
{
    char cwd[MAX_LINE];
    printf("%s\n", getcwd(cwd, MAX_LINE));
}

void handle_set(char *words[])
{
    if (words[1] == NULL || words[2] == NULL)
    {
        printf("Invalid number of arguments\n");
        return;
    }

    if (getenv(words[1]) != NULL)
    {
        // printf("Overwriting existing environment variable %s with %s\n",
        //        words[1], words[2]);
        setenv(words[1], words[2], ADD_OR_OVERWRITE_ENV_VAR);
        return;
    }
    else
    {
        // printf("Setting new environment variable %s with %s\n",
        //        words[1], words[2]);
        setenv(words[1], words[2], ADD_BUT_DO_NOT_OVERWRITE_VAR);
    }
}

void handle_unset(char *words[])
{
    if (words[1] == NULL)
    {
        printf("Invalid number of arguments\n");
        return;
    }

    if (getenv(words[1]) == NULL)
    {
        printf("Environment variable %s does not exist\n", words[1]);
        return;
    }

    // printf("Unsetting environment variable %s\n", words[1]);
    unsetenv(words[1]);
    // printf("Removed environment variable %s\n", words[1]);

    return;
}

void handle_redirection(char *words[])
{
    for (int ix = 0; words[ix] != NULL; ix++)
    {
        if (strcmp(words[ix], "<") == 0)
        {
            int input_fd = open(words[ix + 1], O_RDONLY);
            if (input_fd == -1)
            {
                printf("Error opening file %s\n", words[ix + 1]);
                return;
            }

            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
            words[ix] = NULL;
            continue;
        }
        if (strcmp(words[ix], ">") == 0)
        {
            int output_fd = open(words[ix + 1],
                                 O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (output_fd == -1)
            {
                printf("Error opening file %s\n", words[ix + 1]);
                return;
            }

            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
            words[ix] = NULL;
            break;
        }
    }
    return;
}

void get_env_var(char *env_var, char *value_of_env_var)
{
    if (getenv(env_var) == NULL)
    {
        printf("Environment variable %s does not exist\n", env_var);
        strcpy(value_of_env_var, "");
        return;
    }

    strcpy(value_of_env_var, getenv(env_var));
}

void check_for_env_var(char *words[])
{
    int first_char_of_word = 0;
    for (int ix = 0; words[ix] != NULL; ix++)
    {
        if (words[ix][first_char_of_word] == '$')
        {
            char env_var[MAX_LINE];
            char value_of_env_var[MAX_LINE];

            strcpy(env_var, words[ix] + 1);
            get_env_var(env_var, value_of_env_var);
            strcpy(words[ix], value_of_env_var);
        }
    }
}

int check_for_pipe(char *words[])
{
    int index_of_pipe = -1;
    for (int ix = 0; words[ix] != NULL; ix++)
    {
        if (strcmp(words[ix], "|") == 0)
        {
            return index_of_pipe = ix;
        }
    }
    return index_of_pipe;
}

void check_for_input_redirect(char *words[])
{
    int input_file_index = -1;

    for (int ix = 0; words[ix] != NULL; ix++)
    {
        if (strcmp(words[ix], "<") == 0)
        {
            input_file_index = ix + 1;

            int input_fd = open(words[input_file_index], O_RDONLY);
            if (input_fd == -1)
            {
                printf("Error opening file %s\n", words[input_file_index]);
                return;
            }

            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
            words[ix] = NULL;
            break;
        }
    }
}

void check_for_output_redirect(char *words[])
{
    int output_file_index = -1;

    for (int ix = 0; words[ix] != NULL; ix++)
    {
        if (strcmp(words[ix], ">") == 0)
        {
            int output_fd = open(words[ix + 1],
                                 O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (output_fd == -1)
            {
                printf("Error opening file %s\n", words[ix + 1]);
                return;
            }

            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
            words[ix] = NULL;
            break;
        }
    }
}