#pragma once
#ifndef __INPUT_H__
#define __INPUT_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Sauvegarde des differents évènements dans trois listes:
 *	- pressedKeys pour l'action presser la touche
 *	- releasedKeys pour l'action relever la touche
 *	- heldKeys pour les touches maintenues enfoncees
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "../Utils/utils.h"

#define USER_EVENT 1

/* Liste des flags d'évènements de application */
#define LOAD_PREFACE_EVENT 1
#define LOAD_MENU_EVENT 2
#define LOAD_GAME_EVENT 3

/* Liste des flags d'évènements de player */
#define TELEPORTATION_EVENT 4

#define NB_MOUSE_EVENT 6
#define NB_KEYS_EVENT 512
#define NB_USER_EVENT 100

#define Input_wasKeyPressed(firstKey, ...) _Input_wasKeyPressed(firstKey, ##__VA_ARGS__, -1)
#define Input_wasKeyReleased(firstKey, ...) _Input_wasKeyReleased(firstKey, ##__VA_ARGS__, -1)
#define Input_wasKeyHeld(firstKey, ...) _Input_wasKeyHeld(firstKey, ##__VA_ARGS__, -1)
#define Input_wasMousePressed(firstButton, ...) _Input_wasMousePressed(firstButton, ##__VA_ARGS__, -1)
#define Input_wasMouseReleased(firstButton, ...) _Input_wasMouseReleased(firstButton, ##__VA_ARGS__, -1)
#define Input_wasMouseHeld(firstButton, ...) _Input_wasMouseHeld(firstButton, ##__VA_ARGS__, -1)
#define Input_wasUserEvent(fisrtCode, ...) _Input_wasUserEvent(fisrtCode, ##__VA_ARGS__, -1)

typedef struct t_MOUSE {
	Sint32 x, y;
	bool pressed[NB_MOUSE_EVENT];
	bool released[NB_MOUSE_EVENT];
	bool held[NB_MOUSE_EVENT];
} MOUSE;

typedef struct t_INPUT {
	SDL_Event event;
	bool pressedKeys[NB_KEYS_EVENT];
	bool releasedKeys[NB_KEYS_EVENT];
	bool heldKeys[NB_KEYS_EVENT];
	bool userEvent[NB_USER_EVENT];
	MOUSE mouse;
	bool usedScancode; /* Scancode for QUERTY, Keycode for AZERTY */
	bool quitEvent, hasEvent;
} INPUT;

extern INPUT *intput;

/* initialisation de la sructure INPUT (input est une variable globale ne pas l'allouer) */
bool Input_init(bool usedScancode);
void Input_free();
INPUT *Input_get();

/* rénitialisation des évènements instantanés de la frame précédente (presser, relâcher)*/
void Input_resetFrame();

/* générer un évènement */
void Input_pushEvent(Sint32 type, Sint32 code);

/* traitement et enregistrement des évènements de la SDL */
void Input_event();

/* renvoie true si le bouton quitter est pressé */
bool Input_quitEvent();
/* renvoie true si le bouton renseigné en paramètre est pressé */
bool _Input_wasKeyPressed(int firstKey, ...);
/* renvoie true si le bouton renseigné en paramètre est relâché */
bool _Input_wasKeyReleased(int firstKey, ...);
/* renvoie true si le bouton renseigné en paramètre est maintenue enfoncé */
bool _Input_wasKeyHeld(int firstKey, ...);

/* renvoie true si le bouton de la souris renseigné en paramètre est pressé */
bool _Input_wasMousePressed(Uint8 firstButton, ...);
/* renvoie true si le bouton de la souris renseigné en paramètre est relâché */
bool _Input_wasMouseReleased(Uint8 firstButton, ...);
/* renvoie true si le bouton de la souris renseigné en paramètre est maintenue enfoncé */
bool _Input_wasMouseHeld(Uint8 firstButton, ...);
/* renvoie la position de la souris */
void Input_getMousePosition(SDL_Point *pos);

/* renvoie true si l'évènement renseigné en paramètre est activé */
bool _Input_wasUserEvent(int fisrtCode, ...);

#endif /* __INPUT_H__ */