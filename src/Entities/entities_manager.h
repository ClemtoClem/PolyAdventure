#ifndef __ENTITIES_MANAGER_H__
#define __ENTITIES_MANAGER_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Les PNJ et monstres sont des entités.
 * L'intéraction entre ces entités s'éffectue à l'aide de l'ENTITIES_MANAGER
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "../Game/camera.h"
#include "../Game/map.h"
#include "../Game/tileset.h"
#include "entity.h"

typedef struct t_ENTITIES_MANAGER {
    ENTITY **list;
    ARRAY *displayInFront;
    Uint32 length, maxLength;
} ENTITIES_MANAGER;

extern ENTITIES_MANAGER *entitiesManager;

bool EntitiesManager_init(Uint32 maxLength);
void EntitiesManager_free();
void EntitiesManager_clean();
ENTITIES_MANAGER *EntitiesManager_get();

/* --- Gestion des entités --- */

void EntitiesManager_addEntity(ENTITY *entity);
void EntitiesManager_deleteEntity(ENTITY *entity);
void EntitiesManager_setDispTileSize(Uint32 dispTileSize);
ENTITY *EntitiesManager_getFromType(TypeEntity type);


/* --- Mise à jour et affichage --- */

/* traitement des évènements pour toutes les entités */
void EntitiesManager_event();

/* mise à jour de toutes les entités */
void EntitiesManager_update(MAP *map);

/* affichage des entités */
void EntitiesManager_draw(CAMERA *camera, bool drawBox);

/* affichage des éléments des entités à afficher au premier plan */
void EntitiesManager_drawFrontOf(CAMERA *camera, bool drawBox);

#endif