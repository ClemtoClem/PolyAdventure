#include "logger.h"
#include "utils_string.h"

LOGGER *_log = NULL;

Uint64 fsize(FILE *fp) {
	Uint64 prev = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	Uint64 sz = ftell(fp);
	fseek(fp, prev, SEEK_SET); //go back to where we were
	return sz;
}

char *strCurrentTime(void) {
	time_t tm;
	char *buf;
	struct tm *tm_info;

	if (!(buf = (char *)malloc(sizeof(char) * 9)))
		return NULL;

	tm = time(NULL);
	tm_info = localtime(&tm);
	strftime(buf, 9, "%H:%M:%S", tm_info);

	return buf;
}

char *strCurrentDate(void) {
	time_t tm;
	char *buf;
	struct tm *tm_info;

	if (!(buf = (char *)malloc(sizeof(char) * 9)))
		return NULL;

	tm = time(NULL);
	tm_info = localtime(&tm);
	strftime(buf, 9, "%m/%d/%y", tm_info);

	return buf;
}



void Logger_init(LogDisplayMode displayMode, const char *filename) {
	if (!_log) {
		_log = (LOGGER *) calloc(1, sizeof(LOGGER));

		_log->displayMode    = displayMode;
		_log->maxFileSize    = MAX_FILE_SIZE;

		char *basename = NULL;
    	char *extension = NULL;
		split_filename(filename, &basename, &extension);
		if (extension != NULL) {
			strncpy(_log->filename, basename, SIZE_FILENAME);
			strncpy(_log->file_extension, extension, SIZE_FILENAME);
		} else {
			strncpy(_log->filename, filename, SIZE_FILENAME);
			strcpy(_log->file_extension, "txt");
		}
		if (basename) free(basename);
    	if (extension) free(extension);

		if (displayMode & LogDisplayMode_FILE) {
			if (!Logger_openFile())
				printf("\e[33mImpossible de créer le fichier _log (regardez si il reste de la place dans votre disque)\e[0m\n");
			else {
				char *date = strCurrentDate();
				fprintf(_log->output, "--------------------\n%s\n--------------------\n", date);
				free(date);
			}
		}
	}
}

void Logger_quit() {
	if (_log) {
		Logger_closeFile();
		free(_log);
	}
}

void Logger_enableDisplayMode(LogDisplayMode displayMode) {
	_log->displayMode |= displayMode;
	if ((displayMode & LogDisplayMode_FILE) && !Logger_fileIsOpen()) {
		if (Logger_openFile() == false)
			printf("\e[33mImpossible de créer le fichier _log (regardez si il reste de la place dans votre disque)\e[0m\n");
		else {
			char *date = strCurrentDate();
			fprintf(_log->output, "--------------------\n%s\n--------------------\n", date);
			free(date);
		}
	}
}

void Logger_enable(LogLevel level) {
	if (_log) {
		_log->disable &= ~level;
	}
}

void Logger_disable(LogLevel level) {
	if (_log) {
		_log->disable |= level;
	}
}

bool Logger_isEnable(LogLevel level) {
	if (_log) return !(_log->disable & level);
	else return false;
}

void Logger_setLevel(LogLevel level) {
	if (_log) {
		if (!(_log->disable & level))
			_log->last_level = level;
	}
}

LogLevel Logger_getLevel(void) {
	if (_log) return _log->last_level;
	else return LogLevel_NOTHING;
}

const char *Logger_getLevelStr(LogLevel level) {
	switch (level) {
		case LogLevel_TRACE:	return "TRACE";
		case LogLevel_DEBUG:	return "DEBUG";
		case LogLevel_INFO:		return "INFO";
		case LogLevel_SUCCESS:	return "SUCCESS";
		case LogLevel_WARNING:	return "WARNING";
		case LogLevel_ERROR:	return "ERROR";
		case LogLevel_FATAL:	return "FATAL";
		default: return "\0";
	}
}

const char *Logger_getErrorStr(ErrorType error) {
	switch (error) {
		case ErrorType_FAULT:			return "FAULT";
		case ErrorType_FILE_ACCESS:		return "FILE ACCESS";
		case ErrorType_TYPE:			return "TYPE";
		case ErrorType_NB_ARGUMENTS:	return "NB ARGUMENTS";
		case ErrorType_NB_RETURNS:		return "NB RETURNS";
		case ErrorType_TYPE_ARGUMENTS:	return "TYPE ARGUMENTS";
		case ErrorType_SDL:				return "SDL";
		default: return "\0";
	}
}

bool Logger_fileIsOpen() {
	if (_log && _log->output != NULL) return true;
	return false;
}

bool Logger_openFile() {
	bool success = true;
	if (_log && _log->displayMode & LogDisplayMode_FILE) {
		char *name;
		if (_log->output) fclose(_log->output);
		if (_log->idBackupFiles == 0)
			asprintf(&name, "%s.%s", _log->filename, _log->file_extension);
		else
			asprintf(&name, "%s.%lld.%s", _log->filename, _log->idBackupFiles, _log->file_extension);
		FILE *file = fopen(name, "a");
		if (file != NULL) { // if file exist
			if (fsize(file) >= MAX_FILE_SIZE) {
				_log->idBackupFiles++;
				fclose(file);
				success = Logger_openFile();
			} else {
				_log->output = file;
			}
		} else {
			LOG_ERROR(ErrorType_ERROR, "Impossible de créer le fichier log\n");
			success = false;
		}
		free(name);
	} else {
		success = false;
	}
	return success;
}

void Logger_closeFile() {
	if (_log && _log->output != NULL) {
		fclose(_log->output);
		_log->output = NULL;
	}
}



void Logger_log(LogLevel level, ErrorType errortype, const char* file, Sint32 line, const char* msg, ...) {
	if (_log) {
		if (!(_log->disable & level)) {
			va_list	valist;
			char *str_info, *str_args, *out_msg, *time;

			if (_log->last_level != LogLevel_NOTHING) _log->last_level = level;            
			time = strCurrentTime();

			#if defined(_WIN32) || defined(_WIN64)
			asprintf(&str_info, "[ %s ] %s:%d : ", time, file, line);
			#else
			asprintf(&str_info, "[ %s ] %s:%d : ", time, file, line);
			#endif

			va_start(valist, msg);
			vasprintf(&str_args, msg, valist);
			va_end(valist);
			
			asprintf(&out_msg, "%s%s", str_info, str_args);

			if (_log->displayMode & LogDisplayMode_CONSOLE) {
				switch (level) {
					case LogLevel_TRACE:	fprintf(stdout, "\e[34mTRACE: %s", out_msg); break;
					case LogLevel_DEBUG:	fprintf(stdout, "\e[34mDEBUG: %s", out_msg); break;
					case LogLevel_INFO:		fprintf(stdout, "\e[37mINFO: %s", out_msg); break;
					case LogLevel_SUCCESS:	fprintf(stdout, "\e[1;32mSUCCESS: %s", out_msg); break;
					case LogLevel_WARNING:	fprintf(stdout, "\e[1;33mWARNING: %s", out_msg); break;
					case LogLevel_ERROR:	fprintf(stdout, "\e[1;31mERROR %s: %s", Logger_getErrorStr(errortype), out_msg); break;
					case LogLevel_FATAL:	fprintf(stdout, "\e[1;35mFATAL: %s", out_msg); break;
					default:				fprintf(stdout, "%s", out_msg);
				}
				fprintf(stdout, "\e[0m");
			}
			if ((_log->displayMode & LogDisplayMode_FILE) && (_log->output != NULL)) {
				if (fsize(_log->output)+strlen(out_msg) >= MAX_FILE_SIZE) _log->idBackupFiles++, Logger_openFile();
				if (level != LogLevel_NOTHING) {
					fprintf(_log->output, "%s", Logger_getLevelStr(level));
					if (level == LogLevel_ERROR) fprintf(_log->output, "%s", Logger_getErrorStr(errortype));
					fprintf(_log->output, ": %s", out_msg);
				} else {
					fprintf(_log->output, "%s", out_msg);
				}
			}
			//if (_log->displayMode & LogDisplayMode_SCREEN)

			free(str_info);
			free(str_args);
			free(out_msg);
			free(time);
		}
	}
}