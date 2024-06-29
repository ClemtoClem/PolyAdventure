#include "gui.h"

void Widget_enable(DATA *data) {
	if (data != NULL) {
		switch(data->type) {
			case PANEL_WIDGET_TYPE: Panel_enable((PANEL *) data->buffer); break;
			case INNERBOX_WIDGET_TYPE: InnerBox_enable((INNERBOX *) data->buffer); break;
			case SPRITE_WIDGET_TYPE:
			case LABEL_WIDGET_TYPE:
			case BUTTON_WIDGET_TYPE:
			case TOGGLE_SWITCH_WIDGET_TYPE:
			case SCROLLBAR_WIDGET_TYPE:
			case LEVELBAR_WIDGET_TYPE:
			case COMBOBOX_WIDGET_TYPE:
				Uint8 *buffer = (Uint8 *) data->buffer;
				buffer[1] = true;
				break;
			default: break;
		}
	}
}

void Widget_disable(DATA *data) {
	if (data != NULL) {
		switch(data->type) {
			case PANEL_WIDGET_TYPE: Panel_disable((PANEL *) data->buffer); break;
			case INNERBOX_WIDGET_TYPE: InnerBox_disable((INNERBOX *) data->buffer); break;
			case SPRITE_WIDGET_TYPE:
			case LABEL_WIDGET_TYPE:
			case BUTTON_WIDGET_TYPE:
			case TOGGLE_SWITCH_WIDGET_TYPE:
			case SCROLLBAR_WIDGET_TYPE:
			case COMBOBOX_WIDGET_TYPE:
				Uint8 *buffer = (Uint8 *) data->buffer;
				buffer[1] = true;
				break;
			default: break;
		}
	}
}

void ListWidgets_enable(ARRAY *listWidgets) {
	if (listWidgets != NULL) {
		DATA *data = listWidgets->first;
		while (data != NULL) {
			Widget_enable(data);
			data = data->next;
		}
	}
}

void ListWidgets_disable(ARRAY *listWidgets) {
	if (listWidgets != NULL) {
		DATA *data = listWidgets->first;
		while (data != NULL) {
			Widget_disable(data);
			data = data->next;
		}
	}
}

bool Widget_isWidget(DATA *data) {
	bool result = false;
	if (data != NULL) {
		switch(data->type) {
			case PANEL_WIDGET_TYPE:
			case INNERBOX_WIDGET_TYPE:
			case SPRITE_WIDGET_TYPE:
			case LABEL_WIDGET_TYPE:
			case BUTTON_WIDGET_TYPE:
			case TOGGLE_SWITCH_WIDGET_TYPE:
			case SCROLLBAR_WIDGET_TYPE:
			case COMBOBOX_WIDGET_TYPE:
				result = true;
			default: break;
		}
	}
	return result;
}

bool Widget_isEnable(DATA *data) {
	bool result = false;
	if (Widget_isWidget(data) == true) {
		Uint8 *buffer = (Uint8 *) data->buffer;
		result = buffer[1];
	}
	return result;
}

bool Widget_isDisable(DATA *data) {
	bool result = false;
	if (Widget_isWidget(data) == true) {
		Uint8 *buffer = (Uint8 *) data->buffer;
		result = !buffer[1];
	}
	return result;
}

TypeData Widget_getType(void *widget) {
	TypeData result = NIL_TYPE;
	if (widget != NULL) {
		Uint8 *buffer = (Uint8 *) widget;
		result = buffer[0];
	}
	return result;
}

void *Widget_getVisible(void *root, char *widgetName) {
	void *widget = NULL;
	if (root != NULL) {
		TypeData type = Widget_getType(root);
		if (type == PANEL_WIDGET_TYPE) {
			PANEL *panel = root;
			widget = Array_getFromKey(panel->widgets[panel->visiblePage], widgetName);
			if (widget == NULL) widget = Array_getFromKey(panel->globalWidgets, widgetName);
		} else if (type == INNERBOX_WIDGET_TYPE) {
			widget = Array_getFromKey(((INNERBOX *) root)->widgets, widgetName);
		}
	}
	return widget;
}

bool Widget_existInRoot(void *root, char *widgetName) {
	bool exist = false;
	if (root != NULL) {
		TypeData type = Widget_getType(root);
		if (type == PANEL_WIDGET_TYPE) {
			PANEL *panel = root;
			exist = (Array_getFromKey(panel->widgets[panel->workPage], widgetName) != NULL);
			if (exist == false) exist = (Array_getFromKey(panel->globalWidgets, widgetName) != NULL);
		} else if (type == INNERBOX_WIDGET_TYPE) {
			exist = (Array_getFromKey(((INNERBOX *) root)->widgets, widgetName) != NULL);
		}
	}
	return exist;
}

bool Widget_addToRoot(void *root, char *widgetName, void *widget) {
	bool result = false;
	if (root != NULL && widget != NULL) {
		TypeData typeRoot = Widget_getType(root);
		TypeData typeChild = Widget_getType(widget);
		if (typeRoot == PANEL_WIDGET_TYPE) {
			PANEL *panel = root;
			/* on vérifie que le widget n'existe pas avant de l'ajouter */
			if (Array_getFromKey(panel->globalWidgets, widgetName) == NULL) {
				DATA *data = Data_new(ID_DEFAULT, widgetName, typeChild, widget, Widget_free);
				if (panel->workAllPage == false) {
					// on ajoute le widget sur la page de travail du panel
					Array_append(panel->widgets[panel->workPage], data);
				} else {
					// on ajoute le widget au panel
					Array_append(panel->globalWidgets, data);
					Widget_enable(Array_getFromKey(panel->globalWidgets, widgetName));
				}
				result = true;
			}
		} else if (typeRoot == INNERBOX_WIDGET_TYPE) {
			INNERBOX *innerBox = root;
			/* on vérifie que le widget n'existe pas avant de l'ajouter */
			if (Array_getFromKey(innerBox->widgets, widgetName) == NULL) {
				DATA *data = Data_new(ID_DEFAULT, widgetName, typeChild, widget, Widget_free);
				Array_append(innerBox->widgets, data);
				result = true;
			}
		}
	}
	return result;
}

void Widget_free(void *buffer, TypeData type) {
	if (buffer != NULL) {
		switch(type) {
			case PANEL_WIDGET_TYPE:
				Panel_free((PANEL *) buffer);
				break;
			case INNERBOX_WIDGET_TYPE:
				InnerBox_free((INNERBOX *) buffer);
				break;
			case SPRITE_WIDGET_TYPE:
				Sprite_free((SPRITE *) buffer);
				break;
			case LABEL_WIDGET_TYPE:
				Label_free((LABEL *) buffer);
				break;
			case BUTTON_WIDGET_TYPE:
				Button_free((BUTTON *) buffer);
				break;
			case TOGGLE_SWITCH_WIDGET_TYPE:
				ToggleSwitch_free((TOGGLE_SWITCH *) buffer);
				break;
			case SCROLLBAR_WIDGET_TYPE:
				ScrollBar_free((SCROLLBAR *) buffer);
				break;
			case LEVELBAR_WIDGET_TYPE:
				LevelBar_free((LEVELBAR *) buffer);
				break;
			case COMBOBOX_WIDGET_TYPE:
				ComboBox_free((COMBOBOX *) buffer);
				break;
			default:
				break;
		}
	}
}

bool Widget_onFocused(void *root, char *widgetName) {
	bool result = false;
	if (root != NULL) {
		TypeData typeRoot = Widget_getType(root);
		DATA *data = NULL;
		if (typeRoot == PANEL_WIDGET_TYPE) {
			PANEL *panel = root;
			data = Array_getFromKey(panel->widgets[panel->visiblePage], widgetName);
			if (data == NULL) data = Array_getFromKey(panel->globalWidgets, widgetName);
		} else if (typeRoot == INNERBOX_WIDGET_TYPE) {
			data = Array_getFromKey(((INNERBOX *) root)->widgets, widgetName);
		}
		if (data != NULL) {
			switch(data->type) {
				case INNERBOX_WIDGET_TYPE:
					result = Widget_onFocused(data->buffer, widgetName);
					break;
				case SPRITE_WIDGET_TYPE:
					result = ((SPRITE *) data->buffer)->isFocused;
					break;
				case BUTTON_WIDGET_TYPE:
					result = ((BUTTON *) data->buffer)->isFocused;
					break;
				case TOGGLE_SWITCH_WIDGET_TYPE:
					result = ((TOGGLE_SWITCH *) data->buffer)->isFocused;
					break;
				case SCROLLBAR_WIDGET_TYPE:
					result = ((SCROLLBAR *) data->buffer)->isFocused;
					break;
				case LEVELBAR_WIDGET_TYPE:
					result = ((LEVELBAR *) data->buffer)->isFocused;
					break;
				case COMBOBOX_WIDGET_TYPE:
					result = ((COMBOBOX *) data->buffer)->isFocused;
					break;
				default:
					break;
			}
		}
	}
	return result;
}

bool Widget_onPressed(void *root, char *widgetName) {
	bool result = false;
	if (root != NULL) {
		TypeData typeRoot = Widget_getType(root);
		DATA *data = NULL;
		if (typeRoot == PANEL_WIDGET_TYPE) {
			PANEL *panel = root;
			data = Array_getFromKey(panel->widgets[panel->visiblePage], widgetName);
			if (data == NULL) data = Array_getFromKey(panel->globalWidgets, widgetName);
		} else if (typeRoot == INNERBOX_WIDGET_TYPE) {
			data = Array_getFromKey(((INNERBOX *) root)->widgets, widgetName);
		}
		if (data != NULL) {
			switch(data->type) {
				case INNERBOX_WIDGET_TYPE:
					result = Widget_onPressed(data->buffer, widgetName);
					break;
				case SPRITE_WIDGET_TYPE:
					result = ((SPRITE *) data->buffer)->isPressed;
					break;
				case BUTTON_WIDGET_TYPE:
					result = ((BUTTON *) data->buffer)->isPressed;
					break;
				case TOGGLE_SWITCH_WIDGET_TYPE:
					result = ((TOGGLE_SWITCH *) data->buffer)->isPressed;
					break;
				case SCROLLBAR_WIDGET_TYPE:
					result = ((SCROLLBAR *) data->buffer)->isPressed;
					break;
				case LEVELBAR_WIDGET_TYPE:
					result = ((LEVELBAR *) data->buffer)->isPressed;
					break;
				case COMBOBOX_WIDGET_TYPE:
					result = ((COMBOBOX *) data->buffer)->isPressed;
					break;
				default:
					break;
			}
		}
	}
	return result;
}

bool Widget_onReleased(void *root, char *widgetName) {
	bool result = false;
	if (root != NULL) {
		TypeData typeRoot = Widget_getType(root);
		DATA *data = NULL;
		if (typeRoot == PANEL_WIDGET_TYPE) {
			PANEL *panel = root;
			data = Array_getFromKey(panel->widgets[panel->visiblePage], widgetName);
			if (data == NULL) data = Array_getFromKey(panel->globalWidgets, widgetName);
		} else if (typeRoot == INNERBOX_WIDGET_TYPE) {
			data = Array_getFromKey(((INNERBOX *) root)->widgets, widgetName);
		}
		if (data != NULL) {
			switch(data->type) {
				case INNERBOX_WIDGET_TYPE:
					result = Widget_onReleased(data->buffer, widgetName);
					break;
				case SPRITE_WIDGET_TYPE:
					result = ((SPRITE *) data->buffer)->isReleased;
					break;
				case BUTTON_WIDGET_TYPE:
					result = ((BUTTON *) data->buffer)->isReleased;
					break;
				case TOGGLE_SWITCH_WIDGET_TYPE:
					result = ((TOGGLE_SWITCH *) data->buffer)->isReleased;
					break;
				case SCROLLBAR_WIDGET_TYPE:
					result = ((SCROLLBAR *) data->buffer)->isReleased;
					break;
				case LEVELBAR_WIDGET_TYPE:
					result = ((LEVELBAR *) data->buffer)->isReleased;
					break;
				case COMBOBOX_WIDGET_TYPE:
					result = ((COMBOBOX *) data->buffer)->isReleased;
					break;
				default:
					break;
			}
		}
	}
	return result;
}

bool Widget_onHeld(void *root, char *widgetName) {
	bool result = false;
	if (root != NULL) {
		TypeData typeRoot = Widget_getType(root);
		DATA *data = NULL;
		if (typeRoot == PANEL_WIDGET_TYPE) {
			PANEL *panel = root;
			data = Array_getFromKey(panel->widgets[panel->visiblePage], widgetName);
			if (data == NULL) data = Array_getFromKey(panel->globalWidgets, widgetName);
		} else if (typeRoot == INNERBOX_WIDGET_TYPE) {
			data = Array_getFromKey(((INNERBOX *) root)->widgets, widgetName);
		}
		if (data != NULL) {
			switch(data->type) {
				case INNERBOX_WIDGET_TYPE:
					result = Widget_onHeld(data->buffer, widgetName);
					break;
				case SPRITE_WIDGET_TYPE:
					result = ((SPRITE *) data->buffer)->isHeld;
					break;
				case BUTTON_WIDGET_TYPE:
					result = ((BUTTON *) data->buffer)->isHeld;
					break;
				case TOGGLE_SWITCH_WIDGET_TYPE:
					result = ((TOGGLE_SWITCH *) data->buffer)->isHeld;
					break;
				case SCROLLBAR_WIDGET_TYPE:
					result = ((SCROLLBAR *) data->buffer)->isHeld;
					break;
				case LEVELBAR_WIDGET_TYPE:
					result = ((LEVELBAR *) data->buffer)->isHeld;
					break;
				case COMBOBOX_WIDGET_TYPE:
					result = ((COMBOBOX *) data->buffer)->isHeld;
					break;
				default:
					break;
			}
		}
	}
	return result;
}

/* 
 * ------------------------------------------------
 *  Panel
 * ------------------------------------------------
*/

PANEL *Panel_new(PANEL *rootpanel, char *widgetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, char *imageName, Uint32 nbPages) {
	PANEL *panel = NULL;
	bool exist = false;
	if (rootpanel != NULL && widgetName != NULL && Widget_existInRoot(rootpanel, widgetName)) exist = true;
	if (exist == false) {
		panel = (PANEL *) malloc(sizeof(PANEL));
		if (panel != NULL) {
			panel->selfType = PANEL_WIDGET_TYPE;
			panel->isEnable = false;

			if (imageName != NULL) {
				panel->image = SG_loadImage(imageName);

				panel->tileRect.w = panel->image->w / 3;
				panel->tileRect.h = panel->image->h / 3;
				panel->rect.w = (w >= panel->tileRect.w*2) ? w : panel->tileRect.w*2;
				panel->rect.h = (h >= panel->tileRect.h*2) ? h : panel->tileRect.h*2;
			} else {
				panel->image = NULL;
				panel->rect.w = w;
				panel->rect.h = h;
			}

			panel->rect.x = x;
			panel->rect.y = y;

			panel->nbPages = (nbPages == 0) + nbPages ;
			panel->workPage = 0;
			panel->workAllPage = true;
			panel->visiblePage = 0;
			panel->widgets = (ARRAY **) malloc(panel->nbPages * sizeof(ARRAY *));

			Uint32 page;
			for (page = 0; page < panel->nbPages; page++)
				panel->widgets[page] = (ARRAY *) Array_new(NO_LIMITED);

			panel->globalWidgets = Array_new(NO_LIMITED);

			if (rootpanel != NULL && widgetName != NULL)
				Widget_addToRoot(rootpanel, widgetName, panel);
		}
	}
	return panel;
}

void Panel_free(PANEL *panel) {
	if (panel != NULL) {
		Uint32 page;
		/*for (page = 0; page < panel->nbPages; page++) {
			Array_free(panel->widgets[page], true);
			panel->widgets[page] = NULL;
		}*/
		free(panel->widgets);
		panel->widgets = NULL;
		Array_free(panel->globalWidgets, true);
		panel->globalWidgets = NULL;
		free(panel);
	}
}

void Panel_enable(PANEL *panel) {
	if (panel != NULL && panel->isEnable == false) {
		ListWidgets_enable(panel->widgets[panel->visiblePage]);
		ListWidgets_enable(panel->globalWidgets);
		panel->isEnable = true;
	}
}

void Panel_disable(PANEL *panel) {
	if (panel != NULL && panel->isEnable == true) {
		ListWidgets_disable(panel->widgets[panel->visiblePage]);
		ListWidgets_disable(panel->globalWidgets);
		panel->isEnable = false;
	}
}

bool Panel_isEnable(PANEL *panel) {
	bool result = false;
	if (panel != NULL) {
		result = panel->isEnable;
	}
	return result;
}

bool Panel_isDisable(PANEL *panel) {
	bool result = true;
	if (panel != NULL) {
		result = !panel->isEnable;
	}
	return result;
}

bool Panel_setAllWorkPage(PANEL *panel) {
	bool result = false;
	if (panel != NULL) {
		panel->workAllPage = true;
		result = true;
	}
	return result;
}

bool Panel_setWorkPage(PANEL *panel, Uint32 page) {
	bool result = false;
	if (panel != NULL && page < panel->nbPages) {
		panel->workPage = page;
		panel->workAllPage = false;
		result = true;
	}
	return result;
}

bool Panel_setVisiblePage(PANEL *panel, Uint32 page) {
	bool result = false;
	if (panel != NULL && page < panel->nbPages) {
		/* désactiver les objets qui ne s'afficherons plus */
		if (panel->isEnable) {
			ListWidgets_disable(panel->widgets[panel->visiblePage]);
		}

		panel->visiblePage = page;

		/* activer les objets qui afficherons */
		if (panel->isEnable) {
			ListWidgets_enable(panel->widgets[panel->visiblePage]);
		}

		result = true;
	}
	return result;
}

Uint32 Panel_getWorkPage(PANEL *panel) {
	return panel->workPage;
}

Uint32 Panel_getVisiblePage(PANEL *panel) {
	return panel->visiblePage;
}

bool Panel_nextVisiblePage(PANEL *panel) {
	bool result = false;
	if (panel != NULL && panel->visiblePage < panel->nbPages-1) {
		/* désactiver les objets qui ne s'afficherons plus */
		if (panel->isEnable) {
			ListWidgets_disable(panel->widgets[panel->visiblePage]);
		}

		panel->visiblePage ++;

		/* activer les objets qui afficherons */
		if (panel->isEnable) {
			ListWidgets_enable(panel->widgets[panel->visiblePage]);
		}

		result = true;
	}
	return result;
}

bool Panel_afterVisiblePage(PANEL *panel) {
	bool result = false;
	if (panel != NULL && panel->visiblePage > 0) {
		/* désactiver les objets qui ne s'afficherons plus */
		if (panel->isEnable) {
			ListWidgets_disable(panel->widgets[panel->visiblePage]);
		}

		panel->visiblePage --;

		/* activer les objets qui afficherons */
		if (panel->isEnable) {
			ListWidgets_enable(panel->widgets[panel->visiblePage]);
		}

		result = true;
	}
	return result;
}

void Panel_update(PANEL *panel) {
	if (panel != NULL && panel->isEnable == true) {
		/* Mise à jour des widgets */
		DATA *data = panel->widgets[panel->visiblePage]->first;
		while (data != NULL) {
			switch(data->type) {
				case PANEL_WIDGET_TYPE:
					Panel_update((PANEL *) data->buffer);
					break;
				case INNERBOX_WIDGET_TYPE:
					InnerBox_update((INNERBOX *) data->buffer);
					break;
				case SPRITE_WIDGET_TYPE:
					Sprite_update((SPRITE *) data->buffer);
					break;
				default:
					break;
			}
			data = data->next;
		}

		data = panel->globalWidgets->first;
		while (data != NULL) {
			switch(data->type) {
				case PANEL_WIDGET_TYPE:
					Panel_update((PANEL *) data->buffer);
					break;
				case INNERBOX_WIDGET_TYPE:
					InnerBox_update((INNERBOX *) data->buffer);
					break;
				case SPRITE_WIDGET_TYPE:
					Sprite_update((SPRITE *) data->buffer);
					break;
				default:
					break;
			}
			data = data->next;
		}
	}
}

void Panel_event(PANEL *panel) {
	if (panel != NULL && panel->isEnable == true) {
		/* Evènement sur les widgets */
		DATA *data = panel->widgets[panel->visiblePage]->first;
		while (data != NULL) {
			switch(data->type) {
				case PANEL_WIDGET_TYPE:
					Panel_event((PANEL *) data->buffer);
					break;
				case INNERBOX_WIDGET_TYPE:
					InnerBox_event((INNERBOX *) data->buffer, &panel->rect);
					break;
				case SPRITE_WIDGET_TYPE:
					Sprite_event((SPRITE *) data->buffer, &panel->rect);
					break;
				case LABEL_WIDGET_TYPE:
					Label_event((LABEL *) data->buffer, &panel->rect);
					break;
				case BUTTON_WIDGET_TYPE:
					Button_event((BUTTON *) data->buffer, &panel->rect);
					break;
				case TOGGLE_SWITCH_WIDGET_TYPE:
					ToggleSwitch_event((TOGGLE_SWITCH *) data->buffer, &panel->rect);
					break;
				default:
					break;
			}
			data = data->next;
		}

		data = panel->globalWidgets->first;
		while (data != NULL) {
			switch(data->type) {
				case PANEL_WIDGET_TYPE:
					Panel_event((PANEL *) data->buffer);
					break;
				case INNERBOX_WIDGET_TYPE:
					InnerBox_event((INNERBOX *) data->buffer, &panel->rect);
					break;
				case SPRITE_WIDGET_TYPE:
					Sprite_event((SPRITE *) data->buffer, &panel->rect);
					break;
				case LABEL_WIDGET_TYPE:
					Label_event((LABEL *) data->buffer, &panel->rect);
					break;
				case BUTTON_WIDGET_TYPE:
					Button_event((BUTTON *) data->buffer, &panel->rect);
					break;
				case TOGGLE_SWITCH_WIDGET_TYPE:
					ToggleSwitch_event((TOGGLE_SWITCH *) data->buffer, &panel->rect);
					break;
				default:
					break;
			}
			data = data->next;
		}
	}
}

void Panel_draw(PANEL *panel, SDL_Rect *innerRect) {
	if (panel != NULL && panel->isEnable == true) {
		if (panel->image != NULL) {
			SDL_Rect src, dest;
			Sint32 decalX, decalY;

			if (innerRect != NULL) {
				/* on adapte la position de l'innerBox dans l'interface par rapport à sa position dans le widget parents */
				decalX = innerRect->x + panel->rect.x;
				decalY = innerRect->y + panel->rect.y;
			} else {
				/* position du panel dans la fenêtre */
				decalX = panel->rect.x;
				decalY = panel->rect.y;
			}

			src.w = panel->tileRect.w;
			src.h = panel->tileRect.h;

			if (panel->rect.w > panel->tileRect.w*2) {
				/* côté haut */
				src.x = panel->tileRect.w;
				src.y = 0;
				dest.x = decalX + panel->tileRect.w;
				dest.y = decalY;
				dest.w = panel->rect.w - panel->tileRect.w*2;
				dest.h = panel->tileRect.h;
				SG_render(panel->image->texture, &src, &dest);
					
				/* côté bas */
				src.x = panel->tileRect.w;
				src.y = panel->tileRect.h*2;
				dest.x = decalX + panel->tileRect.w;
				dest.y = decalY + panel->rect.h - panel->tileRect.h;
				dest.w = panel->rect.w - panel->tileRect.w*2;
				dest.h = panel->tileRect.h;
				SG_render(panel->image->texture, &src, &dest);
			}
			
			if (panel->rect.h > panel->tileRect.h*2) {
				/* côté gauche */
				src.x = 0;
				src.y = panel->tileRect.h;
				dest.x = decalX;
				dest.y = decalY + panel->tileRect.h;
				dest.w = panel->tileRect.w;
				dest.h = panel->rect.h - panel->tileRect.h*2;
				SG_render(panel->image->texture, &src, &dest);
					
				/* côté droite */
				src.x = panel->tileRect.w * 2;
				src.y = panel->tileRect.h;
				dest.x = decalX + panel->rect.w - panel->tileRect.w;
				dest.y = decalY + panel->tileRect.h;
				dest.w = panel->tileRect.w;
				dest.h = panel->rect.h - panel->tileRect.h*2;
				SG_render(panel->image->texture, &src, &dest);
			}
			
			if ((panel->rect.w > panel->tileRect.w*2) && (panel->rect.h > panel->tileRect.h*2)) {
				/* centre */
				src.x = panel->tileRect.w;
				src.y = panel->tileRect.h;
				dest.x = decalX + panel->tileRect.w;
				dest.y = decalY + panel->tileRect.h;
				dest.w = panel->rect.w - panel->tileRect.w*2;
				dest.h = panel->rect.h - panel->tileRect.h*2;
				SG_render(panel->image->texture, &src, &dest);
			}

			/* coin haut gauche */
			src.x = 0;
			src.y = 0;
			panel->tileRect.x = panel->rect.x;
			panel->tileRect.y = panel->rect.y;
			if (innerRect != NULL) {
				panel->tileRect.x += innerRect->x;
				panel->tileRect.y += innerRect->y;
			}
			SG_render(panel->image->texture, &src, &panel->tileRect);

			/* coin haut droit */
			src.x = panel->tileRect.w*2;
			src.y = 0;
			panel->tileRect.x = panel->rect.x + panel->rect.w - panel->tileRect.w;
			panel->tileRect.y = panel->rect.y;
			if (innerRect != NULL) {
				panel->tileRect.x += innerRect->x;
				panel->tileRect.y += innerRect->y;
			}
			SG_render(panel->image->texture, &src, &panel->tileRect);
			
			/* coin bas gauche */
			src.x = 0;
			src.y = panel->tileRect.h*2;
			panel->tileRect.x = panel->rect.x;
			panel->tileRect.y = panel->rect.y + panel->rect.h - panel->tileRect.h;
			if (innerRect != NULL) {
				panel->tileRect.x += innerRect->x;
				panel->tileRect.y += innerRect->y;
			}
			SG_render(panel->image->texture, &src, &panel->tileRect);
			
			/* coin bas droit */
			src.x = panel->tileRect.w*2;
			src.y = panel->tileRect.h*2;
			panel->tileRect.x = panel->rect.x + panel->rect.w - panel->tileRect.w;
			panel->tileRect.y = panel->rect.y + panel->rect.h - panel->tileRect.h;
			if (innerRect != NULL) {
				panel->tileRect.x += innerRect->x;
				panel->tileRect.y += innerRect->y;
			}
			SG_render(panel->image->texture, &src, &panel->tileRect);
		}
		
		/* On affiche les widget présent dans le panel */
		
		DATA *data = panel->widgets[panel->visiblePage]->first;
		while (data != NULL) {
			switch(data->type) {
				case PANEL_WIDGET_TYPE:
					Panel_draw((PANEL *) data->buffer, &panel->rect);
					break;
				case INNERBOX_WIDGET_TYPE:
					InnerBox_draw((INNERBOX *) data->buffer, &panel->rect);
					break;
				case SPRITE_WIDGET_TYPE:
					Sprite_draw((SPRITE *) data->buffer, &panel->rect);
					break;
				case LABEL_WIDGET_TYPE:
					Label_draw((LABEL *) data->buffer, &panel->rect);
					break;
				case BUTTON_WIDGET_TYPE:
					Button_draw((BUTTON *) data->buffer, &panel->rect);
					break;
				case TOGGLE_SWITCH_WIDGET_TYPE:
					ToggleSwitch_draw((TOGGLE_SWITCH *) data->buffer, &panel->rect);
					break;
				default:
					break;
			}
			data = data->next;
		}

		data = panel->globalWidgets->first;
		while (data != NULL) {
			switch(data->type) {
				case PANEL_WIDGET_TYPE:
					Panel_draw((PANEL *) data->buffer, &panel->rect);
					break;
				case INNERBOX_WIDGET_TYPE:
					InnerBox_draw((INNERBOX *) data->buffer, &panel->rect);
					break;
				case SPRITE_WIDGET_TYPE:
					Sprite_draw((SPRITE *) data->buffer, &panel->rect);
					break;
				case LABEL_WIDGET_TYPE:
					Label_draw((LABEL *) data->buffer, &panel->rect);
					break;
				case BUTTON_WIDGET_TYPE:
					Button_draw((BUTTON *) data->buffer, &panel->rect);
					break;
				case TOGGLE_SWITCH_WIDGET_TYPE:
					ToggleSwitch_draw((TOGGLE_SWITCH *) data->buffer, &panel->rect);
					break;
				default:
					break;
			}
			data = data->next;
		}
	}
}

/* 
 * ------------------------------------------------
 *  Inner Box
 * ------------------------------------------------
*/

/* taille de l'image : 3 tiles x 3 tiles */
INNERBOX *InnerBox_new(PANEL *panel, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, 
	Sint32 areaX, Sint32 areaY, Sint32 areaW, Sint32 areaH, char *imageName) {
	INNERBOX *innerBox = NULL;
	bool exist = false;
	if (panel != NULL && widegetName != NULL && Widget_existInRoot(panel, widegetName)) exist = true;
	if (exist == false) {
		innerBox = (INNERBOX *) malloc(sizeof(INNERBOX));
		innerBox->selfType = INNERBOX_WIDGET_TYPE;
		innerBox->isEnable = false;

		if (imageName != NULL) {
			innerBox->image = SG_loadImage(imageName);

			innerBox->tileRect.w = innerBox->image->w / 3;
			innerBox->tileRect.h = innerBox->image->h / 3;
			innerBox->rect.w = (w >= innerBox->tileRect.w*2) ? w : innerBox->tileRect.w*2;
			innerBox->rect.h = (h >= innerBox->tileRect.h*2) ? h : innerBox->tileRect.h*2;
		} else {
			innerBox->image = NULL;
			innerBox->rect.w = w;
			innerBox->rect.h = h;
		}

		innerBox->rect.x = x;
		innerBox->rect.y = y;
		
		innerBox->area.x = areaX;
		innerBox->area.y = areaY;
		innerBox->area.w = (w >= areaX) ? w : areaW;
		innerBox->area.h = (h >= areaY) ? h : areaH;

		innerBox->widgets = Array_new(NO_LIMITED);

		if (panel != NULL && widegetName != NULL)
			Widget_addToRoot(panel, widegetName, innerBox);
	}

	return innerBox;
}

void InnerBox_free(INNERBOX *innerbox) {
	if (innerbox != NULL) {
		Array_free(innerbox->widgets, true);
	}
}

void InnerBox_enable(INNERBOX *innerbox) {
	if (innerbox != NULL && innerbox->isEnable == false) {
		ListWidgets_enable(innerbox->widgets);
		innerbox->isEnable = true;
	}
}

void InnerBox_disable(INNERBOX *innerbox) {
	if (innerbox != NULL && innerbox->isEnable == true) {
		ListWidgets_disable(innerbox->widgets);
		innerbox->isEnable = false;
	}
}

bool InnerBox_isEnable(INNERBOX *innerbox) {
	bool result = false;
	if (innerbox != NULL) {
		result = innerbox->isEnable;
	}
	return result;
}

bool InnerBox_isDisable(INNERBOX *innerbox) {
	bool result = true;
	if (innerbox != NULL) {
		result = !innerbox->isEnable;
	}
	return result;
}

void InnerBox_update(INNERBOX *innerbox) {
	if (innerbox != NULL && innerbox->isEnable == true) {
		/* Mise à jour des widgets */
		DATA *data = innerbox->widgets->first;
		while (data != NULL) {
			switch(data->type) {
				case SPRITE_WIDGET_TYPE:
					Sprite_update((SPRITE *) data->buffer);
				default:
					break;
			}
			data = data->next;
		}
	}
}

void InnerBox_event(INNERBOX *innerBox, SDL_Rect *innerRect) {
	if (innerBox != NULL && innerBox->isEnable) {
		SDL_Rect selfInnerRect;
		selfInnerRect.x = innerBox->rect.x + innerRect->x;
		selfInnerRect.y = innerBox->rect.y + innerRect->y;
		selfInnerRect.w = innerBox->rect.w;
		selfInnerRect.h = innerBox->rect.h;

		/* Evènement sur les widgets */
		DATA *data = innerBox->widgets->first;
		while (data != NULL) {
			switch(data->type) {
				case SPRITE_WIDGET_TYPE:
					Sprite_event((SPRITE *) data->buffer, &selfInnerRect);
					break;
				case LABEL_WIDGET_TYPE:
					Label_event((LABEL *) data->buffer, &selfInnerRect);
					break;
				case BUTTON_WIDGET_TYPE:
					Button_event((BUTTON *) data->buffer, &selfInnerRect);
					break;
				case TOGGLE_SWITCH_WIDGET_TYPE:
					ToggleSwitch_event((TOGGLE_SWITCH *) data->buffer, &selfInnerRect);
					break;
				default:
					break;
			}
			data = data->next;
		}
	}
}

void InnerBox_draw(INNERBOX *innerBox, SDL_Rect *innerRect) {
	if (innerBox != NULL && innerBox->isEnable) {
		if (innerBox->image != NULL) {
			SDL_Rect src, dest;
			Sint32 decalX = 0, decalY = 0;

			if (innerRect != NULL) {
				/* on adapte la position de l'innerBox dans l'interface par rapport à sa position dans le widget parents */
				decalX = innerRect->x + innerBox->rect.x;
				decalY = innerRect->y + innerBox->rect.y;
			}

			src.w = innerBox->tileRect.w;
			src.h = innerBox->tileRect.h;

			if (innerBox->rect.w > innerBox->tileRect.w*2) {
				/* côté haut */
				src.x = innerBox->tileRect.w;
				src.y = 0;
				dest.x = decalX + innerBox->tileRect.w;
				dest.y = decalY;
				dest.w = innerBox->rect.w - innerBox->tileRect.w*2;
				dest.h = innerBox->tileRect.h;
				SG_render(innerBox->image->texture, &src, &dest);
				
				/* côté bas */
				src.x = innerBox->tileRect.w;
				src.y = innerBox->tileRect.h*2;
				dest.x = decalX + innerBox->tileRect.w;
				dest.y = decalY + innerBox->rect.h - innerBox->tileRect.h;
				dest.w = innerBox->rect.w - innerBox->tileRect.w*2;
				dest.h = innerBox->tileRect.h;
				SG_render(innerBox->image->texture, &src, &dest);
			}
			
			if (innerBox->rect.h > innerBox->tileRect.h*2) {
				/* côté gauche */
				src.x = 0;
				src.y = innerBox->tileRect.h;
				dest.x = decalX;
				dest.y = decalY + innerBox->tileRect.h;
				dest.w = innerBox->tileRect.w;
				dest.h = innerBox->rect.h - innerBox->tileRect.h*2;
				SG_render(innerBox->image->texture, &src, &dest);
				
				/* côté droite */
				src.x = innerBox->tileRect.w * 2;
				src.y = innerBox->tileRect.h;
				dest.x = decalX + innerBox->rect.w - innerBox->tileRect.w;
				dest.y = decalY + innerBox->tileRect.h;
				dest.w = innerBox->tileRect.w;
				dest.h = innerBox->rect.h - innerBox->tileRect.h*2;
				SG_render(innerBox->image->texture, &src, &dest);
			}
			
			if ((innerBox->rect.w > innerBox->tileRect.w*2) && (innerBox->rect.h > innerBox->tileRect.h*2)) {
				/* centre */
				src.x = innerBox->tileRect.w;
				src.y = innerBox->tileRect.h;
				dest.x = decalX + innerBox->tileRect.w;
				dest.y = decalY + innerBox->tileRect.h;
				dest.w = innerBox->rect.w - innerBox->tileRect.w*2;
				dest.h = innerBox->rect.h - innerBox->tileRect.h*2;
				SG_render(innerBox->image->texture, &src, &dest);
			}

			/* coin haut gauche */
			src.x = 0;
			src.y = 0;
			innerBox->tileRect.x = innerBox->rect.x + innerRect->x;
			innerBox->tileRect.y = innerBox->rect.y + innerRect->y;
			SG_render(innerBox->image->texture, &src, &innerBox->tileRect);

			/* coin haut droit */
			src.x = innerBox->tileRect.w*2;
			src.y = 0;
			innerBox->tileRect.x = innerBox->rect.x + innerBox->rect.w - innerBox->tileRect.w + innerRect->x;
			innerBox->tileRect.y = innerBox->rect.y + innerRect->y;
			SG_render(innerBox->image->texture, &src, &innerBox->tileRect);
			
			/* coin bas gauche */
			src.x = 0;
			src.y = innerBox->tileRect.h*2;
			innerBox->tileRect.x = innerBox->rect.x + innerRect->x;
			innerBox->tileRect.y = innerBox->rect.y + innerBox->rect.h - innerBox->tileRect.h + innerRect->y;
			SG_render(innerBox->image->texture, &src, &innerBox->tileRect);
			
			/* coin bas droit */
			src.x = innerBox->tileRect.w*2;
			src.y = innerBox->tileRect.h*2;
			innerBox->tileRect.x = innerBox->rect.x + innerBox->rect.w - innerBox->tileRect.w + innerRect->x;
			innerBox->tileRect.y = innerBox->rect.y + innerBox->rect.h - innerBox->tileRect.h + innerRect->y;
			SG_render(innerBox->image->texture, &src, &innerBox->tileRect);
		}
	
		/* on affiche tous les widgets que l'innerBox comporte */
		DATA *data = innerBox->widgets->first;
		while (data != NULL) {
			switch(data->type) {
				case SPRITE_WIDGET_TYPE:
					Sprite_draw((SPRITE *) data->buffer, &innerBox->rect);
					break;
				case LABEL_WIDGET_TYPE:
					Label_draw((LABEL *) data->buffer, &innerBox->rect);
					break;
				case BUTTON_WIDGET_TYPE:
					Button_draw((BUTTON *) data->buffer, &innerBox->rect);
					break;
				case TOGGLE_SWITCH_WIDGET_TYPE:
					ToggleSwitch_draw((TOGGLE_SWITCH *) data->buffer, &innerBox->rect);
					break;
				default:
					break;
			}
			data = data->next;
		}
	}
}


/* 
 * ------------------------------------------------
 *  Sprite
 * ------------------------------------------------
*/

SPRITE* Sprite_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h,
	char *imageName, Uint16 nbTilesW, Uint16 nbTilesH, Orientation orientation, Uint16 frameBetweenEachTile, Uint16 frameBetweenAnimation) {
	SPRITE *sprite = NULL;
	IMAGE *image = SG_loadImage(imageName);
	if (image != NULL) {
		sprite = (SPRITE *) malloc(sizeof(SPRITE));
		if (sprite != NULL) {
			sprite->selfType = SPRITE_WIDGET_TYPE;
			sprite->isEnable = false;

			sprite->rect.x = x;
			sprite->rect.y = y;
			sprite->rect.w = w;
			sprite->rect.h = h;
			sprite->nbTilesW = nbTilesW;
			sprite->nbTilesH = nbTilesH;

			sprite->image = image;

			sprite->tileRect.w = image->w / nbTilesW;
			sprite->tileRect.h = image->h / nbTilesH;

			sprite->orientation = orientation;
			
			sprite->frameBetweenEachTile = frameBetweenEachTile;
			sprite->frameBetweenAnimation = frameBetweenAnimation;
			sprite->frame = 0;
			sprite->currentTile = 0;
			sprite->endAnimation = false;

			sprite->isFocused = false;
			sprite->isHeld = false;
			sprite->isPressed = false;
			sprite->isReleased = false;

			if (root != NULL && widegetName != NULL)
				Widget_addToRoot(root, widegetName, sprite);
		}
	}
	return sprite;
}

void Sprite_free(SPRITE *sprite) {
	if (sprite != NULL) {
		free(sprite);
	}
}


void Sprite_update(SPRITE *sprite) {
	if (sprite != NULL && sprite->isEnable && sprite->frameBetweenEachTile > 0) {
		sprite->frame++;
		if (sprite->endAnimation == false) {
			if (sprite->frame >= sprite->frameBetweenEachTile) {
				if (sprite->orientation == HORIZONTAL)
					sprite->currentTile = (sprite->currentTile+1) % sprite->nbTilesW;
				else if (sprite->orientation == VERTICAL)
					sprite->currentTile = (sprite->currentTile+1) % sprite->nbTilesH;
				if (sprite->currentTile == 0 && sprite->frameBetweenAnimation > 0) sprite->endAnimation = true;
				sprite->frame = 0;
			}
		} else {
			if (sprite->frame >= sprite->frameBetweenAnimation) {
				sprite->endAnimation = false;
				sprite->frame = 0;
			}
		}
	}
}

void Sprite_event(SPRITE *sprite, SDL_Rect *innerRect) {
	if (sprite != NULL && sprite->isEnable) {
		SDL_Point pos;
		SDL_Rect rect;

		Input_getMousePosition(&pos);
		rect.x = sprite->rect.x + innerRect->x;
		rect.y = sprite->rect.y + innerRect->y;
		rect.w = sprite->rect.w;
		rect.h = sprite->rect.h;
		sprite->isPressed = false;
		sprite->isReleased = false;
		if (SDL_PointInRect(&pos, &rect)) {
			sprite->isFocused = true;
			if (Input_wasMousePressed(SDL_BUTTON_LEFT)) {
				sprite->isHeld = true;
				sprite->isPressed = true;
			} else if (Input_wasMouseReleased(SDL_BUTTON_LEFT) && sprite->isHeld) {
				sprite->isHeld = false;
				sprite->isReleased = true;
			}
		} else {
			sprite->isFocused = false;
			sprite->isHeld = false;
		}
	}
}

void Sprite_next(SPRITE *sprite) {
	if (sprite != NULL ) {
		if (sprite->orientation == HORIZONTAL)
			sprite->currentTile = (sprite->currentTile+1) % sprite->nbTilesW;
		else if (sprite->orientation == VERTICAL)
			sprite->currentTile = (sprite->currentTile+1) % sprite->nbTilesH;
	}
}

void Sprite_after(SPRITE *sprite) {
	if (sprite != NULL ) {
		if (sprite->currentTile > 0) sprite->currentTile--;
		else {
			if (sprite->orientation == HORIZONTAL)
				sprite->currentTile = sprite->nbTilesW-1;
			else if (sprite->orientation == VERTICAL)
				sprite->currentTile = sprite->nbTilesH-1;
		}
	}
}

void Sprite_draw(SPRITE *sprite, SDL_Rect *innerRect) {
	if (sprite != NULL && sprite->isEnable == true) {
		SDL_Rect dest;

		if (sprite->orientation == HORIZONTAL) {
			sprite->tileRect.x = sprite->currentTile * sprite->tileRect.w;
			sprite->tileRect.y = 0;
		} else {
			sprite->tileRect.x = 0;
			sprite->tileRect.y = sprite->currentTile * sprite->tileRect.h;
		}

		if (innerRect != NULL) {
			/* on adapte la position du bouton dans l'interface par rapport à sa position dans le widget parents */
			dest.x = innerRect->x + sprite->rect.x;
			dest.y = innerRect->y + sprite->rect.y;
		} else {
			dest.x = dest.y = 0;
		}
		dest.w = sprite->rect.w;
		dest.h = sprite->rect.h;

		SG_render(sprite->image->texture, &sprite->tileRect, &dest);
	}
}


/* 
 * ------------------------------------------------
 *  Label
 * ------------------------------------------------
*/

LABEL *Label_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h,
	char *message, char *fontName, Uint16 sizeFont, Uint32 colorFont) {
	LABEL *label = NULL;
	if (Widget_existInRoot(root, widegetName) == false) {
		TTF_Font *font = NULL;
		font = SG_loadFont(fontName, sizeFont);
		if (font != NULL) {
			label = (LABEL *) malloc(sizeof(LABEL));
			label->selfType = LABEL_WIDGET_TYPE;
			label->isEnable = false;

			label->Hcentered = false;
			label->Vcentered = true;

			label->rect.x = x;
			label->rect.y = y;

			SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, message, (SDL_Color) {0, 0, 0, 255});

			label->rect.w = (w == 0) ? surfaceMessage->w : w;
			label->rect.h = (h == 0) ? surfaceMessage->h : h;

			SDL_FreeSurface(surfaceMessage);

			label->message = strdup(message);
			label->font = font;
			label->colorFont = colorFont;

			if (root != NULL && widegetName != NULL)
				Widget_addToRoot(root, widegetName, label);
		}
	}
	return label;
}

void Label_free(LABEL *label) {
	if (label != NULL) {
		free(label->message);
		free(label);
	}
}

void Label_setText(LABEL *label, char *text, ...) {
	if (label != NULL) {
		static char buffer[1000];
	
		/* on formate la chaine à afficher avec les arguments */
		va_list args;
		va_start(args, text);
		vsprintf(buffer, text, args);
		va_end(args);

		free(label->message);
		label->message = strdup(buffer);
	}
}

void Label_changeColor(LABEL *label, Uint32 colorFont) {
	if (label != NULL) {
		label->colorFont = colorFont;
	}
}

void Label_event(LABEL *label, SDL_Rect *innerRect) {
	if (label != NULL && label->isEnable) {

		/* .... */
	}
}

void Label_draw(LABEL *label, SDL_Rect *innerRect) {
	if (label != NULL && label->isEnable) {
		SDL_Rect dest;

		if (innerRect != NULL) {
			dest.x = innerRect->x + label->rect.x;
			dest.y = innerRect->y + label->rect.y;
		} else {
			dest.x = dest.y = 0;
		}
		dest.w = label->rect.w;
		dest.h = label->rect.h;
		
		/* on affiche le texte */
		SG_renderText(false, label->message, label->font, label->colorFont, &dest, label->Hcentered, label->Vcentered);
	}
}

/* 
 * ------------------------------------------------
 *  Button
 * ------------------------------------------------
*/

/* taille de l'image : 9 tiles x 3 tiles */
BUTTON *Button_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h,
	char *imageName, char *message, char *fontName, Uint16 sizeFont, Uint32 colorFont, Uint32 focusedColorFont, Sint32 offsetPressedX, Sint32 offsetPressedY) {
	BUTTON *button = NULL;
	if (Widget_existInRoot(root, widegetName) == false) {
		IMAGE *image = SG_loadImage(imageName);
		TTF_Font *font = SG_loadFont(fontName, sizeFont);
		if (image != NULL && font != NULL) {
			button = (BUTTON *) malloc(sizeof(BUTTON));
			button->selfType = BUTTON_WIDGET_TYPE;
			button->isEnable = false;
				
			button->image = image;

			button->tileRect.w = image->w / 9;
			button->tileRect.h = image->h / 3;

			button->rect.x = x;
			button->rect.y = y;
			button->rect.w = (w >= button->tileRect.w*2) ? w : button->tileRect.w*2;
			button->rect.h = (h >= button->tileRect.h*2) ? h : button->tileRect.h*2;

			button->withMotif = false;

			button->message = strdup(message);
			button->font = font;
			button->colorFont = colorFont;
			button->focusedColorFont = focusedColorFont;

			button->offsetPressedX = offsetPressedX;
			button->offsetPressedY = offsetPressedY;

			button->isFocused = false;
			button->isHeld = false;
			button->isPressed = false;
			button->isReleased = false;

			if (root != NULL && widegetName != NULL)
				Widget_addToRoot(root, widegetName, button);
		}
	}
	return button;
}

/* taille de l'image : 9 tiles x 3 tiles */
/* taille du motif : 2 tiles x 1 tile */
BUTTON *MotifButton_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h,
	char *imageName, char *motifImageName, Sint32 offsetPressedX, Sint32 offsetPressedY) {
	BUTTON *button = NULL;
	if (Widget_existInRoot(root, widegetName) == false) {
		IMAGE *image = SG_loadImage(imageName);
		IMAGE *motif = SG_loadImage(motifImageName);
		if (image != NULL && motif != NULL) {
			button = (BUTTON *) malloc(sizeof(BUTTON));
			button->selfType = BUTTON_WIDGET_TYPE;
			button->isEnable = false;
				
			button->image = image;

			button->tileRect.w = image->w / 9;
			button->tileRect.h = image->h / 3;

			button->rect.x = x;
			button->rect.y = y;
			button->rect.w = (w >= button->tileRect.w*2) ? w : button->tileRect.w*2;
			button->rect.h = (h >= button->tileRect.h*2) ? h : button->tileRect.h*2;

			button->withMotif = true;
			button->motif = motif;

			button->motifTileRect.x = motif->w / 2;
			button->motifTileRect.y = motif->h;
			button->motifTileRect.w = motif->w / 2;
			button->motifTileRect.h = motif->h;

			button->offsetPressedX = offsetPressedX;
			button->offsetPressedY = offsetPressedY;

			button->isFocused = false;
			button->isHeld = false;
			button->isPressed = false;
			button->isReleased = false;
			
			if (root != NULL && widegetName != NULL)
				Widget_addToRoot(root, widegetName, button);
		}
	}
	return button;
}

void Button_free(BUTTON *button) {
	if (button != NULL) {
		free(button->message);
		free(button);
	}
}

void Button_event(BUTTON *button, SDL_Rect *innerRect) {
	if (button != NULL && button->isEnable) {
		SDL_Point pos;
		SDL_Rect rect;

		Input_getMousePosition(&pos);
		rect.x = button->rect.x + innerRect->x;
		rect.y = button->rect.y + innerRect->y;
		rect.w = button->rect.w;
		rect.h = button->rect.h;
		button->isPressed = false;
		button->isReleased = false;
		if (SDL_PointInRect(&pos, &rect)) {
			button->isFocused = true;
			if (Input_wasMousePressed(SDL_BUTTON_LEFT)) {
				button->isHeld = true;
				button->isPressed = true;
			} else if (Input_wasMouseReleased(SDL_BUTTON_LEFT) && button->isHeld) {
				button->isHeld = false;
				button->isReleased = true;
			}
		} else {
			button->isFocused = false;
			button->isHeld = false;
		}
	}
}

void Button_draw(BUTTON *button, SDL_Rect *innerRect) {
	if (button != NULL && button->isEnable) {
		SDL_Rect src, dest;
		Sint32 srcOffsetX, decalX = 0, decalY = 0;

		/* on change l'image en fonction de l'état du bouton */
		srcOffsetX = button->tileRect.w * 3 * (button->isFocused + button->isHeld);

		if (innerRect != NULL) {
			/* on adapte la position du bouton dans l'interface par rapport à sa position dans le widget parents */
			decalX = innerRect->x + button->rect.x;
			decalY = innerRect->y + button->rect.y;
		}

		src.w = button->tileRect.w;
		src.h = button->tileRect.h;

		if (button->rect.w > button->tileRect.w*2) {
			/* côté haut */
			src.x = srcOffsetX + button->tileRect.w;
			src.y = 0;
			dest.x = decalX + button->tileRect.w;
			dest.y = decalY;
			dest.w = button->rect.w - button->tileRect.w*2;
			dest.h = button->tileRect.h;
			SG_render(button->image->texture, &src, &dest);
			
			/* côté bas */
			src.x = srcOffsetX + button->tileRect.w;
			src.y = button->tileRect.h*2;
			dest.x = decalX + button->tileRect.w;
			dest.y = decalY + button->rect.h - button->tileRect.h;
			dest.w = button->rect.w - button->tileRect.w*2;
			dest.h = button->tileRect.h;
			SG_render(button->image->texture, &src, &dest);
		}
		
		if (button->rect.h > button->tileRect.h*2) {
			/* côté gauche */
			src.x = srcOffsetX;
			src.y = button->tileRect.h;
			dest.x = decalX;
			dest.y = decalY + button->tileRect.h;
			dest.w = button->tileRect.w;
			dest.h = button->rect.h - button->tileRect.h*2;
			SG_render(button->image->texture, &src, &dest);
			
			/* côté droite */
			src.x = srcOffsetX + button->tileRect.w * 2;
			src.y = button->tileRect.h;
			dest.x = decalX + button->rect.w - button->tileRect.w;
			dest.y = decalY + button->tileRect.h;
			dest.w = button->tileRect.w;
			dest.h = button->rect.h - button->tileRect.h*2;
			SG_render(button->image->texture, &src, &dest);
		}
		
		if ((button->rect.w > button->tileRect.w*2) && (button->rect.h > button->tileRect.h*2)) {
			/* centre */
			src.x = srcOffsetX + button->tileRect.w;
			src.y = button->tileRect.h;
			dest.x = decalX + button->tileRect.w;
			dest.y = decalY + button->tileRect.h;
			dest.w = button->rect.w - button->tileRect.w*2;
			dest.h = button->rect.h - button->tileRect.h*2;
			SG_render(button->image->texture, &src, &dest);
		}

		/* coin haut gauche */
		src.x = srcOffsetX;
		src.y = 0;
		button->tileRect.x = button->rect.x + innerRect->x;
		button->tileRect.y = button->rect.y + innerRect->y;
		SG_render(button->image->texture, &src, &button->tileRect);

		/* coin haut droit */
		src.x = srcOffsetX + button->tileRect.w*2;
		src.y = 0;
		button->tileRect.x = button->rect.x + button->rect.w - button->tileRect.w + innerRect->x;
		button->tileRect.y = button->rect.y + innerRect->y;
		SG_render(button->image->texture, &src, &button->tileRect);
		
		/* coin bas gauche */
		src.x = srcOffsetX;
		src.y = button->tileRect.h*2;
		button->tileRect.x = button->rect.x + innerRect->x;
		button->tileRect.y = button->rect.y + button->rect.h - button->tileRect.h + innerRect->y;
		SG_render(button->image->texture, &src, &button->tileRect);
		
		/* coin bas droit */
		src.x = srcOffsetX + button->tileRect.w*2;
		src.y = button->tileRect.h*2;
		button->tileRect.x = button->rect.x + button->rect.w - button->tileRect.w + innerRect->x;
		button->tileRect.y = button->rect.y + button->rect.h - button->tileRect.h + innerRect->y;
		SG_render(button->image->texture, &src, &button->tileRect);

		
		dest.x = decalX + button->offsetPressedX * button->isHeld;
		dest.y = decalY + button->offsetPressedY * button->isHeld;
		dest.w = button->rect.w - button->offsetPressedX;
		dest.h = button->rect.h - button->offsetPressedY;

		if (button->withMotif == true) {
			/* on affiche le motif */
			button->motifTileRect.x = button->motifTileRect.w * button->isFocused;
			button->motifTileRect.y = 0;
			SG_render(button->motif->texture, &button->motifTileRect, &dest);
		} else {
			/* on affiche le texte */
			Uint32 color = (button->isFocused) ? button->focusedColorFont : button->colorFont;
			SG_renderText(false, button->message, button->font, color, &dest, true, true);
		}
	}
}

/* 
 * ------------------------------------------------
 *  Toggle Switch
 * ------------------------------------------------
*/

/* taille de l'image : 10 tiles x 6 tiles */
TOGGLE_SWITCH *ToggleSwitch_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h,
	char *imageName, char *messageON, char *messageOFF, char *fontName, Uint16 sizeFont, Uint32 colorFont, Uint32 focusedColorFont,
	Sint32 offsetPressedX, Sint32 offsetPressedY, bool state) {
	TOGGLE_SWITCH *toggleSwitch = NULL;
	if (Widget_existInRoot(root, widegetName) == false) {
		IMAGE *image = SG_loadImage(imageName);
		TTF_Font *font = SG_loadFont(fontName, sizeFont);
		if (image != NULL && font != NULL) {
			toggleSwitch = (TOGGLE_SWITCH *) malloc(sizeof(TOGGLE_SWITCH));
			if (toggleSwitch != NULL) {
				toggleSwitch->selfType = TOGGLE_SWITCH_WIDGET_TYPE;
				toggleSwitch->isEnable = false;

				toggleSwitch->image = image;

				toggleSwitch->tileRect.w = image->w / 10;
				toggleSwitch->tileRect.h = image->h / 6;

				toggleSwitch->rect.x = x;
				toggleSwitch->rect.y = y;
				toggleSwitch->rect.w = (w >= toggleSwitch->tileRect.w*2) ? w : toggleSwitch->tileRect.w*2;
				toggleSwitch->rect.h = (h >= toggleSwitch->tileRect.h*2) ? h : toggleSwitch->tileRect.h*2;

				toggleSwitch->messageON = strdup(messageON);
				toggleSwitch->messageOFF = strdup(messageOFF);
				toggleSwitch->font = font;
				toggleSwitch->colorFont = colorFont;
				toggleSwitch->focusedColorFont = focusedColorFont;

				toggleSwitch->offsetPressedX = offsetPressedX;
				toggleSwitch->offsetPressedY = offsetPressedY;

				toggleSwitch->isFocused = false;
				toggleSwitch->isHeld = false;
				toggleSwitch->isPressed = false;
				toggleSwitch->isReleased = false;
				toggleSwitch->state = state;

				if (root != NULL && widegetName != NULL)
					Widget_addToRoot(root, widegetName, toggleSwitch);
			}
		}
	}
	return toggleSwitch;
}

void ToggleSwitch_free(TOGGLE_SWITCH *toggleSwitch) {
	if (toggleSwitch != NULL) {
		free(toggleSwitch);
	}
}

void ToggleSwitch_event(TOGGLE_SWITCH *toggleSwitch, SDL_Rect *innerRect) {
	if (toggleSwitch != NULL && toggleSwitch->isEnable) {
		SDL_Point pos;
		SDL_Rect rect;

		Input_getMousePosition(&pos);
		rect.x = toggleSwitch->rect.x + innerRect->x;
		rect.y = toggleSwitch->rect.y + innerRect->y;
		rect.w = toggleSwitch->rect.w;
		rect.h = toggleSwitch->rect.h;
		toggleSwitch->isPressed = false;
		toggleSwitch->isReleased = false;
		if (SDL_PointInRect(&pos, &rect)) {
			toggleSwitch->isFocused = true;

			/*  si state = false selection de la surface gauche, sinon selection de la surface droite */
			rect.x = toggleSwitch->rect.x + innerRect->x + (toggleSwitch->rect.w/2) * toggleSwitch->state;
			rect.w = toggleSwitch->rect.w/2;
			if (SDL_PointInRect(&pos, &rect) && Input_wasMousePressed(SDL_BUTTON_LEFT)) {
				toggleSwitch->state ^= 1;
			}
			
			if (Input_wasMousePressed(SDL_BUTTON_LEFT)) {
				toggleSwitch->isHeld = true;
				toggleSwitch->isPressed = true;
			} else if (Input_wasMouseReleased(SDL_BUTTON_LEFT) && toggleSwitch->isHeld) {
				toggleSwitch->isHeld = false;
				toggleSwitch->isReleased = true;
			}
		} else {
			toggleSwitch->isFocused = false;
			toggleSwitch->isHeld = false;
		}
	}
}

void ToggleSwitch_draw(TOGGLE_SWITCH *toggleSwitch, SDL_Rect *innerRect) {
	if (toggleSwitch != NULL && toggleSwitch->isEnable) {
		SDL_Rect src, dest, dest2;
		Sint32 srcOffsetX, srcOffsetY, decalX = 0, decalY = 0;

		/* on change l'image en fonction de l'état du toggleSwitch */
		srcOffsetX = toggleSwitch->tileRect.w * 5 * toggleSwitch->isFocused;
		srcOffsetY = toggleSwitch->tileRect.h * 3 * toggleSwitch->state;

		if (innerRect != NULL) {
			/* on adapte la position du toggleSwitch dans l'interface par rapport à sa position dans le widget parents */
			decalX = innerRect->x + toggleSwitch->rect.x;
			decalY = innerRect->y + toggleSwitch->rect.y;
		}

		src.w = dest2.w = toggleSwitch->tileRect.w;
		src.h = dest2.h = toggleSwitch->tileRect.h;

		if (toggleSwitch->rect.w >= toggleSwitch->tileRect.w*3) {
			/* côté haut ...*/
			src.y = srcOffsetY;
			dest.y = dest2.y = decalY;
			dest.w = (toggleSwitch->rect.w/2) - toggleSwitch->tileRect.w;
			dest.h = toggleSwitch->tileRect.h;
			/* ... gauche */
			src.x = srcOffsetX + toggleSwitch->tileRect.w;
			dest.x = decalX + toggleSwitch->tileRect.w;
			SG_render(toggleSwitch->image->texture, &src, &dest);
			/* ... centrale */
			src.x = srcOffsetX + toggleSwitch->tileRect.w*2;
			dest2.x = decalX + toggleSwitch->rect.w/2 - 2*toggleSwitch->tileRect.w/3;
			SG_render(toggleSwitch->image->texture, &src, &dest2);
			/* ... droit */
			src.x = srcOffsetX + toggleSwitch->tileRect.w*3;
			dest.x = decalX + toggleSwitch->rect.w/2 + toggleSwitch->tileRect.w/3;
			SG_render(toggleSwitch->image->texture, &src, &dest);
		
			/* côté bas ...*/
			src.y = srcOffsetY + toggleSwitch->tileRect.h*2;
			dest.y = dest2.y = decalY + toggleSwitch->rect.h - toggleSwitch->tileRect.h;
			dest.w = (toggleSwitch->rect.w/2) - toggleSwitch->tileRect.w;
			dest.h = toggleSwitch->tileRect.h;
			/* ... gauche */
			src.x = srcOffsetX + toggleSwitch->tileRect.w;
			dest.x = decalX + toggleSwitch->tileRect.w;
			SG_render(toggleSwitch->image->texture, &src, &dest);
			/* ... centrale */
			src.x = srcOffsetX + toggleSwitch->tileRect.w * 2;
			dest2.x = decalX + toggleSwitch->rect.w/2 - 2*toggleSwitch->tileRect.w/3;
			SG_render(toggleSwitch->image->texture, &src, &dest2);
			/* ... droit */
			src.x = srcOffsetX + toggleSwitch->tileRect.w * 3;
			dest.x = decalX + toggleSwitch->rect.w/2 + toggleSwitch->tileRect.w/3;
			SG_render(toggleSwitch->image->texture, &src, &dest);
		}

		if (toggleSwitch->rect.h > toggleSwitch->tileRect.h*2) {
			/* côté gauche */
			src.x = srcOffsetX;
			src.y = srcOffsetY + toggleSwitch->tileRect.h;
			dest.x = decalX;
			dest.y = decalY + toggleSwitch->tileRect.h;
			dest.w = toggleSwitch->tileRect.w;
			dest.h = toggleSwitch->rect.h - toggleSwitch->tileRect.h*2;
			SG_render(toggleSwitch->image->texture, &src, &dest);
			
			/* côté droite */
			src.x = srcOffsetX + toggleSwitch->tileRect.w * 4;
			src.y = srcOffsetY + toggleSwitch->tileRect.h;
			dest.x = decalX + toggleSwitch->rect.w - toggleSwitch->tileRect.w;
			dest.y = decalY + toggleSwitch->tileRect.h;
			dest.w = toggleSwitch->tileRect.w;
			dest.h = toggleSwitch->rect.h - toggleSwitch->tileRect.h*2;
			SG_render(toggleSwitch->image->texture, &src, &dest);
		}
		
		if ((toggleSwitch->rect.w >= toggleSwitch->tileRect.w*3) && (toggleSwitch->rect.h >= toggleSwitch->tileRect.h*2)) {
			/* centre ...*/
			src.y = srcOffsetY + toggleSwitch->tileRect.h;
			dest.y = dest2.y = decalY + toggleSwitch->tileRect.h;
			dest.w = (toggleSwitch->rect.w/2) - toggleSwitch->tileRect.w;
			dest.h = dest2.h = toggleSwitch->rect.h - toggleSwitch->tileRect.h * 2;
			/* ... gauche */
			src.x = srcOffsetX + toggleSwitch->tileRect.w;
			dest.x = decalX + toggleSwitch->tileRect.w;
			SG_render(toggleSwitch->image->texture, &src, &dest);
			/* ... centre */
			src.x = srcOffsetX + toggleSwitch->tileRect.w * 2;
			dest2.x = decalX + toggleSwitch->rect.w/2 - 2*toggleSwitch->tileRect.w/3;
			SG_render(toggleSwitch->image->texture, &src, &dest2);
			/* ... droit */
			src.x = srcOffsetX + toggleSwitch->tileRect.w * 3;
			dest.x = decalX + toggleSwitch->rect.w/2 + toggleSwitch->tileRect.w/3;
			SG_render(toggleSwitch->image->texture, &src, &dest);
		}

		/* coin haut gauche */
		src.x = srcOffsetX;
		src.y = srcOffsetY;
		toggleSwitch->tileRect.x = toggleSwitch->rect.x + innerRect->x;
		toggleSwitch->tileRect.y = toggleSwitch->rect.y + innerRect->y;
		SG_render(toggleSwitch->image->texture, &src, &toggleSwitch->tileRect);

		/* coin haut droit */
		src.x = srcOffsetX + toggleSwitch->tileRect.w * 4;
		src.y = srcOffsetY;
		toggleSwitch->tileRect.x = toggleSwitch->rect.x + toggleSwitch->rect.w - toggleSwitch->tileRect.w + innerRect->x;
		toggleSwitch->tileRect.y = toggleSwitch->rect.y + innerRect->y;
		SG_render(toggleSwitch->image->texture, &src, &toggleSwitch->tileRect);
		
		/* coin bas gauche */
		src.x = srcOffsetX;
		src.y = srcOffsetY + toggleSwitch->tileRect.h * 2;
		toggleSwitch->tileRect.x = toggleSwitch->rect.x + innerRect->x;
		toggleSwitch->tileRect.y = toggleSwitch->rect.y + toggleSwitch->rect.h - toggleSwitch->tileRect.h + innerRect->y;
		SG_render(toggleSwitch->image->texture, &src, &toggleSwitch->tileRect);
		
		/* coin bas droit */
		src.x = srcOffsetX + toggleSwitch->tileRect.w * 4;
		src.y = srcOffsetY + toggleSwitch->tileRect.h * 2;
		toggleSwitch->tileRect.x = toggleSwitch->rect.x + toggleSwitch->rect.w - toggleSwitch->tileRect.w + innerRect->x;
		toggleSwitch->tileRect.y = toggleSwitch->rect.y + toggleSwitch->rect.h - toggleSwitch->tileRect.h + innerRect->y;
		SG_render(toggleSwitch->image->texture, &src, &toggleSwitch->tileRect);
		
		/* on affiche le texte de guauche */
		Uint32 color = (toggleSwitch->isFocused) ? toggleSwitch->focusedColorFont : toggleSwitch->colorFont;
		dest.x = decalX + toggleSwitch->offsetPressedX * toggleSwitch->state;
		dest.y = decalY + toggleSwitch->offsetPressedY * toggleSwitch->state;
		dest.w = toggleSwitch->rect.w/2 - toggleSwitch->offsetPressedX;
		dest.h = toggleSwitch->rect.h - toggleSwitch->offsetPressedY;
		SG_renderText(false, toggleSwitch->messageOFF, toggleSwitch->font, color, &dest, true, true);

		/* on affiche le texte de droite */
		dest.x = decalX + toggleSwitch->rect.w/2 + toggleSwitch->offsetPressedX * (!toggleSwitch->state);
		dest.y = decalY + toggleSwitch->offsetPressedY * (!toggleSwitch->state);
		SG_renderText(false, toggleSwitch->messageON, toggleSwitch->font, color, &dest, true, true);
	}
}

/* 
 * ------------------------------------------------
 *  Scrollbar
 * ------------------------------------------------
*/

SCROLLBAR *Horizontal_ScrollBar_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, char *imageName,
	Uint32 beginValue, Uint32 endValue) {
	SCROLLBAR *scrollBar = NULL;
	if (Widget_existInRoot(root, widegetName) == false) {
		IMAGE *image = SG_loadImage(imageName);
		if (image != NULL) {
			scrollBar = (SCROLLBAR *) malloc(sizeof(SCROLLBAR));
			if (scrollBar != NULL) {
				scrollBar->selfType = SCROLLBAR_WIDGET_TYPE;
				scrollBar->isEnable = false;

				scrollBar->image = image;

				scrollBar->tileRect.w = image->w / 1;
				scrollBar->tileRect.h = image->h / 1;

				scrollBar->orientation = HORIZONTAL;



				if (root != NULL && widegetName != NULL)
					Widget_addToRoot(root, widegetName, scrollBar);
			}
		}
	}
	return scrollBar;
}

SCROLLBAR *Vertical_ScrollBar_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, char *imageName,
	Uint32 beginValue, Uint32 endValue) {
	SCROLLBAR *scrollBar = NULL;
	if (Widget_existInRoot(root, widegetName) == false) {
		IMAGE *image = SG_loadImage(imageName);
		if (image != NULL) {
			scrollBar = (SCROLLBAR *) malloc(sizeof(SCROLLBAR));
			if (scrollBar != NULL) {
				scrollBar->selfType = SCROLLBAR_WIDGET_TYPE;
				scrollBar->isEnable = false;

				scrollBar->image = image;

				scrollBar->tileRect.w = image->w / 1;
				scrollBar->tileRect.h = image->h / 1;

				scrollBar->orientation = VERTICAL;

				/* .... */

				if (root != NULL)
					Widget_addToRoot(root, widegetName, scrollBar);
			}
		}
	}
	return scrollBar;
}

void ScrollBar_free(SCROLLBAR *scrollBar) {
	if (scrollBar != NULL) {
		/* .... */
	}
}

void ScrollBar_event(SCROLLBAR *scrollBar, SDL_Rect *innerRect) {
	if (scrollBar != NULL && scrollBar->isEnable) {
		/* .... */
	}
}

void ScrollBar_draw(SCROLLBAR *scrollBar, SDL_Rect *innerRect) {
	if (scrollBar != NULL && scrollBar->isEnable) {
		Sint32 decalX = 0, decalY = 0;

		if (innerRect != NULL) {

		}
		/* .... */
	}
}

/* 
 * ------------------------------------------------
 *  Levalbar
 * ------------------------------------------------
*/

LEVELBAR *Horizontal_LevelBar_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, char *imageName /* ... */) {
	LEVELBAR *levelBar = NULL;
	if (Widget_existInRoot(root, widegetName) == false) {
		IMAGE *image = SG_loadImage(imageName);
		if (image != NULL) {
			levelBar = (LEVELBAR *) malloc(sizeof(LEVELBAR));
			if (levelBar != NULL) {
				levelBar->selfType = LEVELBAR_WIDGET_TYPE;
				levelBar->isEnable = false;

				levelBar->image = image;

				levelBar->tileRect.w = image->w / 1;
				levelBar->tileRect.h = image->h / 1;

				levelBar->orientation = HORIZONTAL;

				/* .... */

				if (root != NULL && widegetName != NULL)
					Widget_addToRoot(root, widegetName, levelBar);
			}
		}
	}
	return levelBar;
}

LEVELBAR *Vertical_LevelBar_new(void *root, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, char *imageName /* ... */) {
	LEVELBAR *levelBar = NULL;
	if (Widget_existInRoot(root, widegetName) == false) {
		IMAGE *image = SG_loadImage(imageName);
		if (image != NULL) {
			levelBar = (LEVELBAR *) malloc(sizeof(LEVELBAR));
			if (levelBar != NULL) {
				levelBar->selfType = LEVELBAR_WIDGET_TYPE;
				levelBar->isEnable = false;

				levelBar->image = image;

				levelBar->tileRect.w = image->w / 1;
				levelBar->tileRect.h = image->h / 1;

				levelBar->orientation = VERTICAL;

				/* .... */

				if (root != NULL && widegetName != NULL)
					Widget_addToRoot(root, widegetName, levelBar);
			}
		}
	}
	return levelBar;
}

void LevelBar_free(LEVELBAR *levelBar) {
	if (levelBar != NULL) {
		/* .... */
	}
}

void LevelBar_event(LEVELBAR *levelBar, SDL_Rect *innerRect) {
	if (levelBar != NULL && levelBar->isEnable) {
		/* .... */
	}
}

void LevelBar_draw(LEVELBAR *levelBar, SDL_Rect *innerRect) {
	if (levelBar != NULL && levelBar->isEnable) {
		Sint32 decalX = 0, decalY = 0;
		
		if (innerRect != NULL) {

		}
		/* .... */
	}
}

/* 
 * ------------------------------------------------
 *  ComboBox
 * ------------------------------------------------
*/

COMBOBOX *ComboBox_new(PANEL *panel, char *widegetName, Sint32 x, Sint32 y, Sint32 w, Sint32 h, Sint32 scrollBar_w,
	char *innerBoxImageName, char *scrollBarImageName,
	char *items[], Uint16 nbItems, char *fontName, Uint16 sizeFont, Uint32 colorFont, Uint32 highlightColorFont   /* ... */) {
	COMBOBOX *comboBox = NULL;
	if (Widget_existInRoot(panel, widegetName) == false) {
		Sint32 contentH = nbItems * (sizeFont + 2);
		INNERBOX *innerBox = InnerBox_new(NULL, NULL, x, y, w, h, 0, 0, w, contentH, innerBoxImageName);
		SCROLLBAR *scrollBar = Vertical_ScrollBar_new(innerBox, "ScrollBar", w - scrollBar_w, y, scrollBar_w, h, scrollBarImageName, 0, contentH);
		if (innerBox != NULL) {
			comboBox = (COMBOBOX *) malloc(sizeof(COMBOBOX));
			if (comboBox != NULL) {
				comboBox->selfType = COMBOBOX_WIDGET_TYPE;
				comboBox->isEnable = false;

				comboBox->innerBox = innerBox;
				comboBox->scrollBar = scrollBar;

				comboBox->rect.x = x;
				comboBox->rect.y = y;
				comboBox->rect.w = w;
				comboBox->rect.h = w - scrollBar_w;

				/* .... */

				if (panel != NULL && widegetName != NULL)
					Widget_addToRoot(panel, widegetName, comboBox);
			}
		}
	}
	return comboBox;
}

void ComboBox_free(COMBOBOX *comboBox) {
	/* .... */
}

void ComboBox_addItems(COMBOBOX *comboBox, char *items[], Uint16 nbItems) {
	/* .... */
}

void ComboBox_event(COMBOBOX *comboBox, SDL_Rect *innerRect) {
	/* .... */
}

void ComboBox_draw(COMBOBOX *comboBox, SDL_Rect *innerRect) {
	if (comboBox != NULL) {
		Sint32 decalX = 0, decalY = 0;
		
		if (innerRect != NULL) {

		}
		/* .... */
	}
}
