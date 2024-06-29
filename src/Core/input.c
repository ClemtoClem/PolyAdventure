#include "input.h"

INPUT *input = NULL;

bool Input_init(bool usedScancode) {
	bool success = false;
	if (input == NULL) {
		input = malloc(sizeof(INPUT));
		if (input != NULL) {
			memset(input->pressedKeys, 0, NB_KEYS_EVENT);
			memset(input->releasedKeys, 0, NB_KEYS_EVENT);
			memset(input->heldKeys, 0, NB_KEYS_EVENT);
			memset(input->mouse.pressed, 0, NB_MOUSE_EVENT);
			memset(input->mouse.released, 0, NB_MOUSE_EVENT);
			memset(input->mouse.held, 0, NB_MOUSE_EVENT);
			memset(input->userEvent, 0, NB_USER_EVENT);
			input->quitEvent = false;
			input->hasEvent = false;
			input->usedScancode = usedScancode;
			
			SDL_GetMouseState(&input->mouse.x, &input->mouse.y);

			success = true;
		}
	} else {
		success = true;
	}
	return success;
}

void Input_free() {
	if (input != NULL) {
		free(input);
		input = NULL;
	}
}

INPUT *Input_get() {
	return input;
}

void Input_resetFrame() {
	if (input != NULL && input->hasEvent == true) {
		memset(input->pressedKeys, 0, NB_KEYS_EVENT);
		memset(input->releasedKeys, 0, NB_KEYS_EVENT);
		memset(input->mouse.pressed, 0, NB_MOUSE_EVENT);
		memset(input->mouse.released, 0, NB_MOUSE_EVENT);
		memset(input->userEvent, 0, NB_USER_EVENT);
		input->quitEvent = false;
		input->hasEvent = false;
	}
}

void Input_pushEvent(Sint32 type, Sint32 code) {
	if (input != NULL) {
		// On genere un évènement
		SDL_Event evenement;
		evenement.type = type;
		evenement.user.code = code;
		SDL_PushEvent(&evenement);
		input->hasEvent = true;
	}
}

void Input_event() {
	if (input != NULL) {
		Input_resetFrame(input);
		SDL_Scancode touch;
		while(SDL_PollEvent(&input->event)) {
			switch(input->event.type) {
				case SDL_QUIT: /* évènement quitter la fenêtre */
					input->quitEvent = true;
					input->hasEvent = true;
					break;
				case SDL_KEYDOWN: /* évènement presser un bouton */
					if (input->usedScancode) touch = input->event.key.keysym.scancode;
					else touch = SDL_GetScancodeFromKey(input->event.key.keysym.sym);
					
					input->pressedKeys[touch] = true;
					input->releasedKeys[touch] = false;
					input->heldKeys[touch] = true;
					input->hasEvent = true;
					break;
				case SDL_KEYUP: /* évènement relâcher un bouton */
					if (input->usedScancode) touch = input->event.key.keysym.scancode;
					else touch = SDL_GetScancodeFromKey(input->event.key.keysym.sym);
					
					input->pressedKeys[touch] = false;
					input->releasedKeys[touch] = true;
					input->heldKeys[touch] = false;
					input->hasEvent = true;
					break;
				case SDL_MOUSEMOTION: /* évènement déplacer la souris */
					input->mouse.x = input->event.button.x;
					input->mouse.y = input->event.button.y;
					input->hasEvent = true;
					break;
				case SDL_MOUSEBUTTONDOWN: /* évènement presser un bouton de la souris */
					input->mouse.pressed[input->event.button.button] = true;
					input->mouse.released[input->event.button.button] = false;
					input->mouse.held[input->event.button.button] = true;
					input->hasEvent = true;
					break;
				case SDL_MOUSEBUTTONUP: /* évènement relâcher un bouton de la souris */
					input->mouse.pressed[input->event.button.button] = false;
					input->mouse.released[input->event.button.button] = true;
					input->mouse.held[input->event.button.button] = false;
					input->hasEvent = true;
					break;
				case USER_EVENT:
					input->userEvent[input->event.user.code] = true;
				default:  /* autre évènements inutiles */
					input->hasEvent = false;
					break;
			}
		}
	}
}

bool Input_quitEvent() {
	if (input != NULL) {
		return input->quitEvent;
	}
	return false;
}

bool _Input_wasKeyPressed(int firstKey, ...) {
	bool pressed = false;
	if (input != NULL) {
		int key = firstKey;
		SDL_Scancode touch;
		va_list args;
		va_start(args, firstKey);
		while (key != -1) {
			if (input->usedScancode) touch = key;
			else touch = SDL_GetScancodeFromKey(key);
			pressed = pressed || input->pressedKeys[touch];
			key = va_arg(args, int);
		}
		va_end(args);
	}
	return pressed;
}

bool _Input_wasKeyReleased(int firstKey, ...) {
	bool released = false;
	if (input != NULL) {
		int key = firstKey;
		SDL_Scancode touch;
		va_list args;
		va_start(args, firstKey);
		while (key != -1) {
			if (input->usedScancode) touch = key;
			else touch = SDL_GetScancodeFromKey(key);
			released = released || input->releasedKeys[touch];
			key = va_arg(args, int);
		}
		va_end(args);
	}
	return released;
}

bool _Input_wasKeyHeld(int firstKey, ...) {
	bool held = false;
	if (input != NULL) {
		int key = firstKey;
		SDL_Scancode touch;
		va_list args;
		va_start(args, firstKey);
		while (key != -1) {
			if (input->usedScancode) touch = key;
			else touch = SDL_GetScancodeFromKey(key);
			held = held || input->heldKeys[touch];
			key = va_arg(args, int);
		}
		va_end(args);
	}
	return held;
}

bool _Input_wasMousePressed(Uint8 firstButton, ...) {
	bool pressed = false;
	if (input != NULL) {
		int button = firstButton;
		va_list args;
		va_start(args, firstButton);
		while (button != -1) {
			pressed = pressed || input->mouse.pressed[button];
			button = va_arg(args, int);
		}
		va_end(args);
	}
	return pressed;
}

bool _Input_wasMouseReleased(Uint8 firstButton, ...) {
	bool released = false;
	if (input != NULL) {
		int button = firstButton;
		va_list args;
		va_start(args, firstButton);
		while (button != -1) {
			released = released || input->mouse.released[button];
			button = va_arg(args, int);
		}
		va_end(args);
	}
	return released;
}

bool _Input_wasMouseHeld(Uint8 firstButton, ...) {
	bool held = false;
	if (input != NULL) {
		int button = firstButton;
		va_list args;
		va_start(args, firstButton);
		while (button != -1) {
			held = held || input->mouse.held[button];
			button = va_arg(args, int);
		}
		va_end(args);
	}
	return held;
}

void Input_getMousePosition(SDL_Point *pos) {
	if (input != NULL) {
		pos->x = input->mouse.x;
		pos->y = input->mouse.y;
	}
}

bool _Input_wasUserEvent(int fisrtCode, ...) {
	int event = false;
	if (input != NULL) {
		int code = fisrtCode;
		va_list args;
		va_start(args, fisrtCode);
		while (code != -1) {
			event = event || input->userEvent[code];
			code = va_arg(args, int);
		}
		va_end(args);
	}
	return event;
}