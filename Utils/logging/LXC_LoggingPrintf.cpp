
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

void LXC_LoggingPrintf(const char* Tag, const char* Message, ...)
{
	va_list variadicArgs;
	va_start(variadicArgs, Message);

	time_t timeNow;
	time(&timeNow);
	struct tm calendarTime;
	char timeBuf[MAX_LOGGING_TIME_SYMBOLS];
	localtime_s(&calendarTime, &timeNow);

	// Time format: Dayname Day. Month. Year, HH:MM:SS
	// for details see http://www2.hs-fulda.de/~klingebiel/c-stdlib/time.htm
	strftime(timeBuf, MAX_LOGGING_TIME_SYMBOLS, "%a %d. %b. %Y, %H:%M:%S", &calendarTime);

	printf("%s %s", timeBuf, Tag);
	vprintf(Message, variadicArgs);
	printf("\n");

	va_end(variadicArgs);
}