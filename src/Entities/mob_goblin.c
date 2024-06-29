#include "mob_goblin.h"

ENTITY *Mob_Goblin_new(float x, float y, SDL_Rect bodyBoxes[4], SDL_Rect stepBoxes[4], float zoom,
	char* imageName, Sint32 tileSize, Sint32 dispTileSize, Uint16 frameBetweenEachTile, bool firstTileIsStanding, float speed, bool attack) {
    MOB_GOBLIN *Mob_Goblin = NULL;
    ENTITY *entity = Entity_new(x, y, bodyBoxes, stepBoxes, zoom, imageName, tileSize, dispTileSize, frameBetweenEachTile, firstTileIsStanding, speed, attack);
	if (entity != NULL) {
        Mob_Goblin = (MOB_GOBLIN *) calloc(1,sizeof(MOB_GOBLIN));
        if (Mob_Goblin != NULL) {

            // utilisable si besoin de plusieurs vitesses
            // player->walkSpeed = speed;
            
            entity->privateStruct = (void *) Mob_Goblin;
            entity->type = MOB_GOBLIN_ENTITY;
			Mob_Goblin->mobdir = false;
			
			Timer_addCounter(entity->timer, SUIVI_COUNTER, 500, false, false);

            /* Sauvgarde des pointeurs de fonctions */
            entity->functions->free				= &Mob_Goblin_free;
            entity->functions->enableHealth		= &Mob_Goblin_enableHealth;
            entity->functions->disableHealth	= &Mob_Goblin_disableHealth;
            entity->functions->setHealth		= &Mob_Goblin_setHealth;
            entity->functions->setDamage		= &Mob_Goblin_setDamage;
			entity->functions->event			= &Mob_Goblin_event;
			entity->functions->callback			= true;
        }
	}
	return entity;
}

ENTITY *Mob_Goblin_newFromArray(float x, float y, DirectionFacing direction, Sint32 dispTileSize, ARRAY *info) {
	ENTITY *entity = NULL;
	if (info != NULL) {
		char imageName[50];
		bool firstTileIsStanding, attack;
		Sint32 tileSize;
		Uint16 frameBetweenEachTile;
		float zoom, speed;

		Array_getString(info, "imageName", imageName, "blob.png", 50);
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

		entity = Mob_Goblin_new(x, y, bodyBoxes, stepBoxes, zoom, imageName, tileSize, dispTileSize, frameBetweenEachTile, firstTileIsStanding, speed, attack);
		Entity_setDirection(entity, direction);
		//if (attack) Mob_Goblin_EnableAttack(entity);
	}
	return entity;
}

void Mob_Goblin_free(ENTITY *entity) {
    if (entity != NULL && entity->type == MOB_GOBLIN_ENTITY) {
		MOB_GOBLIN *Mob_Goblin = entity->privateStruct;
		if (Mob_Goblin != NULL) {
			free(entity->privateStruct);
			entity->privateStruct = NULL;
		}
	}
}


/* --- Dimension du joueur --- */

/* --- Direction et position --- */

// utilisable si besoin de plusieurs vitesses
// void Mob_Goblin_runSpeed(ENTITY *entity);


/* --- Vie et dégats --- */

void Mob_Goblin_enableHealth(ENTITY *entity) {
    if (entity != NULL && entity->type == MOB_GOBLIN_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Mob_Goblin_enableHealth par Entity_enableHealth */
		Entity_enableHealth(entity);
		entity->functions->callback = true;
	}
}

void Mob_Goblin_disableHealth(ENTITY *entity) {
    if (entity != NULL && entity->type == MOB_GOBLIN_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Mob_Goblin_disableHealth par Entity_disableHealth */
		Entity_disableHealth(entity);
		entity->functions->callback = true;
	}
}

void Mob_Goblin_setHealth(ENTITY *entity, float health, float healthMax, float carefulPerFrame) {
    if (entity != NULL && entity->type == MOB_GOBLIN_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Mob_Goblin_setHealth par Entity_setHealth */
		Entity_setHealth(entity, health, healthMax, carefulPerFrame);
		entity->functions->callback = true;
	}
}

/* damagePerFrame = -1 pour ne pas modifier */
void Mob_Goblin_setDamage(ENTITY *entity, float immediateDamage, float damagePerFrame) {
    if (entity != NULL && entity->type == MOB_GOBLIN_ENTITY) {
		entity->functions->callback = false; /* on interdit le rappel de Player_setDamage par Entity_setDamage */
		Entity_setDamage(entity, immediateDamage, damagePerFrame);
		entity->functions->callback = true;
	}
}

/* carefulPerFrame = -1 pour ne pas modifier */
void Mob_Goblin_setCareful(ENTITY *entity, float immediateCareful, float carefulPerFrame) {

}

/* Renvoie true si la vie de l'entité est à 0 */
bool Mob_Goblin_isDead(ENTITY *entity) {
	return false;
}


/* --- Mise à jour et affichage --- */
void Mob_Goblin_event(ENTITY *entity) {
    if (entity != NULL && entity->type == MOB_GOBLIN_ENTITY) {
		Sint32 posx, posy;
		MOB_GOBLIN *Mob_Goblin = entity->privateStruct; 

		ENTITY *player = EntitiesManager_getFromType(PLAYER_ENTITY);

		FCIRCLE c;
		c.x = entity->x;
		c.y = entity->y;
		c.radius = 4*entity->dispTileSize;

        if (FloatCollisionPointCircle(player->x, player->y, &c)) {
			posx = player->x - entity->x;
			posy = player->y - entity->y;

			if (/*(SDL_abs(posx) >= SDL_abs(posy)) &&*/ !Mob_Goblin->mobdir) {
				if (posx == 0) Mob_Goblin->mobdir = true;
				else if (posx < 0) {
					Entity_setDirection(entity, WEST);		
				}		
				else if (posx > 0){
					Entity_setDirection(entity, EAST);
				}
			} else if (/*(SDL_abs(posx) < SDL_abs(posy)) &&*/ Mob_Goblin->mobdir){
				if (posy == 0) Mob_Goblin->mobdir = false;
				else if (posy < 0) {
					Entity_setDirection(entity, NORTH);
				}		
				else if (posy > 0){
					Entity_setDirection(entity, SOUTH);
				}
				
			}
			Entity_walk(entity);
		}

		// ecrire idle

		entity->functions->callback = false; /* on interdit le rappel de Mob_Goblin_event par Entity_event*/
		Entity_event(entity);
		entity->functions->callback = true;
	}
}
