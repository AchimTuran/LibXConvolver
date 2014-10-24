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



#include "../include/common_types.h"


typedef enum {  LXC_fftModule_Min=0, 
				LXC_fftModule_fftwf,
				LXC_fftModule_LXCfft,
				LXC_fftModule_Max
} LXC_FFT_MODULE;


typedef struct
{
	uint fftSize;					// full fft length
	uint maxInputFrameLength;		// max input frame length in samples
	uint fftZeros;					// quantity of zeros (zero padding)
	float scaleFactor;				// scaling factor for ifft(X)
	void *specific_fftPlan;			// specific fftHandle
} LXC_FFT_PLAN;


typedef struct
{
	// fft functions
	LXC_ERROR_CODE	(*create_fft)(LXC_FFT_PLAN *fftPlan, uint FreqSize, uint TimeSize);
	LXC_ERROR_CODE	(*destroy_fft)(LXC_FFT_PLAN *fftHandle);
	LXC_ERROR_CODE	(*clearBuffers)(LXC_FFT_PLAN *fftPlan);
	LXC_ERROR_CODE	(*fft)(LXC_FFT_PLAN *fftHandle);
	LXC_ERROR_CODE	(*ifft)(LXC_FFT_PLAN *fftHandle);

	// format conversion functions
	LXC_ERROR_CODE	(*fmtc_external_TO_fft)(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size);						// outside LXC format to fft format
	LXC_ERROR_CODE	(*fmtc_fft_TO_external)(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size);						// fft format to outside LXC format
	LXC_ERROR_CODE	(*fmtc_external_TO_fft_2Ch)(void *In1, void *In2, LXC_FFT_PLAN *fftHandle_out, uint Size);		// outside LXC format to fft format (2 Channels)
	LXC_ERROR_CODE	(*fmtc_fft_TO_external_2Ch)(LXC_FFT_PLAN *fftHandle_in, void *Out1, void *Out2, uint Size);		// fft format to outside LXC format (2 Channels)
	LXC_ERROR_CODE	(*fmtc_internal_TO_fft)(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size);						// intern LXC format to fft format
	LXC_ERROR_CODE	(*fmtc_fft_TO_internal)(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size);						// fft format to intern LXC format
} LXC_FFT_CALLBACKS;


typedef struct
{
	LXC_FFT_CALLBACKS fftCallbacks;
	int fftModule;
	LXC_FFT_PLAN fftPlan;
} LXC_FFT_HANDLE;