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
#include <string.h>

#include "include/LXC_Core.h"
#include "include/LXC_Core_config.h"
#include "fftHandles/fftHandles.h"
#include "include/LXC_Core_helper.h"

#if defined(USE_LXC_NATIVE)
	#include "LXCHandles/LXC_Native/LXC_Native.h"
#endif
#if defined(USE_LXC_SSE3)
	#include "LXCHandles/LXC_SSE3/LXC_SSE3.h"
#endif

#include "../LibXConvolverUtils/logging/LXC_Logging.h"

const char LXC_CoreAPIVersionStr[1024] = LXC_CORE_API_VERSION_STR;
char LXC_Core_OptimizationStr[1024];
uint LXC_Core_OptimizationBits;
LXC_ERROR_CODE LXC_Core_status = 0;
LXC_ERROR_CODE g_LXC_Core_lastError = LXC_NO_ERR;
char *g_LXC_HomePath = NULL;

// ------------------------------------
// Helper functions
// ------------------------------------
LXC_ERROR_CODE LXC_Core_getOptimizationModule(LXC_HANDLE *LXCHandle, LXC_OPTIMIZATION_MODULE LXC_module);
LXC_ERROR_CODE LXC_Core_storeFilter(LXC_HANDLE *LXCHandle, LXC_FILTER_HANDLE_CH *Filter);
void LXC_Core_clearHandle(LXC_HANDLE *LXCHandle);
void LXC_Core_clearRingbuffer(LXC_RINGBUFFER *Ringbuffer);
void LXC_Core_clearBuffer(LXC_BUFFER *Buffer);
LXC_ERROR_CODE LXC_Core_initializeFilter( LXC_BUFFER_CALLBACKS *BufferCallbacks, 
										  LXC_RINGBUFFER_CALLBACKS *RingbufferCallbacks,
										  LXC_FFT_CALLBACKS *fftCallbacks,
										  LXC_FFT_HANDLE *fftHandle,
										  LXC_BUFFER *FilterBuffer,
										  LXC_BUFFER *ResultBuffer,
										  LXC_RINGBUFFER *InBuffer,
										  float *h,
										  uint MaxFilterLength,
										  uint SampleFrequency,
										  float *TempSamples );


LXC_ERROR_CODE LXC_Core_init(const char* HomePath)
{
  if (!HomePath)
  {
    return LXC_ERR_INVALID_INPUT;
  }

  size_t strHomeLen = strlen(HomePath);
  strHomeLen++;
  g_LXC_HomePath = (char*)malloc(sizeof(char)*strHomeLen);
  if (!g_LXC_HomePath)
  {
    return LXC_ERR_DYNAMIC_MEMORY;
  }
  strncpy(g_LXC_HomePath, HomePath, strHomeLen);

  strcat(LXC_Core_OptimizationStr, "");
	LXC_Core_OptimizationBits = 0;

	LXC_LOG_INFO("Get LXC module properties.");

#if defined(USE_LXC_NATIVE)
	strcat(LXC_Core_OptimizationStr, LXC_OPT_NATIVE_STR);
	LXC_LOG_INFO("Set %s properties.", LXC_OPT_NATIVE_STR);
	LXC_Core_OptimizationBits = LXC_OPT_NATIVE_BIT;
#endif

#if defined(USE_LXC_SSE3)
	strcat(LXC_Core_OptimizationStr, LXC_OPT_SSE3_STR);
	LXC_LOG_INFO("Set %s properties.", LXC_OPT_SSE3_STR);
	LXC_Core_OptimizationBits = LXC_OPT_SSE3_BIT;
#endif

#if defined(USE_LXC_CUDA)
	strcat(LXC_Core_OptimizationStr, ", ");
	strcat(LXC_Core_OptimizationStr, LXC_OPT_CUDA_STR);
	LXC_LOG_INFO("Set %s properties.", LXC_OPT_CUDA_STR);
	LXC_Core_OptimizationBits |= LXC_OPT_CUDA_BIT;
#endif

	LXC_Core_status = 1;

	return LXC_NO_ERR;
}

LXC_ERROR_CODE LXC_Core_close()
{
	LXC_LOG_DESTROY();

  if (g_LXC_HomePath)
  {
    free(g_LXC_HomePath);
    g_LXC_HomePath = NULL;
  }
	
	return LXC_NO_ERR;
}

LXC_ERROR_CODE LXC_Core_getProperties(LXC_PROPERTIES *LXC_properties)
{
	if(!LXC_properties)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	LXC_ERROR_CODE err;
	if(!LXC_Core_status)
	{
		// ToDo: show some warning message
		err = LXC_Core_init();
		if(err != LXC_NO_ERR)
		{
			return err;
		}
	}

	strcpy(LXC_properties->LXC_Core_APIVersionStr, LXC_CoreAPIVersionStr);
	strcpy(LXC_properties->LXC_Core_OptimizationStr, LXC_Core_OptimizationStr);
	LXC_properties->LXC_Core_OptimizationBits = LXC_Core_OptimizationBits;

	return LXC_NO_ERR;
}

LXC_ptrFilterHandle* LXC_Core_createFilter(float *h, uint Size_h, uint SampleFreq)
{
	if(!h || !Size_h || !SampleFreq)
	{
		LXC_Core_setLastError(LXC_ERR_INVALID_INPUT);
		return NULL;
	}
	const uint size = Size_h;
	
	LXC_FILTER_HANDLE_CH *lxc_filterHandle = (LXC_FILTER_HANDLE_CH*)malloc(sizeof(LXC_FILTER_HANDLE_CH));
	if(!lxc_filterHandle)
	{
		LXC_Core_setLastError(LXC_ERR_DYNAMIC_MEMORY);
		return NULL;
	}

	lxc_filterHandle->h1 = (float*)malloc(sizeof(float)*size);
	if(!lxc_filterHandle->h1)
	{
		free(lxc_filterHandle);
		LXC_Core_setLastError(LXC_ERR_DYNAMIC_MEMORY);
		return NULL;
	}
	lxc_filterHandle->h2 = NULL;

	// copy filter config and samples into the filter handle
	lxc_filterHandle->LXC_filterConfig.maxFilterLength_h1 = size;
	lxc_filterHandle->LXC_filterConfig.maxFilterLength_h2 = 0;
	lxc_filterHandle->LXC_filterConfig.sampleFrequency = SampleFreq;
	for(uint ii=0; ii < size; ii++)
	{
		lxc_filterHandle->h1[ii] = h[ii];
	}

	return (LXC_ptrFilterHandle*)lxc_filterHandle;
}


LXC_ptrFilterHandle* LXC_Core_createFilter2Ch(float *h1, uint Size_h1, float *h2, uint Size_h2, uint SampleFreq)
{
	if(!h1 || !Size_h1 || !h2 || !Size_h2 || !SampleFreq)
	{
		LXC_Core_setLastError(LXC_ERR_INVALID_INPUT);
		return NULL;
	}
	
	LXC_FILTER_HANDLE_CH *lxc_filterHandle = (LXC_FILTER_HANDLE_CH*)malloc(sizeof(LXC_FILTER_HANDLE_CH));
	if(!lxc_filterHandle)
	{
		LXC_Core_setLastError(LXC_ERR_DYNAMIC_MEMORY);
		return NULL;
	}

	lxc_filterHandle->h1 = (float*)malloc(sizeof(float)*Size_h1);
	if(!lxc_filterHandle->h1)
	{
		free(lxc_filterHandle);
		LXC_Core_setLastError(LXC_ERR_DYNAMIC_MEMORY);
		return NULL;
	}

	lxc_filterHandle->h2 = (float*)malloc(sizeof(float)*Size_h2);
	if(!lxc_filterHandle->h2)
	{
		free(lxc_filterHandle->h1);
		free(lxc_filterHandle);
		LXC_Core_setLastError(LXC_ERR_DYNAMIC_MEMORY);
		return NULL;
	}

	// copy filter config and samples into the filter handle
	lxc_filterHandle->LXC_filterConfig.maxFilterLength_h1 = Size_h1;
	lxc_filterHandle->LXC_filterConfig.maxFilterLength_h2 = Size_h2;
	lxc_filterHandle->LXC_filterConfig.sampleFrequency = SampleFreq;
	for(uint ii=0; ii < Size_h1; ii++)
	{
		lxc_filterHandle->h1[ii] = h1[ii];
	}

	for(uint ii=0; ii < Size_h2; ii++)
	{
		lxc_filterHandle->h2[ii] = h2[ii];
	}

	return (LXC_ptrFilterHandle*)lxc_filterHandle;
}

LXC_HANDLE* LXC_Core_getConvolver(LXC_ptrFilterHandle *Filter, uint InputFrameLength, LXC_OPTIMIZATION_MODULE LXC_module, LXC_FFT_MODULE LXC_fftModule)
{
	LXC_ERROR_CODE err = LXC_NO_ERR;

	if(!Filter || !InputFrameLength)
	{
		LXC_Core_setLastError(LXC_ERR_INVALID_INPUT);
		return NULL;
	}

	LXC_HANDLE *LXCHandle = (LXC_HANDLE*)malloc(sizeof(LXC_HANDLE));
	if(!LXCHandle)
	{
		LXC_Core_setLastError(LXC_ERR_DYNAMIC_MEMORY);
		return NULL;
	}
	LXC_Core_clearHandle(LXCHandle);

	// get fft module
	err = LXC_get_fftHandle(&(LXCHandle->fftHandle), LXC_fftModule, LXC_module, InputFrameLength*2, InputFrameLength);
	if(err != LXC_NO_ERR)
	{
		LXCHandle->fftHandle.fftCallbacks.destroy_fft(&(LXCHandle->fftHandle.fftPlan));
		free(LXCHandle);
		LXC_Core_setLastError(err);
		return NULL;
	}

	// get LXC_module
	err = LXC_Core_getOptimizationModule(LXCHandle, LXC_module);
	if(err != LXC_NO_ERR)
	{
		LXCHandle->fftHandle.fftCallbacks.destroy_fft(&(LXCHandle->fftHandle.fftPlan));
		free(LXCHandle);
		LXC_Core_setLastError(err);
		return NULL;
	}

	LXCHandle->LXC_config.LXC_maxInputFrameLength = InputFrameLength;
	err = LXC_Core_storeFilter(LXCHandle, (LXC_FILTER_HANDLE_CH*)Filter);
	if(err != LXC_NO_ERR)
	{
		LXCHandle->fftHandle.fftCallbacks.destroy_fft(&(LXCHandle->fftHandle.fftPlan));
		free(LXCHandle);
		LXC_Core_setLastError(err);
		return NULL;
	}

	return LXCHandle;
}

LXC_ERROR_CODE LXC_Core_getOptimizationModule(LXC_HANDLE *LXCHandle, LXC_OPTIMIZATION_MODULE LXC_module)
{
	if(!LXCHandle)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	LXC_ERROR_CODE err = LXC_NO_ERR;

	// get LXC_module callbacks
	switch(LXC_module)
	{
	#if defined(USE_LXC_NATIVE)
		case LXC_OPT_NATIVE:
			err = LXC_get_NativeCallbacks(LXCHandle);
		break;
	#endif

	#if defined(USE_LXC_SSE3)
		case LXC_OPT_SSE3:
			err = LXC_get_SSE3AllCallbacks(LXCHandle);
		break;
	#endif
		default:
			err = LXC_ERR_UNSUPPORTED_CONVOLVER;
		break;
	}

	return err;
}

LXC_ERROR_CODE LXC_Core_storeFilter(LXC_HANDLE *LXCHandle, LXC_FILTER_HANDLE_CH *Filter)
{
	if(!Filter || !Filter->LXC_filterConfig.sampleFrequency)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	LXC_ERROR_CODE err = LXC_NO_ERR;
	
	float *tempSamples = (float*)malloc(sizeof(float)*LXCHandle->fftHandle.fftPlan.fftSize);
	if(!tempSamples)
	{
		return LXC_ERR_DYNAMIC_MEMORY;
	}

	LXC_BUFFER_CALLBACKS *bufferCallbacks = &(LXCHandle->LXCHandle.bufferCallbacks);
	LXC_RINGBUFFER_CALLBACKS *ringbufferCallbacks = &(LXCHandle->LXCHandle.ringbufferCallbacks);
	LXC_FFT_CALLBACKS *fftCallbacks = &(LXCHandle->fftHandle.fftCallbacks);
	LXC_FFT_PLAN *fftPlan = &(LXCHandle->fftHandle.fftPlan);
	LXC_CONFIG *lxcConfig = &(LXCHandle->LXC_config);
	
	
	lxcConfig->LXC_sampleFrequency = Filter->LXC_filterConfig.sampleFrequency;
	lxcConfig->LXC_maxChannels = 0;

	if(Filter->h1 && Filter->LXC_filterConfig.maxFilterLength_h1)
	{
		lxcConfig->LXC_maxChannels++;
		err = LXC_Core_initializeFilter( bufferCallbacks, ringbufferCallbacks, fftCallbacks,
										 &(LXCHandle->fftHandle), &(LXCHandle->LXCHandle.H1), 
										 &(LXCHandle->LXCHandle.results_H1), &(LXCHandle->LXCHandle.IN_H1),
										 Filter->h1, Filter->LXC_filterConfig.maxFilterLength_h1, 
										 Filter->LXC_filterConfig.sampleFrequency, tempSamples);
		if(err != LXC_NO_ERR)
		{
			free(tempSamples);
			return err;
		}
	}

	if(Filter->h2 && Filter->LXC_filterConfig.maxFilterLength_h2)
	{
		lxcConfig->LXC_maxChannels++;
		err = LXC_Core_initializeFilter( bufferCallbacks, ringbufferCallbacks, fftCallbacks,
										 &(LXCHandle->fftHandle), &(LXCHandle->LXCHandle.H2), 
										 &(LXCHandle->LXCHandle.results_H2), &(LXCHandle->LXCHandle.IN_H2),
										 Filter->h2, Filter->LXC_filterConfig.maxFilterLength_h2, 
										 Filter->LXC_filterConfig.sampleFrequency, tempSamples);
		if(err != LXC_NO_ERR)
		{
			free(tempSamples);
			return err;
		}
	}

	free(tempSamples);
	if((!LXCHandle->LXCHandle.H1.buffer && !LXCHandle->LXCHandle.H2.buffer) || 
		!LXCHandle->LXCHandle.H1.buffer ||
		!lxcConfig->LXC_maxChannels )
	{
		return LXC_ERR_INVALID_INPUT;
	}

	
	err = fftCallbacks->clearBuffers(&(LXCHandle->fftHandle.fftPlan));

	return err;
}

LXC_ERROR_CODE LXC_Core_destroy(LXC_HANDLE *LXCHandle)
{
	if(!LXCHandle)
	{
		return LXC_ERR_INVALID_INPUT;
	}
	// destroy specific fft plan
	LXC_ERROR_CODE err = LXCHandle->fftHandle.fftCallbacks.destroy_fft(&(LXCHandle->fftHandle.fftPlan));

	// destroy buffers
	err = LXCHandle->LXCHandle.bufferCallbacks.LXC_Buffer_destroy(&(LXCHandle->LXCHandle.H1));
	err = LXCHandle->LXCHandle.bufferCallbacks.LXC_Buffer_destroy(&(LXCHandle->LXCHandle.H2));
	err = LXCHandle->LXCHandle.bufferCallbacks.LXC_Buffer_destroy(&(LXCHandle->LXCHandle.results_H1));
	err = LXCHandle->LXCHandle.bufferCallbacks.LXC_Buffer_destroy(&(LXCHandle->LXCHandle.results_H2));

	// destroy ringbuffers
	err = LXCHandle->LXCHandle.ringbufferCallbacks.LXC_Ringbuffer_destroy(&(LXCHandle->LXCHandle.IN_H1));
	err = LXCHandle->LXCHandle.ringbufferCallbacks.LXC_Ringbuffer_destroy(&(LXCHandle->LXCHandle.IN_H2));
	
	return err;
}

LXC_ERROR_CODE LXC_Core_convolve(LXC_HANDLE *LXCHandle, float *x, float *z)
{
	if(!LXCHandle || !x || !z)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	// get all specific buffers, callbacks and handles
	LXC_FFT_PLAN *fftPlan = &(LXCHandle->fftHandle.fftPlan);
	LXC_FFT_CALLBACKS *fft = &(LXCHandle->fftHandle.fftCallbacks);
	const uint timeFrameLength = LXCHandle->LXC_config.LXC_maxInputFrameLength;
	const uint freqFrameLength = fftPlan->fftSize;
	LXC_BUFFER *H = &(LXCHandle->LXCHandle.H1);
	LXC_BUFFER *results_H = &(LXCHandle->LXCHandle.results_H1);
	LXC_RINGBUFFER *in_H = &(LXCHandle->LXCHandle.IN_H1);
	LXC_BUFFER_CALLBACKS *bufferCallbacks = &(LXCHandle->LXCHandle.bufferCallbacks);
	LXC_RINGBUFFER_CALLBACKS *ringbufferCallbacks = &(LXCHandle->LXCHandle.ringbufferCallbacks);
	LXC_CALLBACKS *lxcCallbacks = &(LXCHandle->LXC_callbacks);

	// convert format external, fft, convert format internal
	fft->fmtc_external_TO_fft(x, fftPlan, timeFrameLength);
	fft->fft(fftPlan);
	void *X = ringbufferCallbacks->LXC_Ringbuffer_getNextPart(in_H);
	fft->fmtc_fft_TO_internal(fftPlan, X, freqFrameLength);
	
	// convolution in frequency domain is only a complex multiplication
	// convolve channel1
	uint maxParts = H->maxFilterParts;
	for(uint ii=0; ii < maxParts; ii++)
	{
		void* G = bufferCallbacks->LXC_Buffer_getPart(H, ii);
		void* Z = bufferCallbacks->LXC_Buffer_getPart(results_H, ii);
		void* X = ringbufferCallbacks->LXC_Ringbuffer_getPart(in_H, ii);
		if(H && X && Z)
		{
			lxcCallbacks->LXC_CpxMul(freqFrameLength, X, G, Z);
		}
	}

	// sum all samples
	lxcCallbacks->LXC_CpxAdd(results_H, 0);

	void* Z = bufferCallbacks->LXC_Buffer_getPart(results_H, 0);
	if(!Z)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	// do inverse fast fourier transform
	fft->fmtc_internal_TO_fft(Z, fftPlan, freqFrameLength);
	fft->ifft(fftPlan);
	fft->fmtc_fft_TO_external(fftPlan, z, timeFrameLength);

	return LXC_NO_ERR;
}

LXC_ERROR_CODE LXC_Core_convolve2Ch(LXC_HANDLE *LXCHandle, float *x, float *y, float *z1, float *z2)
{
	if(!LXCHandle || !x || !y || !z1 || !z2)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	// get all specific buffers, callbacks and handles
	LXC_FFT_PLAN *fftPlan = &(LXCHandle->fftHandle.fftPlan);
	LXC_FFT_CALLBACKS *fft = &(LXCHandle->fftHandle.fftCallbacks);
	const uint timeFrameLength = LXCHandle->LXC_config.LXC_maxInputFrameLength;
	const uint freqFrameLength = fftPlan->fftSize;
	LXC_BUFFER *H1 = &(LXCHandle->LXCHandle.H1);
	LXC_BUFFER *H2 = &(LXCHandle->LXCHandle.H2);
	LXC_BUFFER *results_H1 = &(LXCHandle->LXCHandle.results_H1);
	LXC_BUFFER *results_H2 = &(LXCHandle->LXCHandle.results_H2);
	LXC_RINGBUFFER *in_H1 = &(LXCHandle->LXCHandle.IN_H1);
	LXC_RINGBUFFER *in_H2 = &(LXCHandle->LXCHandle.IN_H2);
	LXC_BUFFER_CALLBACKS *bufferCallbacks = &(LXCHandle->LXCHandle.bufferCallbacks);
	LXC_RINGBUFFER_CALLBACKS *ringbufferCallbacks = &(LXCHandle->LXCHandle.ringbufferCallbacks);
	LXC_CALLBACKS *lxcCallbacks = &(LXCHandle->LXC_callbacks);

	// convert format external, fft, convert format internal
	fft->fmtc_external_TO_fft_2Ch(x, y, fftPlan, timeFrameLength);
	fft->fft(fftPlan);
	void* Z0 = bufferCallbacks->LXC_Buffer_getPart(results_H1, 0);
	fft->fmtc_fft_TO_internal(fftPlan, Z0, freqFrameLength);

	// split channels
	void *X = ringbufferCallbacks->LXC_Ringbuffer_getNextPart(in_H1);
	void *Y = ringbufferCallbacks->LXC_Ringbuffer_getNextPart(in_H2);;
	lxcCallbacks->LXC_FreqSplit2Ch(freqFrameLength, Z0, X, Y);

	// convolution in frequency domain is only a complex multiplication
	// convolve channel1
	uint maxParts = H1->maxFilterParts;
	for(uint ii=0; ii < maxParts; ii++)
	{
		void* H = bufferCallbacks->LXC_Buffer_getPart(H1, ii);
		void* Z = bufferCallbacks->LXC_Buffer_getPart(results_H1, ii);
		void* X = ringbufferCallbacks->LXC_Ringbuffer_getPart(in_H1, ii);
		if(H && X && Z)
		{
			lxcCallbacks->LXC_CpxMul(freqFrameLength, X, H, Z);
		}
	}

	// convolve channel2
	maxParts = H2->maxFilterParts;
	for(uint ii=0; ii < maxParts; ii++)
	{
		void* H = bufferCallbacks->LXC_Buffer_getPart(H2, ii);
		void* X = ringbufferCallbacks->LXC_Ringbuffer_getPart(in_H2, ii);
		void* Z = bufferCallbacks->LXC_Buffer_getPart(results_H2, ii);
		if(H && X && Z)
		{
			lxcCallbacks->LXC_CpxMul(freqFrameLength, X, H, Z);
		}
	}

	// sum all samples
	lxcCallbacks->LXC_CpxAdd(results_H1, 0);
	lxcCallbacks->LXC_CpxAdd(results_H2, 0);

	// combine two complex signals in frequency domain
	void* Z1 = bufferCallbacks->LXC_Buffer_getPart(results_H1, 0);
	void* Z2 = bufferCallbacks->LXC_Buffer_getPart(results_H2, 0);
	void* Z = bufferCallbacks->LXC_Buffer_getPart(results_H1, 1);
	if(Z1 && Z2 && Z)
	{
		lxcCallbacks->LXC_FreqCombine2Ch(freqFrameLength, Z1, Z2, Z);
	}
	else
	{
		return LXC_ERR_INVALID_INPUT;
	}

	// do inverse fast fourier transform
	fft->fmtc_internal_TO_fft(Z, fftPlan, freqFrameLength);
	fft->ifft(fftPlan);
	fft->fmtc_fft_TO_external_2Ch(fftPlan, z1, z2, timeFrameLength);

	return LXC_NO_ERR;
}

// -----------------------------------
// Helper Functions
// -----------------------------------
void LXC_Core_setLastError(LXC_ERROR_CODE err)
{
	g_LXC_Core_lastError = err;
}

LXC_ERROR_CODE LXC_Core_getLastError()
{
	return g_LXC_Core_lastError;
}

void LXC_Core_clearHandle(LXC_HANDLE *LXCHandle)
{
	if(LXCHandle)
	{
		// reset fft module
		LXCHandle->fftHandle.fftCallbacks.clearBuffers						= NULL;
		LXCHandle->fftHandle.fftCallbacks.create_fft						= NULL;
		LXCHandle->fftHandle.fftCallbacks.destroy_fft						= NULL;
		LXCHandle->fftHandle.fftCallbacks.fft								= NULL;
		LXCHandle->fftHandle.fftCallbacks.fmtc_external_TO_fft				= NULL;
		LXCHandle->fftHandle.fftCallbacks.fmtc_external_TO_fft_2Ch			= NULL;
		LXCHandle->fftHandle.fftCallbacks.fmtc_fft_TO_external				= NULL;
		LXCHandle->fftHandle.fftCallbacks.fmtc_fft_TO_external_2Ch			= NULL;
		LXCHandle->fftHandle.fftCallbacks.fmtc_fft_TO_internal				= NULL;
		LXCHandle->fftHandle.fftCallbacks.fmtc_internal_TO_fft				= NULL;
		LXCHandle->fftHandle.fftCallbacks.ifft								= NULL;

		LXCHandle->fftHandle.fftModule										= LXC_fftModule_Min;
		LXCHandle->fftHandle.fftPlan.fftSize								= 0;
		LXCHandle->fftHandle.fftPlan.fftZeros								= 0;
		LXCHandle->fftHandle.fftPlan.maxInputFrameLength					= 0;
		LXCHandle->fftHandle.fftPlan.scaleFactor							= 0.0f;
		LXCHandle->fftHandle.fftPlan.specific_fftPlan						= NULL;


		// reset buffer callbacks
		LXCHandle->LXCHandle.bufferCallbacks.LXC_Buffer_create				= NULL;
		LXCHandle->LXCHandle.bufferCallbacks.LXC_Buffer_destroy				= NULL;
		LXCHandle->LXCHandle.bufferCallbacks.LXC_Buffer_getPart				= NULL;

		// reset ringbuffer callbacks
		LXCHandle->LXCHandle.ringbufferCallbacks.LXC_Ringbuffer_create		= NULL;
		LXCHandle->LXCHandle.ringbufferCallbacks.LXC_Ringbuffer_destroy		= NULL;
		LXCHandle->LXCHandle.ringbufferCallbacks.LXC_Ringbuffer_getNextPart = NULL;
		LXCHandle->LXCHandle.ringbufferCallbacks.LXC_Ringbuffer_getPart		= NULL;

		LXCHandle->LXCHandle.maxChannels									= 0;
		
		LXC_Core_clearBuffer(&(LXCHandle->LXCHandle.H1));
		LXC_Core_clearBuffer(&(LXCHandle->LXCHandle.H2));
		LXC_Core_clearBuffer(&(LXCHandle->LXCHandle.results_H1));
		LXC_Core_clearBuffer(&(LXCHandle->LXCHandle.results_H2));

		LXC_Core_clearRingbuffer(&(LXCHandle->LXCHandle.IN_H1));
		LXC_Core_clearRingbuffer(&(LXCHandle->LXCHandle.IN_H2));

		LXCHandle->LXC_config.LXC_maxChannels								= 0;
		LXCHandle->LXC_config.LXC_maxInputFrameLength						= 0;
		LXCHandle->LXC_config.LXC_module									= LXC_OPT_MIN;
		LXCHandle->LXC_config.LXC_sampleFrequency							= 0;

		// reset ringbuffers
	}
}

void LXC_Core_clearBuffer(LXC_BUFFER *Buffer)
{
	if(Buffer)
	{
		Buffer->buffer	= NULL;
		Buffer->maxFilterLength	= 0;
		Buffer->maxFilterPartLength = 0;
		Buffer->maxFilterPartLength_NonZero = 0;
		Buffer->maxFilterParts	= 0;
		Buffer->sampleFrequency = 0;
	}
}


void LXC_Core_clearRingbuffer(LXC_RINGBUFFER *Ringbuffer)
{
	if(Ringbuffer)
	{
		Ringbuffer->buffer	= NULL;
		Ringbuffer->currentElement = 0;
		Ringbuffer->isEmpty = 1;
		Ringbuffer->maxElements = 0;
		Ringbuffer->maxPartLength = 0;
		Ringbuffer->maxPartLength_Half = 0;
		Ringbuffer->sampleFrequency = 0;
	}
}

LXC_ERROR_CODE LXC_Core_initializeFilter(	LXC_BUFFER_CALLBACKS *BufferCallbacks, 
											LXC_RINGBUFFER_CALLBACKS *RingbufferCallbacks,
											LXC_FFT_CALLBACKS *fftCallbacks,
											LXC_FFT_HANDLE *fftHandle,
											LXC_BUFFER *FilterBuffer,
											LXC_BUFFER *ResultBuffer,
											LXC_RINGBUFFER *InBuffer,
											float *h,
											uint MaxFilterLength,
											uint SampleFrequency,
											float *TempSamples )
{
	LXC_ERROR_CODE err = LXC_NO_ERR;

	if( !BufferCallbacks || !BufferCallbacks ||
		!RingbufferCallbacks || !fftCallbacks ||
		!fftHandle || !FilterBuffer || !ResultBuffer ||
		!InBuffer || !h || !MaxFilterLength || 
		!SampleFrequency || !TempSamples )
	{
		return LXC_ERR_INVALID_INPUT;
	}

	LXC_FFT_PLAN *fftPlan = &(fftHandle->fftPlan);

	FilterBuffer->maxFilterLength = MaxFilterLength;
	FilterBuffer->sampleFrequency = SampleFrequency;
	FilterBuffer->maxFilterPartLength = fftHandle->fftPlan.fftSize;
	FilterBuffer->maxFilterPartLength_NonZero = fftHandle->fftPlan.maxInputFrameLength;
	FilterBuffer->maxFilterParts = FilterBuffer->maxFilterLength / FilterBuffer->maxFilterPartLength_NonZero;
	if(FilterBuffer->maxFilterLength % FilterBuffer->maxFilterPartLength_NonZero != 0)
		(FilterBuffer->maxFilterParts)++;

	err = BufferCallbacks->LXC_Buffer_create(FilterBuffer);
	if(err != LXC_NO_ERR)
		return err;


	ResultBuffer->maxFilterLength = FilterBuffer->maxFilterLength;
	ResultBuffer->maxFilterPartLength = FilterBuffer->maxFilterPartLength;
	ResultBuffer->maxFilterPartLength_NonZero = FilterBuffer->maxFilterPartLength_NonZero;
	ResultBuffer->maxFilterParts = FilterBuffer->maxFilterParts;
	ResultBuffer->sampleFrequency = FilterBuffer->sampleFrequency;
	err = BufferCallbacks->LXC_Buffer_create(ResultBuffer);
	if(err != LXC_NO_ERR)
		return err;

	InBuffer->sampleFrequency = FilterBuffer->sampleFrequency;
	InBuffer->maxElements = FilterBuffer->maxFilterPartLength*FilterBuffer->maxFilterParts;
	InBuffer->maxPartLength = FilterBuffer->maxFilterPartLength;
	InBuffer->maxElements = FilterBuffer->maxFilterParts;
	err = RingbufferCallbacks->LXC_Ringbuffer_create(InBuffer);
	if(err != LXC_NO_ERR)
		return err;

	// copy samples to internal structure
	for(uint uiPart=0; uiPart < FilterBuffer->maxFilterParts -1; uiPart++)
	{
		float *p = h + uiPart*FilterBuffer->maxFilterPartLength_NonZero;
		for(uint ii = 0; ii < FilterBuffer->maxFilterPartLength_NonZero; ii++)
			TempSamples[ii] = p[ii];

		// zero padding
		for(uint ii = FilterBuffer->maxFilterPartLength_NonZero; ii < FilterBuffer->maxFilterPartLength; ii++)
			TempSamples[ii] = 0.0f;

		err = fftCallbacks->fmtc_external_TO_fft(TempSamples, fftPlan, 
												 FilterBuffer->maxFilterPartLength);
		if(err != LXC_NO_ERR)
			return err;

		err = fftCallbacks->fft(fftPlan);
		if(err != LXC_NO_ERR)
			return err;

		err = fftCallbacks->fmtc_fft_TO_internal(fftPlan, 
												 BufferCallbacks->LXC_Buffer_getPart(FilterBuffer, uiPart),
												 FilterBuffer->maxFilterPartLength);
		if(err != LXC_NO_ERR)
			return err;
	}

	// copy last filter part
	uint lastSamples = MaxFilterLength - FilterBuffer->maxFilterPartLength_NonZero*(FilterBuffer->maxFilterParts -1);
	float *p = h + MaxFilterLength - lastSamples;
	for(uint ii=0; ii < lastSamples; ii++)
		TempSamples[ii] = p[ii];

	// zero padding
	for(uint ii = lastSamples; ii < FilterBuffer->maxFilterPartLength; ii++)
		TempSamples[ii] = 0.0f;

	err = fftCallbacks->fmtc_external_TO_fft(TempSamples, fftPlan, FilterBuffer->maxFilterPartLength);
	if(err != LXC_NO_ERR)
		return err;

	err = fftCallbacks->fft(fftPlan);
	if(err != LXC_NO_ERR)
		return err;

	fftCallbacks->fmtc_fft_TO_internal(fftPlan, 
									   BufferCallbacks->LXC_Buffer_getPart(FilterBuffer, FilterBuffer->maxFilterParts), 
									   FilterBuffer->maxFilterPartLength);
	if(err != LXC_NO_ERR)
		return err;

	return LXC_NO_ERR;
}