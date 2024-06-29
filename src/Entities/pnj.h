#ifndef __PNJ_H__
#define __PNJ_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Gestion du blob (affichage + deplacement)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "entity.h"
#include "player.h"
#include "entities_manager.h"

/* ne pas mettre un identifiant de compteur déjà definie dans entity.h */
#define SUIVI_COUNTER 50

typedef struct t_PNJ {
	
} PNJ;

ENTITY *PNJ_new(float x, float y, SDL_Rect bodyBoxes[4], SDL_Rect stepBoxes[4], float zoom,
	char* imageName, Sint32 tileSize, Sint32 dispTileSize, Uint16 frameBetweenEachTile, bool firstTileIsStanding, float speed, bool attack);
ENTITY *PNJ_newFromArray(float x, float y, DirectionFacing direction, Sint32 dispTileSize, ARRAY *info);
void PNJ_free(ENTITY *entity);

/* --- Direction et position --- */

void PNJ_runSpeed(ENTITY *entity);

/* --- Vie et dégats --- */

/* --- Mise à jour et affichage --- */

void PNJ_event(ENTITY *entity);

void PNJ_update(ENTITY *entity, MAP *map);

/* mouvment du blob au repos */
void PNJ_idle(ENTITY *entity);

#endif /* __PNJ_H__ */