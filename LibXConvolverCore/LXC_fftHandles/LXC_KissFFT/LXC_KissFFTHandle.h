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
#include "../LXC_fftHandles_types.h"
#include "../../include/LXC_Core_types.h"
#include "../LXC_fftHandles_types.h"

// KissFFTHandle function prototypes
LXC_ERROR_CODE LXC_get_KissFFTHandleCallbacks(LXC_FFT_CALLBACKS *Callbacks, LXC_OPTIMIZATION_MODULE LXC_module);
LXC_ERROR_CODE LXC_KissFFT_create(LXC_FFT_PLAN *fftPlan, uint FreqSize, uint TimeSize);
LXC_ERROR_CODE LXC_KissFFT_destroy(LXC_FFT_PLAN *fftHandle);
LXC_ERROR_CODE LXC_KissFFT_fft(LXC_FFT_PLAN *fftHandle);
LXC_ERROR_CODE LXC_KissFFT_ifft(LXC_FFT_PLAN *fftHandle);
LXC_ERROR_CODE LXC_KissFFT_clearBuffers(LXC_FFT_PLAN *fftPlan);
