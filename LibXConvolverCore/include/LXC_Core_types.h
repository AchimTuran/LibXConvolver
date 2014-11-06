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



#include "LXC_CommonTypes.h"
#include "../LXC_fftHandles/LXC_fftHandles_types.h"

typedef enum {  LXC_OPT_MIN	=0,
				LXC_OPT_NATIVE,
				LXC_OPT_SSE2,
				LXC_OPT_SSE3,
				LXC_OPT_AVX,
				LXC_OPT_NEON,
				LXC_OPT_CUDA,
				LXC_OPT_OPENCL,
				LXC_OPT_MAX
} LXC_OPTIMIZATION_MODULE;

#define LXC_OPT_NATIVE_BIT		1<<(LXC_OPT_NATIVE -1)
#define LXC_OPT_SSE2_BIT		  1<<(LXC_OPT_SSE2	 -1)
#define LXC_OPT_SSE3_BIT		  1<<(LXC_OPT_SSE3	 -1)
#define LXC_OPT_AVX_BIT			  1<<(LXC_OPT_AVX		 -1)
#define LXC_OPT_NEON_BIT		  1<<(LXC_OPT_NEON	 -1)
#define LXC_OPT_CUDA_BIT		  1<<(LXC_OPT_CUDA	 -1)
#define LXC_OPT_OPENCL_BIT		1<<(LXC_OPT_OPENCL -1)

// ERROR CODES
#define LXC_NO_ERR								              0x00000000
#define LXC_ERR_INVALID_INPUT					          0x10000001
#define LXC_ERR_NOT_INIT                        0x10000002
#define LXC_ERR_RESAMPLER   					          0x10000030
#define LXC_ERR_FFTWF_PLAN_CREATION             0x10000040
#define LXC_ERR_FFTWF_WISDOM_EXPORT             0x10000041

#define LXC_ERR_DYNAMIC_MEMORY					        0x10000100
#define LXC_ERR_NULL_POINTER					          0x10000101

#define LXC_ERR_UNSUPPORTED_CONVOLVER			      0x10000200
#define LXC_ERR_UNSUPPORTED_MODULE_CALLBACKS    0x10000201

#define LXC_ERR_UNSUPPORTED_FFT_MODULE			    0x10000300

#define LXC_ERR_UNSUPPORTED_FFT_FMTC			      0x10000400


// WARNING CODES
#define LXC_WARN_SIZE									          0x10010000


// Input Data flags
#define LXC_CONSECUTIVE_SAMPLES 1<<1		// ch1_1, ch2_1, ..., chM_1, ch1_2, ch2_2, ..., chM_2, ...
#define LXC_SEPERATE_SAMPLES	1<<2		// ch1_1, ..., ch1_N, ... chM_1, ..., chM_N
#define LXC_DYNAMIC_STRUCT		1<<5		// struct was created with new or malloc

typedef float LXC_cpx_sseFloat[2];
typedef float LXC_cpx_cuFloat[2];
typedef float LXC_cpx_clFloat[2];
typedef float LXC_cpx_neonFloat[2];

#define LXC_NO_SCALE  1.0f

typedef struct
{
	char LXC_Core_APIVersionStr[1024];
	char LXC_Core_OptimizationStr[1024];
	uint LXC_Core_OptimizationBits;
} LXC_PROPERTIES;

typedef struct
{
	void	*buffer;						// specific buffer pointer
	uint	sampleFrequency;				// sample frequency in Hz
	uint	maxFilterParts;					// Maximum filter partitions.
	uint	maxFilterPartLength;			// Maximum filter partition length in samples.
	uint	maxFilterPartLength_NonZero;	// Maximum filter partition length in samples.
	uint	maxFilterLength;				// Maximum filter length in samples.
} LXC_BUFFER;


typedef struct
{
	void	*buffer;					// specific buffer pointer
	uint	sampleFrequency;			// sample frequency in Hz
	uint	maxElements;				// Maximum buffer elements.
	uint	maxPartLength;				// Maximum length of an element in sizeof(type).
	uint	maxPartLength_Half;			// Maximum filter partition length in samples.
	uint	currentElement;				// Latest position in the ringbuffer.
	char	isEmpty;					// 0>=isEmpty if it contains no data else 0<isEmpty
} LXC_RINGBUFFER;

typedef struct
{
	LXC_ERROR_CODE (*LXC_CpxMul)(uint Size, void *X, void *H, void *Y);
	LXC_ERROR_CODE (*LXC_CpxAdd)(LXC_BUFFER *ResultBuffer, float ScaleFactor);
	LXC_ERROR_CODE (*LXC_FreqCombine2Ch)(uint Size, void *X, void *Y, void *Z);
	LXC_ERROR_CODE (*LXC_FreqSplit2Ch)(uint Size, void *Z, void *X, void *Y);
} LXC_CALLBACKS;


typedef struct
{
	void*			(*LXC_Buffer_getPart)(LXC_BUFFER *Buffer, uint Part);
	LXC_ERROR_CODE	(*LXC_Buffer_create)(LXC_BUFFER *Buffer);
	LXC_ERROR_CODE	(*LXC_Buffer_destroy)(LXC_BUFFER *Buffer);
} LXC_BUFFER_CALLBACKS;

typedef struct
{
	void*			(*LXC_Ringbuffer_getPart)(LXC_RINGBUFFER *Ringbuffer, uint Element);
	void*			(*LXC_Ringbuffer_getNextPart)(LXC_RINGBUFFER *Ringbuffer);
	LXC_ERROR_CODE	(*LXC_Ringbuffer_create)(LXC_RINGBUFFER *Ringbuffer);
	LXC_ERROR_CODE	(*LXC_Ringbuffer_destroy)(LXC_RINGBUFFER *Ringbuffer);
} LXC_RINGBUFFER_CALLBACKS;

typedef struct
{
	int		LXC_module;					// Stores the used processing optimization module. For more details see LXC_OPTIMIZATION_IDS.
	uint	LXC_sampleFrequency;		// Stores the used sampling frequency.
	uint	LXC_maxChannels;			// Maximum input/output channels.  Only 1 or 2
	uint	LXC_maxInputFrameLength;	// Maximum input/output framelength in samples.
} LXC_CONFIG;


typedef struct
{
	LXC_BUFFER H1;
	LXC_BUFFER H2;
	LXC_RINGBUFFER IN_H1;
	LXC_RINGBUFFER IN_H2;
	LXC_BUFFER results_H1;
	LXC_BUFFER results_H2;

	uint LXC_maxChannels;

	// callbacks for buffer processing
	LXC_BUFFER_CALLBACKS LXC_bufferCallbacks;
	LXC_RINGBUFFER_CALLBACKS LXC_ringbufferCallbacks;
} LXC_PLAN;

typedef struct
{
	LXC_CALLBACKS	LXC_callbacks;
	LXC_CONFIG		LXC_config;
	LXC_FFT_HANDLE	LXC_fftHandle;
	LXC_PLAN		LXC_Handle;
} LXC_HANDLE;

typedef struct
{
	uint	sampleFrequency;		// Stores the used sampling frequency.
	uint	maxFilterLength_h1;		// Maximum filter length in samples.
	uint	maxFilterLength_h2;		// Maximum filter length in samples.
} LXC_FILTER_CONFIG;

typedef struct
{
	float	*h1;
	float	*h2;
	LXC_FILTER_CONFIG LXC_filterConfig;
} LXC_FILTER_HANDLE_CH;

typedef void* LXC_ptrFilterHandle;