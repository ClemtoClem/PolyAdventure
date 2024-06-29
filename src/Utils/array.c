#include "array.h"

/* Create data */
DATA *Data_new(Uint32 id, char *key, TypeData type, void *buffer, void (*freeFunc) (void *, TypeData)) {
	DATA *data = calloc(1, sizeof(DATA));
	if (data != NULL) {
		data->id = id;
		data->key = (key != NULL) ? strdup(key) : NULL;
		data->type = type;
		data->buffer = buffer;
		data->freeFunc = freeFunc;
		data->next = NULL;
	}
	//printf("New Data\n");
	return data;
}

/* Delete data */
void Data_free(DATA *data, bool freeBuffer) {
	if (data != NULL) {
		if (freeBuffer == true) {
			if (data->freeFunc != NULL) {
				data->freeFunc(data->buffer, data->type);
			} else {
				if (data->type == ARRAY_TYPE) {
					Array_free((ARRAY *) data->buffer, true);
				} else {
					free(data->buffer);
				}
			}
		}
		data->buffer = NULL;
		if (data->key != NULL) {
			free(data->key);
			data->key = NULL;
		}
		data->next = NULL;
		free(data);
	}
}

/* Create array */
ARRAY *Array_new(Uint32 limitedLength) {
	ARRAY *array = calloc(1, sizeof(ARRAY));
	if (array != NULL) {
		array->first = NULL;
		array->length = 0;
		array->limitedLength = limitedLength;
		pthread_mutex_init(&array->lock, NULL);
	}
	return array;
}

/* Delete array */
void Array_free(ARRAY *array, bool freeBuffer) {
	if (array != NULL) {
		Array_clear(array, freeBuffer);
		pthread_mutex_destroy(&array->lock);
		free(array);
	}
}

/* Clear array */
void Array_clear(ARRAY *array, bool freeBuffer) {
	if (array != NULL) {
		pthread_mutex_lock(&array->lock);
		DATA *data = array->first;
		DATA *next;
		while (data != NULL) {
			next = data->next;
			Data_free(data, freeBuffer);
			data->next = NULL;
			data = next;
		}
		array->first = NULL;
		array->length = 0;
		pthread_mutex_unlock(&array->lock);
	}
}

/* Return the length of the array */
Uint32 Array_length(ARRAY *array) {
	if (array != NULL) {
		pthread_mutex_lock(&array->lock);
		Uint32 length = array->length;
		pthread_mutex_unlock(&array->lock);
		return length;
	} else {
		return 0;
	}
}

/* Return true if array is full */
bool Array_isFull(ARRAY *array) {
	if (array != NULL && array->limitedLength != 0 && array->length < array->limitedLength) {
		return true;
	} else {
		return false;
	}
}

/* Add data to the end of the array. */
void Array_append(ARRAY *array, DATA *data) {
	if (array != NULL && data != NULL && !Array_isFull(array)) {
		pthread_mutex_lock(&array->lock);
		if (array->length == 0) {
			array->first = data;
		} else {
			DATA *last_data = Array_getLast(array);
			last_data->next = data;
		}
		array->length++;
		pthread_mutex_unlock(&array->lock);
	}
}

/* Add data to the begin of the array. */
void Array_prepend(ARRAY *array, DATA *data) {
	if (array != NULL && data != NULL && !Array_isFull(array)) {
		pthread_mutex_lock(&array->lock);
		if (array->length == 0) {
			array->first = data;
		} else {
			data->next = array->first;
			array->first = data;
		}
		array->length++;
		pthread_mutex_unlock(&array->lock);
	}
}

/* Del object to index in the array. */
void Array_delFromIndex(ARRAY *array, Uint32 index, bool freeBuffer) {
	if (array != NULL && index < array->length) {
		pthread_mutex_lock(&array->lock);
		Uint32 i = 0;
		DATA *data = array->first;
		DATA *next, *after = NULL;
		while (data != NULL) {
			next = data->next;
			if (i == index) {
				if (after == NULL) array->first = next;
				else after->next = next;
				Data_free(data, freeBuffer);
				array->length --;
				break;
			}
			i++;
			after = data;
			data = next;
		}
		pthread_mutex_unlock(&array->lock);
	}
}

/* Del first object with this id in the array. */
void Array_delFromId(ARRAY *array, Uint32 id, bool freeBuffer) {
	if (array != NULL) {
		pthread_mutex_lock(&array->lock);
		DATA *data = array->first;
		DATA *next, *after = NULL;
		while (data != NULL) {
			next = data->next;
			if (data->id == id) {
				if (after == NULL) array->first = next;
				else after->next = next;
				Data_free(data, freeBuffer);
				array->length --;
				break;
			}
			after = data;
			data = next;
		}
		pthread_mutex_unlock(&array->lock);
	}
}

/* Del first  object with this key in the array. */
void Array_delFromKey(ARRAY *array, char *key, bool freeBuffer) {
	if (array != NULL && key != NULL) {
		pthread_mutex_lock(&array->lock);
		DATA *data = array->first;
		DATA *next, *after = NULL;
		while (data != NULL) {
			next = data->next;
			if (strcmp(data->key, key) == 0) {
				if (after == NULL) array->first = next;
				else after->next = next;
				Data_free(data, freeBuffer);
				array->length --;
				break;
			}
			after = data;
			data = next;
		}
		pthread_mutex_unlock(&array->lock);
	}
}

/* Del first object with this type in the array. */
void Array_delFromType(ARRAY *array, TypeData type, bool freeBuffer) {
	if (array != NULL) {
		pthread_mutex_lock(&array->lock);
		DATA *data = array->first;
		DATA *next, *after = NULL;
		while (data != NULL) {
			next = data->next;
			if (data->type == type) {
				if (after == NULL) array->first = next;
				else after->next = next;
				Data_free(data, freeBuffer);
				array->length --;
				break;
			}
			after = data;
			data = next;
		}
		pthread_mutex_unlock(&array->lock);
	}
}

/* Insert object to index in the array. */
void Array_insert(ARRAY *array, Uint32 index, DATA *new_data) {
	if (array != NULL && new_data != NULL && index < array->length) {
		pthread_mutex_lock(&array->lock);
		DATA *data = array->first;
		if (index == 0 ) {
			new_data->next = data;
			array->first = new_data;
		} else {
			Uint32 i = 1;
			while (data->next != NULL) {
				if (i == index) {
					new_data->next = data->next;
					data->next = new_data;
					break;
				}
				data = data->next;
			}
		}
		array->length++;
		pthread_mutex_unlock(&array->lock);
	}
}

/* Permute object to old index to index. */
void Array_permute(ARRAY *array, Uint32 index1, Uint32 index2) {
	if (array != NULL && index1 != index2) {
		DATA *data1 = Array_getFromIndex(array, index1);
		DATA *data2 = Array_getFromIndex(array, index2);
		DATA *temp = data1->next;
		data1->next = data2->next;
		data2->next = temp;
		if (index1 == 0) {
			array->first = data2;
			temp = Array_getFromIndex(array, index2-1);
			temp->next = data1;
		} else if (index2 == 0) {
			array->first = data1;
			temp = Array_getFromIndex(array, index1-1);
			temp->next = data2;
		} else {
			temp = Array_getFromIndex(array, index1-1);
			temp->next = data2;
			temp = Array_getFromIndex(array, index2-1);
			temp->next = data1;
		}
	}
}

/* Get last object the array. */
DATA *Array_getLast(ARRAY *array) {
	if (array != NULL) {
		DATA *data = array->first;
		while (data->next != NULL)
			data = data->next;
		return data;
	} else {
		return NULL;
	}
}

/* Get object to index in the array. */
DATA *Array_getFromIndex(ARRAY *array, Uint32 index) {
	if (array != NULL && index < array->length) {
		pthread_mutex_lock(&array->lock);
		Uint32 i = 0;
		DATA *data = array->first;
		while (data != NULL) {
			if (i == index) {
				pthread_mutex_unlock(&array->lock);
				return data;
			}
			i++;
			data = data->next;
		}
		pthread_mutex_unlock(&array->lock);
	}
	return NULL;
}

/* Get first object with this id in the array. */
DATA *Array_getFromId(ARRAY *array, Uint32 id) {
	if (array != NULL) {
		pthread_mutex_lock(&array->lock);
		DATA *data = array->first;
		while (data != NULL) {
			if (data->id == id) {
				pthread_mutex_unlock(&array->lock);
				return data;
			}
			data = data->next;
		}
		pthread_mutex_unlock(&array->lock);
	}
	return NULL;
}

/* Get first  object with this key in the array. */
DATA *Array_getFromKey(ARRAY *array, char *key) {
	if (array != NULL) {
		pthread_mutex_lock(&array->lock);
		DATA *data = array->first;
		while (data != NULL) {
			if (strcmp(data->key, key) == 0) {
				pthread_mutex_unlock(&array->lock);
				return data;
			}
			data = data->next;
		}
		pthread_mutex_unlock(&array->lock);
	}
	return NULL;
}

/* Get first object with this type in the array. */
DATA *Array_getFromType(ARRAY *array, TypeData type) {
	if (array != NULL) {
		pthread_mutex_lock(&array->lock);
		DATA *data = array->first;
		while (data != NULL) {
			if (data->type == type) {
				pthread_mutex_unlock(&array->lock);
				return data;
			}
			data = data->next;
		}
		pthread_mutex_unlock(&array->lock);
	}
	return NULL;
}

/* Get type of object to index in the array. */
TypeData Array_getTypeFromIndex(ARRAY *array, Uint32 index) {
	if (array != NULL) {
		pthread_mutex_lock(&array->lock);
		Uint32 i = 0;
		DATA *data = array->first;
		while (data != NULL) {
			if (i == index) {
				pthread_mutex_unlock(&array->lock);
				return data->type;
			}
			i++;
			data = data->next;
		}
		pthread_mutex_unlock(&array->lock);
	}
	return NIL_TYPE;
}

/* Get first type of object with this id in the array. */
TypeData Array_getTypeFromId(ARRAY *array, Uint32 id) {
	if (array != NULL) {
		pthread_mutex_lock(&array->lock);
		DATA *data = array->first;
		while (data != NULL) {
			if (data->id == id) {
				pthread_mutex_unlock(&array->lock);
				return data->type;
			}
			data = data->next;
		}
		pthread_mutex_unlock(&array->lock);
	}
	return NIL_TYPE;
}

/* Get first type of object with this key in the array. */
TypeData Array_getTypeFromKey(ARRAY *array, char *key) {
	if (array != NULL) {
		pthread_mutex_lock(&array->lock);
		DATA *data = array->first;
		while (data != NULL) {
			if (strcmp(data->key, key) == 0) {
				pthread_mutex_unlock(&array->lock);
				return data->type;
			}
			data = data->next;
		}
		pthread_mutex_unlock(&array->lock);
	}
	return NIL_TYPE;
}

/* Type to string */
void ConvertTypeToString(TypeData type, char str[40]) {
	if (str != NULL) {
		switch (type) {
			case NIL_TYPE: strcpy(str, "NIL"); break;
			case BOOL_TYPE: strcpy(str, "BOOL"); break;
			case UINT8_TYPE: strcpy(str, "UINT8"); break;
			case SINT8_TYPE: strcpy(str, "SINT8"); break;
			case UINT16_TYPE: strcpy(str, "UINT16"); break;
			case SINT16_TYPE: strcpy(str, "SINT16"); break;
			case UINT32_TYPE: strcpy(str, "UINT32"); break;
			case SINT32_TYPE: strcpy(str, "SINT32"); break;
			case UINT64_TYPE: strcpy(str, "UINT64"); break;
			case SINT64_TYPE: strcpy(str, "SINT64"); break;
			case FLOAT_TYPE: strcpy(str, "FLOAT"); break;
			case DOUBLE_TYPE: strcpy(str, "DOUBLE"); break;
			case STRING_TYPE: strcpy(str, "STRING"); break;
			case BUFFER_TYPE: strcpy(str, "BUFFER"); break;
			case ARRAY_TYPE: strcpy(str, "ARRAY"); break;
			case RECT_TYPE: strcpy(str, "RECT"); break;
			case FLOAT_RECT_TYPE: strcpy(str, "FLOAT RECT"); break;
			case CIRCLE_TYPE: strcpy(str, "CIRCLE"); break;
			case FLOAT_CIRCLE_TYPE: strcpy(str, "FLOAT CIRCLE"); break;
			case FONT_TYPE: strcpy(str, "FONT"); break;
			case SURFACE_TYPE: strcpy(str, "SURFACE"); break;
			case TEXTURE_TYPE: strcpy(str, "TEXTURE"); break;
			case IMAGE_TYPE: strcpy(str, "IMAGE"); break;
			case SOUND_TYPE: strcpy(str, "SOUND"); break;
			case MUSIC_TYPE: strcpy(str, "MUSIC"); break;
			case PANEL_WIDGET_TYPE: strcpy(str, "PANEL WIDGET"); break;
			case INNERBOX_WIDGET_TYPE: strcpy(str, "INNERBOX WIDGET"); break;
			case SPRITE_WIDGET_TYPE: strcpy(str, "SPRITE WIDGET"); break;
			case LABEL_WIDGET_TYPE: strcpy(str, "LABEL WIDGET"); break;
			case BUTTON_WIDGET_TYPE: strcpy(str, "BUTTON WIDGET"); break;
			case TOGGLE_SWITCH_WIDGET_TYPE: strcpy(str, "TOGGLE SWITCH WIDGET"); break;
			case SCROLLBAR_WIDGET_TYPE: strcpy(str, "SCROLLBAR WIDGET"); break;
			case LEVELBAR_WIDGET_TYPE: strcpy(str, "LEVELBAR WIDGET"); break;
			case COMBOBOX_WIDGET_TYPE: strcpy(str, "COMBOBOX WIDGET"); break;
			default: strcpy(str, "BUFFRER"); break;
		}
	}
}

void Data_BufferToString(DATA *data, char *dest, size_t len) {
	if (data != NULL && dest != NULL) {
		char *text;
		dest[0] = '\0';
		switch(data->type) {
			case BOOL_TYPE:
				if (*((bool *) data->buffer) == true)
					strncpy(dest, "true", len);
				else
					strncpy(dest, "false", len);
				break;
			case UINT8_TYPE:
				asprintf(&text, "%hhu", *((Uint8 *) data->buffer));
				strncpy(dest, text, len);
				break;
			case SINT8_TYPE:
				asprintf(&text, "%hhd", *((Sint8 *) data->buffer));
				strncpy(dest, text, len);
				break;
			case UINT16_TYPE:
				asprintf(&text, "%hu", *((Uint16 *) data->buffer));
				strncpy(dest, text, len);
				break;
			case SINT16_TYPE:
				asprintf(&text, "%hd", *((Sint16 *) data->buffer));
				strncpy(dest, text, len);
				break;
			case UINT32_TYPE:
				asprintf(&text, "%u", *((Uint32 *) data->buffer));
				strncpy(dest, text, len);
				break;
			case SINT32_TYPE:
				asprintf(&text, "%d", *((Sint32 *) data->buffer));
				strncpy(dest, text, len);
				break;
			case UINT64_TYPE:
			#if defined(_WIN32) || defined(_WIN64)
				asprintf(&text, "%llu", *((Uint64 *) data->buffer));
			#else
				asprintf(&text, "%lu", *((Uint64 *) data->buffer));
			#endif
				strncpy(dest, text, len);
				break;
			case SINT64_TYPE:
			#if defined(_WIN32) || defined(_WIN64)
				asprintf(&text, "%lld", *((Sint64 *) data->buffer));
			#else
				asprintf(&text, "%ld", *((Sint64 *) data->buffer));
			#endif
				strncpy(dest, text, len);
				break;
			case FLOAT_TYPE:
				asprintf(&text, "%f", *((float *) data->buffer));
				strncpy(dest, text, len);
				break;
			case STRING_TYPE:
				strncpy(dest, data->buffer, len);
				break;
			case RECT_TYPE: {
				SDL_Rect *rect = (SDL_Rect *) data->buffer;
				asprintf(&text, "rect(%d, %d, %d, %d)", rect->x, rect->y, rect->w, rect->h);
				strncpy(dest, text, len);
				break;
			}
			case FLOAT_RECT_TYPE:  {
				SDL_FRect *rect = (SDL_FRect *) data->buffer;
				asprintf(&text, "frect(%f, %f, %f, %f)", rect->x, rect->y, rect->w, rect->h);
				strncpy(dest, text, len);
				break;
			}
			case CIRCLE_TYPE: {
				CIRCLE *circle = data->buffer;
				asprintf(&text, "circle(%d, %d, %d)", circle->x, circle->y, circle->radius);
				strncpy(dest, text, len);
				break;
			}
			case FLOAT_CIRCLE_TYPE: {
				FCIRCLE *circle = data->buffer;
				asprintf(&text, "fcircle(%f, %f, %f)", circle->x, circle->y, circle->radius);
				strncpy(dest, text, len);
				break;
			}
			case ARRAY_TYPE: {
				ARRAY *array = data->buffer;
				DATA *data = array->first;
				text = malloc(len+1);
				strcat(dest, "{");
				while (data != NULL) {
					if (len - strlen(dest) > 6) {
						Data_BufferToString(data, text, len-strlen(dest)-6);
						if (strlen(text) != 0) {
							strcat(dest, text);
							if (data->next != NULL) strcat(dest, ", ");
						}
					} else {
						strcat(dest, " ...");
						break;
					}
					data = data->next;
				}
				strcat(dest, "}");
				break;
			}
			default: break;
		}
		free(text);
	}
}

/* convert array to string */
void Array_BufferToString(ARRAY *array, char *dest, size_t len) {
	if (array != NULL && dest != NULL) {
		pthread_mutex_lock(&array->lock);
		char *text = malloc(len+1);
		text[0] = '\0';
		dest[0] = '\0';
		
		DATA *data = array->first;

		strcat(dest, "{");
		while (data != NULL) {
			if (len - strlen(dest) > 6) {
				Data_BufferToString(data, text, len-strlen(dest)-6);
				if (strlen(text) != 0) {
					strcat(dest, text);
					if (data->next != NULL) strcat(dest, ", ");
				}
			} else {
				strcat(dest, " ...");
				break;
			}
			data = data->next;
		}
		strcat(dest, "}");
		free(text);
		pthread_mutex_unlock(&array->lock);
	}
}

/* create pointer on the constant */
bool   *ptr_bool(bool val)     { bool *res   = malloc(sizeof(bool));   *res = val; return res; }
Uint8  *ptr_uint8(Uint8 val)   { Uint8 *res  = malloc(sizeof(Uint8));  *res = val; return res; }
Sint8  *ptr_int8(Sint8 val)    { Sint8 *res  = malloc(sizeof(Sint8));  *res = val; return res; }
Uint16 *ptr_uint16(Uint16 val) { Uint16 *res = malloc(sizeof(Uint16)); *res = val; return res; }
Sint16 *ptr_int16(Sint16 val)  { Sint16 *res = malloc(sizeof(Sint16)); *res = val; return res; }
Uint32 *ptr_uint32(Uint32 val) { Uint32 *res = malloc(sizeof(Uint32)); *res = val; return res; }
Sint32 *ptr_int32(Sint32 val)  { Sint32 *res = malloc(sizeof(Sint32)); *res = val; return res; }
Uint64 *ptr_uint64(Uint64 val) { Uint64 *res = malloc(sizeof(Uint64)); *res = val; return res; }
Sint64 *ptr_int64(Sint64 val)  { Sint64 *res = malloc(sizeof(Sint64)); *res = val; return res; }
float  *ptr_float(float val)   { float *res  = malloc(sizeof(float));  *res = val; return res; }
double *ptr_double(double val) { double *res = malloc(sizeof(double)); *res = val; return res; }
char   *ptr_string(char *val)  { return strdup(val); }

void Array_appendBool(ARRAY *array, bool value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, BOOL_TYPE, ptr_bool(value), NULL));
	}
}

void Array_appendUint8(ARRAY *array, Uint8 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, UINT8_TYPE, ptr_uint8(value), NULL));
	}
}

void Array_appendSint8(ARRAY *array, Sint8 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, SINT8_TYPE, ptr_int8(value), NULL));
	}
}

void Array_appendUint16(ARRAY *array, Uint16 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, UINT16_TYPE, ptr_uint16(value), NULL));
	}
}

void Array_appendSint16(ARRAY *array, Sint16 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, SINT16_TYPE, ptr_int16(value), NULL));
	}
}

void Array_appendUint32(ARRAY *array, Uint32 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, UINT32_TYPE, ptr_uint32(value), NULL));
	}
}

void Array_appendSint32(ARRAY *array, Sint32 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, SINT32_TYPE, ptr_int32(value), NULL));
	}
}

void Array_appendUint64(ARRAY *array, Uint64 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, UINT64_TYPE, ptr_uint64(value), NULL));
	}
}

void Array_appendSint64(ARRAY *array, Sint64 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, SINT64_TYPE, ptr_int64(value), NULL));
	}
}

void Array_appendFloat(ARRAY *array, float value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, FLOAT_TYPE, ptr_float(value), NULL));
	}
}

void Array_appendDouble(ARRAY *array, double value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, DOUBLE_TYPE, ptr_double(value), NULL));
	}
}

void Array_appendString(ARRAY *array, char *str) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, STRING_TYPE, ptr_string(str), NULL));
	}
}

void Array_appendRect(ARRAY *array, SDL_Rect *rect) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, RECT_TYPE, rect, NULL));
	}
}

void Array_appendFloatRect(ARRAY *array, SDL_FRect *rect) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, FLOAT_RECT_TYPE, rect, NULL));
	}
}

void Array_appendCircle(ARRAY *array, CIRCLE *circle) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, CIRCLE_TYPE, circle, NULL));
	}
}

void Array_appendFloatCircle(ARRAY *array, FCIRCLE *circle) {
	if (array != NULL && !Array_isFull(array)) {
		Array_append(array, Data_new(ID_DEFAULT, NULL, FLOAT_CIRCLE_TYPE, circle, NULL));
	}
}


void Array_prependBool(ARRAY *array, bool value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, BOOL_TYPE, ptr_bool(value), NULL));
	}
}

void Array_prependUint8(ARRAY *array, Uint8 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, UINT8_TYPE, ptr_uint8(value), NULL));
	}
}

void Array_prependSint8(ARRAY *array, Sint8 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, SINT8_TYPE, ptr_int8(value), NULL));
	}
}

void Array_prependUint16(ARRAY *array, Uint16 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, UINT16_TYPE, ptr_uint16(value), NULL));
	}
}

void Array_prependSint16(ARRAY *array, Sint16 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, SINT16_TYPE, ptr_int16(value), NULL));
	}
}

void Array_prependUint32(ARRAY *array, Uint32 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, UINT32_TYPE, ptr_uint32(value), NULL));
	}
}

void Array_prependSint32(ARRAY *array, Sint32 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, SINT32_TYPE, ptr_int32(value), NULL));
	}
}

void Array_prependUint64(ARRAY *array, Uint64 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, UINT64_TYPE, ptr_uint64(value), NULL));
	}
}

void Array_prependSint64(ARRAY *array, Sint64 value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, SINT64_TYPE, ptr_int64(value), NULL));
	}
}

void Array_prependFloat(ARRAY *array, float value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, FLOAT_TYPE, ptr_float(value), NULL));
	}
}

void Array_prependDouble(ARRAY *array, double value) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, DOUBLE_TYPE, ptr_double(value), NULL));
	}
}

void Array_prependString(ARRAY *array, char *str) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, STRING_TYPE, ptr_string(str), NULL));
	}
}

void Array_prependRect(ARRAY *array, SDL_Rect *rect) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, RECT_TYPE, rect, NULL));
	}
}

void Array_prependFloatRect(ARRAY *array, SDL_FRect *rect) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, FLOAT_RECT_TYPE, rect, NULL));
	}
}

void Array_prependCircle(ARRAY *array, CIRCLE *circle) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, CIRCLE_TYPE, circle, NULL));
	}
}

void Array_prependFloatCircle(ARRAY *array, FCIRCLE *circle) {
	if (array != NULL && !Array_isFull(array)) {
		Array_prepend(array, Data_new(ID_DEFAULT, NULL, FLOAT_CIRCLE_TYPE, circle, NULL));
	}
}

bool Array_getBool(ARRAY *array, char *key, bool defaultValue) {
	bool val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			char *wordsarray[6] = {"true", "yes", "1", "false", "no", "0"};
			bool valuearray[6] = {true, true, true, false, false, false};
			int i;
			for (i = 0; i < 6; i++) {
				if (strcmp(data->buffer, wordsarray[i]) == 0)
					val = valuearray[i];
			}
		} else if (data->type == BOOL_TYPE) {
			val = *((bool *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

Uint8 Array_getUint8(ARRAY *array, char *key, Uint8 defaultValue) {
	Uint8 val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "%hhu", &val);
		} else if (data->type == UINT8_TYPE) {
			val = *((Uint8 *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

Sint8 Array_getSint8(ARRAY *array, char *key, Sint8 defaultValue) {
	Sint8 val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "%hhd", &val);
		} else if (data->type == SINT8_TYPE) {
			val = *((Sint8 *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

Uint16 Array_getUint16(ARRAY *array, char *key, Uint16 defaultValue) {
	Uint16 val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "%hu", &val);
		} else if (data->type == UINT16_TYPE) {
			val = *((Uint16 *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

Sint16 Array_getSint16(ARRAY *array, char *key, Sint16 defaultValue) {
	Sint16 val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "%hd", &val);
		} else if (data->type == SINT16_TYPE) {
			val = *((Sint16 *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

Uint32 Array_getUint32(ARRAY *array, char *key, Uint32 defaultValue) {
	Uint32 val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "%d", &val);
		} else if (data->type == UINT32_TYPE) {
			val = *((Uint32 *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

Sint32 Array_getSint32(ARRAY *array, char *key, Sint32 defaultValue) {
	Sint32 val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "%d", &val);
		} else if (data->type == SINT32_TYPE) {
			val = *((Sint32 *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

Uint64 Array_getUint64(ARRAY *array, char *key, Uint64 defaultValue) {
	Uint64 val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "%llu", &val);
		} else if (data->type == UINT64_TYPE) {
			val = *((Uint64 *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

Sint64 Array_getSint64(ARRAY *array, char *key, Sint64 defaultValue) {
	Sint64 val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "%lld", &val);
		} else if (data->type == SINT64_TYPE) {
			val = *((Sint64 *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

float Array_getFloat(ARRAY *array, char *key, float defaultValue) {
	float val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "%f", &val);
		} else if (data->type == FLOAT_TYPE) {
			val = *((float *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

double Array_getDouble(ARRAY *array, char *key, double defaultValue) {
	double val = defaultValue;
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "%lf", &val);
		} else if (data->type == DOUBLE_TYPE) {
			val = *((double *) data->buffer);
		}
		pthread_mutex_unlock(&array->lock);
	}
	return val;
}

void Array_getString(ARRAY *array, char *key, char *dest, char *defaultValue, size_t len) {
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			strncpy(dest, data->buffer, len);
		}
		pthread_mutex_unlock(&array->lock);
	} else {
		strncpy(dest, defaultValue, len);
	}
}

void Array_getRect(ARRAY *array, char *key, SDL_Rect *destRect, SDL_Rect *defaultRect) {
	RectCopy(destRect, defaultRect);
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "rect(%d,%d,%d,%d=", &destRect->x, &destRect->y, &destRect->w, &destRect->h);
		} else if (data->type == RECT_TYPE) {
			SDL_Rect *rect =  data->buffer;
			RectCopy(destRect, rect);
		}
		pthread_mutex_unlock(&array->lock);
	}
}

void Array_getFloatRect(ARRAY *array, char *key, SDL_FRect *destRect, SDL_FRect *defaultRect) {
	FloatRectCopy(destRect, defaultRect);
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "frect(%f,%f,%f,%f)", &destRect->x, &destRect->y, &destRect->w, &destRect->h);
		} else if (data->type == FLOAT_RECT_TYPE) {
			SDL_FRect *rect =  data->buffer;
			FloatRectCopy(destRect, rect);
		}
		pthread_mutex_unlock(&array->lock);
	}
}

void Array_getCircle(ARRAY *array, char *key, CIRCLE *destCircle, CIRCLE *defaultCircle) {
	CircleCopy(destCircle, defaultCircle);
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "rect((%d,%d,%d)", &destCircle->x, &destCircle->y, &destCircle->radius);
		} else if (data->type == RECT_TYPE) {
			CIRCLE *circle =  data->buffer;
			CircleCopy(destCircle, circle);
		}
		pthread_mutex_unlock(&array->lock);
	}
}

void Array_getFloatCircle(ARRAY *array, char *key, FCIRCLE *destCircle, FCIRCLE *defaultCircle) {
	FloatCircleCopy(destCircle, defaultCircle);
	DATA *data = Array_getFromKey(array, key);
	if (data != NULL) {
		pthread_mutex_lock(&array->lock);
		if (data->type == STRING_TYPE) {
			sscanf(data->buffer, "frect(%f,%f,%f)", &destCircle->x, &destCircle->y, &destCircle->radius);
		} else if (data->type == FLOAT_RECT_TYPE) {
			FCIRCLE *circle =  data->buffer;
			FloatCircleCopy(destCircle, circle);
		}
		pthread_mutex_unlock(&array->lock);
	}
}

ARRAY *Array_getArray(ARRAY *array, char *key) {
	ARRAY *array2 = NULL;
	if (array != NULL && key != NULL) {
		DATA *data = Array_getFromKey(array, key);
		if (data != NULL && data->type == ARRAY_TYPE) {
			array2 = (ARRAY *) data->buffer;
		}
	}
	return array2;
}

void Array_print(ARRAY *array, size_t printLen) {
	if (array != NULL) {
		char *dest = malloc(printLen+1);
		dest[0] = '\0';
		char temp[50];
		DATA *data;
		for (Uint32 i = 0; i < array->length; i++) {
			data = Array_getFromIndex(array, i);
			pthread_mutex_lock(&array->lock);
			printf("array[%d] = { id : %u, ", i, data->id);
			if (data->key != NULL) printf("key : \"%s\", ", data->key);
			else printf(" key : , ");
			ConvertTypeToString(data->type, temp);
			printf("type : \"%s\", ", temp);
			if (data->type == ARRAY_TYPE) {
				printf("buffer : {\n");
				pthread_mutex_unlock(&array->lock);
				Array_print(data->buffer, printLen);
				pthread_mutex_lock(&array->lock);
				printf("}\n");
			} else {
				Data_BufferToString(data, dest, printLen);
				printf("buffer : %s }\n", dest);
			}
			pthread_mutex_unlock(&array->lock);
		}
		free(dest);
	}
}

void Array_test() {
	printf ("---- TEST array ----\n");

	Uint32 MOT_1 = 1;
	Uint32 MOT_2 = 2;
	Uint32 REEL_1 = 3;
	Uint32 REEL_2 = 4;
	Uint32 ENTIER_1 = 5;
	Uint32 ENTIER_2 = 6;

	ARRAY *array = Array_new(NO_LIMITED);

	Array_append(array, Data_new(MOT_1, "MOT_1", STRING_TYPE, ptr_string("BONJOUR"), NULL));
	Array_append(array, Data_new(MOT_2, "MOT_2", STRING_TYPE, ptr_string("SALUT"), NULL));
	Array_append(array, Data_new(REEL_1, "REEL_1", FLOAT_TYPE, ptr_float(1.458f), NULL));
	Array_append(array, Data_new(REEL_2, "REEL_2", FLOAT_TYPE, ptr_float(-125.456f), NULL));
	Array_append(array, Data_new(ENTIER_1, "ENTIER_1", UINT32_TYPE, ptr_int32(1234), NULL));
	Array_append(array, Data_new(ENTIER_2, "ENTIER_2", UINT32_TYPE, ptr_int32(-987), NULL));

	char dest[500];
	Array_BufferToString(array, dest, 500);

	printf("array = %s\n", dest);

	printf("array.length = %d\n\n", Array_length(array));

	Data_BufferToString(Array_getFromId(array, REEL_2), dest, 500);
	printf("array{REEL_2} = %s\n", dest);

	Data_BufferToString(Array_getFromKey(array, "ENTIER_1"), dest, 500);
	printf("array{\"ENTIER_1\"} = %s\n\n", dest);

	Array_print(array, 500);

	Array_free(array, true);

	LOG_SUCCESS("----- SUCCESS -----\n\n");
}