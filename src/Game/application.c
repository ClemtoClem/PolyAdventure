#include "application.h"

APPLICATION *application = NULL;

bool Application_init(bool debugMode) {
	bool success = false;
	if (application == NULL) {
		ARRAY *setting = NULL;
		TIMER *timer = NULL;
		
		setting = (ARRAY *) Array_new(NO_LIMITED);
		if (setting == NULL) {
			LOG_ERROR(ErrorType_FAULT, "Echec d'allocation de la structure ARRAY\n");
			goto EndAppInit;
		}

		char screenTitle[100];
		Sint32 screenWidth, screenHeight;
		
		if (!Array_loadConfigurationFile(setting, "./res/config.conf")) {
			LOG_ERROR(ErrorType_ERROR, "Don't open config.conf\n");
			goto EndAppInit;
		}
		Array_getString(setting, "screenTitle", screenTitle, "Game with SDL2", 100);
		screenWidth = Array_getSint32(setting, "screenWidth", 640);
		screenHeight = Array_getSint32(setting, "screenHeight", 480);

		timer = Timer_new(true);
		if (timer == NULL) {
			LOG_ERROR(ErrorType_FAULT, "Echec d'allocation de la structure TIMER\n");
			goto EndAppInit;
		}

		if (!SG_init(screenTitle, screenWidth, screenHeight, WHITE)) {
			LOG_ERROR(ErrorType_FAULT, "Echec de l'initialisation de la structure SOUND_GRAPHICS\n");
			goto EndAppInit;
		}

		SG_openAudio();
		SG_loadMusic("cave.ogg");
		SG_loadMusic("classic.ogg");
		SG_loadMusic("club.ogg");
		SG_loadMusic("glass.ogg");
		SG_loadMusic("grassy.ogg");
		SG_loadMusic("main.ogg");
		SG_loadMusic("win.ogg");

		if (!Input_init(false)) {
			LOG_ERROR(ErrorType_ERROR, "Echec de l'initialisation de la structure INPUT\n");
			goto EndAppInit;
		}

		if (!SaveFile_init()) {
			LOG_ERROR(ErrorType_ERROR, "Echec de l'initialisation de la structure SAVE_MANAGER\n");
			goto EndAppInit;
		}

		application = calloc(1, sizeof(APPLICATION));
		if (application == NULL) {
			LOG_ERROR(ErrorType_FAULT, "Echec de la création de la structure APPLICATION\n");
			goto EndAppInit;
		}
		application->background = SG_loadImage("underwater-light-beams.png");
		application->debugMode = debugMode;
		
		success = true;
EndAppInit:
		if (success == true ) {
			application->setting = setting;
			application->timer = timer;

			LOG_SUCCESS("INIT SUCCESS\n");
		} else {
			if (setting != NULL) Array_free(setting, true);
			if (timer != NULL) Timer_free(timer);

			SG_free();
			Input_free();
			SaveFile_free();
			application = NULL;

			LOG_ERROR(ErrorType_ERROR, "INIT FAILED\n");
		}
	} else {
		success = true;
	}
	return success;
}

bool Application_free() {
	bool reload = false;
	if (application != NULL) {
		reload = application->reload;
		if (application->setting != NULL) Array_free(application->setting, true);
		if (application->timer != NULL) Timer_free(application->timer);

		SG_closeAudio();
		SG_free();
		Input_free();
		SaveFile_free();
		EntitiesManager_free();

		Preface_free();
		Menu_free();
		//Game_free();

		free(application);
		application = NULL;
		LOG_SUCCESS("QUIT SUCCESS\n");
	}
	return reload;
}

#define MAX_FPS 20
void Application_mainloop() {
	if (application != NULL) {
		Uint32 currentTime = 0, afterTime = 0;

		application->quit = false;
		application->reload = false;
		application->id = ID_PREFACE;

		while(!application->quit && !application->reload) {
			Input_event();

			if (Input_wasUserEvent(LOAD_PREFACE_EVENT)) {
				Input_pushEvent(USER_EVENT, 0);
				application->id = ID_PREFACE;
				application->preface.load = false;
			}

			if (Input_wasUserEvent(LOAD_MENU_EVENT)) {
				Input_pushEvent(USER_EVENT, 0);
				application->menu.load = false;
				application->id = ID_MENU;
			}

			if (Input_wasUserEvent(LOAD_GAME_EVENT)) {
				Input_pushEvent(USER_EVENT, 0);
				application->game.load = false;
				application->game.is_prepared = false;
				application->id = ID_GAME;
			}

			if (application->id == ID_PREFACE) {
				if (!application->preface.load) Preface_load();
				Preface_event();
			} else if (application->id == ID_MENU) {
				if (!application->menu.load) {
					Menu_load();
					SG_stopMusic();
					SG_playMusic("classic.ogg", -1);
				}
				Menu_event();
			} else if (application->id == ID_GAME) {
				/* charger les ressources du jeu si cela pas été effectué */
				if (!application->game.load) {
					if (!Game_load()) {
						LOG_ERROR(ErrorType_ERROR, "Error lodding game\n");
						application->quit = true;
					} else {
						LOG_DEBUG("Game prepare\n");
						Game_prepare();
					}
				}
				Game_event();
			}
			
			Timer_update(application->timer);

			if (application->id == ID_PREFACE) {
				Preface_update();
				Preface_draw();
			} else if (application->id == ID_MENU) {
				Menu_update();
				Menu_draw();
			} else if (application->id == ID_GAME) {
				Game_update();
				Game_draw();
			}

			/* regular delay */
			currentTime = SDL_GetTicks(); // Temps de fin d'éxécution (depuis début boucle)
			application->fps = currentTime - afterTime;
			if (application->fps < MAX_FPS) {
				SDL_Delay(MAX_FPS - application->fps); // on repose le CPU
			}
			afterTime = SDL_GetTicks(); // Temps de fin de la boucle (= début de la suivante !)
		}
	}
}

/* --- PREFACE --- */

void Preface_load() {
	if (application != NULL) {
		PREFACE *preface = &application->preface;

		preface->colorMax = WHITE;
		preface->colorMin = 0x0066CCFF;
		preface->incremente = true;

		if (application->debugMode && preface->debuglabel != NULL) {
			preface->debuglabel = Label_new(NULL, "DebugLabel", 0, 5, SG_getWidth(), 0, "Debug Mode", "police04B.ttf", 20, ORANGE);
			preface->debuglabel->Hcentered = true;
		}
		
		if (preface->title == NULL) {
			preface->title = Label_new(NULL, NULL, 0, 50, SG_getWidth(), 0, SG_getTitle(), "police04B.ttf", 45, preface->colorMin);
			preface->title->isEnable = true;
			preface->title->Hcentered = true;
		}
		if (preface->message == NULL) {
			preface->message = Label_new(NULL, NULL, 0, SG_getHeight() - 50, SG_getWidth(), 0, "Press Enter", "police04B.ttf", 20, WHITE);
			preface->message->isEnable = true;
			preface->message->Hcentered = true;
		}

		Timer_addCounter(application->timer, COLOR_COUNTER, 2, true, true);
		Timer_addCounter(application->timer, CLIGNOTE_COUNTER, 50, true, true);
		preface->load = true;
	}
}

void Preface_free() {
	if (application != NULL) {
		PREFACE *preface = &application->preface;

		if (preface->title != NULL)   Label_free(preface->title);
		if (preface->message != NULL) Label_free(preface->message);
	}
}

void Preface_event() {
	if (application != NULL) {
		//PREFACE *preface = &application->preface;

		if (Input_quitEvent() || Input_wasKeyReleased(SDLK_ESCAPE)) {
			application->quit = true;
		}
		if (Input_wasKeyReleased(SDLK_RETURN, SDLK_RETURN2)
			|| Input_wasMousePressed(SDL_BUTTON_LEFT)) {
			Input_pushEvent(USER_EVENT, LOAD_MENU_EVENT);
		}
	}
}

void Preface_update() {
	if (application != NULL) {
		PREFACE *preface = &application->preface;

		if (Timer_counterIsFinish(application->timer, COLOR_COUNTER)) {
			Uint8 R, G, B, R1, G1, B1, R2, G2, B2;;
			R = (preface->title->colorFont & RED_MASK) >> 24;
			G = (preface->title->colorFont & GREEN_MASK) >> 16;
			B = (preface->title->colorFont & BLUE_MASK) >> 8;

			R1 = (preface->colorMax & RED_MASK) >> 24;
			G1 = (preface->colorMax & GREEN_MASK) >> 16;
			B1 = (preface->colorMax & BLUE_MASK) >> 8;

			R2 = (preface->colorMin & RED_MASK) >> 24;
			G2 = (preface->colorMin & GREEN_MASK) >> 16;
			B2 = (preface->colorMin & BLUE_MASK) >> 8;

			if (preface->incremente) {
				if (R != R1) R += (R1 > R) - (R1 < R);
				if (G != G1) G += (G1 > G) - (G1 < G);
				if (B != B1) B += (B1 > B) - (B1 < B);
			} else {
				if (R != R2) R += (R2 > R) - (R2 < R);
				if (G != G2) G += (G2 > G) - (G2 < G);
				if (B != B2) B += (B2 > B) - (B2 < B);
			}

			preface->title->colorFont = (R<<24) + (G<<16) + (B<<8) + 255;
			
			if (preface->title->colorFont == preface->colorMin || preface->title->colorFont == preface->colorMax) {
				preface->incremente = !preface->incremente;
			}
		}

		if (Timer_counterIsFinish(application->timer, CLIGNOTE_COUNTER)) {
			preface->message->isEnable = !preface->message->isEnable;
		}
	}
}

void Preface_draw() {
	if (application != NULL) {
		PREFACE *preface = &application->preface;

		SDL_Rect win = {0, 0, SG_getWidth(), SG_getHeight()};
		SG_clear();

		SG_render(application->background->texture, NULL, &win);

        if (application->debugMode)
			Label_draw(preface->debuglabel, &win);
		Label_draw(preface->title, &win);
		Label_draw(preface->message, &win);

		SG_present();
	}
}

/* --- MENU --- */

void Menu_load() {
	if (application != NULL) {
		MENU *menu = &application->menu;

		if (menu->panel == NULL) {
			/* panel sans image */
			menu->panel = (PANEL *) Panel_new(NULL, "MenuPanel", 15, 15, SG_getWidth() - 30, SG_getHeight() - 30, NULL, 3);

			// initialisation de la page 0 du panel (menu principal)
			Panel_setWorkPage(menu->panel, 0);
			LABEL *title;
            title = Label_new(menu->panel, "TitleLabel", 0, 35, menu->panel->rect.w, 0, SG_getTitle(), "police04B.ttf", 45, BLUE_ELECTRICAL);
            title->Hcentered = true;
			Button_new(menu->panel, "PlayButton", menu->panel->rect.w/2 - 100, 140, 200, 50, "button_blue.png",
				"Play", "toon_around.ttf", 25, GRAY(A0), GRAY(F0), OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			Button_new(menu->panel, "CreditButton",  menu->panel->rect.w/2 - 100, 240, 200, 50, "button_blue.png",
				"Credit", "toon_around.ttf", 25, GRAY(A0), GRAY(F0), OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			Button_new(menu->panel, "QuitButton", menu->panel->rect.w/2 - 100, 340, 200, 50, "button_blue.png",
				"Quit", "toon_around.ttf", 25, GRAY(A0), GRAY(F0), OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			if (application->debugMode) {
				title = Label_new(menu->panel, "DebugLabel", menu->panel->rect.w - 155, menu->panel->rect.h - 180, menu->panel->rect.w, 0, "Debug Mode", "police04B.ttf", 20, ORANGE);
				Button_new(menu->panel, "ReloadButton", menu->panel->rect.w - 130, menu->panel->rect.h - 140, 120, 50, "button_orange.png",
					"Reload", "toon_around.ttf", 25, 0xA0A0A0FF, 0xF0F0F0FF, OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			}

			// initialisation de la page 1 du panel (menu ouvrir partie)
			//char pseudo[30], PartButtonName[30], ConfigButtonName[30], TrashButtonName[30];
			Panel_setWorkPage(menu->panel, 1);
			title = Label_new(menu->panel, "TitleLabel", 0, 35, menu->panel->rect.w, 0, "Choose your part", "police04B.ttf", 45, 0x0D63FEFF);
			title->Hcentered = true;

			Button_new(menu->panel, "FirstButton", menu->panel->rect.w/2 - 150, 140, 200, 50, "button_blue.png",
				"First part", "toon_around.ttf", 25, GRAY(A0), GRAY(F0), OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			MotifButton_new(menu->panel, "FirstConfigButton", menu->panel->rect.w/2 + 60, 140, 50, 50,
				"button_orange.png", "config.png", OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			MotifButton_new(menu->panel, "FirstTrashButton", menu->panel->rect.w/2 + 120, 140, 50, 50,
				"button_red.png", "trash.png", OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);

			MotifButton_new(menu->panel, "ReturnButton", 10, menu->panel->rect.h - 64, 50, 50,
				"button_blue.png", "arrow_left.png", OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);

			// initialisation de la page 0 du panel (menu principal)
			Panel_setWorkPage(menu->panel, 2);
			title = Label_new(menu->panel, "TitleLabel", 0, 10, menu->panel->rect.w, 0, SG_getTitle(), "police04B.ttf", 45, 0x0D63FEFF);
			title->Hcentered = true;

			MotifButton_new(menu->panel, "ReturnButton", 10, menu->panel->rect.h - 64, 50, 50,
				"button_blue.png", "arrow_left.png", OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
		}
/*
		SaveFile_makeListPseudo();
		SAVE_MANAGER *save = SaveFile_get();
		DATA *data = save->listPseudo->first;
		Uint32 i = 1;
		while (data != NULL) {
			data = data->next;
		}
*/
		Panel_setVisiblePage(menu->panel, 0);
		Panel_enable(menu->panel);
		menu->load = true;
	}
}

void Menu_free() {
	if (application != NULL) {
		MENU *menu = &application->menu;

		if (menu->panel == NULL) {
			Panel_free(menu->panel);
		}
	}
}

void Menu_event() {
	if (application != NULL) {
		MENU *menu = &application->menu;

		if (Input_quitEvent()) {
			application->quit = true;
		}

		Panel_event(menu->panel);

		// Event panel n°0
		if (Panel_getVisiblePage(menu->panel) == 0) {
			if (Widget_onReleased(menu->panel, "PlayButton")) {
				Panel_setVisiblePage(menu->panel, 1);
			} else if (Widget_onReleased(menu->panel, "CreditButton")) {
				Panel_setVisiblePage(menu->panel, 2);
			} else if (Widget_onReleased(menu->panel, "QuitButton")) {
				application->quit = true;
			} else if (Widget_onReleased(menu->panel, "ReloadButton")) {
				application->reload = true;
            }
		}

		// Event panel n°1
		else if (Panel_getVisiblePage(menu->panel) == 1) {
			if (Widget_onReleased(menu->panel, "ReturnButton")) {
				Panel_setVisiblePage(menu->panel, 0);
			}
			if (Widget_onReleased(menu->panel, "FirstButton")) {
				Input_pushEvent(USER_EVENT, LOAD_GAME_EVENT);
			}
		}

		// Event panel n°2
		else if (Panel_getVisiblePage(menu->panel) == 2) {
			if (Widget_onReleased(menu->panel, "ReturnButton")) {
				Panel_setVisiblePage(menu->panel, 0);
			}
		}
	}
}

void Menu_update() {
	if (application != NULL) {
		MENU *menu = &application->menu;
		Panel_update(menu->panel);
	}
}

void Menu_draw() {
	if (application != NULL) {
		MENU *menu = &application->menu;

		SDL_Rect win = {0, 0, SG_getWidth(), SG_getHeight()};
		SG_clear();

		SG_render(application->background->texture, NULL, &win);

		Panel_draw(menu->panel, NULL);

		if (menu->panel->visiblePage == 2) {
			SDL_Rect rect1 = { 20, 80, SG_getWidth()-40, 130 + 25 * 7};
			SDL_Rect rect2 = { 25, 85, SG_getWidth()-50, 120 + 25 * 7};
			
			TTF_Font *font = SG_loadFont("arial.ttf", 20);

			SG_drawColor(MAGENTA);
			SG_drawRect(&rect1, true);
			SG_drawColor(WHITE);
			SG_drawRect(&rect2, true);
			SG_printf(false, 40, 110, font, GRAY(70), "Game version : %s", GAME_VERSION);
			SG_printf(false, 40, 135, font, GRAY(70), "Company : Les IESE d'abord");
			SG_printf(false, 40, 160, font, GRAY(70), "Program owner : ");
			SG_printf(false, 85, 185, font, GRAY(70), "CHARRIERE Clement");
			SG_printf(false, 40, 210, font, GRAY(70), "Authors : ");
			SG_printf(false, 85, 235, font, GRAY(70), "BATTE Theodore, l'homme qui ne craque jamais");
			SG_printf(false, 85, 260, font, GRAY(70), "BOUCHE Romain, goofy dans l'ame, goofy dans le sang");
			SG_printf(false, 85, 285, font, GRAY(70), "CHARBONNIER Arthur, bicolor dans les yeux");
			SG_printf(false, 85, 310, font, GRAY(70), "CHARRIERE Clement, le programmeur opportun");
			SG_printf(false, 85, 335, font, GRAY(70), "COPPOLA Valentin, l'astre entrepreneur");
		}

		SG_present();
	}
}

/* --- GAME --- */

bool Game_load() {
	if (application != NULL) {
		GAME *game = &application->game;

		if (game->panel == NULL) {
			char screenTitle[100], tilesetImgName[100];

			//Array_print(application->setting, 4000);
			Array_getString(application->setting, "screenTitle", screenTitle, "Game", 100);
			game->dispTileSize = Array_getSint32(application->setting, "dispTileSize", 64); /* on normalise la taille de tous les tiles à afficher */
			Array_getString(application->setting, "tilesetImgName", tilesetImgName, "tileset.png", 100);
			game->tileSize = Array_getSint32(application->setting, "tileSize", 16);
			Array_getString(application->setting, "firstMapName", game->currentMapName, "world", 100);
			if (game->dispTileSize < MIN_ZOOM) game->dispTileSize = MIN_ZOOM;
			if (game->dispTileSize > MAX_ZOOM) game->dispTileSize = MAX_ZOOM;

			/* on active l'affichage des log si on est en mode débugger, sinon l'utilisateur pourra l'activer en appuyant sur la touche <i> */
			game->gameDebugMode = application->debugMode;

			game->tileset = (TILESET *) Tileset_new(tilesetImgName, game->tileSize, game->dispTileSize);
			if (game->tileset == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure TILESET\n"); return false; }

			if (!EntitiesManager_init(400)) {
				LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure ENTITIES_MANAGER\n"); return false;
			}
			
			game->camera = (CAMERA *) Camera_new();
			if (game->camera == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure CAMERA\n"); return false; }

			game->map = (MAP *) Map_new();
			if (game->map == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure MAP\n"); return false; }

			/* ------ TEST ------ */
/*
			SDL_Rect bodyBox = {9, 4, 47, 58};
			SDL_Rect stepBox = {18, 50, 28, 12};
			game->player = (ENTITY *) Player_new(20.5, 17.5, &bodyBox, &stepBox, 1.75, "polyman.png", 64, game->dispTileSize, 5, true, 0.05);
			if (game->player == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure PLAYER\n"); return false; }
			Entity_setDirection(game->player, SOUTH);
			EntitiesManager_addEntity(game->player);

			SDL_Rect dragon_bodyBox = {5, 4, 57, 55};
			SDL_Rect dragon_stepBox = {17, 44, 30, 14};
			
			SDL_Rect blob_bodyBox = {11, 15, 41, 37};
			SDL_Rect blob_stepBox = {11, 41, 41, 11};

			ENTITY *entity;

			entity = Mob_Dragon_new(22.5, 20.5, &dragon_bodyBox, &dragon_stepBox, 2, "dragon.png", 64, game->dispTileSize, 5, true, 0.025);
			if (entity == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure ENTITY\n"); return false; }
			Entity_setDirection(entity, SOUTH);
			EntitiesManager_addEntity(entity);

			entity = Mob_Dragon_new(20.5, 22.5, &dragon_bodyBox, &dragon_stepBox, 2, "dragon.png", 64, game->dispTileSize, 5, true, 0.025);
			if (entity == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure ENTITY\n"); return false; }
			Entity_setDirection(entity, EAST);
			EntitiesManager_addEntity(entity);

			entity = Mob_Blob_new(25.5, 19.5, &blob_bodyBox, &blob_stepBox, 2, "blob.png", 64, game->dispTileSize, 5, true, 0.025);
			if (entity == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure ENTITY\n"); return false; }
			Entity_setDirection(entity, NORTH);
			EntitiesManager_addEntity(entity);

			entity = Mob_Dragon_new(22.5, 24.5, &dragon_bodyBox, &dragon_stepBox, 2, "dragon.png", 64, game->dispTileSize, 5, true, 0.025);
			if (entity == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure ENTITY\n"); return false; }
			Entity_setDirection(entity, NORTH);
			EntitiesManager_addEntity(entity);
*/
			/* ------------------ */
			
			/* Création d'un menu */
			game->panel = (PANEL *) Panel_new(NULL, NULL,
				(float) (SG_getWidth()/2)*0.1, (float) (SG_getHeight()/2)*0.1,
				(float) SG_getWidth()*0.9, (float) SG_getHeight()*0.9, "panel.png", 3);
			if (game->panel == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure PANEL\n"); return false; }
			
			/* widgets sur toutes les pages du panel */
			Label_new(game->panel, "title", 20, 5, game->panel->rect.w - 167, 40,
				"-- Options --", "toon_around.ttf", 40, WHITE);
			MotifButton_new(game->panel, "BSave", game->panel->rect.w - 100, 5, 40, 40,
				"button_blue.png", "save.png", OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			MotifButton_new(game->panel, "BClose", game->panel->rect.w - 54, 5, 40, 40,
				"button_red.png", "cross.png", OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			
			/* initialisation de la page 0 du panel */
			Panel_setWorkPage(game->panel, 0);
			Label_new(game->panel, "numPage", game->panel->rect.w-52, game->panel->rect.h-42, 60, 40, "1/3", "toon_around.ttf", 30, WHITE);
			MotifButton_new(game->panel, "BNext", game->panel->rect.w - 162, 5, 40, 40,
				"button_blue.png", "arrow_right.png", OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			InnerBox_new(game->panel, "I", 15, 50, game->panel->rect.w - 36, game->panel->rect.h / 2 - 21,
				0, 0, game->panel->rect.w - 36, 1000, "innerBox.png");
			Button_new(game->panel, "B1", 15, game->panel->rect.h / 2 + 40, 100, 50,
				"button_blue.png", "Bouton 1", "toon_around.ttf", 25, GRAY(A0), GRAY(F0),
					OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			ToggleSwitch_new(game->panel, "T1", 15, game->panel->rect.h / 2 + 96, 100, 50,
				"toggleSwitch.png", "ON", "OFF", "toon_around.ttf", 25, GRAY(A0), GRAY(F0),
					OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED, true);
			ToggleSwitch_new(game->panel, "T2", 15, game->panel->rect.h / 2 + 156, 100, 50,
				"toggleSwitch.png", "ON", "OFF", "toon_around.ttf", 25, GRAY(A0), GRAY(F0),
					OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED, false);
			
			/* initialisation de la page 1 du panel */
			Panel_setWorkPage(game->panel, 1);
			Label_new(game->panel, "numPage", game->panel->rect.w-52, game->panel->rect.h-42, 60, 40,
				"2/3", "toon_around.ttf", 30, WHITE);
			MotifButton_new(game->panel, "BNext", game->panel->rect.w - 162, 5, 40, 40,
				"button_blue.png", "arrow_right.png", OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			MotifButton_new(game->panel, "BAfter", game->panel->rect.w - 208, 5, 40, 40,
				"button_blue.png", "arrow_left.png", OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			InnerBox_new(game->panel, "I", game->panel->rect.w/2, 50, game->panel->rect.w/2 - 21,
				game->panel->rect.h - 86, 0, 0, game->panel->rect.w/2 - 21, 1000, "innerBox.png");
			Button_new(game->panel, "B2", 15, 50, 100, 50, "button_blue.png", "Bouton 2",
				"toon_around.ttf", 25, 0xA0A0A0FF, 0xF0F0F0FF, OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			
			/* initialisation de la page 2 du panel */
			Panel_setWorkPage(game->panel, 2);
			Label_new(game->panel, "numPage", game->panel->rect.w-52, game->panel->rect.h-42,
				60, 40, "3/3", "toon_around.ttf", 30, WHITE);
			MotifButton_new(game->panel, "BAfter", game->panel->rect.w - 208, 5, 40, 40,
				"button_blue.png", "arrow_left.png", OFFSET_TEXT_PRESSED, OFFSET_TEXT_PRESSED);
			Panel_setVisiblePage(game->panel, 0);

			game->load = true;

			LOG_SUCCESS("GAME LOAD\n");
		} else {			
			game->load = true;
		}
	}
	return true;
}

void Game_prepare() {
	if (application != NULL) {
		srand(time(NULL));
		GAME *game = &application->game;
		if (game->load && !game->is_prepared) {
		
			LOG_DEBUG("Load Map\n");
			if (Map_load(game->map, game->currentMapName, game->tileset->nbTilesWidth*game->tileset->nbTilesHeight) == false) {
				LOG_ERROR(ErrorType_ERROR, "Echec du chargement de la map\n");
			}
			
			LOG_DEBUG("Load entities\n");
			Game_loadEntities();
			
			char musicName[50];
			Array_getString(game->map->setting, "musicName", musicName, "grassy.ogg", 50);
			SG_stopMusic();
			SG_playMusic(musicName, -1);

			game->is_prepared = true;
			LOG_SUCCESS("GAME IS PREPARED\n");
		}
	}
}

void Game_loadEntities() {
	if (application != NULL) {
		GAME *game = &application->game;

		ARRAY *configEntities = Array_getArray(game->map->setting, "configEntities");
		ARRAY *initEntity;
		ARRAY *infoEntities = Array_getArray(application->setting, "infoEntities");
		ARRAY *infoEntity;

		DATA *data = configEntities->first;
		float x, y;
		char name[50], directionStr[10];
		DirectionFacing direction;
		ENTITY *entity;

		ENTITIES_MANAGER *manag = EntitiesManager_get();
		if (manag->length > 0) {
			for (Uint32 i = manag->length-1; i > 0; i--) {
				if (manag->list[i]->type != PLAYER_ENTITY)
					EntitiesManager_deleteEntity(manag->list[i]);
			}
		}

		/* création et positionnement des entités */
		while (data != NULL) {
			initEntity = (ARRAY *) data->buffer;
			data = data->next;

			Array_getString(initEntity, "name", name, "ENTITY", 50);
			x = Array_getFloat(initEntity, "x", 0.0);
			y = Array_getFloat(initEntity, "y", 0.0);
			Array_getString(initEntity, "direction", directionStr, "NORTH", 10);
			
			if (strcmp(directionStr, "NORTH") == 0) direction = NORTH;
			if (strcmp(directionStr, "SOUTH") == 0) direction = SOUTH;
			if (strcmp(directionStr, "EAST") == 0) direction = EAST;
			if (strcmp(directionStr, "WEST") == 0) direction = WEST;

			entity = NULL;
			infoEntity = Array_getArray(infoEntities, name);
			if (strcmp(name, "PLAYER") == 0) {
				if (game->player == NULL) entity = Player_newFromArray(x, y, direction, game->dispTileSize, infoEntity);
			} else if (strcmp(name, "PNJ1") == 0 || strcmp(name, "PNJ2") == 0) {
				entity = PNJ_newFromArray(x, y, direction, game->dispTileSize, infoEntity);
			} else if (strcmp(name, "BLOB") == 0 || strcmp(name, "MINI_BLOB") == 0) {
				entity = Mob_Blob_newFromArray(x, y, direction, game->dispTileSize, infoEntity);
			} else if (strcmp(name, "GOBLIN") == 0 || strcmp(name, "MINI_GOBLIN") == 0) {
				entity = Mob_Goblin_newFromArray(x, y, direction, game->dispTileSize, infoEntity);
			} else if (strcmp(name, "SKELETON") == 0) {
				entity = Mob_Skeleton_newFromArray(x, y, direction, game->dispTileSize, infoEntity);
			} else if (strcmp(name, "DRAGON") == 0 || strcmp(name, "DRAGON_BOSS") == 0) {
				entity = Mob_Dragon_newFromArray(x, y, direction, game->dispTileSize, infoEntity);
			}
			if (entity != NULL) {
				if (game->player == NULL && entity->type == PLAYER_ENTITY) game->player = entity;
				EntitiesManager_addEntity(entity);
			}
		}
		EntitiesManager_setDispTileSize(game->dispTileSize);
		LOG_SUCCESS("LOAD ENTYTIES\n");
	}
}

void Game_free() {
	if (application != NULL) {
		GAME *game = &application->game;
		//if (game->panel != NULL) Panel_free(game->panel);
		if (game->map != NULL) Map_free(game->map);
		if (game->tileset != NULL) Tileset_free(game->tileset);
		if (game->camera != NULL) Camera_free(game->camera);
		EntitiesManager_clean();
		game->panel = NULL;
	}
}

void Game_event() {
	static bool nextEvt = false, afterEvt = false;
	static bool saveEvt = false, closePanelEvt = false, quitEvt = false;
	static bool musicPlay = true;

	if (application != NULL) {
		GAME *game = &application->game;
		if (Input_quitEvent()) {
			application->quit = true;
		}

		/* si le menu est désactivé on gère les évènements du jeu*/
		if (Panel_isDisable(game->panel)) {
			/* gestion de la téléportation si l'évènement TELEPORTATION_EVENT a été envoyé */
			if (Input_wasUserEvent(TELEPORTATION_EVENT)) {
				Input_pushEvent(USER_EVENT, 0);

				char strpos[8];
				Uint16 i, j;
				Entity_getLookMapPosition(game->player, &i, &j);
				sprintf(strpos, "%u_%u", i, j);
				/* on récupère l'information de téléportation dans le "téléportations array" qui se situe elle même dans le "setting array" */
				ARRAY *teleport = Array_getArray(Array_getArray(game->map->setting, "teleportations"), strpos);
				if (teleport != NULL) {
					float x, y;
					x = Array_getFloat(teleport, "x", 0.0);
					y = Array_getFloat(teleport, "y", 0.0);
					bool changeMap = Array_getBool(teleport, "changeMap", false);

					switch(game->player->directionFacing) {
						case SOUTH: y += game->player->zoom; break;
						case NORTH: y -= game->player->zoom; break;
						case EAST: x += game->player->zoom; break;
						case WEST: x -= game->player->zoom; break;
						default: break;
					}

					Entity_setPosition(game->player, x, y);
					if (changeMap == true) {
						Array_getString(teleport, "name", game->currentMapName, "world", 100);
						game->is_prepared = false;				
						Game_prepare();
					}
				}
			}

			if (Input_wasKeyReleased(SDLK_ESCAPE)) {
				/* on met le jeu en pause */
				game->pause = true;
				Panel_enable(game->panel);
			}

			if (Input_wasKeyReleased(SDLK_i)) {
				game->gameDebugMode = !game->gameDebugMode;
			}

			if (Input_wasKeyReleased(SDLK_m)) {
				if (musicPlay) {
					LOG_INFO("Mute music\n");
					SG_muteMusic();
				} else {
					LOG_INFO("Play music\n");
					SG_unMuteMusic();
				}
				musicPlay = !musicPlay;
			}

			/* zommer */
			if (Input_wasKeyReleased(SDLK_KP_PLUS, SDLK_o)) {
				game->dispTileSize += 4;
				if (game->dispTileSize > MAX_ZOOM) game->dispTileSize = MAX_ZOOM;
				EntitiesManager_setDispTileSize(game->dispTileSize);
				Tileset_update(game->tileset, game->dispTileSize);
			}

			/* dézommer */
			if (Input_wasKeyReleased(SDLK_KP_MINUS, SDLK_p)) {
				game->dispTileSize -= 4;
				if (game->dispTileSize < MIN_ZOOM) game->dispTileSize = MIN_ZOOM;
				EntitiesManager_setDispTileSize(game->dispTileSize);
				Tileset_update(game->tileset, game->dispTileSize);
			}

			EntitiesManager_event();
		} else { /* On gère les évènements dans menu si il est activé */
			Panel_event(game->panel);

			if (Widget_onReleased(game->panel, "BNext")) nextEvt = true;
			if (Widget_onReleased(game->panel, "BAfter")) afterEvt = true;
			if (Widget_onReleased(game->panel, "BSave")) saveEvt = true;
			if (Widget_onReleased(game->panel, "BClose")) quitEvt = true;

			/* On ferme le panel si la touche échap est relâchée */
			if (Input_wasKeyReleased(SDLK_ESCAPE)) closePanelEvt = true;

			/* on change effectue l'action après avoir vérifié les évènements sur tous les widgets,
				pour éviter qu'un widget s'active par inadvertance */
			if (nextEvt) { Panel_nextVisiblePage(game->panel); nextEvt = false; }
			if (afterEvt) { Panel_afterVisiblePage(game->panel); afterEvt = false; }
			if (saveEvt) { SaveFile_save(); saveEvt = false; }
			if (closePanelEvt) {
				Panel_disable(game->panel);
				closePanelEvt = false;
				/* on relance le jeu si on ferme le menu*/
				game->pause = false;
			}
			if (quitEvt) {
				Panel_disable(game->panel);
				quitEvt = false;
				Input_pushEvent(USER_EVENT, LOAD_MENU_EVENT);
			}
		}
	}
}

void Game_gameDebugMode() {
	if (application != NULL) {
		GAME *game = &application->game;
		TTF_Font *font = SG_loadFont("arial.ttf", 14);
		char text[100];
		float x, y;
		
		Entity_getPosition(game->player, &x, &y);
		SG_printf(true, 2, 0, font, WHITE, "Player position : %f %f", x, y);

		SG_printf(true, 2, 14, font, WHITE, "Player zoom : %f", game->player->zoom);

		switch(Entity_getDirection(game->player)) {
			case NORTH: strcpy(text, "Player direction : NORTH"); break;
			case SOUTH: strcpy(text, "Player direction : SOUTH"); break;
			case EAST:  strcpy(text, "Player direction : EAST"); break;
			case WEST:  strcpy(text, "Player direction : WEST"); break;
			default: break;
		}
		SG_printf(true, 2, 28, font, WHITE, text);
		
		switch(game->player->motionType) {
			case STANDING: strcpy(text, "Player is STANDING"); break;
			case WALKING: strcpy(text, "Player is WALKING"); break;
			case AUTO_WALKING: strcpy(text, "Player is AUTO WALKING"); break;
			default: break;
		}
		SG_printf(true, 2, 42, font, WHITE, text);
		SG_printf(true, 2, 56, font, WHITE, "Colision Facing : %hu", Entity_getCollisionFacing(game->player));
		SG_printf(true, 2, 68, font, WHITE, "Map : %s", game->currentMapName);
	}
}

void Game_update() {
	if (application != NULL) {
		GAME *game = &application->game;
		
		if (Panel_isDisable(game->panel)) {
			EntitiesManager_update(game->map);
			Camera_goto(game->camera, game->player->x, game->player->y, game->dispTileSize, game->map->width, game->map->height);
		}
		Panel_update(game->panel);
	}
}

void Game_draw() {
	if (application != NULL) {
		GAME *game = &application->game;
		/* Effacer l'écran */
		SG_clear();

		/* Affichage de la map */
		Map_draw(game->map, game->camera, game->tileset, game->dispTileSize);

		/* Afficher les entités dans entitiesManager */
		EntitiesManager_draw(game->camera, game->gameDebugMode);

		/* Affichage des calque la map en premier plan */
		Map_drawFrontOf(game->map, game->camera, game->tileset, game->dispTileSize);

		/* Afficher éléments graphique des entités à afficher au premier plan */
		EntitiesManager_drawFrontOf(game->camera, game->gameDebugMode);

		/* affichage de information de debuggage */
		if (game->gameDebugMode) Game_gameDebugMode();

		/* affichage du menu */
		Panel_draw(game->panel, NULL);

		/* Envoyer le renderer au processeur graphique */
		SG_present();
	}
}
