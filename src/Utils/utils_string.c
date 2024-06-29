#include "utils_string.h"

char **split(int *occurrences, const char *str, char c) {
    if (!str || !occurrences) {
        return NULL;
    }

    int i = 0;
    *occurrences = 0;

    // Compter les occurrences du caractère de délimitation
    for (i = 0; str[i]; i++) {
        if (str[i] == c) {
            (*occurrences)++;
        }
    }
    // Ajouter une occurrence pour le dernier segment (ou pour une chaîne vide)
    (*occurrences)++;

    // Allouer de la mémoire pour le tableau de pointeurs de chaînes
    char **words = (char **)malloc(sizeof(char *) * (*occurrences));
    if (!words) {
        return NULL;
    }

    const char *start = str;
    int index = 0;

    // Parcourir la chaîne et extraire les segments
    for (i = 0; i <= strlen(str); i++) {
        if (str[i] == c || str[i] == '\0') {
            int length = &str[i] - start;
            words[index] = (char *)malloc(length + 1);
            if (!words[index]) {
                // Libérer la mémoire allouée en cas d'erreur
                for (int j = 0; j < index; j++) {
                    free(words[j]);
                }
                free(words);
                return NULL;
            }
            strncpy(words[index], start, length);
            words[index][length] = '\0'; // Null-terminate the string
            index++;
            start = &str[i + 1];
        }
    }

    return words;
}

void free_split(char **words, int count) {
    for (int i = 0; i < count; i++) {
        free(words[i]);
    }
    free(words);
}

void split_filename(const char *filename, char **basename, char **extension) {
    // Trouver la position du dernier point dans le nom du fichier
    const char *dot = strrchr(filename, '.');

    if (!dot || dot == filename) {
        // Pas d'extension trouvée ou point au début du nom de fichier
        *basename = strdup(filename);
        *extension = NULL;
    } else {
        // Copier le nom de base
        size_t basename_length = dot - filename;
        *basename = (char *)malloc(basename_length + 1);
        if (*basename) {
            strncpy(*basename, filename, basename_length);
            (*basename)[basename_length] = '\0'; // Ajouter le caractère de fin de chaîne
        }

        // Copier l'extension
        *extension = strdup(dot + 1);
    }
}