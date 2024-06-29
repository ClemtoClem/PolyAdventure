#include "camera.h"

CAMERA *Camera_new() {
	CAMERA *camera = malloc(sizeof(CAMERA));
	if (camera != NULL) {
		camera->x = 0;
		camera->y = 0;
		camera->width = SG_getWidth();
		camera->height = SG_getHeight();
	}
	return camera;
}

void Camera_free(CAMERA *camera) {
	if (camera != NULL) {
		free(camera);
	}
}

void Camera_goto(CAMERA *camera, float x, float y, Uint16 dispTileSize, int mapWidth, int mapHeight) {
	if (camera != NULL) {
		camera->x = (Sint32) ((float) x*dispTileSize) - camera->width/2;
		camera->y = (Sint32) ((float) y*dispTileSize) - camera->height/2;

		if (camera->x < 0) camera->x = 0;
		if (camera->y < 0) camera->y = 0;
		if (camera->x >= mapWidth*dispTileSize - camera->width)
			camera->x = mapWidth*dispTileSize - camera->width -1;
		if (camera->y >= mapHeight*dispTileSize - camera->height)
			camera->y = mapHeight*dispTileSize - camera->height -1;
	}
}