
/*	
 *		Copyright (C) 2014 Achim Turan, Achim.Turan@o2online.de
 *		https://github.com/AchimTuran/LibXConvolver
 *
 *	This file is part of LibXConvolver
 *
 *	LibXConvolver is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	LibXConvolver is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with LibXConvolver.  If not, see <http://www.gnu.org/licenses/>.
 *
 */



#include <time.h>

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "LXC_Logging.h"

FILE *g_loggingFilePtr = NULL;
char g_FallbackToPrintf = 0;

void LXC_LoggingFile_open()
{
#if defined(LXC_LOGGING_FILE_NAME)
	g_loggingFilePtr = fopen(LXC_LOGGING_FILE_NAME, "w");
#else
	g_loggingFilePtr = fopen("LXC.log", "w");
#endif

	if(!g_loggingFilePtr)
	{
		g_FallbackToPrintf = 1;
	}
}

void LXC_LoggingFile(const char* Tag, const char* Message, ...)
{
	time_t timeNow;
	time(&timeNow);
	struct tm calendarTime;
	char timeBuf[MAX_LOGGING_TIME_SYMBOLS];
	localtime_s(&calendarTime, &timeNow);

	// Time format: Dayname Day. Month. Year, HH:MM:SS
	// for details see http://www2.hs-fulda.de/~klingebiel/c-stdlib/time.htm
	strftime(timeBuf, MAX_LOGGING_TIME_SYMBOLS, "%a %d. %b. %Y, %H:%M:%S", &calendarTime);
	if(g_FallbackToPrintf)
	{
		va_list variadicArgs;
		va_start(variadicArgs, Message);
		LXC_LoggingPrintf(Tag, Message, variadicArgs);
		va_end(variadicArgs);
	}
	else
	{
		if(!g_loggingFilePtr)
		{
			LXC_LoggingFile_open();
		}

		if(g_loggingFilePtr)
		{
			va_list variadicArgs;
			va_start(variadicArgs, Message);
			fprintf(g_loggingFilePtr, "%s %s", timeBuf, Tag);
			vfprintf(g_loggingFilePtr, Message, variadicArgs);
			fprintf(g_loggingFilePtr, "\n");
			va_end(variadicArgs);
		}
		else
		{
			g_FallbackToPrintf = 1;
			va_list variadicArgs;
			va_start(variadicArgs, Message);
			LXC_LoggingPrintf(Tag, Message, variadicArgs);
			va_end(variadicArgs);
		}
	}
}

void LXC_LoggingFile_close()
{
	if(g_loggingFilePtr)
	{
		if(fclose(g_loggingFilePtr) != 0)
		{
			// ToDo: show some error message
		}

		g_loggingFilePtr = NULL;
	}
}