#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#include "exec_args.h"
#include "path_funcs.h"
#include "handle_operations.h"

#define MAX_LINE 1000
#define READ_SIDE 0
#define WRITE_SIDE 1

void copy_words_from_to(char *words[], char *new_words[], int start, int end)
{
    for (int dest_ix = 0; dest_ix < end - start + 1; dest_ix++)
    {
        new_words[dest_ix] = words[start + dest_ix];
    }
    new_words[end - start + 1] = NULL;
}

void execute_command(char *words[], char *absolute_path, int background)
{
    pid_t child_pid = fork();
    if (child_pid == 0)
    {
        // check for input and output redirection
        handle_redirection(words);

        // execute_command(words, absolute_path, background);
        execve(absolute_path, words, NULL);
        printf("Command not found: %s\n", words[0]);
        perror("Error");
        exit(1);
    }
    else
    {
        if (background == 1)
        {
            waitpid(child_pid, NULL, 0);
        }
    }
    return;
}

void execute_piped_commands(char *words[], char *absolute_path, int background,
                            int index_of_pipe, int length_of_array)
{
    char *first_cmd[MAX_LINE];
    char *second_cmd[MAX_LINE];

    // copy the first command into first_cmd
    // and the second command into second_cmd
    copy_words_from_to(words, first_cmd, 0, index_of_pipe - 1);
    copy_words_from_to(words, second_cmd, index_of_pipe + 1, length_of_array);

    int pipefd[2];
    pipe(pipefd);

    int command1_pid = fork();
    if (command1_pid == 0)
    {
        check_for_input_redirect(first_cmd);

        close(pipefd[READ_SIDE]);
        dup2(pipefd[WRITE_SIDE], STDOUT_FILENO);
        close(pipefd[WRITE_SIDE]);

        execve(absolute_path, first_cmd, NULL);
        printf("Command not found: %s\n", absolute_path);
        perror("Error");
        exit(1);
    }

    int command2_pid = fork();
    if (command2_pid == 0)
    {
        check_for_output_redirect(second_cmd);

        close(pipefd[WRITE_SIDE]);
        dup2(pipefd[READ_SIDE], STDIN_FILENO);
        close(pipefd[READ_SIDE]);

        char absolute_path2[MAX_LINE];
        if (!find_absolute_path(second_cmd[0], absolute_path2))
        {
            printf("Command not found: %s\n", second_cmd[0]);
            return;
        }

        execve(absolute_path2, second_cmd, NULL);
        printf("Command not found: %s\n", absolute_path2);
        perror("Error");
        exit(1);
    }

    close(pipefd[READ_SIDE]);
    close(pipefd[WRITE_SIDE]);

    if (background == 1)
    {
        waitpid(command1_pid, NULL, 0);
        // printf("command 1 exited\n");
        waitpid(command2_pid, NULL, 0);
        // printf("command 2 exited\n");
    }
}

void process_command(char *words[], char *absolute_path, int length_of_array)
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

        // check for any environment variables in the command
        check_for_env_var(words);

        // check for & to run in background.
        // (1 = Do not run in background, 0 = Run in background)
        int background = 1;
        for (int ix = 0; words[ix] != NULL; ix++)
        {
            if (strcmp(words[ix], "&") == 0)
            {
                background = 0;
                words[ix] = NULL;
                break;
            }
        }

        // if no piping, execute the single command
        int index_of_pipe = check_for_pipe(words);
        if (index_of_pipe != -1)
        {
            execute_piped_commands(words, absolute_path, background,
                                   index_of_pipe, length_of_array);
            return;
        }
        else
        {
            execute_command(words, absolute_path, background);
        }
    }
}
