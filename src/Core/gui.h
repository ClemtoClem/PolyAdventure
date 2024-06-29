#ifndef __GUI_H__
#define __GUI_H__

/*
 ********************************************
 *				SOURCE CODE					*
 ********************************************
 * Affichage d'un panel à l'ecran dans lequel on pourra ajouter des widgets
 * Les widgets disponibles sont :
 *   - Panel,
 *   - InnerBox,   (A finir)
 *   - Sprite,
 *   - Label,      (A finir)
 *   - Button,
 *   - ToggleSwitch,
 *   - ScrollBar,  (A finir)
 *   - LevelBar,   (A finir)
 *   - ComboBox    (A finir)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../Utils/colors.h"
#include "../Utils/logger.h"
#include "../Utils/timer.h"
#include "../Utils/array.h"
#include "../Utils/utils.h"

#include "sound_graphics.h"
#include "input.h"
#include "image.h"

typedef enum {
	HORIZONTAL,
	VERTICAL,
} Orientation;


/**
 * @brief Activer le widget
 * @param data pointeur vers une DATA
 */
void Widget_enable(DATA *data);

/**
 * @brief Désactiver le widget
 * @param data pointeur vers une DATA
 */
void Widget_disable(DATA *data);

void ListWidgets_enable(ARRAY *listWidgets);
void ListWidgets_disable(ARRAY *listWidgets);

bool Widget_isWidget(DATA *data);

/**
 * @brief Vérifie si le widget est acvtivé
 * @param data pointeur vers une DATA
 * @return true si le widget est activé
 * @return false si le widget est désactivé
 */
bool Widget_isEnable(DATA *data);

/**
 * @brief Vérifie si le widget est déacvtivé
 * @param data pointeur vers une DATA
 * @return true si le widget est désactivé
 * @return false si le widget est activé
 */
bool Widget_isDisable(DATA *data);

TypeData Widget_getType(void *widget);

void *Widget_getVisible(void *root, char *widgetName);

/**
 * @brief Vérifie si le widget existe dans le widget parent
 * @param root pointeur du widget parent
 * @param widgetName nom du widget à vérifier
 * @return true si le widget existe
 * @return false si le widget n'existe pas
 */
bool Widget_existInRoot(void *root, char *widgetName);

/**
 * @brief Ajoute un widget à un widget parent
 * @param root pointeur du widget parent
 * @param widgetName nom du widget à ajouter
 * @param widget pointeur du widget à ajouter
 * @return true si le widget à été ajouté
 * @return false si le widget n'a pas pu être ajouté ou si un paramètre est NULL
 */
bool Widget_addToRoot(void *root, char *widgetName, void *widget);

/**
 * @brief Détruire un widget
 * @param buffer private data
 * @param type type widget
 */
void Widget_free(void *buffer, TypeData type);

/**
 * @brief Vérifie si le widget est survolé par la souris
 * @param root pointeur du widget parent
 * @param widgetName nom du widget
 * @return true si le widget est survolé
 * @return false si il ne l'est pas
 */
bool Widget_onFocused(void *root, char *widgetName);

/**
 * @brief Vérifie si le widget est préssé par la souris
 * @param root pointeur du widget parent
 * @param widgetName nom du widget
 * @return true si le widget est survolé
 * @return false si il ne l'est pas
 */
bool Widget_onPressed(void *root, char *widgetName);

/**
 * @brief Vérifie si le widget est relâché par la souris
 * @param root pointeur du widget parent
 * @param widgetName nom du widget
 * @return true si le widget est survolé
 * @return false si il ne l'est pas
 */
bool Widget_onReleased(void *root, char *widgetName);

/**
 * @brief Vérifie si le widget est maintenu par la souris
 * @param root pointeur du widget parent
 * @param widgetName nom du widget
 * @return true si le widget est survolé
 * @return false si il ne l'est pas
 */
bool Widget_onHeld(void *root, char *widgetName);

/* 
 * ------------------------------------------------
 *  Panel
 * ------------------------------------------------
*/

/**
 * @brief Gestion des widgets dans un panneau à plusieurs pages
 */
typedef struct t_PANEL {
	Uint8 selfType;
	bool isEnable;

	IMAGE *image;
	SDL_Rect rect, tileRect;
	Sint32 innerW, innerH;

	Uint32 nbPages; /* nombre de pages */
	Uint32 workPage; /* page sur lequel on effectue les modifications */
	bool workAllPage;
	Uint32 visiblePage; /* page à afficher */
	ARRAY **widgets;
	
	ARRAY *globalWidgets; /* widgets qui seront présent tous le temps */
} PANEL;

/**
 * @brief Création de la structure PANEL
 * @param rootpanel PANEL parent
 * @param widgetName nom du widget
 * @param x position x du panel dans la fenêtre graphique
 * @param y position y du panel dans la fenêtre graphique
 * @param w largeur
 * @param h hauteur
 * @param imageName nom du tileset d'image ( taille : 3 tiles x 3 tiles )
 * @param nbPages nb de pages que comporte le Panel
 * @return PANEL* 
 */
PANEL *Panel_new(PANEL *rootpanel, char *widgetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, char *imageName, Uint32 nbPages);

/**
 * @brief Destruction de la structure PANEL
 * @param panel pointeur de la structure PANEL
 */
void Panel_free(PANEL *panel);

/**
 * @brief Activer le panel (affichage et mise à jour)
 * @param panel pointeur de la structure PANEL
 */
void Panel_enable(PANEL *panel);

/**
 * @brief Désactiver le panel (affichage et mise à jour)
 * @param panel pointeur de la structure PANEL
 */
void Panel_disable(PANEL *panel);

/**
 * @brief Vérifie si le panel est activé
 * @param panel pointeur de la structure PANEL
 * @return true si le panel est activé
 * @return fasle si désactivé
 */
bool Panel_isEnable(PANEL *panel);

/**
 * @brief Vérifie si le panel est désactivé
 * @param panel pointeur de la structure PANEL
 * @return true si le panel est désactivé
 * @return fasle si activé
 */
bool Panel_isDisable(PANEL *panel);

/**
 * @brief Changer la page de travail
 * @param panel pointeur de la structure PANEL
 * @param page numéro de la page
 * @return true si ok
 * @return false pas ok
 */
bool Panel_setWorkPage(PANEL *panel, Uint32 page);
bool Panel_setAllWorkPage(PANEL *panel);

/**
 * @brief Changer la page visible
 * @param panel pointeur de la structure PANEL
 * @param page numéro de la page
 * @return true si ok
 * @return false pas ok
 */
bool Panel_setVisiblePage(PANEL *panel, Uint32 page);

/**
 * @brief Retourne le numéro de la page actuelle de travail
 * @param panel pointeur de la structure PANEL
 * @return Uint32 numéro de la page
 */

Uint32 Panel_getWorkPage(PANEL *panel);
/**
 * @brief Retourne le numéro de la page de actuelle visible
 * @param panel pointeur de la structure PANEL
 * @return Uint32 numéro de la page
 */
Uint32 Panel_getVisiblePage(PANEL *panel);

/**
 * @brief Passer à la page suivante ( désactive les widget qui ne seront plus affiché et ceux qui le deviennent )
 * @param panel pointeur de la structure PANEL
 * @return true si OK
 * @return false si pas OK
 */
bool Panel_nextVisiblePage(PANEL *panel);

/**
 * @brief Passer à la page précédente ( désactive les widget qui ne seront plus affiché et ceux qui le deviennent )
 * @param panel pointeur de la structure PANEL
 * @return true si OK
 * @return false si pas OK
 */
bool Panel_afterVisiblePage(PANEL *panel);

void Panel_update(PANEL *panel);

/**
 * @brief Mettre à jour le panel
 * @param panel pointeur de la structure PANEL
 */
void Panel_event(PANEL *panel);

/**
 * @brief Afficher le panel
 * @param panel pointeur de la structure PANEL
 * @param innerRect (pointeur) zone duquel dépend le PANEL
 */
void Panel_draw(PANEL *panel, SDL_Rect *innerRect);

/* 
 * ------------------------------------------------
 *  Inner Box
 * ------------------------------------------------
*/

/**
 * @brief Zone dans lequel on peut insérer des widgets sans qu'il dépasse de cette zone
 */
typedef struct t_INNERBOX {
	Uint8 selfType;
	bool isEnable;

	IMAGE *image;
	SDL_Rect rect, tileRect;
	SDL_Rect area;

	ARRAY *widgets;
} INNERBOX;

/**
 * @brief Création de la structure INNERBOX
 * @param panel pointeur de la structure parent dans lequel s'affichera l'InnerBox
 * @param widegetName nom du widget
 * @param x position x de l'InnerBox dans le Panel
 * @param y position y de l'InnerBox dans le Panel
 * @param w largeur
 * @param h hauteur
 * @param areaX position x de l'InnerBox dans le Panel
 * @param areatY position y de l'InnerBox dans le Panel
 * @param areaW largeur
 * @param areaH hauteur
 * @param imageName nom du tileset d'image ( taille : 3 tiles x 3 tiles )
 * @return INNERBOX*
 */
INNERBOX *InnerBox_new(PANEL *panel, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, 
	Sint32 areaX, Sint32 areaY, Sint32 areaW, Sint32 areaH, char *imageName);

/**
 * @brief Destruction de la structure INNERBOX
 * @param innerbox pointeur de la structure INNERBOX
 */
void InnerBox_free(INNERBOX *innerbox);

/* On fait la même chose qu'avec les fontions du panel */
void InnerBox_enable(INNERBOX *innerBox);
void InnerBox_disable(INNERBOX *innerBox);
bool InnerBox_isEnable(INNERBOX *innerBox);
bool InnerBox_isDisable(INNERBOX *innerBox);

void InnerBox_update(INNERBOX *innerbox);

/**
 * @brief Evèments dans l'InnerBox
 * @param innerBox pointeur de la structure INNERBOX
 * @param innerRect (pointeur) zone duquel dépend l'INNERBOX
 */
void InnerBox_event(INNERBOX *innerBox, SDL_Rect *innerRect);

/**
 * @brief Afficher un INNERBOX avec tous les widgets qu'il possède
 * @param innerBox pointeur de la structure INNERBOX
 * @param innerRect (pointeur) zone duquel dépend l'INNERBOX
 */
void InnerBox_draw(INNERBOX *innerBox, SDL_Rect *innerRect);

/* 
 * ------------------------------------------------
 *  Sprite
 * ------------------------------------------------
*/

typedef struct t_SPRITE {
	Uint8 selfType;
	bool isEnable;

	IMAGE *image;
	SDL_Rect rect, tileRect;
	Uint16 nbTilesW, nbTilesH;

	Orientation orientation;
	
	Uint16 frameBetweenEachTile; // attente entre chaque tile (0 pour changement manuel)
	Uint16 frameBetweenAnimation; // attente à la fin de l'animation
	Uint16 frame; // compteur de frame
	Uint16 currentTile; // id de l'image à afficher
	bool endAnimation;

	bool isFocused;
	bool isPressed;
	bool isReleased;
	bool isHeld;
} SPRITE;

SPRITE* Sprite_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h,
	char *imageName, Uint16 nbTilesW, Uint16 nbTilesH, Orientation orientation, Uint16 frameBetweenEachTile, Uint16 frameBetweenAnimation);
void Sprite_free(SPRITE *sprite);

void Sprite_update(SPRITE *sprite);
void Sprite_event(SPRITE *sprite, SDL_Rect *innerRect);
void Sprite_next(SPRITE *sprite);
void Sprite_after(SPRITE *sprite);
void Sprite_draw(SPRITE *sprite, SDL_Rect *innerRect);


/* 
 * ------------------------------------------------
 *  Label
 * ------------------------------------------------
*/

/**
 * @brief zone de texte
 */
typedef struct t_LABEL {
	Uint8 selfType;
	bool isEnable;

	SDL_Rect rect;
	bool scroll;

	char *message;
	TTF_Font *font;
	Uint32 colorFont;

	bool Hcentered, Vcentered;
} LABEL;

/**
 * @brief Création de la structure LABEL
 * @param root pointeur de la structure parent dans lequel s'affichera l'InnerBox
 * @param widegetName nom du widget
 * @param x position x de l'InnerBox dans le Panel
 * @param y position y de l'InnerBox dans le Panel
 * @param w largeur
 * @param h hauteur
 * @param message texte à afficher
 * @param fontName nom de la police de texte
 * @param sizeFont taille de la police
 * @param colorFont couleur de la police
 * @return LABEL* 
 */
LABEL *Label_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h,
	char *message, char *fontName, Uint16 sizeFont, Uint32 colorFont);

/**
 * @brief Destruction de la structure LABEL
 * @param label pointeur de la structure LABEL
 */
void Label_free(LABEL *text);

void Label_setText(LABEL *label, char *text, ...);

void Label_changeColor(LABEL *label, Uint32 colorFont);

/**
 * @brief Evènement sur le label
 * @param label pointeur de la structure LABEL
 * @param innerRect (pointeur) zone duquel dépend le LABEL
 */
void Label_event(LABEL *label, SDL_Rect *innerRect);

/**
 * @brief Afficher le label
 * @param label pointeur de la structure LABEL
 * @param innerRect (pointeur) zone duquel dépend le LABEL
 */
void Label_draw(LABEL *label, SDL_Rect *innerRect);


/* 
 * ------------------------------------------------
 *  Button
 * ------------------------------------------------
*/

/**
 * @brief Bouton rectangulaire
 */
typedef struct t_BUTTON {
	Uint8 selfType;
	bool isEnable;

	IMAGE *image;
	SDL_Rect rect, tileRect;

	bool withMotif; /* si le bouton affiche un motif ou du texte*/
	IMAGE *motif;
	SDL_Rect motifTileRect;

	char *message;
	TTF_Font *font;
	Uint32 colorFont, focusedColorFont;

	Sint32 offsetPressedX; // decalage en abcsisse du texte ou du motif lorsque le bouton est pressé
	Sint32 offsetPressedY; // decalage en ordonnée du texte ou du motif lorsque le bouton est pressé

	bool isFocused;
	bool isPressed;
	bool isReleased;
	bool isHeld;
} BUTTON;

/**
 * @brief Création de la structure BUTTON
 * @param panel pointeur de la structure parent dans lequel s'affichera le bouton
 * @param widegetName nom du widget
 * @param x position x de l'InnerBox dans le Panel
 * @param y position y de l'InnerBox dans le Panel
 * @param w largeur
 * @param h hauteur
 * @param imageName nom du tileset d'image ( taille : 9 tiles x 3 tiles )
 * @param message teste à afficher dans le bouton
 * @param fontName nom de la police de texte
 * @param sizeFont taille de la police
 * @param colorFont couleur de la police
 * @param offsetPressedX  decalage en abcsisse du texte lorsque le bouton est pressé
 * @param offsetPressedY decalage en ordonnée du texte lorsque le bouton est pressé
 * @return BUTTON*
 */
BUTTON *Button_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h,
	char *imageName, char *message, char *fontName, Uint16 sizeFont, Uint32 colorFont, Uint32 focusedColorFont, Sint32 offsetPressedX, Sint32 offsetPressedY);

/**
 * @brief Création de la structure BUTTON
 * @param panel pointeur de la structure parent dans lequel s'affichera le bouton
 * @param widegetName nom du widget
 * @param x position x de l'InnerBox dans le Panel
 * @param y position y de l'InnerBox dans le Panel
 * @param w largeur
 * @param h hauteur
 * @param imageName nom du tileset d'image ( taille : 9 tiles x 3 tiles )
 * @param motifImageName nom du motif à afficher dans le bouton ( taille : 2 tiles x 1 tile )
 * @param offsetPressedX  decalage en abcsisse du motif lorsque le bouton est pressé
 * @param offsetPressedY decalage en ordonnée du motif lorsque le bouton est pressé
 * @return BUTTON*
 */
BUTTON *MotifButton_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h,
	char *imageName, char *motifImageName, Sint32 offsetPressedX, Sint32 offsetPressedY);

/**
 * @brief  Destruction de la structure BUTTON
 * @param button pointeur de la structure BUTTON
 */
void Button_free(BUTTON *button);

/**
 * @brief Mettre à jour le bouton
 * @param button pointeur de la structure BUTTON
 * @param innerRect (pointeur) zone duquel dépend le bouton
 */
void Button_event(BUTTON *button, SDL_Rect *innerRect);

/**
 * @brief Afficher le bouton
 * @param button pointeur de la structure BUTTON
 * @param innerRect (pointeur) zone duquel dépend le bouton
 */
void Button_draw(BUTTON *button, SDL_Rect *innerRect);

/* 
 * ------------------------------------------------
 *  Toggle Switch
 * ------------------------------------------------
*/

/**
 * @brief Intérrupteur qui change d'état lorsque l'on clic dessus (ON ou OFF)
 */
typedef struct t_TOGGLE_SWITCH {
	Uint8 selfType;
	bool isEnable;

	IMAGE *image;
	SDL_Rect rect, tileRect;

	char *messageON;
	char *messageOFF;
	TTF_Font *font;
	Uint32 colorFont, focusedColorFont;

	Sint32 offsetPressedX; // decalage en abcsisse du texte ou du motif lorsque le toogleSwitch est pressé
	Sint32 offsetPressedY; // decalage en ordonnée du texte ou du motif lorsque le toogleSwitch est pressé

	bool isFocused;
	bool isPressed;
	bool isReleased;
	bool isHeld;
	bool state;
} TOGGLE_SWITCH;

/**
 * @brief Création d'un TOGGLE_SWITCH
 * @param root pointeur de la structure parent dans lequel s'affichera l'interrupteur
 * @param widegetName nom du widget
 * @param x position x de l'InnerBox dans le Panel
 * @param y position y de l'InnerBox dans le Panel
 * @param w largeur
 * @param h hauteur
 * @param imageName nom du tileset d'image ( taille : 9 tiles x 3 tiles )
 * @param messageON message à afficher dans la partie droite
 * @param messageOFF message à afficher dans la partie gauche
 * @param fontName police du texte
 * @param sizeFont taille du texte
 * @param colorFont couleur
 * @param offsetPressedX decalage en abcsisse du texte lorsque le bouton est pressé
 * @param offsetPressedY decalage en ordonnée du texte lorsque le bouton est pressé
 * @param state état initiale du bouton
 * @return TOGGLE_SWITCH* 
 */
TOGGLE_SWITCH *ToggleSwitch_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h,
	char *imageName, char *messageON, char *messageOFF, char *fontName, Uint16 sizeFont, Uint32 colorFont, Uint32 focusedColorFont,
	Sint32 offsetPressedX, Sint32 offsetPressedY, bool state);

/**
 * @brief Destruction d'un TOGGLE_SWITCH
 * @param toggleSwitch 
 */
void ToggleSwitch_free(TOGGLE_SWITCH *toggleSwitch);

/**
 * @brief Mise à jour d'un TOGGLE_SWITCH
 * @param toggleSwitch
 * @param innerRect 
 */
void ToggleSwitch_event(TOGGLE_SWITCH *toggleSwitch, SDL_Rect *innerRect);

/**
 * @brief Affichage d'un TOGGLE_SWITCH
 * @param toggleSwitch
 * @param innerRect 
 */
void ToggleSwitch_draw(TOGGLE_SWITCH *toggleSwitch, SDL_Rect *innerRect);

/* 
 * ------------------------------------------------
 *  Scrollbar
 * ------------------------------------------------
*/

/**
 * @brief barre de défilement
 */
typedef struct t_SCROLLBAR {
	Uint8 selfType;
	bool isEnable;

	IMAGE *image;
	SDL_Rect rect, tileRect;
	Orientation orientation;

	/* .... */

	bool isFocused;
	bool isPressed;
	bool isReleased;
	bool isHeld;
} SCROLLBAR;

SCROLLBAR *Horizontal_ScrollBar_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, char *imageName,
	Uint32 beginValue, Uint32 endValue);
SCROLLBAR *Vertical_ScrollBar_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, char *imageName,
	Uint32 beginValue, Uint32 endValue);
void ScrollBar_free(SCROLLBAR *scrollBar);

void ScrollBar_event(SCROLLBAR *scrollBar, SDL_Rect *innerRect);
void ScrollBar_draw(SCROLLBAR *scrollBar, SDL_Rect *innerRect);

/* 
 * ------------------------------------------------
 *  Levelbar
 * ------------------------------------------------
*/

/**
 * @brief Barre de niveau
 */
typedef struct t_LEVELBAR {
	Uint8 selfType;
	bool isEnable;

	IMAGE *image;
	SDL_Rect rect, tileRect;
	Orientation orientation;

	/* .... */

	bool isFocused;
	bool isPressed;
	bool isReleased;
	bool isHeld;
} LEVELBAR;

LEVELBAR *Horizontal_LevelBar_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, char *imageName /* ... */);
LEVELBAR *Vertical_LevelBar_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, char *imageName /* ... */);
void LevelBar_free(LEVELBAR *LevelBar);

void LevelBar_event(LEVELBAR *LevelBar, SDL_Rect *innerRect);
void LevelBar_draw(LEVELBAR *LevelBar, SDL_Rect *innerRect);


/* 
 * ------------------------------------------------
 *  ComboBox
 * ------------------------------------------------
*/

/**
 * @brief Elément d'interface graphique qui réunit une zone de texte dans un innerBox et une liste déroulante avec un scrollbar
 */
typedef struct t_COMBOBOX {
	Uint8 selfType;
	bool isEnable;

	INNERBOX *innerBox;
	SCROLLBAR *scrollBar;

	SDL_Rect rect;

	ARRAY *items;

	/* .... */

	bool isFocused;
	bool isPressed;
	bool isReleased;
	bool isHeld;
} COMBOBOX;

COMBOBOX *ComboBox_new(PANEL *panel, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 scrollBar_h,
	char *innerBoxImageName, char *scrollBarImageName,
	char *items[], Uint16 nbItems, char *fontName, Uint16 sizeFont, Uint32 colorFont, Uint32 highlightColorFont   /* ... */);
void ComboBox_free(COMBOBOX *comboBox);

void ComboBox_addItems(COMBOBOX *comboBox, char *items[], Uint16 nbItems);

void ComboBox_event(COMBOBOX *comboBox, SDL_Rect *innerRect);
void ComboBox_draw(COMBOBOX *comboBox, SDL_Rect *innerRect);


#endif /* __GUI_H__ */