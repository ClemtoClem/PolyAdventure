#ifndef __MOB_BLOB_H__
#define __MOB_BLOB_H__

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

typedef struct t_MOB_BLOB {
	float runSpeed;
	bool mobdir;
} MOB_BLOB;

ENTITY *Mob_Blob_new(float x, float y, SDL_Rect bodyBoxes[4], SDL_Rect stepBoxes[4], float zoom,
	char* imageName, Sint32 tileSize, Sint32 dispTileSize, Uint16 frameBetweenEachTile, bool firstTileIsStanding, float speed, bool attack);
ENTITY *Mob_Blob_newFromArray(float x, float y, DirectionFacing direction, Sint32 dispTileSize, ARRAY *info);
void Mob_Blob_free(ENTITY *entity);

/* --- Direction et position --- */

void Mob_Blob_runSpeed(ENTITY *entity);

/* --- Vie et dégats --- */

void Mob_Blob_enableHealth(ENTITY *entity);
void Mob_Blob_disableHealth(ENTITY *entity);
void Mob_Blob_setHealth(ENTITY *entity, float health, float healthMax, float carefulPerFrame);
/* damagePerFrame = -1 pour ne pas modifier */
void Mob_Blob_setDamage(ENTITY *entity, float immediateDamage, float damagePerFrame);
/* carefulPerFrame = -1 pour ne pas modifier */
void Mob_Blob_setCareful(ENTITY *entity, float immediateCareful, float carefulPerFrame);
/* Renvoie true si la vie de l'entité est à 0 */
bool Mob_Blob_isDead(ENTITY *entity);

/* --- Mise à jour et affichage --- */

void Mob_Blob_event(ENTITY *entity);

/* mouvment du blob au repos */
void Mob_Blob_idle(ENTITY *entity);

#endif /* __MOB_BLOB_H__ */