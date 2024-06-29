/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Gestion des messages log
 * Options de configurations
*/

#ifndef __LOGGER_H
#define __LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "utils.h"

#define SIZE_FILENAME 256
#define MAX_FILE_SIZE 1048576L /* 1 MB */

#if defined(_WIN32) || defined(_WIN64)
	#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define LOG(msg, ...) \
	Logger_log(LogLevel_NOTHING, ErrorType_NOTHING, __FILENAME__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_DEBUG(msg, ...) \
	Logger_log(LogLevel_DEBUG, ErrorType_NOTHING, __FILENAME__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) \
	Logger_log(LogLevel_INFO, ErrorType_NOTHING, __FILENAME__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_SUCCESS(msg, ...) \
	Logger_log(LogLevel_SUCCESS , ErrorType_NOTHING, __FILENAME__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) \
	Logger_log(LogLevel_WARNING, ErrorType_NOTHING, __FILENAME__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_ERROR(typeError, msg, ...) \
	Logger_log(LogLevel_ERROR, typeError, __FILENAME__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) \
	Logger_log(LogLevel_FATAL, ErrorType_NOTHING, __FILENAME__, __LINE__, msg, ##__VA_ARGS__)

/* Niveaux de priorité pour les messages log */
typedef enum LogLevel {
	LogLevel_NOTHING = 0,
	LogLevel_TRACE = 1 << 6,			/* pour suivre l'évolution d'un varibale dans une boucle while */
	LogLevel_DEBUG = 1 << 5,			/* pour les messages de débogage, utiles pour les développeurs */
	LogLevel_INFO  = 1 << 4,			/* pour les messages informatifs, utiles pour les utilisateurs */
	LogLevel_SUCCESS=1 << 3,			/* pour indiquer un succès */
	LogLevel_WARNING  = 1 << 2,			/* pour les messages d’avertissement, utiles pour signaler des situations anormales mais non critiques */
	LogLevel_ERROR = 1 << 1,			/* pour les messages d’erreur, utiles pour signaler des situations critiques qui empêchent le bon fonctionnement du programme */
	LogLevel_FATAL = 1 << 0				/* pour les messages fatals, utiles pour signaler des situations irrécupérables qui nécessitent l’arrêt du programme */
} LogLevel;

/* Enumération des types d'erreurs (LogLevel_ERROR) */
typedef enum ErrorType {
	ErrorType_NOTHING = 0,
	ErrorType_FAULT = 1 << 1,			/* Un pointeur manipulé n'adresse pas un espace mémoire valide */
	ErrorType_FILE_ACCESS = 1 << 2,		/* Erreur d'accès à un fichier */
	ErrorType_TYPE = 1 << 3,			/* Erreur de type */
	ErrorType_NB_ARGUMENTS = 1 << 4,	/* Nombre d'arguments invalide */
	ErrorType_NB_RETURNS = 1 << 5,		/* Nombre de variables en retour d'une fonction invalide */
	ErrorType_TYPE_ARGUMENTS = 1 << 6,	/* Erreur de type d'arguments */
	ErrorType_ERROR = 1 << 7,			/* Erreur sans spécification particulière */
	ErrorType_SDL = 1 << 8,				/* errue avec une fonction SDL */
} ErrorType;

typedef enum LogDisplayMode {
	LogDisplayMode_CONSOLE = 1 << 0,			/* affichage dans la console */
	LogDisplayMode_FILE    = 1 << 1,			/* affichage dans un fichier */
	LogDisplayMode_SCREEN  = 1 << 2,			/* affichage dans l'interface graphique */
	LogDisplayMode_ALL     = 0x7 				/* Tout les affichages */
} LogDisplayMode;


typedef struct LOGGER {
	FILE *output;						/* le pointeur sur le fichier log courant */
	char filename[SIZE_FILENAME];		/* le nom du fichier log */
	char file_extension[SIZE_FILENAME];	/* l'extension du fichier log */
	Uint64 currentFileSize;				/* la taille du fichier log courant */
	Uint64 maxFileSize;					/* la taille maximal d'un fichier log */
	Uint64 idBackupFiles;				/* le nombre de fichiers log existant */
	Uint8 maxBackupFiles;					/* le nombre max de fichiers log */
	LogDisplayMode displayMode;					/* les types d'affichage du logger (généralement LogDisplayMode_ALL)*/

	LogLevel last_level;				/* Le niveau du dernier message entrée */
	ErrorType error_type;

	Uint8 disable;							/* Les niveaux désactivés */
} LOGGER;

extern LOGGER *_log;

Uint64 fsize(FILE *fp); /* Revoie la taille du fichier */
char *strCurrentTime(void);
char *strCurrentDate(void);

/* ------ Constructor ------ */
void Logger_init(LogDisplayMode displayMode, const char *filename);

/* ------ Destructor ------- */
void Logger_quit();

/* -------- Methods -------- */
void Logger_enableDisplayMode(LogDisplayMode displayMode);

void Logger_enable(LogLevel level);   	/* activer un niveau */
void Logger_disable(LogLevel level);	/* désactiver un niveau */
bool Logger_isEnable(LogLevel level);	/* regarder si le niveau est activé */
void Logger_setLevel(LogLevel level);	/* lever un niveau */
LogLevel Logger_getLevel(void);			/* regarder le niveau*/
const char *Logger_getLevelStr(LogLevel level);
const char *Logger_getErrorStr(ErrorType error);

bool Logger_fileIsOpen();
bool Logger_openFile();					/* Ouvrir un fichier pour écrire les logs (ferme l'ancien fichier utilisé) */
void Logger_closeFile();				/* Fermer le fichier pour écrire les logs */

void Logger_log(LogLevel level, ErrorType errortype, const char* file, Sint32 line, const char* msg, ...); /* Ecrire un log */

#endif // __LOGGER_H