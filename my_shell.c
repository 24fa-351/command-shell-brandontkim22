#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#include "exec_args.h"
#include "path_funcs.h"

#define MAX_LINE 1000

int main(int argc, char *argv[])
{
    char line[MAX_LINE];
    char absolute_path[MAX_LINE];
    char *words[MAX_LINE];

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

        // check if the user entered nothing. Have to check for empty
        // string since we removed the newline character
        if (strcmp(line, "") == 0)
        {
            continue;
        }

        // split the line into words
        split_line(line, words, ' ');

        if (words[0] == NULL)
        {
            printf("No command entered\n");
            return 1;
        }

        int length_of_array = 0;
        // calculate the length of the array
        for (int ix = 0; words[ix] != NULL; ix++)
        {
            length_of_array++;
        }

        process_command(words, absolute_path, length_of_array);
    }
    return 0;
}