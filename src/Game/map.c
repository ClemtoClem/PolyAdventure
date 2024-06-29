#include "map.h"

MAP *Map_new() {
	MAP *map = calloc(1, sizeof(MAP));
	if (map == NULL) {
		LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure MAP\n");
		return NULL;
	}
	
	map->isLoad = false;
	map->setting = Array_new(NO_LIMITED);
	return map;
}

void Map_free(MAP *map) {
	if (map == NULL) {
		Map_clean(map);
		free(map);
	}
}

void Map_clean(MAP *map) {
	if (map != NULL) {
		Uint8 k;
		Uint16 j;
		Array_clear(map->setting, true);
		if (map->graphic_buffer != NULL) {
			for (k = 0; k < map->tracings; k++) {
				for (j = 0; j < map->height; j++) {
					free(map->graphic_buffer[k][j]);
				}
				free(map->graphic_buffer[k]);
			}
			free(map->graphic_buffer);
		}
		if (map->collision_buffer != NULL) {
			for (j = 0; j < map->height; j++) {
				free(map->collision_buffer[j]);
			}
			free(map->collision_buffer);
		}
		map->graphic_buffer = NULL;
		map->collision_buffer = NULL;
	}
}

bool Map_load(MAP *map, char *filename, int nbTileTileset) {
	bool result = false;
	char *path_file_name = NULL;
	if (map != NULL) {
		map->isLoad = false;

		Uint8 tracings, k;
		Uint16 idTile, idCollision, tileIdOffset, tileCollisionIdOffset;
		Uint16 width, height, j, i;
		FILE *file;
		
		Map_clean(map);
		
		LOG_DEBUG("Array_loadConfigurationFile(%s)\n", filename);

		asprintf(&path_file_name, "./res/map/%s.conf", filename);
		if (!Array_loadConfigurationFile(map->setting, path_file_name)) {
			asprintf(&path_file_name, "../res/map/%s.conf", filename);
			if (!Array_loadConfigurationFile(map->setting, path_file_name)) {
				LOG_ERROR(ErrorType_FILE_ACCESS, "Fichier conf introuvable\n");
				goto EndMapLoad;
			}
		}
		LOG_DEBUG("Ouverture %s réussie\n", filename);

		width = Array_getUint16(map->setting, "width", 0);
		height = Array_getUint16(map->setting, "height", 0);
		tracings = Array_getUint8(map->setting, "tracings", 0);
		map->zeroIsNotTile = Array_getBool(map->setting, "zeroIsNotTile", false);
		map->backgroundTile = Array_getUint16(map->setting, "backgroundTile", 1);
		map->nbTracingsInFrontOf = Array_getUint8(map->setting, "nbTracingsInFrontOf", 1);
		tileIdOffset = Array_getUint16(map->setting, "tileIdOffset", 0);
		tileCollisionIdOffset = Array_getUint16(map->setting, "tileCollisionIdOffset", 0);
		if (width == 0 || height == 0 || tracings == 0) {
			LOG_ERROR(ErrorType_ERROR, "Variables incorrectes dans le ficher de configuration\n");
			goto EndMapLoad;
		}

		asprintf(&path_file_name, "./res/map/%s.map", filename);
		file = fopen(path_file_name, "r");
		if (file == NULL) {
			asprintf(&path_file_name, "../res/map/%s.map", filename);
			file = fopen(path_file_name, "r");
		}
		if (file == NULL) {
			LOG_ERROR(ErrorType_FILE_ACCESS, "Echec, Fichier \"%s\" introuvable\n", path_file_name);
			goto EndMapLoad;
		}

		map->width = width;
		map->height = height;
		map->tracings = tracings;

		if (map->graphic_buffer == NULL) {
			map->graphic_buffer = (Uint16 ***) calloc(map->tracings, sizeof(Uint16 **));
			for (k = 0; k < map->tracings; k++) {
				map->graphic_buffer[k] = (Uint16 **) calloc(map->height, sizeof(Uint16 *));
				for (j = 0; j < map->height; j++) {
					map->graphic_buffer[k][j] = (Uint16 *) calloc(map->width, sizeof(Uint16));
				}
			}
		}
		
		for (k = 0; k < map->tracings; k++) {
			for (j = 0; j < map->height; j++) {
				for (i = 0; i < map->width; i++) {
					fscanf(file, "%hu", &idTile);
					if(getc(file) == EOF && (k != map->tracings-1 || j != map->height-1 || i != map->width-1)) goto EndMapLoad;
					map->graphic_buffer[k][j][i] = (idTile != 0) ? idTile - tileIdOffset : 0;
				}
			}
			getc(file);
		}

		fclose(file);

		/* Reccupération de la map de colision */
		
		asprintf(&path_file_name, "./res/map/%s.cmap", filename);
		file = fopen(path_file_name, "r");
		if (file == NULL) {
			asprintf(&path_file_name, "../res/map/%s.cmap", filename);
			file = fopen(path_file_name, "r");
		}
		if (file == NULL) {
			LOG_ERROR(ErrorType_FILE_ACCESS, "Echec, Fichier \"%s\" introuvable\n", path_file_name);
			goto EndMapLoad;
		}

		if (map->collision_buffer == NULL) {
			map->collision_buffer = (Uint16 **) malloc(map->height*sizeof(Uint16 *));
			for (j = 0; j < map->height; j++) {
				map->collision_buffer[j] = (Uint16 *) calloc(map->width, sizeof(Uint16));
			}
		}

		for (j = 0; j < map->height; j++) {
			for (i = 0; i < map->width; i++) {
				fscanf(file, "%hu", &idCollision);
				if(getc(file) == EOF && (j != map->height-1 || i != map->width-1)) goto EndMapLoad;
				
				map->collision_buffer[j][i] = (idCollision != 0) ? idCollision - tileCollisionIdOffset : 0;
			}
		}

		fclose(file);

		/* ------ TEST ------ */
/*
		for (k = 0; k < map->tracings; k++) {
			printf("----- %d -----\n", k);
			for (j = 0; j < map->height; j++) {
				for (i = 0; i < map->width; i++) {
					printf("%d ", map->graphic_buffer[k][j][i]);
				}
				printf("\n");
			}
		}
*/
		/* ------------------- */

		map->isLoad = true;
		result = true;
	}
EndMapLoad:
	if (path_file_name != NULL) free(path_file_name);
	return result;
}

void Map_draw(MAP *map, CAMERA *camera, TILESET *tileset, Uint16 dispTileSize) {
	if (map != NULL && map->isLoad == true && camera != NULL && tileset != NULL) {
		Sint32 x, y, i, j, i1, j1, i2, j2, decalX, decalY, id;
		Uint8 k;
		i1 = (camera->x / dispTileSize);
		j1 = (camera->y / dispTileSize);
		i2 = (camera->x + camera->width) / dispTileSize;
		j2 = (camera->y + camera->height) / dispTileSize;

		decalX = camera->x % dispTileSize;
		decalY = camera->y % dispTileSize;

		for (j = j1; j <= j2; j++) {
			for (i = i1; i <= i2; i++) {
				if (j >= 0 && j < map->height && i >= 0 && i < map->width) {
					x = (i-i1) * dispTileSize - decalX;
					y = (j-j1) * dispTileSize - decalY;
					id = map->backgroundTile;
					if (map->zeroIsNotTile == true) id --;
					if (id != 0)
						Tileset_drawTile(tileset, x, y, id);
					for (k = 0; k < map->tracings - map->nbTracingsInFrontOf; k++) {
						id = map->graphic_buffer[k][j][i];
						if (map->zeroIsNotTile == true && id != 0) id --;
						if (id != 0)
							Tileset_drawTile(tileset, x, y, id);
					}
				}
			}
		}
	}
}

void Map_drawFrontOf(MAP *map, CAMERA *camera, TILESET *tileset, Uint16 dispTileSize) {
	if (map != NULL && map->isLoad == true && camera != NULL && tileset != NULL) {
		Sint32 x, y, i, j, i1, j1, i2, j2, decalX, decalY, id;
		Uint8 k;
		i1 = (camera->x / dispTileSize);
		j1 = (camera->y / dispTileSize);
		i2 = (camera->x + camera->width) / dispTileSize;
		j2 = (camera->y + camera->height) / dispTileSize;

		decalX = camera->x % dispTileSize;
		decalY = camera->y % dispTileSize;

		for (j = j1; j <= j2; j++) {
			for (i = i1; i <= i2; i++) {
				if (j >= 0 && j < map->height && i >= 0 && i < map->width) {
					x = (i-i1) * dispTileSize - decalX;
					y = (j-j1) * dispTileSize - decalY;
					for (k = map->tracings - map->nbTracingsInFrontOf; k < map->tracings; k++) {
						id = map->graphic_buffer[k][j][i];
						if (map->zeroIsNotTile == true && id != 0) id --;
						if (id != 0)
							Tileset_drawTile(tileset, x, y, id);
					}
				}
			}
		}
	}
}
