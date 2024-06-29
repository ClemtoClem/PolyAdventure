#ifndef __PLAYER_H__
#define __PLAYER_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Gestion du joueur (affichage + déplacement)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "../Utils/utils.h"
#include "../Game/map.h"
#include "../Game/camera.h"
#include "../Core/gui.h"
#include "entity.h"
#include "entities_manager.h"

/* ne pas mettre un identifiant de compteur déjà definie dans entity.h */
#define AFTER_ROTATE_COUNTER 50

typedef struct t_PLAYER {
	PANEL *headband;
	LABEL *labelPolypoints;

	float runSpeed, walkSpeed;
	bool verifCollision, run;
} PLAYER;

ENTITY *Player_new(float x, float y, SDL_Rect bodyBoxes[4], SDL_Rect stepBoxes[4], float zoom,
	char* imageName, Sint32 tileSize, Sint32 dispTileSize, Uint16 frameBetweenEachTile, bool firstTileIsStanding, float speed, bool attack);
ENTITY *Player_newFromArray(float x, float y, DirectionFacing direction, Sint32 dispTileSize, ARRAY *info);
void Player_free(ENTITY *entity);

/* --- Direction et position --- */

void Player_setPosition(ENTITY *entity, float x, float y);

void Player_runSpeed(ENTITY *entity);
void Player_walkSpeed(ENTITY *entity);

/* --- Vie et dégats --- */

void Player_enableHealth(ENTITY *entity);
void Player_disableHealth(ENTITY *entity);
void Player_setHealth(ENTITY *entity, float health, float healthMax, float carefulPerFrame);
/* damagePerFrame = -1 pour ne pas modifier */
void Player_setDamage(ENTITY *entity, float immediateDamage, float damagePerFrame);
/* carefulPerFrame = -1 pour ne pas modifier */
void Player_setCareful(ENTITY *entity, float immediateCareful, float carefulPerFrame);
/* Renvoie true si la vie de l'entité est à 0 */
bool Player_isDead(ENTITY *entity);

/* --- Mise à jour et affichage --- */

void Player_event(ENTITY *entity);

void Player_update(ENTITY *entity, MAP *map);

void Player_draw(ENTITY *entity, CAMERA *camera, bool drawBox);

void Player_drawFrontOf(ENTITY *entity, CAMERA *camera, bool drawBox);

#endif /* __PLAYER_H__ */