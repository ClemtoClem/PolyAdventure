#ifndef __APPLICATION_H__
#define __APPLICATION_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Application (Preface + Menu + Game)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "../Utils/colors.h"
#include "../Utils/logger.h"
#include "../Utils/array.h"
#include "../Utils/configuration_file.h"
#include "../Utils/timer.h"
#include "../Core/gui.h"
#include "camera.h"
#include "tileset.h"
#include "map.h"
#include "save_manager.h"

#include "../Entities/entities_manager.h"
#include "../Entities/player.h"
#include "../Entities/mob_blob.h"
#include "../Entities/mob_goblin.h"
#include "../Entities/mob_dragon.h"
#include "../Entities/mob_skeleton.h"
#include "../Entities/pnj.h"

#define GAME_VERSION "0.3.0"

#define MIN_ZOOM 16
#define MAX_ZOOM 128

#define ID_PREFACE 1
#define ID_MENU 2
#define ID_GAME 3

#define BACKGROUND_COLOR (120 << 24) + (30 << 16) + (45 << 8) + 0xFF

/* Liste des flags du timer */
#define COLOR_COUNTER 1
#define CLIGNOTE_COUNTER 2

/* Liste des flags d'évènements */
#define OFFSET_TEXT_PRESSED 4

#define ENTITY_DEFAULT_ZOOM 1.5

/* preface */
typedef struct {
    LABEL *debuglabel, *title, *message;
    Uint32 colorMax, colorMin;
    bool incremente, load;
} PREFACE;

/* menu */
typedef struct {
	PANEL *panel;
	bool load;
} MENU;

/* game */
typedef struct {
	char currentMapName[100];
	TILESET *tileset;
	ENTITY *player;
	CAMERA *camera;
	MAP *map;
	PANEL *panel;

	Sint32 tileSize, dispTileSize;
	
	bool gameDebugMode, pause, load, is_prepared;
} GAME;

typedef struct t_APPLICATION {
	ARRAY *setting;
	Uint8 id;
    bool debugMode;
    bool quit, reload;

	IMAGE *background;
	TIMER *timer;

	PREFACE preface;
	MENU menu;
	GAME game;

	Uint32 fps; /* frame/ms */
} APPLICATION;

extern APPLICATION *application;

bool Application_init(bool debugMode);
bool Application_free();
void Application_mainloop();

/* --- PREFACE --- */

void Preface_load();
void Preface_free();
void Preface_event();
void Preface_update();
void Preface_draw();

/* --- MENU --- */

void Menu_load();
void Menu_free();
void Menu_event();
void Menu_update();
void Menu_draw();

/* --- GAME --- */

bool Game_load();
void Game_prepare();
void Game_loadEntities();
void Game_free();
void Game_event();
void Game_update();
void Game_gameDebugMode();
void Game_draw();

#endif /* __APPLICATION_H__ */