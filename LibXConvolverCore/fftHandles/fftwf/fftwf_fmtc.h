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



#include "../../include/common_types.h"
#include "../../include/LXC_Core_types.h"
#include "../fftHandles_types.h"

// fftwfHandle format conversion function prototypes
LXC_ERROR_CODE fmtc_float_TO_fftwf(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size);
LXC_ERROR_CODE fmtc_fftwf_TO_float(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size);
LXC_ERROR_CODE fmtc_float_TO_fftwf_2Ch(void *In1, void *In2, LXC_FFT_PLAN *fftHandle_out, uint Size);
LXC_ERROR_CODE fmtc_fftwf_TO_float_2Ch(LXC_FFT_PLAN *fftHandle_in, void *Out1, void *Out2, uint Size);
LXC_ERROR_CODE fmtc_LXCcpxFloat_TO_fftwf(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size);
LXC_ERROR_CODE fmtc_fftwf_TO_LXCcpxFloat(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size);

#if defined(USE_LXC_SSE3)
	LXC_ERROR_CODE fmtc_LXCcpxSSE3Float_TO_fftwf(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size);
	LXC_ERROR_CODE fmtc_fftwf_TO_LXCcpxSSE3Float(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size);
	LXC_ERROR_CODE fmtc_LXCcpxSSE3Float_TO_fftwf_SSE3_K2(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size);
	LXC_ERROR_CODE fmtc_fftwf_TO_LXCcpxSSE3Float_SSE3_K2(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size);
#endif