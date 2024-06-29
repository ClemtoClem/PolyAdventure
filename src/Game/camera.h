#ifndef __CAMERA_H__
#define __CAMERA_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Camera permet de gerer la parite visible de la map à l'ecran
 */

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../Utils/utils.h"
#include "../Core/sound_graphics.h"

typedef struct t_CAMERA {
	Sint32 x, y;
	Uint32 width, height;
} CAMERA;

CAMERA *Camera_new();
void Camera_free(CAMERA *camera);

/* positionne la camera par rapport à la position centrale x,y */
void Camera_goto(CAMERA *camera, float x, float y, Uint16 dispTileSize, int mapWidth, int mapHeight);

#endif /* __CAMERA_H__*/