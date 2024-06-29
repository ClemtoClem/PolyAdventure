#pragma once
#ifndef __SOUND_GRAPHICS_H__
#define __SOUND_GRAPHICS_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * SG permet de regrouper tous les elements les plus important de la SDL.
 * Deplus les ressources sont enregistre dans une liste ce qui evite de les reimportes à chaque fois
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "../Utils/colors.h"
#include "../Utils/logger.h"
#include "../Utils/array.h"
#include "image.h"

#define MAX_FOUREGROUND_TEXTURES 20

typedef struct t_TARGET_TEXTURE {
	SDL_Texture *tex;
	SDL_Rect *srcrect;
	SDL_Rect *dstrect;
} TARGET_TEXTURE;

typedef struct t_SOUND_GRAPHICS {
	Sint32 width, height;
	Uint32 bgColor;
	SDL_Window *window;
	SDL_Renderer *renderer;
	ARRAY *ressources;
	Mix_Music *currentMusic;
	bool audioInited, muteMusic;
	int loopMusic;

	TARGET_TEXTURE foregroundTex[MAX_FOUREGROUND_TEXTURES];
	Uint16 nbForegroundTex;
} SOUND_GRAPHICS;

extern SOUND_GRAPHICS *sound_graphics;

/* Convertir une couleur RGBA 32 bits en structure SDL_Color */
SDL_Color hexColorToRGBA(Uint32 hex);

/**
 * @brief Initialisation de la structure SOUND_GRAPHICS ("sound_graphics" est une variable globale ne pas initialiser)
*/
bool SG_init(char *title, Sint32 width, Sint32 height, Uint32 bgColor);

/**
 * @brief Detruire la structure SOUND_GRAPHICS
*/
void SG_free();

SOUND_GRAPHICS *SG_get();

char *SG_getTitle();
void SG_setTitle(char *title);
Sint32 SG_getWidth();
Sint32 SG_getHeight();

/**
 * @brief Importer une image (.png)
 * @attention Ne pas free l'image, cela se fera automatiquement à la destruction de la structure SOUND_GRAPHICS
 * @param imzgeName nom de l'image à importer dans le dossier ./res/images/
 */
IMAGE *SG_loadImage(char *imageName);
void freeImage(void *buffer, TypeData type);

/**
 * @brief Importer une police (.ttf)
 * @attention Ne pas free la police, cela se fera automatiquement à la destruction de la structure SOUND_GRAPHICS
 * @param fontName nom de la police à importer dans le dossier ./res/fonts/
 * @param fontSize taille de la police
 */
TTF_Font *SG_loadFont(char *fontName, Uint16 fontSize);
void freeFont(void *buffer, TypeData type);

bool SG_openAudio();

void SG_closeAudio();

Mix_Chunk* SG_loadSound(char *soundName);
void freeSound(void *buffer, TypeData type);

void SG_playSound(char *soundName);

/* couper le son */
void SG_muteMusic();

/* remètre le son */
void SG_unMuteMusic();

Mix_Music* SG_loadMusic(char *musicName);
void freeMusic(void *buffer, TypeData type);

void SG_playMusic(char *musicName, int loop);

void SG_stopMusic();

/**
 * @brief Effacer le renderer
 */
void SG_clear();

SDL_Renderer *SG_getRenderer();

/**
 * @brief Sélectionner une couleur de dessin
 */
void SG_drawColor(Uint32 rgba);

/**
 * @brief Dessiner un rectangle dans le renderer
 */
void SG_drawRect(SDL_Rect *rect, bool fill);

/**
 * @brief Envoyer le renderer au processeur graphique
 */
void SG_present();

/**
 * @brief Insérer une texture dans le renderer
 */
void SG_render(SDL_Texture *texture, SDL_Rect *src, SDL_Rect *dest);

/**
 * @brief Créer une texture
*/
SDL_Texture *SG_createTexture(Sint32 w, Sint32 h);

/**
 * @brief Set a texture as the current rendering target
 */
void SG_setTarget(SDL_Texture *texture);

/**
 * @brief Mettre toutes les textures que l'on souhaite afficher au premier plan
*/
void SG_AddForegroundTexture(SDL_Texture *texture, SDL_Rect *srcrect, SDL_Rect *dstrect);

/**
 * @brief Afficher toutes les textures à mettre au premier plan
*/
void SG_RenderForegroundTextures();

/**
 * @brief Insérer du text dans le renderer
 * @param foreground si le texte doit obligatoirement apparaître en premier plan
 * @param text text à afficher
 * @param font police
 * @param textColor couleur du texte
 * @param rect rectangle pour restrindre l'affichage du texte (w = 0 pour ne pas restrindre la longeur, h = 0 pour ne pas restrindre la largeur)
 * @param centerX centrer le texte dans le rectangle
 * @param centerY centrer le texte dans le rectangle
*/
void SG_renderText(bool foreground, char* text, TTF_Font* font, Uint32 textColor, SDL_Rect *rect, bool centerX, bool centerY);


/**
 * @brief Adaptation du printf pour afficher du texte formatté dans l'interface graphique
 * @param foreground si le texte doit obligatoirement apparaître en premier plan
*/
void SG_printf(bool foreground, Sint32 x, Sint32 y, TTF_Font *font, Uint32 textColor, char *text, ... );

#endif  /* __SOUND_GRAPHICS_H__ */