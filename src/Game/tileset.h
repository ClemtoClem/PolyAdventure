#ifndef __TILESET_H__
#define __TILESET_H__

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../Core/sound_graphics.h"
#include "camera.h"

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Gestion de l'affichage des tiles à l'écran à partir d'une image
 */

typedef struct t_TILESET {
	IMAGE *image;
	Uint16 tileSize;
	Uint16 nbTilesWidth, nbTilesHeight;
	SDL_Rect srcRect, destRect;
} TILESET;

TILESET *Tileset_new(char *imageName, Uint16 tileSize, Uint16 dispTileSize);
void Tileset_free(TILESET *tileset);

void Tileset_update(TILESET *tileset, Uint16 dispTileSize);
void Tileset_drawTile(TILESET *tileset, Sint32 x, Sint32 y, Uint16 idTile);

#endif /* __TILESET_H__*/