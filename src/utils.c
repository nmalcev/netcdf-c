#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char* join_str(const char* s1, const char* s2)
{
    char* result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* clone_str(const char* s1)
{
    char* result = malloc(strlen(s1) + 1);
    strcpy(result, s1);
    return result;
}
