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



#include <malloc.h>
#include <stdio.h>

#include "LXC_SSE3_types.h"
#include "LXC_SSE3RingBuffer.h"

#include <intrin.h>		//types
#include <mmintrin.h>	//SSE Intrinsics

void* LXC_SSE3Ringbuffer_getPart(LXC_RINGBUFFER *Ringbuffer, uint Element)
{
	if(!Ringbuffer || !Ringbuffer->buffer || Element >= Ringbuffer->maxElements || Ringbuffer->isEmpty)
	{
		return NULL;
	}

	LXC_SSE3cpxFloat *p = (LXC_SSE3cpxFloat*)Ringbuffer->buffer;


	int tempIndex = (int)Ringbuffer->currentElement - (int)Element;
	if( tempIndex < 0 )
	{
		tempIndex = (int)Ringbuffer->currentElement + (int)Ringbuffer->maxElements - (int)Element;
	}

	return (void*)(p + tempIndex*Ringbuffer->maxPartLength);
}

void* LXC_SSE3Ringbuffer_getNextPart(LXC_RINGBUFFER *Ringbuffer)
{
	if(!Ringbuffer || !Ringbuffer->buffer || !Ringbuffer->maxElements)
	{
		return NULL;
	}

	// update the write index from the CFFTWCpxRingBuffer
	(Ringbuffer->currentElement)++;
	if(Ringbuffer->currentElement >= Ringbuffer->maxElements)
	{
		Ringbuffer->currentElement = 0;
	}
	
	LXC_SSE3cpxFloat *p = (LXC_SSE3cpxFloat*)Ringbuffer->buffer;

	if(Ringbuffer->isEmpty)
	{
		Ringbuffer->isEmpty = 0;
	}

	return (void*)(p + Ringbuffer->currentElement*Ringbuffer->maxPartLength);
}


LXC_ERROR_CODE LXC_SSE3Ringbuffer_create(LXC_RINGBUFFER *Ringbuffer)
{
	if(!Ringbuffer || !Ringbuffer->sampleFrequency || !Ringbuffer->maxPartLength || !Ringbuffer->maxElements)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	const uint maxElements = Ringbuffer->maxPartLength*Ringbuffer->maxElements;
		
	LXC_SSE3cpxFloat *p	= (LXC_SSE3cpxFloat*)_aligned_malloc(sizeof(LXC_SSE3cpxFloat)*maxElements, LXC_SSE3_ALIGN);
	// http://stackoverflow.com/questions/21328985/sse-reinterpret-cast-m128-instead-of-mm-load-ps
	// float *C = _mm_malloc(size * sizeof(*C), 16); 
	// or
	// float *C = _aligned_malloc(size * sizeof(*C), 16); 
	if(!p)
	{
		// reset the state of the buffer handle
		Ringbuffer->buffer				= NULL;
		Ringbuffer->sampleFrequency		= 0;
		Ringbuffer->maxElements			= 0;
		Ringbuffer->maxPartLength		= 0;
		Ringbuffer->maxPartLength_Half	= 0;
		Ringbuffer->currentElement		= 0;
		Ringbuffer->isEmpty				= 1;

		return LXC_ERR_DYNAMIC_MEMORY;
	}

	for(uint ii=0; ii < maxElements; ii++)
	{
		p[ii][0] = 0.0f;
		p[ii][1] = 0.0f;
	}

	// set this to max buffer pointer --> so we get the first element when we write data to the buffer
	Ringbuffer->currentElement = Ringbuffer->maxElements;
	Ringbuffer->buffer = (void*)p;

	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_SSE3Ringbuffer_destroy(LXC_RINGBUFFER *Ringbuffer)
{
	if(!Ringbuffer)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	// reset the state of the buffer handle
	Ringbuffer->sampleFrequency		= 0;
	Ringbuffer->maxElements			= 0;
	Ringbuffer->maxPartLength		= 0;
	Ringbuffer->maxPartLength_Half	= 0;
	Ringbuffer->currentElement		= 0;


	if(Ringbuffer->buffer)
	{
		_mm_free(Ringbuffer->buffer);
		Ringbuffer->buffer = NULL;
	}


	return LXC_NO_ERR;
}