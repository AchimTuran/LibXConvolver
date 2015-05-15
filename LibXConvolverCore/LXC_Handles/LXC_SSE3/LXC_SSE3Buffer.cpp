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

#if defined(TARGET_LINUX)
  #include <mm_malloc.h>
#endif

#include "LXC_SSE3Buffer.h"
#include "LXC_SSE3_types.h"
#include "../../include/LXC_Core_helper.h"

void* LXC_SSE3Buffer_getPart(LXC_BUFFER *Buffer, uint Part)
{
	if(!Buffer || !Buffer->buffer || Part >= Buffer->maxFilterParts)
	{
		LXC_Core_setLastError(LXC_ERR_INVALID_INPUT);
		return NULL;
	}

	LXC_SSE3cpxFloat *p = (LXC_SSE3cpxFloat*)(Buffer->buffer);
	
	return (void*)(p + Part*Buffer->maxFilterPartLength);
}


LXC_ERROR_CODE LXC_SSE3Buffer_create(LXC_BUFFER *Buffer)
{
	if(!Buffer || !Buffer->maxFilterPartLength || !Buffer->maxFilterParts)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	const uint maxElements = Buffer->maxFilterPartLength*Buffer->maxFilterParts;
#if defined(TARGET_WINDOWS)
	LXC_SSE3cpxFloat *p	= (LXC_SSE3cpxFloat*)_aligned_malloc(sizeof(LXC_SSE3cpxFloat)*maxElements, LXC_SSE3_ALIGN);
#elif defined(TARGET_LINUX)
	LXC_SSE3cpxFloat *p = (LXC_SSE3cpxFloat*)_mm_malloc(sizeof(LXC_SSE3cpxFloat)*maxElements, LXC_SSE3_ALIGN);
#endif
	// http://stackoverflow.com/questions/21328985/sse-reinterpret-cast-m128-instead-of-mm-load-ps
	// float *C = _mm_malloc(size * sizeof(*C), 16); 
	// or
	// float *C = _aligned_malloc(size * sizeof(*C), 16); 
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


LXC_ERROR_CODE LXC_SSE3Buffer_destroy(LXC_BUFFER *Buffer)
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
		_mm_free(Buffer->buffer);
		Buffer->buffer = NULL;
	}


	return LXC_NO_ERR;
}
