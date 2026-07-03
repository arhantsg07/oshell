#include "parser.h"

void str_to_tok(char *token, char *args[], int *argc)
{
    while (token != NULL && *argc < MAX_LINE / 2)
    {
        args[(*argc)++] = token;
        token = strtok(NULL, " \t");
    }
    args[*argc] = NULL;
}