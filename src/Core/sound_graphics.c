#include "sound_graphics.h"

SOUND_GRAPHICS *sound_graphics = NULL;

SDL_Color hexColorToRGBA(Uint32 hex) {
	SDL_Color color;
	color.r = (hex >> 24) & 0xFF;
	color.g = (hex >> 16) & 0xFF;
	color.b = (hex >> 8)  & 0xFF;
	color.a = hex & 0xFF;

	return color;
}

bool SG_init(char *title, Sint32 width, Sint32 height, Uint32 bgColor) {
	bool success = false;
	if (sound_graphics == NULL) {
		ARRAY *ressources		= NULL;
		SDL_Window *window		= NULL;
		SDL_Renderer *renderer	= NULL;
		
		ressources = (ARRAY *) Array_new(NO_LIMITED);
		if (ressources == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure ARRAY\n"); goto EndSG; }

		window = (SDL_Window *) SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,  SDL_WINDOW_OPENGL);
		if (window == NULL) {
			LOG_ERROR(ErrorType_SDL, "Could not create SDL window: %s\n", SDL_GetError());
			goto EndSG;
		}

		renderer = (SDL_Renderer *) SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if (renderer == NULL) {
			LOG_ERROR(ErrorType_SDL, "Could not create SDL renderer: %s\n", SDL_GetError());
			goto EndSG;
		}

		sound_graphics = (SOUND_GRAPHICS *) calloc(1, sizeof(SOUND_GRAPHICS));
		if (sound_graphics == NULL) { LOG_ERROR(ErrorType_FAULT, "Echec de l'allocation de la structure SOUND_GRAPHICS\n"); goto EndSG; }

		SG_openAudio();

		success = true;
	EndSG:
		if (success == false) {
			if (ressources != NULL)	Array_free(ressources, true);
			if (renderer != NULL)	SDL_DestroyRenderer(renderer);
			if (window != NULL)		SDL_DestroyWindow(window);
			sound_graphics = false;
		} else {
			sound_graphics->width		= width;
			sound_graphics->height	= height;
			sound_graphics->bgColor	= bgColor;
			sound_graphics->ressources = ressources;
			sound_graphics->window	= window;
			sound_graphics->renderer	= renderer;
		}
	} else {
		success = true;
	}
	return success;
}

void SG_free() {
	if (sound_graphics != NULL) {
		if (sound_graphics->nbForegroundTex > 0) {
			for (Uint16 i = 0; i < sound_graphics->nbForegroundTex; i++) {
				SDL_DestroyTexture(sound_graphics->foregroundTex[sound_graphics->nbForegroundTex].tex);
			}
		}

		SG_closeAudio();
		if (sound_graphics->ressources != NULL) Array_free(sound_graphics->ressources, true);
		if (sound_graphics->renderer != NULL)   SDL_DestroyRenderer(sound_graphics->renderer);
		if (sound_graphics->window != NULL)	  SDL_DestroyWindow(sound_graphics->window);
		free(sound_graphics);
		sound_graphics = false;
	}
}

SOUND_GRAPHICS *SG_get() {
	return sound_graphics;
}

char *SG_getTitle() {
	return (char *) SDL_GetWindowTitle(sound_graphics->window);
}

void SG_setTitle(char *title) {
	if (sound_graphics != NULL) {
		SDL_SetWindowTitle(sound_graphics->window, title);
	}
}

Sint32 SG_getWidth() {
	if (sound_graphics != NULL) {
		return sound_graphics->width;
	} else {
		return 0;
	}
}

Sint32 SG_getHeight() {
	if (sound_graphics != NULL) {
		return sound_graphics->height;
	} else {
		return 0;
	}
}

IMAGE *SG_loadImage(char *imageName) {
	IMAGE *image = NULL;
	if (sound_graphics != NULL) {
		/* On importe l'image si cela n'a pas dejà été effectué, sinon on la réccupère de la liste */
		DATA *data = Array_getFromKey(sound_graphics->ressources, imageName);
		if (data == NULL) {
			char *imagePath;
			SDL_Surface* surface;

			asprintf(&imagePath, "./res/images/%s", imageName);
			surface = IMG_Load(imagePath);
			if (surface == NULL) {
				asprintf(&imagePath, "../res/images/%s", imageName);
				surface = IMG_Load(imagePath);
			}
			if (surface == NULL) {
				LOG_ERROR(ErrorType_FILE_ACCESS, "Image \"%s\" introuvable\n", imagePath);
				return NULL;
			}
		
			SDL_Texture *texture = (SDL_Texture *) SDL_CreateTextureFromSurface(sound_graphics->renderer, surface);
			image = Image_new(texture, surface->w, surface->h);
			Array_append(sound_graphics->ressources, Data_new(ID_DEFAULT, imageName, IMAGE_TYPE, image, freeImage));

			SDL_FreeSurface(surface);
			LOG_SUCCESS("Importation de %s\n", imagePath);
			free(imagePath);
		} else {
			if (data->type == IMAGE_TYPE) {
				image = (IMAGE *) data->buffer;
			}
		}
	}
	return image;
}

void freeImage(void *buffer, TypeData type) {
	if (buffer != NULL && type == IMAGE_TYPE) {
		Image_free((IMAGE *) buffer);
	}
}

TTF_Font *SG_loadFont(char *fontName, Uint16 fontSize) {
	TTF_Font* font = NULL;
	if (sound_graphics != NULL) {
		char sizeNameFont[100];
		sprintf(sizeNameFont, "%s:%d", fontName, fontSize);
		/* On importe la police si cela n'a pas dejà été effectué, sinon on la réccupère de la liste */
		DATA *data = Array_getFromKey(sound_graphics->ressources, sizeNameFont);
		if (data == NULL) {
			char *fontPath;
			asprintf(&fontPath, "./res/fonts/%s", fontName);
			font = TTF_OpenFont(fontPath, fontSize);
			if (font == NULL) {
				asprintf(&fontPath, "../res/fonts/%s", fontName);
				font = TTF_OpenFont(fontPath, fontSize);
			}
			if (font == NULL) {
				LOG_ERROR(ErrorType_FILE_ACCESS, "Police \"%s\" introuvable\n", fontPath);
				return NULL;
			}
			Array_append(sound_graphics->ressources, Data_new(ID_DEFAULT, sizeNameFont, FONT_TYPE, font, freeFont));
			LOG_SUCCESS("Importation de %s\n", fontPath);
			free(fontPath);
		} else {
			if (data->type == FONT_TYPE) {
				font = (TTF_Font *) data->buffer;
			}
		}
	}
	return font;
}

void freeFont(void *buffer, TypeData type) {
	if (buffer != NULL && type == FONT_TYPE) {
		TTF_CloseFont((TTF_Font *) buffer);
	}
}

bool SG_openAudio() {
	bool success = false;
	if (sound_graphics != NULL && sound_graphics->audioInited == false) {
		// definition des propriétés audio
		int frequency	= 22050;
		Uint16 format	= AUDIO_S16;
		int channels	= 2;
		int chunksize	= 4096;

		if (Mix_OpenAudio(frequency, format, channels, chunksize) < 0) {
			LOG_ERROR(ErrorType_SDL, "Erreur d'ouverture audio %s\n", SDL_GetError());
		} else {
			success = true;
			sound_graphics->audioInited = true;
		}
	}
	return success;
}

void SG_closeAudio() {
	if (sound_graphics != NULL && sound_graphics->audioInited == true) {
		Mix_CloseAudio();
		sound_graphics->audioInited = false;
	}
}

Mix_Chunk* SG_loadSound(char *soundName) {
	Mix_Chunk* sound = NULL;
	if (sound_graphics != NULL && sound_graphics->audioInited == true) {
		/* On importe la musique si cela n'a pas dejà été effectué, sinon on la réccupère de la liste */
		DATA *data = Array_getFromKey(sound_graphics->ressources, soundName);
		if (data == NULL) {
			char soundPath[100];

			sprintf(soundPath, "./res/sounds/%s", soundName);
			sound = Mix_LoadWAV(soundPath);
			if (sound == NULL) {
				sprintf(soundPath, "../res/sounds/%s", soundName);
				sound = Mix_LoadWAV(soundPath);
			}
			if (sound == NULL) {
				LOG_ERROR(ErrorType_FILE_ACCESS, "Sound \"%s\" introuvable\n", soundPath);
				return NULL;
			}
		
			Array_append(sound_graphics->ressources, Data_new(ID_DEFAULT, soundName, SOUND_TYPE, sound, freeSound));

			LOG_SUCCESS("Importation de %s\n", soundPath);
		} else {
			if (data->type == SOUND_TYPE) {
				sound = (Mix_Chunk *) data->buffer;
			}
		}
	}
	return sound;
}

void freeSound(void *buffer, TypeData type) {
	if (buffer != NULL && type == SOUND_TYPE) {
		Mix_FreeChunk((Mix_Chunk *) buffer);
	}
}

void SG_playSound(char *soundName) {
	if (sound_graphics != NULL && sound_graphics->audioInited == true) {
		DATA *data = Array_getFromKey(sound_graphics->ressources, soundName);
		Mix_Chunk *sound = NULL;
		if (data != NULL && data->type == MUSIC_TYPE) {
			sound = (Mix_Chunk *) data->buffer;
		} else {
			sound = SG_loadSound(soundName);
		}

		if (sound != NULL) {
			Mix_PlayChannel(-1, sound, 0);
		}
	}
}

Mix_Music* SG_loadMusic(char *musicName) {
	Mix_Music* music = NULL;
	if (sound_graphics != NULL && sound_graphics->audioInited == true) {
		/* On importe la musique si cela n'a pas dejà été effectué, sinon on la réccupère de la liste */
		DATA *data = Array_getFromKey(sound_graphics->ressources, musicName);
		if (data == NULL) {
			char musicPath[100];

			sprintf(musicPath, "./res/sounds/%s", musicName);
			music = Mix_LoadMUS(musicPath);
			if (music == NULL) {
				sprintf(musicPath, "../res/sounds/%s", musicName);
				music = Mix_LoadMUS(musicPath);
			}
			if (music == NULL) {
				LOG_ERROR(ErrorType_FILE_ACCESS, "Music \"%s\" introuvable\n", musicPath);
				return NULL;
			}
		
			Array_append(sound_graphics->ressources, Data_new(ID_DEFAULT, musicName, MUSIC_TYPE, music, freeMusic));

			LOG_SUCCESS("Importation de %s\n", musicPath);
		} else {
			if (data->type == MUSIC_TYPE) {
				music = (Mix_Music *) data->buffer;
			}
		}
	}
	return music;
}

void freeMusic(void *buffer, TypeData type) {
	if (buffer != NULL && type == MUSIC_TYPE) {
		Mix_FreeMusic((Mix_Music *) buffer);
	}
}

void SG_playMusic(char *musicName, int loop) {
	if (sound_graphics != NULL && sound_graphics->audioInited == true) {
		SG_stopMusic();
		DATA *data = Array_getFromKey(sound_graphics->ressources, musicName);
		Mix_Music *music = NULL;
		if (data != NULL && data->type == MUSIC_TYPE) {
			music = (Mix_Music  *) data->buffer;
		} else {
			music = SG_loadMusic(musicName);
		}

		if (music != NULL) {
			sound_graphics->currentMusic = music;
			sound_graphics->loopMusic = loop;
			sound_graphics->muteMusic = false;
			Mix_FadeInMusic(music, loop, 0);
		}
	}
}

void SG_stopMusic() {
	if (sound_graphics != NULL && sound_graphics->audioInited == true && sound_graphics->currentMusic != NULL) {
		sound_graphics->currentMusic = NULL;
		Mix_FadeOutMusic(0);
	}
}

void SG_muteMusic() {
	if (sound_graphics != NULL && sound_graphics->audioInited == true && sound_graphics->currentMusic != NULL && sound_graphics->muteMusic == false) {
		sound_graphics->muteMusic = true;
		Mix_FadeOutMusic(0);
	}
}

void SG_unMuteMusic() {
	if (sound_graphics != NULL && sound_graphics->audioInited == true && sound_graphics->currentMusic != NULL && sound_graphics->muteMusic == true) {
		sound_graphics->muteMusic = false;
		Mix_FadeInMusic(sound_graphics->currentMusic, sound_graphics->loopMusic, 0);
	}
}

void SG_clear() {
	if (sound_graphics != NULL) {
		SDL_Color c = hexColorToRGBA(sound_graphics->bgColor);
		SDL_SetRenderDrawColor(sound_graphics->renderer, c.r, c.g, c.b, c.a); // Set the renderer's draw color
		SDL_RenderClear(sound_graphics->renderer);
	}
}

SDL_Renderer *SG_getRenderer() {
	if (sound_graphics != NULL) {
		return sound_graphics->renderer;
	}
	return NULL;
}

void SG_drawColor(Uint32 rgba) {
	if (sound_graphics != NULL) {
		SDL_SetRenderDrawColor(sound_graphics->renderer, (rgba>>24) & 0xFF, (rgba>>16) & 0xFF, (rgba>>8) & 0xFF, rgba & 0xFF);
	}
}

void SG_drawRect(SDL_Rect *rect, bool fill) {
	if (sound_graphics != NULL) {
		if (fill) SDL_RenderFillRect(sound_graphics->renderer, rect);
		else SDL_RenderDrawRect(sound_graphics->renderer, rect);
	}
}

void SG_present() {
	if (sound_graphics != NULL) {
		SG_RenderForegroundTextures();
		SDL_RenderPresent(sound_graphics->renderer);
	}
}

void SG_render(SDL_Texture *texture, SDL_Rect *src, SDL_Rect *dest) {
	if (sound_graphics != NULL) {
		if (SDL_RenderCopy(sound_graphics->renderer, texture, src, dest) != 0) // Copy the final texture to the buffer
			LOG_ERROR(ErrorType_SDL, "Texture SDL_RenderCopy : %s\n", SDL_GetError());
	}
}

SDL_Texture *SG_createTexture(Sint32 w, Sint32 h) {
	if (sound_graphics != NULL && w > 0 && h > 0) {
		return SDL_CreateTexture(sound_graphics->renderer, SDL_PIXELFORMAT_RGBA8888, 
			SDL_TEXTUREACCESS_TARGET, w, h);
	}
	return NULL;
}

void SG_setTarget(SDL_Texture *texture) {
	if (sound_graphics != NULL) {
		if (SDL_SetRenderTarget(sound_graphics->renderer, texture) != 0)
			LOG_ERROR(ErrorType_SDL, "Texture SDL_SetRenderTarget : %s\n", SDL_GetError());
	}
}

void SG_AddForegroundTexture(SDL_Texture *texture, SDL_Rect *srcrect, SDL_Rect *dstrect) {
	if (sound_graphics != NULL && texture != NULL && sound_graphics->nbForegroundTex < MAX_FOUREGROUND_TEXTURES) {
		sound_graphics->foregroundTex[sound_graphics->nbForegroundTex].tex = texture;
		if (srcrect == NULL) sound_graphics->foregroundTex[sound_graphics->nbForegroundTex].srcrect = NULL;
		else {
			sound_graphics->foregroundTex[sound_graphics->nbForegroundTex].srcrect = (SDL_Rect *) malloc(sizeof(SDL_Rect));
			RectCopy(sound_graphics->foregroundTex[sound_graphics->nbForegroundTex].srcrect, srcrect);
		}
		if (dstrect == NULL) sound_graphics->foregroundTex[sound_graphics->nbForegroundTex].dstrect = NULL;
		else {
			sound_graphics->foregroundTex[sound_graphics->nbForegroundTex].dstrect = (SDL_Rect *) malloc(sizeof(SDL_Rect));
			RectCopy(sound_graphics->foregroundTex[sound_graphics->nbForegroundTex].dstrect, dstrect);
		}
		sound_graphics->nbForegroundTex++;
	} else {
		LOG_WARNING("The maximum number of target textures has been reached");
	}
}

void SG_RenderForegroundTextures() {
	if (sound_graphics != NULL && sound_graphics->nbForegroundTex > 0) {
		for (Uint16 i = 0; i < sound_graphics->nbForegroundTex; i++) {
			SDL_RenderCopy(sound_graphics->renderer, sound_graphics->foregroundTex[i].tex,
				sound_graphics->foregroundTex[i].srcrect, sound_graphics->foregroundTex[i].dstrect);
			if (sound_graphics->foregroundTex[i].srcrect) free(sound_graphics->foregroundTex[i].srcrect);
			if (sound_graphics->foregroundTex[i].dstrect) free(sound_graphics->foregroundTex[i].dstrect);
			SDL_DestroyTexture(sound_graphics->foregroundTex[i].tex);
		}
		sound_graphics->nbForegroundTex = 0;
	}
}

void SG_renderText(bool foreground, char* text, TTF_Font* font, Uint32 textColor, SDL_Rect *rect, bool centerX, bool centerY) {
	if (sound_graphics != NULL) {
		SDL_Color c = hexColorToRGBA(textColor);
		SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, c);
		SDL_Texture* message = SDL_CreateTextureFromSurface(sound_graphics->renderer, surfaceMessage);

		SDL_Rect src;
		SDL_Rect dest;

		src.x = 0;
		src.y = 0;
		dest.x = rect->x;
		dest.y = rect->y;

		if (rect->w != 0 && surfaceMessage->w > rect->w) {
			src.w = rect->w;
			if (centerX == true) src.x += surfaceMessage->w/2 - src.w/2;
		} else {
			src.w = surfaceMessage->w;
			if (rect->w != 0 && centerX == true) dest.x += rect->w/2 - src.w/2;
		}
			
		if (rect->h != 0 && surfaceMessage->h > rect->h) {
			src.h = rect->h;
			if (centerY == true) src.y += surfaceMessage->h/2 - src.h/2;
		} else {
			src.h = surfaceMessage->h;
			if (rect->h != 0 && centerY == true) dest.y += rect->h/2 - src.h/2;
		}

		dest.w = src.w;
		dest.h = src.h;

		SDL_FreeSurface(surfaceMessage);
		if (foreground) {
			SG_AddForegroundTexture(message, &src, &dest);
		} else {
			SDL_RenderCopy(sound_graphics->renderer, message, &src, &dest);
			SDL_DestroyTexture(message);
		}
	}
}

void SG_printf(bool foreground, Sint32 x, Sint32 y, TTF_Font *font, Uint32 textColor, char *text, ... ) {
	if (sound_graphics != NULL) {
		static char buffer[1000];
		SDL_Rect dest;
		
		/* on formate la chaîne à afficher avec les arguments */
		va_list args;
		va_start(args, text);
		vsprintf(buffer, text, args);
		va_end(args);
		
		SDL_Color c = hexColorToRGBA(textColor);
		SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, buffer, c);
		SDL_Texture* message = SDL_CreateTextureFromSurface(sound_graphics->renderer, surfaceMessage);

		dest.x = x;
		dest.y = y;
		dest.w = surfaceMessage->w;
		dest.h = surfaceMessage->h;
		
		SDL_FreeSurface(surfaceMessage);
		if (foreground) {
			SG_AddForegroundTexture(message, NULL, &dest);
		} else {
			SDL_RenderCopy(sound_graphics->renderer, message, NULL, &dest);
			SDL_DestroyTexture(message);
		}
	}
}