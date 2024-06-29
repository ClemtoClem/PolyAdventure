#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>

#include "Utils/logger.h"
#include "Game/application.h"



bool Init() {
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		LOG_ERROR(ErrorType_SDL, "Echec de l'initialisation de SDL : %s\n", SDL_GetError());
		return false;
	}
	if (IMG_Init(IMG_INIT_PNG) < 0) {
		LOG_ERROR(ErrorType_SDL, "Echec de l'initialisation de IMG : %s\n", IMG_GetError());
		return false;
	}
	if (TTF_Init() < 0) {
		LOG_ERROR(ErrorType_SDL, "Echec de l'initialisation de TTF : %s\n", TTF_GetError());
		return false;
	}
	if (Mix_Init(MIX_INIT_OGG) < 0) {
		LOG_ERROR(ErrorType_SDL, "Echec de l'initialisation de Mix : %s\n", Mix_GetError());
		return false;
	}
	return true;
}

void Quit() {
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	bool debugMode = false;
	
	Logger_init(LogDisplayMode_FILE | LogDisplayMode_SCREEN, "log.out");
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-debug") == 0) {
			LOG_DEBUG("Debug is enable\n");
			Logger_enableDisplayMode(LogDisplayMode_CONSOLE);
			debugMode = true;
		}
		/*else if (strcmp(argv[i], "-out") == 0) {
			i++;
			if (i < argc) {
				Logger_openFile(argv[i]);
			} else {
				LOG_ERROR(ErrorType_FILE_ACCESS, "Out file name for debug isn't write in arguments (ex: -out file.txt)\n");
			}
		}*/
		else {
			LOG_ERROR(ErrorType_ERROR, "Unknow argument %s\n", argv[i]);
		}
	}

	if (Init()) {
		bool reload = false;
		do {
			if (Application_init(debugMode)) {
				Application_mainloop();
				reload = Application_free();
			}
		} while (reload);
	}

	Logger_quit();
	Quit();
	
	return 0;
}