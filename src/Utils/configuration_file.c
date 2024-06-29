#include "configuration_file.h"

// Utility functions
bool estUnEspace(char c);
bool estUnChiffre(char c);
bool estUnCaractereSyntaxique(char c);
bool stringCompare(char *str1, char *str2, size_t len);

bool estUnEspace(char c) {
	if (c == ' ' || c == '\t') return true;
	return false;
}

bool estUnChiffre(char c) {
	if (c >= '0' && c <= '9') return true;
	return false;
}

bool estUnCaractereSyntaxique(char c) {
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || estUnChiffre(c)) return true;
	return false;
}

bool stringCompare(char *str1, char *str2, size_t len) {
	size_t j;
	for (j = 0; j < len; j++) {
		if (str1[j] != str2[j]) return false;
	}
	return true;
}

void *StringToAllocValue(TypeData type, char *str) {
	void *buffer = NULL;
	switch (type) {
		case UINT8_TYPE: {
			Uint8 val;
			sscanf(str, "%hhu", &val);
			buffer = (void *) ptr_uint8(val);
		} break;
		case SINT8_TYPE: {
			Sint8 val;
			sscanf(str, "%hhd", &val);
			buffer = (void *) ptr_int8(val);
		} break;
		case UINT16_TYPE: {
			Uint16 val;
			sscanf(str, "%hu", &val);
			buffer = (void *) ptr_uint16(val);
		} break;
		case SINT16_TYPE: {
			Sint16 val;
			sscanf(str, "%hd", &val);
			buffer = (void *) ptr_int16(val);
		} break;
		case UINT32_TYPE: {
			Uint32 val;
			sscanf(str, "%u", &val);
			buffer = (void *) ptr_uint32(val);
		} break;
		case SINT32_TYPE: {
			Sint32 val;
			sscanf(str, "%d", &val);
			buffer = (void *) ptr_int32(val);
		} break;
		case UINT64_TYPE: {
			Uint64 val;
		#if defined(_WIN32) || defined(_WIN64)
			sscanf(str, "%llu", &val);
		#else
			sscanf(str, "%lu", &val);
		#endif
			buffer = (void *) ptr_uint64(val);
		} break;
		case SINT64_TYPE: {
			Sint64 val;
		#if defined(_WIN32) || defined(_WIN64)
			sscanf(str, "%lld", &val);
		#else
			sscanf(str, "%ld", &val);
		#endif
			buffer = (void *) ptr_int64(val);
		} break;
		case FLOAT_TYPE: {
			float val;
			sscanf(str, "%f", &val);
			buffer = (void *) ptr_float(val);
		} break;
		case DOUBLE_TYPE: {
			double val;
			sscanf(str, "%lf", &val);
			buffer = (void *) ptr_double(val);
		} break;
		default: break;
	}
	return buffer;
}

bool readInstruction(FILE *file, char *instruction, ARRAY *array, int *line) {
    char nomVariable[100];
    char valeurVariable[500];
    
    // Ignorer les lignes vides et les commentaires
    if (instruction[0] == '#' || instruction[0] == '\0' || instruction[0] == '\n' || instruction[0] == '\r')
        return true;
    
    bool definirVariable = false;
    bool definirValeur = false;
    bool nommerAvecId = false;
    size_t i, j = 0;
    
    nomVariable[0] = '\0';
    valeurVariable[0] = '\0';
    
    for (i = 0; i < strlen(instruction); i++) {
        char c = instruction[i];
        
        if (!definirValeur && !definirVariable) {
            if (c == '%') {
                nommerAvecId = true;
            } else if (estUnCaractereSyntaxique(c)) {
                nomVariable[j] = c;
                j++;
                definirVariable = true;
            } else {
                LOG_ERROR(ErrorType_ERROR, "ligne %d, Mauvaise syntaxe, Utilisation de caractere interdit pour definir une variable (%s)\n", *line, instruction);
                return false;
            }
        } else if (definirVariable) {
            if (estUnCaractereSyntaxique(c)) {
                nomVariable[j] = c;
                j++;
            } else if (c == '=') {
                nomVariable[j] = '\0';
                j = 0;
                while(instruction[i+j+1] != '\n' && instruction[i+j+1] != '\0' && instruction[i+j+1] != ';') {
                    valeurVariable[j] = instruction[i+j+1];
                    j++;
                }
                valeurVariable[j] = '\0';
                j = 0;
                definirVariable = false;
                definirValeur = true;
            } else {
                LOG_ERROR(ErrorType_ERROR, "ligne %d, Mauvaise syntaxe, utilisation de caractere interdit pour definir une variable (%s)\n", *line, instruction);
                return false;
            }
        }
    }
    
    // Convertir la valeur en DATA et l'ajouter à ARRAY
    if (definirValeur) {
        if (valeurVariable[0] == '{') {
            // Cas où la valeur est un bloc
            ARRAY *array2 = Array_new(NO_LIMITED);
            if (!readBlock(file, array2, line)) return false;
            if (nommerAvecId) {
                Uint32 id;
                #if defined(_WIN32) || defined(_WIN64)
                if (sscanf(nomVariable, "%u", &id) == 1)
                #else
                if (sscanf(nomVariable, "%lu", &id) == 1)
                #endif
                    Array_append(array, Data_new(id, NULL, ARRAY_TYPE, array2, NULL));
                else
                    Array_append(array, Data_new(ID_DEFAULT, NULL, ARRAY_TYPE, array2, NULL));
            } else {
                Array_append(array, Data_new(ID_DEFAULT, nomVariable, ARRAY_TYPE, array2, NULL));
            }
            return true;
        } else {
            // Cas où la valeur est une donnée simple
            DATA *data = convertStringtoData(nomVariable, valeurVariable, *line);
            if (data != NULL) {
                Array_append(array, data);
                return true;
            }
        }
    }
    
    return false;
}


DATA *convertStringtoData(char *nomVariable, char *str, int line) {	
	bool estUnNombre = false;
	bool decimal = false, negatif = false;
	char c, valeurVariable[500] = "\0";
	size_t i, j = 0;
	TypeData type = NIL_TYPE;

	/* sinon on recherche le type et la valeur de la variable et on l'ajoute dans la liste */
	if (stringCompare(str, "true", 4) || stringCompare(str, "yes", 3)) {
		type = BOOL_TYPE;
		return Data_new(ID_DEFAULT, nomVariable, BOOL_TYPE, ptr_bool(true), NULL);
	} else if (stringCompare(str, "false", 5) || stringCompare(str, "no", 2)) {
		type = BOOL_TYPE;
		return Data_new(ID_DEFAULT, nomVariable, BOOL_TYPE, ptr_bool(false), NULL);
	} else if (stringCompare(str, "circle(", 7)) {
		CIRCLE *circle = malloc(sizeof(CIRCLE));
		
		#if defined(_WIN32) || defined(_WIN64)
		if (sscanf(str, "circle(%d,%d,%d)", &circle->x, &circle->y, &circle->radius) != 3)
		#else
		if (sscanf(str, "circle(%ld,%ld,%ld)", &circle->x, &circle->y, &circle->radius) != 3)
		#endif
		{
			LOG_ERROR(ErrorType_ERROR, "ligne %ld, Mauvais parametre pour definir le cercle\n", line);
			return false;
		}
		return Data_new(ID_DEFAULT, nomVariable, CIRCLE_TYPE, circle, NULL);
	} else if (stringCompare(str, "rect(", 5)) {
		SDL_Rect *rect = malloc(sizeof(SDL_Rect));
		if (sscanf(str, "rect(%d,%d,%d,%d)", &rect->x, &rect->y, &rect->w, &rect->h) != 4) {
			LOG_ERROR(ErrorType_ERROR, "ligne %ld, Mauvais parametre pour definir le rectangle\n", line);
			return false;
		}
		return Data_new(ID_DEFAULT, nomVariable, RECT_TYPE, rect, NULL);
	} else if (stringCompare(str, "fcircle(", 8)) {
		FCIRCLE *circle = malloc(sizeof(FCIRCLE));
		if (sscanf(str, "fcircle(%f,%f,%f)", &circle->x, &circle->y, &circle->radius) != 3) {
			LOG_ERROR(ErrorType_ERROR, "ligne %ld, Mauvais parametre pour definir le cercle\n", line);
			return false;
		}
		return Data_new(ID_DEFAULT, nomVariable, FLOAT_CIRCLE_TYPE, circle, NULL);
	} else if (stringCompare(str, "frect(", 6)) {
		SDL_FRect *rect = malloc(sizeof(SDL_FRect));
		if (sscanf(str, "frect(%f,%f,%f,%f)", &rect->x, &rect->y, &rect->w, &rect->h) != 4) {
			LOG_ERROR(ErrorType_ERROR, "ligne %ld, Mauvais parametre pour definir le cercle\n", line);
			return false;
		}
		return Data_new(ID_DEFAULT, nomVariable, FLOAT_RECT_TYPE, rect, NULL);
	} else if (str[0] == '"') {
		j = 0;
		while(str[j+1] != '\0' && str[j+1] != '\n' && str[j+1] != ';') {
			if (str[j+1] == '"' && str[j] != '\\') break;
			valeurVariable[j] = str[j+1];
			j++;
		}
		valeurVariable[j] = '\0';
		LOG_DEBUG("La variable %s est une chaine de caractères: %s\n", nomVariable, valeurVariable);
		return Data_new(ID_DEFAULT, nomVariable, STRING_TYPE, strdup(valeurVariable), NULL);
	} else {
		j = 0;
		for (i = 0; i < strlen(str); i++) {
			c = str[i];
			if (c == 'u') {
				if (type == NIL_TYPE) type = UINT32_TYPE;
				else if (type == SINT16_TYPE) type = UINT16_TYPE;
				else if (type == SINT8_TYPE) type = UINT8_TYPE;
				else if (type == UINT64_TYPE) type = UINT64_TYPE;
				else {
					LOG_ERROR(ErrorType_ERROR, "ligne %ld, Le typage doit etre defini avant la valeur\n", line);
					return NULL;
				}
			} else if (c == 'h') {
				if (type == NIL_TYPE) type = SINT16_TYPE;
				else if (type == SINT16_TYPE) type = SINT8_TYPE;
				else {
					LOG_ERROR(ErrorType_ERROR, "ligne %ld, Le typage doit etre defini avant la valeur\n", line);
					return NULL;
				}
			} else if (c == 'l') {
				if (type == NIL_TYPE) type = SINT64_TYPE;
			} else if (c == 'f') {
				if (type == NIL_TYPE) type = FLOAT_TYPE;
				else if (type == SINT64_TYPE) type = DOUBLE_TYPE;
				else {
					LOG_ERROR(ErrorType_ERROR, "ligne %ld, Le typage doit etre defini avant la valeur\n", line);
					return NULL;
				}
			} else if (c == '.') {
				if (type == NIL_TYPE) {
					type = FLOAT_TYPE;
					estUnNombre = true;
					valeurVariable[j] = '0';
					valeurVariable[j+1] = '.';
					decimal = true;
					j+=2;
				} else if (type == SINT32_TYPE || (type == FLOAT_TYPE && decimal == false)) {
					type = FLOAT_TYPE;
					valeurVariable[j] = '.';
					j++;
				} else {
					LOG_ERROR(ErrorType_ERROR, "ligne %ld, Un flottant ne comporte qu'un point\n", line);
					return NULL;
				}
			} else if (c == '-') {
				if (type == NIL_TYPE && j == 0 && negatif == false) {
					type = SINT32_TYPE;
					estUnNombre = true;
					negatif = true;
					valeurVariable[j] = '-';
					j++;
				} else {
					LOG_ERROR(ErrorType_ERROR, "ligne %ld, Mauvaise position du signe\n", line);
					return NULL;
				}	
			} else if (estUnChiffre(c)) {
				if (type == NIL_TYPE) { type = SINT32_TYPE; estUnNombre = true; }
				else if (!estUnNombre && type >= UINT8_TYPE && type <= DOUBLE_TYPE) estUnNombre = true;
				if (estUnNombre) {
					valeurVariable[j] = c;
					j++;
				}
			}
		}
		valeurVariable[j] = '\0';
		
		char stringType[40];
		ConvertTypeToString(type, stringType);
		LOG_DEBUG("type de la variable : %s\n", stringType);
		LOG_DEBUG("valeur de la variable : %s\n", valeurVariable);

		if (estUnNombre) {
			return Data_new(ID_DEFAULT, nomVariable, type, StringToAllocValue(type, valeurVariable), NULL);
		}
		
		LOG_DEBUG("continue\n");
	}
	return NULL;
}

bool readBlock(FILE *file, ARRAY *array, int *line) {
    if (array != NULL && file != NULL) {
        int i;
        char c;
        char instruction[MAX_LINE_LENGTH];
        
        while (!feof(file)) {
            instruction[0] = '\0';
            for (i = 0; i < MAX_LINE_LENGTH - 1; i++) {
                c = fgetc(file);
                if (c != '\n' && c != EOF) {
                    if (!estUnEspace(c)) {
                        instruction[i] = c;
                    } else {
                        i--;
                    }
                } else {
                    instruction[i] = '\0';
                    break;
                }
            }
            
            // Vérifie si la ligne lue est une fermeture de bloc '}'
            if (instruction[0] == '}') {
                return true;
            }
            
            // Vérifie si la ligne n'est pas vide ou un commentaire
            if (instruction[0] != '\n' && instruction[0] != '\0' && instruction[0] != '#') {
                // Appel à readInstruction pour traiter l'instruction lue
                LOG_DEBUG("traiter l'instruction lue : %s\n", instruction);
                if (!readInstruction(file, instruction, array, line)) {
                    return false;
                }
            }
            
            (*line)++;
        }
        return true;
    } else {
        return false;
    }
}


bool Array_loadConfigurationFile(ARRAY *array, char *filename) {
	bool success = false;
	if (array != NULL) {
		char *filePath;
		FILE *file = NULL;

		if (strstr(filename, ".conf") == NULL) {
			LOG_ERROR(ErrorType_ERROR, "Le fichier de configuration doit être un fichier de type .conf");
			return false;
		}

		asprintf(&filePath, "./%s", filename);
		file = fopen(filePath, "r");
		if (file == NULL) {
			asprintf(&filePath, "../%s", filename);
			file = fopen(filePath, "r");
		}
		if (file == NULL) {
			LOG_ERROR(ErrorType_FILE_ACCESS,"Fichier de configuration \"%s\" introuvable", filename);
			return false;
		}

		int line = 0;
		LOG_DEBUG("Début du parsage du fichier\n");
		success = readBlock(file, array, &line);
		if (!success) LOG_ERROR(ErrorType_ERROR, "Echec du parsage du fichier\n");

		free(filePath);
		fclose(file);
	}
	return success;
}
