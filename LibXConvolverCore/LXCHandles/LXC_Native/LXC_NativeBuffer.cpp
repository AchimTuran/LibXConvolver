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



#include <stdio.h>
#include <malloc.h>

#include "LXC_NativeBuffer.h"
#include "../../include/LXC_Core_helper.h"

void* LXC_NativeBuffer_getPart(LXC_BUFFER *Buffer, uint Part)
{
	if(!Buffer || !Buffer->buffer || Part >= Buffer->maxFilterParts)
	{
		LXC_Core_setLastError(LXC_ERR_INVALID_INPUT);
		return NULL;
	}

	LXC_cpxFloat *p = (LXC_cpxFloat*)(Buffer->buffer);
	
	return (void*)(p + Part*Buffer->maxFilterPartLength);
}


LXC_ERROR_CODE LXC_NativeBuffer_create(LXC_BUFFER *Buffer)
{
	if(!Buffer || !Buffer->maxFilterPartLength || !Buffer->maxFilterParts)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	const uint maxElements = Buffer->maxFilterPartLength*Buffer->maxFilterParts;
	LXC_cpxFloat *p = (LXC_cpxFloat*)malloc(sizeof(LXC_cpxFloat)*maxElements);
	if(!p)
	{
		// reset the state of the buffer handle
		Buffer->maxFilterLength = 0;
		Buffer->maxFilterPartLength = 0;
		Buffer->maxFilterPartLength_NonZero = 0;
		Buffer->maxFilterParts = 0;
		Buffer->sampleFrequency = 0;
		Buffer->buffer = NULL;

		return LXC_ERR_DYNAMIC_MEMORY;
	}

	for(uint ii=0; ii < maxElements; ii++)
	{
		p[ii][0] = 0.0f;
		p[ii][1] = 0.0f;
	}

	Buffer->buffer = (void*)p;

	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_NativeBuffer_destroy(LXC_BUFFER *Buffer)
{
	if(!Buffer)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	// reset the state of the buffer handle
	Buffer->maxFilterLength = 0;
	Buffer->maxFilterPartLength = 0;
	Buffer->maxFilterPartLength_NonZero = 0;
	Buffer->maxFilterParts = 0;
	Buffer->sampleFrequency = 0;

	if(Buffer->buffer)
	{
		free(Buffer->buffer);
		Buffer->buffer = NULL;
	}


	return LXC_NO_ERR;
}