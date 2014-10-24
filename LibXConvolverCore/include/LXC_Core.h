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



#include "LXC_Core_types.h"
#include "common_types.h"

LXC_ERROR_CODE LXC_Core_init();
LXC_ERROR_CODE LXC_Core_close();
LXC_ERROR_CODE LXC_get_Properties(LXC_PROPERTIES *LXC_properties);
LXC_ERROR_CODE LXC_Core_destroy(LXC_HANDLE *LXCHandle);
LXC_ERROR_CODE LXC_Core_getProperties(LXC_PROPERTIES *LXC_properties);
LXC_HANDLE* LXC_Core_getConvolver(LXC_ptrFilterHandle *Filter, uint InputFrameLength, LXC_OPTIMIZATION_MODULE LXC_module, LXC_FFT_MODULE LXC_fftModule);
LXC_ptrFilterHandle* LXC_Core_createFilter(float *h, uint Size_h, uint SampleFreq);
LXC_ptrFilterHandle* LXC_Core_createFilter2Ch(float *h1, uint Size_h1, float *h2, uint Size_h2, uint SampleFreq);
LXC_ERROR_CODE LXC_Core_getLastError();

LXC_ERROR_CODE LXC_Core_convolve(LXC_HANDLE *LXCHandle, float *x, float *z);
LXC_ERROR_CODE LXC_Core_convolve2Ch(LXC_HANDLE *LXCHandle, float *x, float *y, float *z1, float *z2);
