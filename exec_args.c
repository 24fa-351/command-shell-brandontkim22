#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#include "exec_args.h"
#include "path_funcs.h"

#define ADD_OR_OVERWRITE_ENV_VAR 1
#define ADD_BUT_DO_NOT_OVERWRITE_VAR 0
#define MAX_LINE 1000

void handle_cd(char *words[])
{
    char cwd[MAX_LINE];
    chdir(words[1]);
    printf("Changed directory to %s\n", getcwd(cwd, MAX_LINE));
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
        printf("Overwriting existing environment variable %s with %s\n",
               words[1], words[2]);
        setenv(words[1], words[2], ADD_OR_OVERWRITE_ENV_VAR);
        return;
    }
    else
    {
        printf("Setting new environment variable %s with %s\n",
               words[1], words[2]);
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

    printf("Unsetting environment variable %s\n", words[1]);
    unsetenv(words[1]);
    printf("Removed environment variable %s\n", words[1]);

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
            char env_var[MAX_LINE]; // might change MAX_LINE to new name
            char value_of_env_var[MAX_LINE];

            strcpy(env_var, words[ix] + 1);
            get_env_var(env_var, value_of_env_var);
            strcpy(words[ix], value_of_env_var);
        }
    }
}

void process_command(char *words[], char *absolute_path)
{
    if (strcmp(words[0], "cd") == 0) // test cd
    {
        handle_cd(words);
        return;
    }
    else if (strcmp(words[0], "pwd") == 0) // test pwd
    {
        handle_pwd(words);
        return;
    }
    else if (strcmp(words[0], "set") == 0) // test set
    {
        handle_set(words);
        return;
    }
    else if (strcmp(words[0], "unset") == 0) // test unset
    {
        handle_unset(words);
        return;
    }
    else // test other commands
    {
        // find the absolute path of the command
        if (!find_absolute_path(words[0], absolute_path))
        {
            printf("Command not found: %s\n", words[0]);
            return;
        }
        printf("absolute_path = %s\n", absolute_path);

        // check for any environment variables in the command
        check_for_env_var(words);

        pid_t pid = fork();
        if (pid == 0)
        {
            // check for input and output redirection
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
                    printf("Redirecting output to file %s\n", words[ix + 1]);
                    int output_fd = open(words[ix + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
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

            execve(absolute_path, words, NULL);
            printf("Command not found: %s\n", words[0]);
            perror("Error");
            exit(1);
        }
        else
        {
            wait(NULL);
        }

        return;
    }
}

void execute_piped_commands(char *words[], char *absolute_path)
{
    printf("Piping not implemented yet\n");
    return;
}
