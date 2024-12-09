#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#include "exec_args.h"
#include "path_funcs.h"

#define MAX_LINE 80

int main(int argc, char *argv[])
{
    char line[MAX_LINE];
    char absolute_path[1000];
    char *words[1000];

    while (1)
    {
        printf("my_shell> ");
        fgets(line, MAX_LINE, stdin);
        line[strlen(line) - 1] = '\0'; // remove newline character

        // check if the user entered exit
        if (!strcmp(line, "exit") || !strcmp(line, "quit"))
        {
            break;
        }

        // split the line into words
        split_line(line, words, ' ');

        if (words[0] == NULL)
        {
            printf("No command entered\n");
            return 1;
        }

        // print the separated args from the line
        for (int ix = 0; words[ix] != NULL; ix++)
        {
            printf("words[%d] = %s\n", ix, words[ix]);
        }

        // want to check for piping and redirects first. if no piping do below
        // otherwise do piping and redirect modified funcs of below

        process_command(words, absolute_path);

        printf("You Entered: %s\n", line);
    }
    return 0;
}