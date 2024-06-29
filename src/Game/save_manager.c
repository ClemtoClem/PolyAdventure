#include "save_manager.h"

SAVE_MANAGER *saveManager = NULL;

bool SaveFile_init() {
	bool success = false;
	if (saveManager == NULL) {
		saveManager = (SAVE_MANAGER *) calloc(1, sizeof(SAVE_MANAGER));
		saveManager->listPseudo = Array_new(NO_LIMITED);
		success = true;
	} else {
		success = true;
	}
	return success;
}

void SaveFile_free() {
	if (saveManager != NULL) {
		if (saveManager->listPseudo != NULL) {
			Array_free(saveManager->listPseudo, true);
		}
		if (saveManager->currentSaveFile != NULL) SaveFile_close();
		free(saveManager);
		saveManager = NULL;
	}
}

SAVE_MANAGER *SaveFile_get() {
	return saveManager;
}

void SaveFile_makeListPseudo() {
	if (saveManager != NULL) {
		FILE *file = NULL;
		char *fileName, pseudo[30];
		do {
			asprintf(&fileName, "res/data/save%ld.data", saveManager->listPseudo->length+1);
			FILE *file = fopen(fileName, "rb");
			if (file == NULL) {
				asprintf(&fileName, "../res/data/save%ld.data", saveManager->listPseudo->length+1);
				file = fopen(fileName, "rb");
			}
			if (file != NULL) {
				fscanf(file, "%s\n", pseudo);
				Array_appendString(saveManager->listPseudo, pseudo);
				fclose(file);
				saveManager->currentSaveFile++;
			}
		} while (file != NULL);
		free(fileName);
	}
}

Uint16 nbSaveFile() {
	if (saveManager != NULL)
		return (Uint16) saveManager->listPseudo->length;
	return 0;
}

void SaveFile_create() {
	if (saveManager != NULL) {


		saveManager->idCurrentSaveFile ++;
	}
}

void SaveFile_open(Uint16 id) {
	if (saveManager != NULL) {
		if (saveManager->currentSaveFile == NULL && id < saveManager->idCurrentSaveFile) {
			char fileName[30];
			sprintf(fileName, "res/data/save%d.data", id);
			saveManager->currentSaveFile = fopen(fileName, "rb+");
			if (saveManager->currentSaveFile == NULL) {
				sprintf(fileName, "../res/data/save%d.data", id);
				saveManager->currentSaveFile = fopen(fileName, "rb+");
			}
			if (saveManager->currentSaveFile != NULL) {
				saveManager->idCurrentSaveFile = id;
			}
		}
		fseek(saveManager->currentSaveFile, SEEK_SET, 0);
	}
}

void SaveFile_close() {
	if (saveManager != NULL && saveManager->currentSaveFile != NULL) {
		fclose(saveManager->currentSaveFile);
		saveManager->idCurrentSaveFile = 0;
		saveManager->currentSaveFile = NULL;
	}
}

void SaveFile_load() {
	if (saveManager != NULL && saveManager->currentSaveFile != NULL) {

	}
}

void SaveFile_save() {
	if (saveManager != NULL && saveManager->currentSaveFile != NULL) {

	}
}