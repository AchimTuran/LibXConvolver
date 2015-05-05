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



#include "../../include/LXC_CommonTypes.h"
#include "../../include/LXC_Core_types.h"
#include "../LXC_fftHandles_types.h"

// KissFFTHandle format conversion function prototypes
LXC_ERROR_CODE LXC_fmtc_float_TO_KissFFT(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size);
LXC_ERROR_CODE LXC_fmtc_KissFFT_TO_float(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size);
LXC_ERROR_CODE LXC_fmtc_float_TO_KissFFT_2Ch(void *In1, void *In2, LXC_FFT_PLAN *fftHandle_out, uint Size);
LXC_ERROR_CODE LXC_fmtc_KissFFT_TO_float_2Ch(LXC_FFT_PLAN *fftHandle_in, void *Out1, void *Out2, uint Size);

#if defined(USE_LXC_NATIVE)
LXC_ERROR_CODE LXC_fmtc_LXCcpxFloat_TO_KissFFT(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size);
LXC_ERROR_CODE LXC_fmtc_KissFFT_TO_LXCcpxFloat(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size);
#endif

#undef USE_LXC_SSE3
#if defined(USE_LXC_SSE3)
	LXC_ERROR_CODE LXC_fmtc_LXCcpxSSE3Float_TO_KissFFT(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size);
	LXC_ERROR_CODE LXC_fmtc_KissFFT_TO_LXCcpxSSE3Float(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size);
	LXC_ERROR_CODE LXC_fmtc_LXCcpxSSE3Float_TO_KissFFT_SSE3_K2(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size);
	LXC_ERROR_CODE LXC_fmtc_KissFFT_TO_LXCcpxSSE3Float_SSE3_K2(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size);
#endif
