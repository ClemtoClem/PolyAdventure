#include "tileset.h"

TILESET *Tileset_new(char *imageName, Uint16 tileSize, Uint16 dispTileSize) {
	TILESET *tileset = NULL;
	IMAGE *image = SG_loadImage(imageName);
	if (image != NULL) {
		tileset = (TILESET *) malloc(sizeof(TILESET));
		if (tileset != NULL) {
			tileset->image = image;

			tileset->tileSize	   = tileSize;
			tileset->nbTilesWidth   = image->w / tileSize;
			tileset->nbTilesHeight  = image->h / tileSize;

			tileset->srcRect.w	= tileset->srcRect.h = tileSize;
			tileset->destRect.w	= tileset->destRect.h = dispTileSize;
		}
	}
	return tileset;
}

void Tileset_free(TILESET *tileset) {
	if (tileset != NULL) {
		free(tileset);
	}
}

void Tileset_update(TILESET *tileset, Uint16 dispTileSize) {
	if (tileset != NULL) {
		tileset->destRect.w = tileset->destRect.h = dispTileSize;
	}
}

void Tileset_drawTile(TILESET *tileset, Sint32 x, Sint32 y, Uint16 idTile) {
	tileset->srcRect.x = (idTile % tileset->nbTilesWidth) * tileset->srcRect.w;
	tileset->srcRect.y = (idTile / tileset->nbTilesWidth) * tileset->srcRect.h;

	tileset->destRect.x = x;	// Set destination rectangle X
	tileset->destRect.y = y;	// Set destination rectangle Y
	SG_render(tileset->image->texture, &tileset->srcRect, &tileset->destRect); // Render the tile
}