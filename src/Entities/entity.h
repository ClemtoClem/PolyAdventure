#ifndef __ENTITY_H__
#define __ENTITY_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Les personnages et monstres sont des entites.
 * Cette structure permettera de gerer les animations de l'entite dans les differentes directions (Bord, Sud, Est, Ouest)
 * Pour creer un entité il faut lui renseigner sa position à l'initialisation, un rectangle de collision, le chemin du tileset,
 * la taille d'une tile (carre) et sa vitesse de deplacement.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "../Core/sound_graphics.h"
#include "../Core/input.h"
#include "../Utils/timer.h"
#include "../Game/camera.h"
#include "../Game/map.h"
#include "../Game/tileset.h"

#define ZOOM_ENTITY 1.5 /* on zoome un peu l'entité */

#define FRAME_TILE_COUNTER 1

typedef enum t_TypeEntity {
	DEFAULT_ENTITY = 0,
	PLAYER_ENTITY,
	PNJ_ENTITY,
	MOB_BLOB_ENTITY,
	MOB_DRAGON_ENTITY,
	MOB_GOBLIN_ENTITY,
	MOB_SKELETON_ENTITY
} TypeEntity;

typedef enum t_MotionType {
	ERROR_MOTION = -1,
	STANDING = 0,
	WALKING, AUTO_WALKING
} MotionType;

/* direction de l'entité */
typedef enum t_DirectionFacing {
	ERROR_DIRECTION = -1,
	NORTH = 0, SOUTH, WEST, EAST
} DirectionFacing;

typedef enum t_CollisionId {
	NO_COLLISION = 0,
	BLOCK_COLLISION = 1,	// murs, blocks, arbre
	MESSAGE_COLLISION = 2,	// pancarte et autre
	ACCES_COLLISION = 3,	// passage devant être validé par la touche <entrée>
	WATER_COLLISION = 4,
	PASSAGE_COLLISION = 5,	// passage (entrée grotte, echelle, sortie de maison ...)
	LAVA_COLLISION = 6,
	EAST_MOVER_COLLISION = 7,
	SOUTH_MOVER_COLLISION = 8,
	WEST_MOVER_COLLISION = 9,
	NORTH_MOVER_COLLISION = 10,
} CollisionId;


typedef struct t_ENTITY ENTITY;

/* Liste des functions disponibles, lors de la création d'un type d'entité il peut être plus important de créer des fonctions propre à ce type,
 * les pointeur de ces functions peut être enregistré sur une varible de cette liste.
 * Par exemple, pour le type PLAYER, un panel doit être affiché. Une fonction Player_draw donc été crée. Pour que l'entité utilise cette fonction
 * l'adresse de la fonction Player_draw est sauvgardé dans le pointeur de fonction draw. 
 * Si on souhaite utiliser la fonction par défaut le pointeur vaut NULL */
typedef struct {
	void (*free)			(ENTITY *); /* Ne pas utiliser Entity_free dans la fonction dont son adresse est sauvgardée dans void (*free) (ENTITY *entity). Entity_free sera executée automatiquement à la fin de l'execution de la fonction void (*free) (ENTITY *entity) */
	void (*setDispTileSize)	(ENTITY *, Sint32);
	void (*getBodyBox)		(ENTITY *, SDL_FRect *);
	void (*getStepBox)		(ENTITY *, SDL_FRect *);
	bool (*stepCollision)	(ENTITY *, MAP *);
	bool (*bodyCollision)	(ENTITY *, MAP *);
	bool (*conflict)		(ENTITY *, ENTITY *);
	void (*setPosition)		(ENTITY *, float, float);
	void (*getPosition)		(ENTITY *, float *, float *);
	void (*getMapPosition)  (ENTITY *, Uint16 *, Uint16 *);
	void (*getLookMapPosition)  (ENTITY *, Uint16 *, Uint16 *);
	void (*setDirection)	(ENTITY *, DirectionFacing);
	DirectionFacing (*getDirection) (ENTITY *);
	void (*standing)		(ENTITY *);
	void (*walk)			(ENTITY *);
	void (*autoWalk)		(ENTITY *, float);
	void (*enableHealth)	(ENTITY *);
	void (*disableHealth)	(ENTITY *);
	void (*setHealth)		(ENTITY *, float, float, float);
	void (*setDamage)		(ENTITY *, float, float);
	void (*setCareful)		(ENTITY *, float, float);
	bool (*isDead)			(ENTITY *);
	void (*event)			(ENTITY *);
	void (*update)			(ENTITY *, MAP *);
	void (*draw)			(ENTITY *, CAMERA *, bool);
	void (*drawFrontOf)		(ENTITY *, CAMERA *, bool);

	/* activer ou désactiver l'appel de fonctions pour que la fonction appelante soit pas rappelée en boucle avec la fontions appelé 
	 * Par exemple, Entity_draw appel Player_draw qui appel Entity_draw */
	bool callback;
} ENTITY_FUNCTIONS;

struct t_ENTITY {
	TypeEntity type;
	void *privateStruct;

	IMAGE *image; /* tileset utilisé pour afficher l'entité */
	Sint32 nbTilesWidth, nbTilesHeight;
	Sint32 dispTileSize;
	SDL_Rect srcRect;  /* position et taille du tile à reccupérer dans l'image */
	SDL_Rect destRect; /* position et taille de l'entité à l'écran */
	/* Le rectangle de collision est propre à la taille du tile de l'entité.
	 * Comme l'entité ne remplie pas entierement le tile, on peut définir un
	 * rectangle qui vas permettre de savoir à partir de quelle position
	 * l'entité est en collision avec un obstacle.
	*/
	SDL_FRect bodyBoxes[4], stepBoxes[4];
	Uint16 currentAnimateTile; /* id du sprite à afficher dans la direction de l'entité */
	float zoom; /* grossir l'entité */

	bool firstTileIsStanding; /* si le premier tile ne doit être afficher lorsque l'entité est en marche */
	
	float x, y; /* coordonnées du centre de l'entité dans la map */
	float dx, dy; /* déplacement à effectuer */
	float speed; /* vitesse de déplacement */
	float autoWalkingEnd_x, autoWalkingEnd_y; /* position de fin en mode automatique */

	
	DirectionFacing directionFacing;
	MotionType motionType;

	Uint16 idCollisionFacing;

	bool attack;
	bool enableHealth; /* activer la gestion de la vie et des dégâts */
	float health, healthMax; /* niveau de vie de l'entité */
	bool sick; /* true si l'entité est malade */
	float carefulPerFrame; /* de combient l'entité gagne de la vie par frame (si enableHealth == true et sick == false)*/
	float damagePerFrame; /* de combient l'entité perd de la vie par frame (si enableHealth == true et sick == true)*/

	TIMER *timer;

	ENTITY_FUNCTIONS *functions;

	bool collisionFromOtherEntity, collisionStepFromOtherEntity;
};

/**
 * @brief Création de la structure ENTITY
 * @param x position x de l'entité dans la map (1.0 ≡ dispTileSize)
 * @param y position y de l'entité dans la map (1.0 ≡ dispTileSize)
 * @param bodyBoxes les différentes box du corps de l'entité pout les quatres directions
 * @param stepBoxes les différentes box des pieds de l'entité pout les quatres directions
 * @param zoom grossissement de l'entité
 * @param imageName 
 * @param tileSize taille d'un tile dans le tileset
 * @param dispTileSize taille du tile à l'écran
 * @param frameBetweenEachTile nombre de frames entre chaque animation de tiles
 * @param firstTileIsStanding si le premier tile correspond à la position de pause
 * @param speed vitesse de déplacement de l'entité
 * @return ENTITY* 
 */
ENTITY *Entity_new(float x, float y, SDL_Rect bodyBoxes[4], SDL_Rect stepBoxes[4], float zoom,
	char *imageName, Sint32 tileSize, Sint32 dispTileSize, Uint16 frameBetweenEachTile, bool firstTileIsStanding, float speed, bool attack);
void Entity_free(ENTITY *entity);


/* --- Dimension de l'entitée --- */

void Entity_setDispTileSize(ENTITY *entity, Sint32 dispTileSize);
void Entity_getBodyBox(ENTITY *entity, SDL_FRect *getRect);
void Entity_getStepBox(ENTITY *entity, SDL_FRect *getRect);
bool Entity_stepCollision(ENTITY *entity, MAP *map);
bool Entity_bodyCollision(ENTITY *entity, MAP *map);

/* --- Direction et position --- */

/* modifier la position */
void Entity_setPosition(ENTITY *entity, float x, float y);
/* reccupérer le position en flottant */
void Entity_getPosition(ENTITY *entity, float *x, float *y);
/* reccupérer le position en entier */
void Entity_getMapPosition(ENTITY *entity, Uint16 *x, Uint16 *y);
/* reccupérer la position en entier de l'objet situé devant l'entité */
void Entity_getLookMapPosition(ENTITY *entity, Uint16 *x, Uint16 *y);

/* id du tile devant l'entité */
Uint16 Entity_getCollisionFacing(ENTITY *entity);

/* changer la direction */
void Entity_setDirection(ENTITY *entity, DirectionFacing direction);
/* retourner la direction de l'entité */
DirectionFacing Entity_getDirection(ENTITY *entity);
/* tourner l'entité à gauche */
DirectionFacing Entity_turnLeft(ENTITY *entity);
/* tourner l'entité à droite */
DirectionFacing Entity_turnRight(ENTITY *entity);

/* mettre l'entité en position de repos */
void Entity_standing(ENTITY *entity);

void Entity_walk(ENTITY *entity);
void Entity_autoWalk(ENTITY *entity, float distance);

bool Entity_conflict(ENTITY *entity, ENTITY *other_entity);

/* --- Vie et dégats --- */

void Entity_enableHealth(ENTITY *entity);
void Entity_disableHealth(ENTITY *entity);
void Entity_setHealth(ENTITY *entity, float health, float healthMax, float carefulPerFrame);
/* damagePerFrame = -1 pour ne pas modifier */
void Entity_setDamage(ENTITY *entity, float immediateDamage, float damagePerFrame);
/* carefulPerFrame = -1 pour ne pas modifier */
void Entity_setCareful(ENTITY *entity, float immediateCareful, float carefulPerFrame);
/* Renvoie true si la vie de l'entité est à 0 */
bool Entity_isDead(ENTITY *entity);

/* --- Mise à jour et affichage --- */

/* réception des évènements */
void Entity_event(ENTITY *entity);

/* mise à jour de l'entité */
void Entity_update(ENTITY *entity, MAP *map);

/* affichage de l'entité */
void Entity_draw(ENTITY *entity, CAMERA *camera, bool drawBox);

/* affichage des éléments graphique de l'entité à afficher au premier plan */
void Entity_drawFrontOf(ENTITY *entity, CAMERA *camera, bool drawBox);

#endif /* __ENTITY_H__ */