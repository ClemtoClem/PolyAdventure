#include "player.h"

ENTITY *Player_new(float x, float y, SDL_Rect bodyBoxes[4], SDL_Rect stepBoxes[4], float zoom,
	char* imageName, Sint32 tileSize, Sint32 dispTileSize, Uint16 frameBetweenEachTile, bool firstTileIsStanding, float speed, bool attack) {
	PLAYER *player = NULL;
	ENTITY *entity = Entity_new(x, y, bodyBoxes, stepBoxes, zoom, imageName, tileSize, dispTileSize, frameBetweenEachTile, firstTileIsStanding, speed, attack);
	if (entity != NULL) {
		PANEL *headband = (PANEL *) Panel_new(NULL, NULL, SG_getWidth() - 270, 10, 260, 40, "headband.png", 3);
		if (headband != NULL) {
			player = (PLAYER *) calloc(1, sizeof(PLAYER));
			if (player != NULL) {

				/* Création d'un bandeau */
				Sprite_new(headband, "heart1", 4, 1, 32, 32, "heart.png", 17, 1, HORIZONTAL, 8, 500);
				Sprite_new(headband, "heart2", 34, 1, 32, 32, "heart.png", 17, 1, HORIZONTAL, 8, 500);
				Sprite_new(headband, "heart3", 64, 1, 32, 32, "heart.png", 17, 1, HORIZONTAL, 8, 500);
				Sprite_new(headband, "heart4", 94, 1, 32, 32, "heart.png", 17, 1, HORIZONTAL, 8, 500);
				player->labelPolypoints = (LABEL *) Label_new(headband, "polypoints", 0, 0, 0, headband->rect.h-4, "Polypoints : 0000", "toon_around.ttf", 16, 0x7ABC9AFF);
				player->labelPolypoints->rect.x = headband->rect.w - player->labelPolypoints->rect.w - 14;

				Panel_enable(headband);
				player->headband = headband;

				player->runSpeed = speed*2;
				player->walkSpeed = speed;
				player->run = false;
				
				entity->privateStruct = (void *) player;
				entity->type = PLAYER_ENTITY;
			
				Timer_addCounter(entity->timer, AFTER_ROTATE_COUNTER, frameBetweenEachTile, false, true);

				/* Sauvgarde des pointeurs de fonctions */
				entity->functions->free				= &Player_free;
				entity->functions->enableHealth		= &Player_enableHealth;
				entity->functions->disableHealth	= &Player_disableHealth;
				entity->functions->setHealth		= &Player_setHealth;
				entity->functions->setDamage		= &Player_setDamage;
				entity->functions->setCareful		= &Player_setCareful;
				entity->functions->event			= &Player_event;
				entity->functions->update			= &Player_update;
				entity->functions->draw				= &Player_draw;
				entity->functions->drawFrontOf		= &Player_drawFrontOf;
				entity->functions->callback			= true;
			}
		} else {
			LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure PANEL\n");
		}
	}
	return entity;
}

ENTITY *Player_newFromArray(float x, float y, DirectionFacing direction, Sint32 dispTileSize, ARRAY *info) {
	ENTITY *entity = NULL;
	if (info != NULL) {
		char imageName[50];
		bool firstTileIsStanding, attack;
		Sint32 tileSize;
		Uint16 frameBetweenEachTile;
		float zoom, speed;

		Array_getString(info, "imageName", imageName, "polyman.png", 50);
		firstTileIsStanding = Array_getBool(info, "firstTileIsStanding", true);
		attack = Array_getBool(info, "attack", true);
		tileSize = Array_getSint32(info, "tileSize", 64);
		zoom = Array_getFloat(info, "zoom", 1.0);
		frameBetweenEachTile = Array_getUint16(info, "frameBetweenEachTile", 5);
		speed = Array_getFloat(info, "speed", 0.025);

		SDL_Rect bodyBoxes[4], stepBoxes[4], defaultBox;
		TypeData typedata;

		RectSet(&defaultBox, 0, 0, tileSize, tileSize);

		/* on reccupère la taille des bodyBoxes */
		typedata = Array_getTypeFromKey(info, "bodyBox");
		if (typedata == RECT_TYPE) {
			Array_getRect(info, "bodyBox", &bodyBoxes[0], &defaultBox);
			RectCopy(&bodyBoxes[1], &bodyBoxes[0]);
			RectCopy(&bodyBoxes[2], &bodyBoxes[0]);
			RectCopy(&bodyBoxes[3], &bodyBoxes[0]);
		} else if (typedata == ARRAY_TYPE) {
			ARRAY *boxes = Array_getArray(info, "bodyBox");
			Array_getRect(boxes, "default", &defaultBox, &defaultBox);
			Array_getRect(boxes, "north", &bodyBoxes[NORTH], &defaultBox);
			Array_getRect(boxes, "south", &bodyBoxes[SOUTH], &defaultBox);
			Array_getRect(boxes, "west", &bodyBoxes[WEST], &defaultBox);
			Array_getRect(boxes, "east", &bodyBoxes[EAST], &defaultBox);
		}
		
		/* on reccupère la taille des stepBoxes */
		typedata = Array_getTypeFromKey(info, "stepBox");
		if (typedata == RECT_TYPE) { /* stepBox = rect(x, y, w, h) */
			Array_getRect(info, "stepBox", &stepBoxes[0], &defaultBox);
			RectCopy(&stepBoxes[1], &stepBoxes[0]);
			RectCopy(&stepBoxes[2], &stepBoxes[0]);
			RectCopy(&stepBoxes[3], &stepBoxes[0]);
		} else if (typedata == ARRAY_TYPE) {  /* stepBox = { north = rect(x1, y1, w1, h1); south = rect(x2, y2, w2, h2); default = rect(x3, y3, w3, h3) }  */
			ARRAY *boxes = Array_getArray(info, "stepBox");
			Array_getRect(boxes, "default", &defaultBox, &defaultBox);
			Array_getRect(boxes, "north", &stepBoxes[NORTH], &defaultBox);
			Array_getRect(boxes, "south", &stepBoxes[SOUTH], &defaultBox);
			Array_getRect(boxes, "west", &stepBoxes[WEST], &defaultBox);
			Array_getRect(boxes, "east", &stepBoxes[EAST], &defaultBox);
		}

		entity = Player_new(x, y, bodyBoxes, stepBoxes, zoom, imageName, tileSize, dispTileSize, frameBetweenEachTile, firstTileIsStanding, speed, attack);
		Entity_setDirection(entity, direction);
		
		//if (attack) Player_EnableAttack(entity);
	}
	return entity;
}

void Player_free(ENTITY *entity) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		PLAYER *player = entity->privateStruct;
		if (player != NULL) {
			if (player->headband != NULL) {
				Panel_free(player->headband);
				player->headband = NULL;
			}
			free(entity->privateStruct);
			entity->privateStruct = NULL;
		}
	}
}

/* --- Direction et position --- */

void Player_runSpeed(ENTITY *entity) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		PLAYER *player = entity->privateStruct;
		entity->speed = player->runSpeed;
		player->run = true;
	}
}

void Player_walkSpeed(ENTITY *entity) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		PLAYER *player = entity->privateStruct;
		entity->speed = player->walkSpeed;
		player->run = false;
	}
}

/* --- Vie et dégats --- */

void Player_enableHealth(ENTITY *entity) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Player_enableHealth par Entity_enableHealth */
		Entity_enableHealth(entity);
		entity->functions->callback = true;
	}
}
void Player_disableHealth(ENTITY *entity) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Player_disableHealth par Entity_disableHealth */
		Entity_disableHealth(entity);
		entity->functions->callback = true;
	}
}
void Player_setHealth(ENTITY *entity, float health, float healthMax, float carefulPerFrame) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Player_setHealth par Entity_setHealth */
		Entity_setHealth(entity, health, healthMax, carefulPerFrame);
		entity->functions->callback = true;
	}
}

void Player_setDamage(ENTITY *entity, float immediateDamage, float damagePerFrame) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Player_setDamage par Entity_setDamage */
		Entity_setDamage(entity, immediateDamage, damagePerFrame);
		entity->functions->callback = true;
	}
}

void Player_setCareful(ENTITY *entity, float immediateCareful, float carefulPerFrame) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Player_setCareful par Entity_setCaraful */
		Entity_setCareful(entity, immediateCareful, carefulPerFrame);
		entity->functions->callback = true;
	}
}

/* --- Mise à jour et affichage --- */

void Player_event(ENTITY *entity) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		if (Input_wasKeyPressed(SDLK_a)) {
			Player_runSpeed(entity);
		}

		if (Input_wasKeyReleased(SDLK_a)) {
			Player_walkSpeed(entity);
		}

		/* gestion de l'interaction avec le decor après appui sur la touche <Enter> */
		if (Input_wasKeyPressed(SDLK_SPACE)) {
			switch (entity->idCollisionFacing) {
				case MESSAGE_COLLISION: printf("lire message\n"); break;
				case ACCES_COLLISION:
					Input_pushEvent(USER_EVENT, TELEPORTATION_EVENT); break;
				default: break;
			}
		}

		DirectionFacing direction = ERROR_DIRECTION;
		bool up = Input_wasKeyHeld(SDLK_UP, SDLK_z);
		bool down = Input_wasKeyHeld(SDLK_DOWN, SDLK_s);
		bool left = Input_wasKeyHeld(SDLK_LEFT, SDLK_q);
		bool right = Input_wasKeyHeld(SDLK_RIGHT, SDLK_d);

		//DirectionFacing dir = entity->directionFacing;

		if (up) {
			direction = NORTH;
		} else if (down) {
			direction = SOUTH;
		}

		if (left) {
			direction = WEST;
		} else if (right) {
			direction = EAST;
		}

		entity->functions->callback = false; /* on interdit le rappel de Player_event par Entity_event */

		if (direction != ERROR_DIRECTION) {
			COUNTER *count = Timer_getCounter(entity->timer, AFTER_ROTATE_COUNTER);
			
			/* si la nouvelle direction n'est pas égale à la précédente
			 * on fait juste tourner le personnage sur lui même et
			 * on active un timer pour que le personnage avance dans plusieur frammes si la touche reste enfoncée */
			if (entity->motionType == STANDING && entity->directionFacing != direction) {
				Entity_setDirection(entity, direction);
				count->value = 0;
				if (count->finish) count->finish = false;
			} else if (count->finish) {
				Entity_setDirection(entity, direction);
				Entity_walk(entity);
			} /* else {
				no move
			}*/
		}
		
		Entity_event(entity);

		entity->functions->callback = true;

		PLAYER *player = entity->privateStruct;
		if (player != NULL) {
			Panel_event(player->headband);
		}
	}
}

void Player_update(ENTITY *entity, MAP *map) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Player_update par Entity_update */
		Entity_update(entity, map);
		entity->functions->callback = true;

		if (entity->idCollisionFacing) {
			switch(entity->idCollisionFacing) {
				// Téléportation du joueur si devant lui se situe un passage
				case PASSAGE_COLLISION: {
					Input_pushEvent(USER_EVENT, TELEPORTATION_EVENT);
					break;
				}
				case ACCES_COLLISION: {
					TTF_Font *font = SG_loadFont("arial.ttf", 30);
					SG_printf(true, SG_getWidth()/2-140, SG_getHeight()-40, font, YELLOW, "Press SPACE to access");
					break;
				}
				case MESSAGE_COLLISION: {
					TTF_Font *font = SG_loadFont("arial.ttf", 30);
					SG_printf(true, SG_getWidth()/2-200, SG_getHeight()-40, font, YELLOW, "Press SPACE to read the mesage");
					break;
				}
				default: break;
			}
		}

		PLAYER *player = entity->privateStruct;
		if (player != NULL) {
			Panel_update(player->headband);
		}
	}
}

void Player_draw(ENTITY *entity, CAMERA *camera, bool drawBox) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Player_draw par Entity_draw */
		Entity_draw(entity, camera, drawBox);
		entity->functions->callback = true;

		/* ... */
	}
}

void Player_drawFrontOf(ENTITY *entity, CAMERA *camera, bool drawBox) {
	if (entity != NULL && entity->type == PLAYER_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Player_drawFrontOf par Entity_drawFrontOf */
		Entity_drawFrontOf(entity, camera, drawBox);
		entity->functions->callback = true;

		PLAYER *player = entity->privateStruct;
		if (player != NULL) {
			Panel_draw(player->headband, NULL);
		}
	}
}
