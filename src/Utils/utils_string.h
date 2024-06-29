#ifndef __UTILS_STRING_H
#define __UTILS_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **split(int *occurrences, const char *str, char c);

void free_split(char **words, int count);

void split_filename(const char *filename, char **basename, char **extension);

#endif