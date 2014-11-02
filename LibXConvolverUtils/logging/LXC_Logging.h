#pragma once

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



#include "LXC_Logging_config.h"

#if defined(USE_LXC_LOGGING_PRINTF)
	void LXC_LoggingPrintf_close();
	void LXC_LoggingPrintf_open();
	void LXC_LoggingPrintf(const char* Tag, const char* Message, ...);
	#define LXC_LOG_DEBUG(Message, ...) LXC_LoggingPrintf(LXC_LOGGING_DEBUG_STR, Message, ## __VA_ARGS__)
	#define LXC_LOG_INFO(Message, ...) LXC_LoggingPrintf(LXC_LOGGING_INFO_STR, Message, ## __VA_ARGS__)
	#define LXC_LOG_WARNING(Message, ...) LXC_LoggingPrintf(LXC_LOGGING_WARNING_STR, Message, ## __VA_ARGS__)
	#define LXC_LOG_ERROR(Message, ...) LXC_LoggingPrintf(LXC_LOGGING_ERROR_STR, Message, ## __VA_ARGS__)
	#define LXC_LOG_CLOSE()
	#define LXC_LOG_OPEN()
#elif defined(USE_LXC_LOGGING_FILE)
	void LXC_LoggingPrintf(const char* Tag, const char* Message, ...);
	void LXC_LoggingFile(const char* Tag, const char* Message, ...);
	void LXC_LoggingFile_close();
	void LXC_LoggingFile_open();
	#define LXC_LOG_DEBUG(Message, ...) LXC_LoggingFile(LXC_LOGGING_DEBUG_STR, Message, ## __VA_ARGS__)
	#define LXC_LOG_INFO(Message, ...) LXC_LoggingFile(LXC_LOGGING_INFO_STR, Message, ## __VA_ARGS__)
	#define LXC_LOG_WARNING(Message, ...) LXC_LoggingFile(LXC_LOGGING_WARNING_STR, Message, ## __VA_ARGS__)
	#define LXC_LOG_ERROR(Message, ...) LXC_LoggingFile(LXC_LOGGING_ERROR_STR, Message, ## __VA_ARGS__)
	#define LXC_LOG_CLOSE() LXC_LoggingFile_close()
	#define LXC_LOG_OPEN()	LXC_LoggingFile_open()
#else
	#define LXC_LOG_DEBUG(Message, ...)
	#define LXC_LOG_INFO(Message, ...)
	#define LXC_LOG_WARNING(Message, ...)
	#define LXC_LOG_ERROR(Message, ...)
	#define LXC_LOG_CLOSE()
	#define LXC_LOG_OPEN()
#endif