#include "Logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

void WriteLog(const char* format, ...)
{
	FILE* file = NULL;

	if (fopen_s(&file, "NikonCapture.log", "a") != 0 || file == NULL)
	{
		return;
	}

	time_t now = time(NULL);
	struct tm localTime;

	localtime_s(&localTime, &now);

	fprintf(
		file,
		"[%04d-%02d-%02d %02d:%02d:%02d] ",
		localTime.tm_year + 1900,
		localTime.tm_mon + 1,
		localTime.tm_mday,
		localTime.tm_hour,
		localTime.tm_min,
		localTime.tm_sec
	);

	va_list args;
	va_start(args, format);
	vfprintf(file, format, args);
	va_end(args);

	fprintf(file, "\n");

	fclose(file);
}