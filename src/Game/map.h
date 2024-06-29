#ifndef __MAP_H__
#define __MAP_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Gestion et affichage de la map
 */

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "../Core/sound_graphics.h"
#include "camera.h"
#include "tileset.h"
#include "../Utils/array.h"
#include "../Utils/configuration_file.h"

typedef struct {
	ARRAY *setting;
	Uint16 width, height; /* taille de la map */
	Uint8 tracings; /** nombre de calques supperposés */
	Uint8 nbTracingsInFrontOf; /** nombre de calques supperposés à afficher devant les entités */

	Uint16 backgroundTile; /** id du sprites en arrière plan */
	Uint16 ***graphic_buffer; /** id des sprites dans la tilemap */
	Uint16 **collision_buffer; /** buffer pour la collision */

	bool isLoad, zeroIsNotTile;
} MAP;

/* Création de la structure MAP */
MAP *Map_new();
/* Destruction de la structure MAP*/
void Map_free(MAP *map);
/* Destruction de graphic_buffer */
void Map_clean(MAP *map);
/* Remplissage de graphic_buffer à partir d'un fichier de configuration (.conf) et d'un fichier map (.map) */
bool Map_load(MAP *map, char *filename, int nbTileTileset);

/* Affichage de la map */
void Map_draw(MAP *map, CAMERA *camera, TILESET *tileset, Uint16 dispTileSize);

/* Affichage des deux calques de la map en premier plan */
void Map_drawFrontOf(MAP *map, CAMERA *camera, TILESET *tileset, Uint16 dispTileSize);

#endif /* __MAP_H__ */