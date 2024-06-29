/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Parsage rapide d'un fichier conf personnalisé avec reconnaissance de type 
 * convertit sous forme de liste ARRAY pour pouvoir être utilisé lors
 * de l'execution du programme
 */

#ifndef __CONFIGURATION_FILE_H__
#define __CONFIGURATION_FILE_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "array.h"
#include "logger.h"

#define MAX_LINE_LENGTH 2048

void *StringToAllocValue(TypeData type, char *str);
DATA *convertStringtoData(char *nomVariable, char *str, int line);
bool readInstruction(FILE *file, char *instruction, ARRAY *array, int *line);
bool readBlock(FILE *file, ARRAY *array, int *line);

/* Load conf file to string value with key, return true if success */
bool Array_loadConfigurationFile(ARRAY *array, char *filename);

#endif /* __CONFIGURATION_FILE_H__ */
