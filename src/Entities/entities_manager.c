#include "entities_manager.h"

ENTITIES_MANAGER *entitiesManager = NULL;

bool EntitiesManager_init(Uint32 maxLength) {
    bool success = false;
    if (entitiesManager == NULL) {
        entitiesManager = (ENTITIES_MANAGER *) calloc(1, sizeof(ENTITIES_MANAGER));
        if (entitiesManager != NULL) {
            entitiesManager->list = (ENTITY **) calloc(maxLength, sizeof(ENTITY *));
            entitiesManager->displayInFront = Array_new(NO_LIMITED);
            entitiesManager->length = 0;
            entitiesManager->maxLength = maxLength;
            success = true;
        }
    } else {
        success = true;
    }
    return success;
}

void EntitiesManager_free() {
    if (entitiesManager != NULL) {
        for (Uint32 i = 0; i < entitiesManager->length; i++) {
            Entity_free(entitiesManager->list[i]);
        }
        Array_free(entitiesManager->displayInFront, true);
        free(entitiesManager->list);
        free(entitiesManager);
        entitiesManager = NULL;
    }
}

void EntitiesManager_clean() {
    if (entitiesManager != NULL) {
        for (Uint32 i = 0; i < entitiesManager->length; i++) {
            Entity_free(entitiesManager->list[i]);
        }
        memset(entitiesManager->list, 0, sizeof(ENTITY *));
        entitiesManager->length = 0;
    }
}

ENTITIES_MANAGER *EntitiesManager_get() {
    if (entitiesManager != NULL) {
        return entitiesManager;
    } else {
        return NULL;
    }
}


/* --- Gestion des entités --- */

void EntitiesManager_addEntity(ENTITY *entity) {
    if (entitiesManager != NULL && entitiesManager->length < entitiesManager->maxLength) {
        entitiesManager->list[entitiesManager->length] = entity;
        Array_append(entitiesManager->displayInFront, Data_new(entitiesManager->length, NULL, UINT32_TYPE, ptr_uint32(entitiesManager->length), NULL));
        entitiesManager->length++;
    }
}

void EntitiesManager_deleteEntity(ENTITY *entity) {
    if (entitiesManager != NULL) {
        Uint32 i, j;
        for (i = 0; i < entitiesManager->length; i++) {
            if (entitiesManager->list[i] == entity) {
                Entity_free(entitiesManager->list[i]);
                Array_delFromId(entitiesManager->displayInFront, entitiesManager->length, true);

                for (j = i; j < entitiesManager->length-1; j++) {
                    entitiesManager->list[j] = entitiesManager->list[j+1];
                }
                entitiesManager->list[entitiesManager->length-1] = NULL;
                entitiesManager->length--;
                break;
            }
        }
    }
}

void EntitiesManager_setDispTileSize(Uint32 dispTileSize) {
    if (entitiesManager != NULL) {
        Uint32 i;
        for (i = 0; i < entitiesManager->length; i++) {
            Entity_setDispTileSize(entitiesManager->list[i], dispTileSize);
        }
    }
}

ENTITY *EntitiesManager_getFromType(TypeEntity type) {
    if (entitiesManager != NULL) {
        ENTITY *entity;
        Uint32 i;
        for (i = 0; i < entitiesManager->displayInFront->length; i++) {
            entity = entitiesManager->list[i];
            if (type == entity->type) return entity;
        }
    }
    return NULL;
}

/* --- Mise à jour et affichage --- */

void EntitiesManager_event() {
    if (entitiesManager != NULL) {
        for (Uint32 i = 0; i < entitiesManager->length; i++) {
            Entity_event(entitiesManager->list[i]);
        }
    }
}

Uint32 Array_getUint32FromIndex(ARRAY *array, Uint32 index) {
    DATA *data = Array_getFromIndex(array, index);
    return *((Uint32*) data->buffer);
}

void EntitiesManager_update(MAP *map) {
    if (entitiesManager != NULL) {
        ARRAY *dispInFront = entitiesManager->displayInFront;
        Uint32 id1, id2, i, j;
        bool inFrontOf;
        
        for (i = 0; i < entitiesManager->length; i++) {
            entitiesManager->list[i]->collisionFromOtherEntity = false;
            entitiesManager->list[i]->collisionStepFromOtherEntity = false;
        }

        for (i = 0; i < entitiesManager->length; i++) {
            id1 = Array_getUint32FromIndex(dispInFront, i);
            /* gestion des collisions */
            for (j = 0; j < entitiesManager->length; j++) {
                id2 = Array_getUint32FromIndex(dispInFront, j);
                /* on évite de comparer l'entité avec elle même */
                if (id1 != id2 && i < j) {
                    inFrontOf = Entity_conflict(entitiesManager->list[id1], entitiesManager->list[id2]);
                    if (inFrontOf && i < j && entitiesManager->list[i]->collisionStepFromOtherEntity == false) {
                        Array_permute(dispInFront, i, j);
                    }
                }
            }
        }

        for (i = 0; i < entitiesManager->length; i++) {
            Entity_update(entitiesManager->list[i], map);
        }
    }
}

void EntitiesManager_draw(CAMERA *camera, bool drawBox) {
    if (entitiesManager != NULL) {
        DATA *data;
        Uint32 i, id;
        for (i = 0; i < entitiesManager->displayInFront->length; i++) {
            data = Array_getFromIndex(entitiesManager->displayInFront, i);
            id = *((Uint32 *) data->buffer);
            Entity_draw(entitiesManager->list[id], camera, drawBox);
        }
    }
}

void EntitiesManager_drawFrontOf(CAMERA *camera, bool drawBox) {
    if (entitiesManager != NULL) {
        DATA *data;
        Uint32 i, id;
        for (i = 0; i < entitiesManager->displayInFront->length; i++) {
            data = Array_getFromIndex(entitiesManager->displayInFront, i);
            id = *((Uint32 *) data->buffer);
            Entity_drawFrontOf(entitiesManager->list[id], camera, drawBox);
        }
    }
}
