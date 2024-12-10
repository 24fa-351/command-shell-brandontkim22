#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAX_LINE 1000

void add_character_to_string(char *string, char character)
{
    int length_of_string = strlen(string);
    string[length_of_string] = character;
    string[length_of_string + 1] = '\0';
}

void split_line(char *line, char *words[], char delimiter)
{
    int word_count = 0;
    char *next_char = line;
    char current_word[MAX_LINE];
    strcpy(current_word, "");

    while (*next_char != '\0')
    {
        if (*next_char == delimiter) // check for delimiter
        {
            words[word_count++] = strdup(current_word);
            strcpy(current_word, "");
        }
        else
        {
            add_character_to_string(current_word, *next_char);
        }
        ++next_char;
    }
    words[word_count++] = strdup(current_word);

    words[word_count] = NULL;
}

bool find_absolute_path(char *command, char *absolute_path)
{
    char *directories[MAX_LINE];

    split_line(getenv("PATH"), directories, ':');

    for (int ix = 0; directories[ix] != NULL; ix++)
    {
        char path[MAX_LINE];
        strcpy(path, directories[ix]);
        add_character_to_string(path, '/');
        strcat(path, command);

        if (access(path, X_OK) == 0)
        {
            strcpy(absolute_path, path);
            return true;
        }
    }
    return false;
}
