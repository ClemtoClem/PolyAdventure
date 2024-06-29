#ifndef __SAVE_MANAGER_H__
#define __SAVE_MANAGER_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Gestion des sauvgarde
 */

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "../Entities/entities_manager.h"

typedef struct t_SAVE_MANAGER {
	ARRAY *listPseudo;

	Uint16 idCurrentSaveFile;
	FILE *currentSaveFile;
} SAVE_MANAGER;

extern SAVE_MANAGER *saveManager;

/** Initialisation de la structure SAVE_MANAGER (saveManager est une variable globale ne pas l'allouer) */
bool SaveFile_init();
void SaveFile_free();
SAVE_MANAGER *SaveFile_get();

void SaveFile_makeListPseudo();
Uint16 nbSaveFile();

/** Création d'un fichier de sauvgarde */
void SaveFile_create();

/** Ouverture d'un fichier de sauvgarde,
 * Si le fichier est déjà ouverte on remet le curseur au debut */
void SaveFile_open(Uint16 id);

/** Fermeture du fichier de sauvgarde */
void SaveFile_close();

/** Initialiser les données du jeu à partir du fichier de sauvgarde */
void SaveFile_load();

/* Saugrader les données du jeu dans le fichier de sauvgarde */
void SaveFile_save();

#endif /* __SAVE_MANAGER_H__ */