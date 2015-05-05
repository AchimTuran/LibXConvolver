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



#include "../LXC_KissFFT/LXC_KissFFTHandle.h"

#include "../../include/LXC_Core_types.h"
#include "../LXC_fftHandles_types.h"
#include "../../LXC_Handles/LXC_SSE3/LXC_SSE3_types.h"
#include "../../../LibXConvolverUtils/LXC_Logging/LXC_Logging.h"


#include <malloc.h>
#include <string.h>

#include "../LXC_KissFFT/LXC_KissFFT_fmtc.h"
#include "../LXC_KissFFT/LXC_KissFFTHandle_types.h"

char g_KissFFT_needCleanup = 1;

LXC_ERROR_CODE LXC_get_KissFFTHandleCallbacks(LXC_FFT_CALLBACKS *Callbacks, LXC_OPTIMIZATION_MODULE LXC_module)
{
	if(!Callbacks)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	switch(LXC_module)
	{
	#if defined(USE_LXC_NATIVE)
		case LXC_OPT_NATIVE:
      LXC_LOG_INFO("Get KissFFT module with Native format conversion functions.");
			Callbacks->LXC_fmtc_internal_TO_fft  = LXC_fmtc_LXCcpxFloat_TO_KissFFT;
			Callbacks->LXC_fmtc_fft_TO_internal  = LXC_fmtc_KissFFT_TO_LXCcpxFloat;
		break;
	#endif

	#if defined(USE_LXC_SSE3)
		case LXC_OPT_SSE3:
      LXC_LOG_INFO("Get KissFFT module with Native format conversion functions (SSE3 is currently unsupported).");
      Callbacks->LXC_fmtc_internal_TO_fft  = LXC_fmtc_LXCcpxFloat_TO_KissFFT;
      Callbacks->LXC_fmtc_fft_TO_internal  = LXC_fmtc_KissFFT_TO_LXCcpxFloat;
      //Callbacks->LXC_fmtc_internal_TO_fft  = LXC_fmtc_LXCcpxSSE3Float_TO_KissFFT_SSE3_K2;
			//Callbacks->LXC_fmtc_fft_TO_internal  = LXC_fmtc_KissFFT_TO_LXCcpxSSE3Float_SSE3_K2;
		break;
	#endif

		default:
      LXC_LOG_ERROR("Selected unsupported format conversion functions!");
			return LXC_ERR_UNSUPPORTED_FFT_FMTC;
		break;
	}

	// get format conversion functions
	Callbacks->LXC_fmtc_external_TO_fft     = LXC_fmtc_float_TO_KissFFT;
	Callbacks->LXC_fmtc_fft_TO_external     = LXC_fmtc_KissFFT_TO_float;
	Callbacks->LXC_fmtc_external_TO_fft_2Ch = LXC_fmtc_float_TO_KissFFT_2Ch;
	Callbacks->LXC_fmtc_fft_TO_external_2Ch = LXC_fmtc_KissFFT_TO_float_2Ch;

	// get fft functions
	Callbacks->LXC_create_fft   = LXC_KissFFT_create;
	Callbacks->LXC_destroy_fft  = LXC_KissFFT_destroy;
	Callbacks->LXC_clearBuffers = LXC_KissFFT_clearBuffers;
	Callbacks->LXC_fft          = LXC_KissFFT_fft;
	Callbacks->LXC_ifft         = LXC_KissFFT_ifft;

	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_KissFFT_create(LXC_FFT_PLAN *fftPlan, uint FreqSize, uint TimeSize)
{
	if(!fftPlan || !FreqSize || !TimeSize)
	{
		return LXC_ERR_INVALID_INPUT;
	}
	fftPlan->LXC_specific_fftPlan = NULL;
	fftPlan->LXC_fftSize = 0;

	// reset KissFFT library
  if(g_KissFFT_needCleanup)
  {
    kiss_fft_cleanup();
    g_KissFFT_needCleanup = 0;
  }

	LXC_KISSFFT_HANDLE* KissFFTHandle = (LXC_KISSFFT_HANDLE*)malloc(sizeof(LXC_KISSFFT_HANDLE));
	if(!KissFFTHandle)
	{
		// ToDo: show some error message
    return LXC_ERR_DYNAMIC_MEMORY;
	}
	KissFFTHandle->x_in     = NULL;
	KissFFTHandle->x_out    = NULL;
	KissFFTHandle->X        = NULL;
	KissFFTHandle->fft_cfg  = NULL;
	KissFFTHandle->ifft_cfg = NULL;

	// allocate KissFFT complex data
	kiss_fft_cpx *x_in = (kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*FreqSize);
	if(!x_in)
	{
		// ToDo: show some error message
		free(KissFFTHandle);
		return LXC_ERR_DYNAMIC_MEMORY;
	}

	kiss_fft_cpx *x_out = (kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*FreqSize);
	if(!x_out)
	{
		// ToDo: show some error message
		free(x_in);
		free(KissFFTHandle);
    return LXC_ERR_DYNAMIC_MEMORY;
	}

	kiss_fft_cpx *X = (kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*FreqSize);
	if(!X)
	{
		// ToDo: show some error message
		free(x_in);
		free(x_out);
		free(KissFFTHandle);
    return LXC_ERR_DYNAMIC_MEMORY;
	}

	// initialize fft buffers
	for(uint ii=0; ii < FreqSize; ii++)
	{
		x_in[ii].r = 0.0f;
		x_in[ii].i = 0.0f;

		x_out[ii].r = 0.0f;
		x_out[ii].i = 0.0f;

		X[ii].r = 0.0f;
		X[ii].i = 0.0f;
	}

	// init fftw library and plans
  KissFFTHandle->fft_cfg  = kiss_fft_alloc((int)FreqSize, FORWARD_KISS_FFT, 0, 0);
  KissFFTHandle->ifft_cfg = kiss_fft_alloc((int)FreqSize, INVERSE_KISS_FFT, 0, 0);

	if(!KissFFTHandle->ifft_cfg || !KissFFTHandle->ifft_cfg)
	{
		// ToDo: show some error message
		//throw EXCEPTION_COUT_HANDLER("fftw plans not created! FATAL ERROR!!!");
		free(x_in);
		free(x_out);
		free(X);
		free(KissFFTHandle);
    return LXC_ERR_FFT_CREATION_FAILED;
	}

	// ToDo: show some log message
	//cout << "successfull initialized fftw for " << m_Name << endl;

	KissFFTHandle->x_in = x_in;
	KissFFTHandle->x_out = x_out;
	KissFFTHandle->X = X;
	fftPlan->LXC_specific_fftPlan = (void*)KissFFTHandle;
	fftPlan->LXC_fftSize = FreqSize;
	fftPlan->LXC_maxInputFrameLength = TimeSize;
	fftPlan->LXC_fftZeros = FreqSize - fftPlan->LXC_maxInputFrameLength;
	fftPlan->LXC_scaleFactor = 1.0f/((float)FreqSize);

	return LXC_NO_ERR;
}

LXC_ERROR_CODE LXC_KissFFT_destroy(LXC_FFT_PLAN *fftHandle)
{
	if(!fftHandle || !fftHandle->LXC_specific_fftPlan)
	{
		return LXC_ERR_INVALID_INPUT;
	}
	LXC_KISSFFT_HANDLE* KissFFTHandle = (LXC_KISSFFT_HANDLE*)fftHandle->LXC_specific_fftPlan;

	if(KissFFTHandle->x_in)
	{
		free(KissFFTHandle->x_in);
	}
	KissFFTHandle->x_in = NULL;

	if(KissFFTHandle->x_out)
	{
		free(KissFFTHandle->x_out);
	}
	KissFFTHandle->x_out = NULL;

	if(KissFFTHandle->X)
	{
		free(KissFFTHandle->X);
	}
	KissFFTHandle->X = NULL;

	// destroy KissFFT plans
	free(KissFFTHandle->fft_cfg);
	free(KissFFTHandle->ifft_cfg);

	// destroy fftHandle
	if(fftHandle->LXC_specific_fftPlan)
	{
		free(fftHandle->LXC_specific_fftPlan);
	}
	fftHandle->LXC_specific_fftPlan = NULL;

	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_KissFFT_clearBuffers(LXC_FFT_PLAN *fftPlan)
{
	if(!fftPlan || !fftPlan->LXC_specific_fftPlan)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	kiss_fft_cpx *x_in = ((LXC_KISSFFT_HANDLE*)fftPlan->LXC_specific_fftPlan)->x_in;
	kiss_fft_cpx *x_out = ((LXC_KISSFFT_HANDLE*)fftPlan->LXC_specific_fftPlan)->x_out;
	kiss_fft_cpx *X = ((LXC_KISSFFT_HANDLE*)fftPlan->LXC_specific_fftPlan)->X;

	if(!X || !x_in || !x_out)
	{
		return LXC_ERR_NULL_POINTER;
	}

	for(uint ii=0; ii < fftPlan->LXC_fftSize; ii++)
	{
		x_in[ii].r = 0.0f;
		x_in[ii].i = 0.0f;

		x_out[ii].r = 0.0f;
		x_out[ii].i = 0.0f;

		X[ii].r = 0.0f;
		X[ii].i = 0.0f;
	}

	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_KissFFT_fft(LXC_FFT_PLAN *fftHandle)
{
	if(!fftHandle || !fftHandle->LXC_specific_fftPlan)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	LXC_KISSFFT_HANDLE* KissFFTHandle = (LXC_KISSFFT_HANDLE*)fftHandle->LXC_specific_fftPlan;
	kiss_fft(KissFFTHandle->fft_cfg, KissFFTHandle->x_in, KissFFTHandle->X);
	
	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_KissFFT_ifft(LXC_FFT_PLAN *fftHandle)
{
	if(!fftHandle || !fftHandle->LXC_specific_fftPlan)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	LXC_KISSFFT_HANDLE* KissFFTHandle = (LXC_KISSFFT_HANDLE*)fftHandle->LXC_specific_fftPlan;
	kiss_fft(KissFFTHandle->ifft_cfg, KissFFTHandle->X, KissFFTHandle->x_out);

	return LXC_NO_ERR;
}
