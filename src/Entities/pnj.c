#include "pnj.h"

ENTITY *PNJ_new(float x, float y, SDL_Rect bodyBoxes[4], SDL_Rect stepBoxes[4], float zoom,
	char* imageName, Sint32 tileSize, Sint32 dispTileSize, Uint16 frameBetweenEachTile, bool firstTileIsStanding, float speed, bool attack) {
    PNJ *pnj = NULL;
    ENTITY *entity = Entity_new(x, y, bodyBoxes, stepBoxes, zoom, imageName, tileSize, dispTileSize, frameBetweenEachTile, firstTileIsStanding, speed, attack);
	if (entity != NULL) {
        pnj = (PNJ *) calloc(1, sizeof(PNJ));
        if (pnj != NULL) {

            // utilisable si besoin de plusieurs vitesses
            // player->walkSpeed = speed;
            
            entity->privateStruct = (void *) pnj;
            entity->type = PNJ_ENTITY;
        
			Timer_addCounter(entity->timer, SUIVI_COUNTER, 500, false, false);

            /* Sauvgarde des pointeurs de fonctions */
            entity->functions->free		= &PNJ_free;
			entity->functions->event	= &PNJ_event;
			entity->functions->update	= &PNJ_update;
			entity->functions->callback	= true;
        }
	}
	return entity;
}

ENTITY *PNJ_newFromArray(float x, float y, DirectionFacing direction, Sint32 dispTileSize, ARRAY *info) {
	ENTITY *entity = NULL;
	if (info != NULL) {
		char imageName[50];
		bool firstTileIsStanding, attack;
		Sint32 tileSize;
		Uint16 frameBetweenEachTile;
		float zoom, speed;

		Array_getString(info, "imageName", imageName, "personnage_1.png", 50);
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

		entity = PNJ_new(x, y, bodyBoxes, stepBoxes, zoom, imageName, tileSize, dispTileSize, frameBetweenEachTile, firstTileIsStanding, speed, attack);
		Entity_setDirection(entity, direction);
		//if (attack) Blob_EnableAttack(entity);
	}
	return entity;
}

void PNJ_free(ENTITY *entity) {
    if (entity != NULL && entity->type == PNJ_ENTITY) {
		PNJ *pnj = entity->privateStruct;
		if (pnj != NULL) {
			free(entity->privateStruct);
			entity->privateStruct = NULL;
		}
	}
}


/* --- Dimension du joueur --- */

/* --- Direction et position --- */

// utilisable si besoin de plusieurs vitesses
// void PNJ_runSpeed(ENTITY *entity);


/* --- Vie et dégats --- */


/* --- Mise à jour et affichage --- */
void PNJ_event(ENTITY *entity) {
    if (entity != NULL && entity->type == PNJ_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de PNJ_event par Entity_event*/
		Entity_event(entity);
		entity->functions->callback = true;
	}
}

void PNJ_update(ENTITY *entity, MAP *map) {
	if (entity != NULL && entity->type == PNJ_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de PNJ_update par Entity_update */
		Entity_update(entity, map);
		entity->functions->callback = true;
	}
}

//Entity_draw(entity, sound_graphics, camera, drawBox);