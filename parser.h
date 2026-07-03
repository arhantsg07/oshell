#ifndef PARSER_H
#define PARSER_H
#include <stddef.h>
#include <string.h>

#define MAX_LINE 80

void str_to_tok(char *token, char *args[], int *argc);

#endif