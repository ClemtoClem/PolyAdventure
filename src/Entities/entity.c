#include "entity.h"

ENTITY *Entity_new(float x, float y, SDL_Rect bodyBoxes[4], SDL_Rect stepBoxes[4], float zoom,
	char *imageName, Sint32 tileSize, Sint32 dispTileSize, Uint16 frameBetweenEachTile, bool firstTileIsStanding, float speed, bool attack) {
	ENTITY *entity = NULL;
	IMAGE *image = SG_loadImage(imageName);
	if (image != NULL) {
		entity = (ENTITY *)calloc(1, sizeof(ENTITY));
		if (entity != NULL) {
			entity->image = image;

			entity->nbTilesWidth  = image->w / tileSize;
			entity->nbTilesHeight = image->h / tileSize;

			/* coordonnées du centre de l'entité dans la map */
			entity->x = x;
			entity->y = y;

			entity->srcRect.w = tileSize;
			entity->srcRect.h = tileSize;
			entity->destRect.w = dispTileSize*zoom;
			entity->destRect.h = dispTileSize*zoom;
			entity->dispTileSize = dispTileSize;
			entity->zoom = zoom;

			for (int i = 0; i < 4; i++) {
				entity->bodyBoxes[i].x = (float) bodyBoxes[i].x/dispTileSize;
				entity->bodyBoxes[i].y = (float) bodyBoxes[i].y/dispTileSize;
				entity->bodyBoxes[i].w = (float) bodyBoxes[i].w/dispTileSize;
				entity->bodyBoxes[i].h = (float) bodyBoxes[i].h/dispTileSize;
				
				entity->stepBoxes[i].x = (float) stepBoxes[i].x/dispTileSize;
				entity->stepBoxes[i].y = (float) stepBoxes[i].y/dispTileSize;
				entity->stepBoxes[i].w = (float) stepBoxes[i].w/dispTileSize;
				entity->stepBoxes[i].h = (float) stepBoxes[i].h/dispTileSize;
			}
			//entity->frameBetweenEachTile = frameBetweenEachTile;

			entity->speed = speed;
			entity->attack = attack;
			entity->firstTileIsStanding = firstTileIsStanding;

			entity->privateStruct = NULL;
			entity->type = DEFAULT_ENTITY;
			
			entity->timer = (TIMER *) Timer_new(true);
			Timer_addCounter(entity->timer, FRAME_TILE_COUNTER, frameBetweenEachTile, true, true);

			entity->collisionFromOtherEntity = false;
			entity->collisionStepFromOtherEntity = false;

			/* default functions */
			entity->functions = (ENTITY_FUNCTIONS *) calloc(1, sizeof(ENTITY_FUNCTIONS));
			entity->functions->callback = false;
		}
	}
	return entity;
}

void Entity_free(ENTITY *entity) {
	if (entity != NULL) {
		if (entity->functions->free != NULL) entity->functions->free(entity);
		Timer_free(entity->timer);
		free(entity);
	}
}

/* --- Dimension de l'entité --- */

void Entity_setDispTileSize(ENTITY *entity, Sint32 dispTileSize) {
	if (entity != NULL) {
		if (entity->functions->setDispTileSize != NULL && entity->functions->callback == true) {
			entity->functions->setDispTileSize(entity, dispTileSize);
		} else {
			entity->destRect.w = dispTileSize*entity->zoom;
			entity->destRect.h = dispTileSize*entity->zoom;
			entity->dispTileSize = dispTileSize;
		}
	}
}

void Entity_getBodyBox(ENTITY *entity, SDL_FRect *getRect) {
	if (entity != NULL && getRect != NULL) {
		if (entity->functions->getBodyBox != NULL && entity->functions->callback == true) {
			entity->functions->getBodyBox(entity, getRect);
		} else {
			/*
			 *    |       |
			 *  --+-------+--
			 *    | +---+ | Calcul la zone de collision du coprs de l'entité
			 *    | | o | | en fonction de sa position et de son déplacement
			 *    | |/|\| |
			 *    | |/ \| |
			 *    | +---+ |
			 *  --+-------+--
			 *    |       |
			*/
			int i = (entity->directionFacing == ERROR_DIRECTION)? NORTH: entity->directionFacing;
			getRect->x = entity->x + entity->dx + ((entity->bodyBoxes[i].x - 0.5)*entity->zoom);
			getRect->y = entity->y + entity->dy + ((entity->bodyBoxes[i].y - 0.5)*entity->zoom);
			getRect->w = entity->bodyBoxes[i].w*entity->zoom;
			getRect->h = entity->bodyBoxes[i].h*entity->zoom;
		}
	}
}

void Entity_getStepBox(ENTITY *entity, SDL_FRect *getRect) {
	if (entity != NULL && getRect != NULL) {
		if (entity->functions->getStepBox != NULL && entity->functions->callback == true) {
			entity->functions->getStepBox(entity, getRect);
		} else {
			/*
			 *    |       |
			 *  --+-------+--
			 *    |   o   | Calcul la zone de collision des pieds de l'entité
			 *    |  /|\  | en fonction de sa position et de son déplacement
			 *    | +---+ |
			 *    | |/ \| |
			 *    | +---+ |
			 *  --+-------+--
			 *    |       |
			*/
			int i = (entity->directionFacing == ERROR_DIRECTION)? NORTH: entity->directionFacing;
			getRect->x = entity->x + entity->dx + ((entity->stepBoxes[i].x - 0.5)*entity->zoom);
			getRect->y = entity->y + entity->dy + ((entity->stepBoxes[i].y - 0.5)*entity->zoom);
			getRect->w = entity->stepBoxes[i].w*entity->zoom;
			getRect->h = entity->stepBoxes[i].h*entity->zoom;
		}
	}
}

bool Entity_stepCollision(ENTITY *entity, MAP *map) {
	bool collision = false;

	if (entity != NULL) {
		if (entity->functions->stepCollision != NULL && entity->functions->callback == true) {
			collision = entity->functions->stepCollision(entity, map);
		} else {
			/* collision avec un tile */
			float i1, j1, i2, j2;
			Uint16 i, j, id;
			SDL_FRect stepBox;

			Entity_getStepBox(entity, &stepBox);

			i1 = SDL_floorf(stepBox.x);
			j1 = SDL_floorf(stepBox.y);
			i2 = SDL_floorf(stepBox.x+stepBox.w);
			j2 = SDL_floorf(stepBox.y+stepBox.h);

			for (i = i1; i <= i2; i++) {
				for (j = j1; j <= j2; j++) {
					id = map->collision_buffer[j][i];
					if (id != 0) {
						collision = true;
						break;
					}
				}
			}
		}
	}
	return collision;
}

bool Entity_bodyCollision(ENTITY *entity, MAP *map) {
	bool collision = false;

	if (entity != NULL) {
		if (entity->functions->bodyCollision != NULL && entity->functions->callback == true) {
			collision = entity->functions->bodyCollision(entity, map);
		} else {
			/* collision avec un tile */
			Uint16 i, j, i1, j1, i2, j2, id;
			SDL_FRect bodyBox;

			Entity_getBodyBox(entity, &bodyBox);

			i1 = (Uint16) SDL_floorf(bodyBox.x);
			j1 = (Uint16) SDL_floorf(bodyBox.y);
			i2 = (Uint16) SDL_floorf(bodyBox.x+bodyBox.w);
			j2 = (Uint16) SDL_floorf(bodyBox.y+bodyBox.h);

			for (i = i1; i <= i2; i++) {
				for (j = j1; j <= j2; j++) {
					id = map->collision_buffer[j][i];
					if (id != 0) collision = true;
				}
			}
		}
	}
	return collision;
}


bool Entity_conflict(ENTITY *entity, ENTITY *other_entity) {
	bool inFrontOf = false;
	if (entity != NULL && other_entity != NULL) {
		if (entity->functions->conflict != NULL && entity->functions->callback == true) {
			entity->functions->conflict(entity, other_entity);
		} else {
			SDL_FRect body_rect, body_other_rect;
			SDL_FRect step_rect, step_other_rect;

			Entity_getBodyBox(entity, &body_rect);
			Entity_getBodyBox(other_entity, &body_other_rect);
			if (FloatCollisionRect(&body_rect, &body_other_rect)) {
				entity->collisionFromOtherEntity = true;
				other_entity->collisionFromOtherEntity = true;

				Entity_getStepBox(entity, &step_rect);
				Entity_getStepBox(other_entity, &step_other_rect);
				if (FloatCollisionRect(&step_rect, &step_other_rect)) {
					entity->collisionStepFromOtherEntity = true;
					other_entity->collisionStepFromOtherEntity = true;
				}

				if (step_rect.y > step_other_rect.y) {
					inFrontOf = true;
				}
			}
		}
	}
	return inFrontOf;
}


/* --- Direction et position --- */

void Entity_setPosition(ENTITY *entity, float x, float y) {
	if (entity != NULL) {
		if (entity->functions->setPosition != NULL && entity->functions->callback == true) {
			entity->functions->setPosition(entity, x, y);
		} else {
			entity->x = x;
			entity->y = y;
			entity->motionType = STANDING;
		}
	}
}

void Entity_getPosition(ENTITY *entity, float *x, float *y) {
	if (entity != NULL) {
		if (entity->functions->getPosition != NULL && entity->functions->callback == true) {
			entity->functions->getPosition(entity, x, y);
		} else {
			*x = entity->x;
			*y = entity->y;
		}
	}
}

void Entity_getMapPosition(ENTITY *entity, Uint16 *x, Uint16 *y) {
	if (entity != NULL) {
		if (entity->functions->getMapPosition != NULL && entity->functions->callback == true) {
			entity->functions->getMapPosition(entity, x, y);
		} else {
			Sint32 sx = SDL_floorf(entity->x);
			*x = (Uint16) ((sx < 0) ? 0 : sx); 
			Sint32 sy = SDL_floorf(entity->y);
			*y = (Uint16) ((sy < 0) ? 0 : sy);
		}
	}
}

void Entity_getLookMapPosition(ENTITY *entity, Uint16 *x, Uint16 *y) {
	if (entity != NULL) {
		if (entity->functions->getLookMapPosition != NULL && entity->functions->callback == true) {
			entity->functions->getLookMapPosition(entity, x, y);
		} else {
			Sint32 sx = SDL_floorf(entity->x);
			Sint32 sy = SDL_floorf(entity->y);
			*x = (Uint16) ((sx < 0) ? 0 : sx); 
			*y = (Uint16) ((sy < 0) ? 0 : sy);
			switch(entity->directionFacing) {
				case EAST:  (*x)++; break;
				case WEST:  (*x)--; break;
				case SOUTH: (*y)++; break;
				case NORTH: break;
				default: break;
			}
		}
	}
}

void Entity_setDirection(ENTITY *entity, DirectionFacing direction) {
	if (entity != NULL) {
		if (entity->functions->setDirection != NULL && entity->functions->callback == true) {
			entity->functions->setDirection(entity, direction);
		} else {
			entity->directionFacing = direction;
			entity->motionType = STANDING;
		}
	}
}

DirectionFacing Entity_getDirection(ENTITY *entity) {
	DirectionFacing dir = ERROR_DIRECTION;
	if (entity != NULL) {
		if (entity->functions->getDirection != NULL && entity->functions->callback == true) {
			dir = entity->functions->getDirection(entity);
		} else {
			dir = entity->directionFacing;
		}
	}
	return dir;
}

DirectionFacing Entity_turnRight(ENTITY *entity) {
	DirectionFacing dir = ERROR_DIRECTION;
	if (entity != NULL) {
		switch (entity->directionFacing) {
			case NORTH: dir = EAST;  break;
			case EAST:  dir = SOUTH; break;
			case SOUTH: dir = WEST;  break;
			case WEST:  dir = NORTH; break;
			default: break;
		}
		entity->directionFacing = dir;
	}
	return dir;
}

DirectionFacing Entity_turnLeft(ENTITY *entity) {
	DirectionFacing dir = ERROR_DIRECTION;
	if (entity != NULL) {
		switch (entity->directionFacing) {
			case NORTH: dir = WEST;  break;
			case WEST:  dir = SOUTH; break;
			case SOUTH: dir = EAST;  break;
			case EAST:  dir = NORTH; break;
			default: break;
		}
		entity->directionFacing = dir;
	}
	return dir;
}

Uint16 Entity_getCollisionFacing(ENTITY *entity) {
	if (entity != NULL) {
		return entity->idCollisionFacing;
	}
	return 0;
}

void Entity_standing(ENTITY *entity) {
	if (entity != NULL) {
		if (entity->functions->standing != NULL && entity->functions->callback == true) {
			entity->functions->standing(entity);
		} else {
			entity->dx = entity->dy = 0;
			entity->motionType = STANDING;
			entity->currentAnimateTile = 0;
		}
	}
}

void Entity_walk(ENTITY *entity) {
	if (entity != NULL) {
		if (entity->functions->walk != NULL && entity->functions->callback == true) {
			entity->functions->walk(entity);
		} else {
			switch(entity->directionFacing) {
				case NORTH:
					entity->dy = -entity->speed;  break;
				case SOUTH:
					entity->dy =  entity->speed; break;
				case WEST:
					entity->dx = -entity->speed; break;
				case EAST:
					entity->dx =  entity->speed; break;
				default: break;
			}
			entity->motionType = WALKING; 
		}
	}
}

void Entity_autoWalk(ENTITY *entity, float distance) {
	if (entity != NULL) {
		if (entity->functions->autoWalk != NULL && entity->functions->callback == true) {
			entity->functions->autoWalk(entity, distance);
		} else {
			/* on recentre l'entité */
			entity->x = SDL_floorf(entity->x) + 0.5;
			entity->y = SDL_floorf(entity->y) + 0.5;

			distance = SDL_floorf(distance);

			entity->autoWalkingEnd_x = entity->x;
			entity->autoWalkingEnd_y = entity->y;
			Sint32 move = entity->speed * entity->dispTileSize;
			switch(entity->directionFacing) {
				case NORTH:
					if (distance >= 0) entity->dy = -move;
					else entity->dy = move;
					entity->autoWalkingEnd_y = entity->y - distance;
					break;
				case SOUTH:
					if (distance >= 0) entity->dy = move;
					else entity->dy = -move;
					entity->autoWalkingEnd_y = entity->y + distance;
					break;
				case WEST:
					if (distance >= 0) entity->dx = -move;
					else entity->dx = move;
					entity->autoWalkingEnd_x = entity->x - distance;
					break;
				case EAST:
					if (distance >= 0) entity->dx =  move;
					else entity->dx =  -move;
					entity->autoWalkingEnd_x = entity->x + distance;
					break;
				default: break;
			}
			entity->motionType = AUTO_WALKING; 
		}
	}
}


/* --- Vie et dégats --- */

void Entity_enableHealth(ENTITY *entity) {
	if (entity != NULL) {
		if (entity->functions->enableHealth != NULL && entity->functions->callback == true) {
			entity->functions->enableHealth(entity);
		} else {
			entity->enableHealth = true;
		}
	}
}

void Entity_disableHealth(ENTITY *entity) {
	if (entity != NULL) {
		if (entity->functions->disableHealth != NULL && entity->functions->callback == true) {
			entity->functions->disableHealth(entity);
		} else {
			entity->enableHealth = false;
		}
	}
}

void Entity_setHealth(ENTITY *entity, float health, float healthMax, float carefulPerFrame) {
	if (entity != NULL) {
		if (entity->functions->setHealth != NULL && entity->functions->callback == true) {
			entity->functions->setHealth(entity, health, healthMax, carefulPerFrame);
		} else {
			entity->health = health;
			entity->healthMax = healthMax;
			entity->carefulPerFrame = carefulPerFrame;
		}
	}
}

void Entity_setDamage(ENTITY *entity, float immediateDamage, float damagePerFrame) {
	if (entity != NULL) {
		if (entity->functions->setDamage != NULL && entity->functions->callback == true) {
			entity->functions->setDamage(entity, immediateDamage, damagePerFrame);
		} else {
			if (entity->enableHealth == true) {
				//entity->health = (entity->health > immediateDamage) ? entity->health - immediateDamage : 0;
				//if (damagePerFrame != -1) entity->damagePerFrame = damagePerFrame;
			}
		}
	}
}

void Entity_setCareful(ENTITY *entity, float immediateCareful, float carefulPerFrame) {
	if (entity != NULL) {
		if (entity->functions->setCareful != NULL && entity->functions->callback == true) {
			entity->functions->setCareful(entity, immediateCareful, carefulPerFrame);
		} else {
			if (entity->enableHealth == true) {
				//entity->health = (entity->health + immediateCareful < entity->healthMax) ? entity->health + immediateCareful : entity->healthMax;
				//if (immediateCareful != -1) entity->carefulPerFrame = carefulPerFrame;
			}
		}
	}
}

bool Entity_isDead(ENTITY *entity) {
	bool dead = false;
	if (entity != NULL) {
		if (entity->functions->isDead != NULL && entity->functions->callback == true) {
			entity->functions->isDead(entity);
		} else {
			if (entity->enableHealth == true && entity->health <= 0)
				dead = true;
		}
	}
	return dead;
}

/* --- Mise à jour et affichage --- */

void Entity_event(ENTITY *entity) {
	if (entity != NULL) {
		if (entity->functions->event != NULL && entity->functions->callback == true) {
			entity->functions->event(entity);
		} else {
			
		}
	}
}

void Entity_update(ENTITY *entity, MAP *map) {
	if (entity != NULL) {
		if (entity->functions->update != NULL && entity->functions->callback == true) {
			entity->functions->update(entity, map);
		} else {
			bool move = false, stepCollision;

			Timer_update(entity->timer);
			stepCollision = Entity_stepCollision(entity, map);

			if ((entity->motionType == WALKING || entity->motionType == AUTO_WALKING) && (entity->dx != 0 || entity->dy != 0)) {
				/* on regarde si l'entité est en collision avec au moins un obstacle ou une autre entité */
				if (entity->collisionStepFromOtherEntity == false && stepCollision == false) {
					move = true;
					entity->x += entity->dx;
					entity->y += entity->dy;

					if (entity->motionType == WALKING) {
						entity->dx = 0;
						entity->dy = 0;
					}
					if (entity->motionType == AUTO_WALKING) {
						if ((SDL_floorf(entity->x) == SDL_floorf(entity->autoWalkingEnd_x))
							&& (SDL_floorf(entity->y) == SDL_floorf(entity->autoWalkingEnd_y))) {
							entity->x = SDL_floorf(entity->x) + 0.5;
							entity->y = SDL_floorf(entity->y) + 0.5;
							entity->dx = 0;
							entity->dy = 0;
						}
					}
				} else {
					entity->dx = 0;
					entity->dy = 0;
				}
			}

			/* Défilement des tiles de l'entité lorsqu'elle se déplace pour l'animer */
			COUNTER *count = Timer_getCounter(entity->timer, FRAME_TILE_COUNTER);
			if (move == true) {
				if (count->activate == false) count->activate = true;
				if (count->finish) {
					entity->currentAnimateTile++;
					if (entity->currentAnimateTile % entity->nbTilesWidth == 0)
						entity->currentAnimateTile = entity->firstTileIsStanding;
				}
			} else {
				if (count->finish) {
					Entity_standing(entity);
					count->value = 0;
					count->activate = false;
				}
			}

			/* Rechercher l'id de collision */
			if (stepCollision) {
				Uint16 i, j;
				Entity_getLookMapPosition(entity, &i, &j);
				entity->idCollisionFacing = map->collision_buffer[j][i];
				if (entity->idCollisionFacing == NO_COLLISION) {
					entity->idCollisionFacing = map->collision_buffer[j][i];
				}
			} else {
				if (entity->idCollisionFacing) entity->idCollisionFacing = NO_COLLISION;
			}
		}
	}
}

void Entity_draw(ENTITY *entity, CAMERA *camera, bool drawBox) {
	if (entity != NULL) {
		if (entity->functions->draw != NULL && entity->functions->callback == true) {
			entity->functions->draw(entity, camera, drawBox);
		} else {
			if (entity->motionType == WALKING || entity->motionType == AUTO_WALKING)
				entity->srcRect.x = entity->srcRect.w * entity->currentAnimateTile;
			else
				entity->srcRect.x = 0;
			entity->srcRect.y = entity->srcRect.h * entity->directionFacing;

			entity->destRect.x = SDL_floorf((float) (entity->x - entity->zoom/2.0)*entity->dispTileSize) - camera->x;
			entity->destRect.y = SDL_floorf((float) (entity->y - entity->zoom/2.0)*entity->dispTileSize) - camera->y;
			
			SG_render(entity->image->texture, &entity->srcRect, &entity->destRect);

			if (drawBox == true) {
				SDL_Rect bodyRect, stepRect;
				SDL_FRect bodyBox, stepBox;

				Entity_getBodyBox(entity, &bodyBox);
				bodyRect.x = SDL_floorf((float) bodyBox.x*entity->dispTileSize) - camera->x;
				bodyRect.y = SDL_floorf((float) bodyBox.y*entity->dispTileSize) - camera->y;
				bodyRect.w = SDL_floorf((float) bodyBox.w*entity->dispTileSize);
				bodyRect.h = SDL_floorf((float) bodyBox.h*entity->dispTileSize);
				SG_drawColor((entity->collisionFromOtherEntity)? PURPLE : FIRE_BRICK);
				SG_drawRect(&bodyRect, false);
				
				Entity_getStepBox(entity, &stepBox);
				stepRect.x = SDL_floorf((float) stepBox.x*entity->dispTileSize) - camera->x;
				stepRect.y = SDL_floorf((float) stepBox.y*entity->dispTileSize) - camera->y;
				stepRect.w = SDL_floorf((float) stepBox.w*entity->dispTileSize);
				stepRect.h = SDL_floorf((float) stepBox.h*entity->dispTileSize);
				SG_drawColor((entity->collisionStepFromOtherEntity)? ORANGE : GREEN);
				SG_drawRect(&stepRect, false);

				char typename[50];
				switch (entity->type) {
					case DEFAULT_ENTITY: strcpy(typename, "ENTITY"); break;
					case PLAYER_ENTITY: strcpy(typename, "PLAYER"); break;
					case PNJ_ENTITY: strcpy(typename, "PNJ"); break;
					case MOB_BLOB_ENTITY: strcpy(typename, "BLOB"); break;
					case MOB_GOBLIN_ENTITY: strcpy(typename, "GOBLIN"); break;
					case MOB_DRAGON_ENTITY: strcpy(typename, "DRAGON"); break;
					case MOB_SKELETON_ENTITY: strcpy(typename, "SKELETON"); break;
					default: strcpy(typename, "ENTITY"); break;
				}
				Uint16 fontsize = entity->dispTileSize/5;
				TTF_Font *font = SG_loadFont("arial.ttf", fontsize);
				SG_printf(false, bodyRect.x, bodyRect.y - fontsize - 2, font,
					(entity->collisionFromOtherEntity)? PURPLE : FIRE_BRICK, typename);
			}
		}
	}
}

void Entity_drawFrontOf(ENTITY *entity, CAMERA *camera, bool drawBox) {
	if (entity != NULL) {
		if (entity->functions->drawFrontOf != NULL && entity->functions->callback == true) {
			entity->functions->drawFrontOf(entity, camera, drawBox);
		}
	}
}