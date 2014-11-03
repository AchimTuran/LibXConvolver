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
#include "LXC_Native_types.h"

LXC_ERROR_CODE LXC_get_NativeAllCallbacks(LXC_HANDLE *LXCHandle);
LXC_ERROR_CODE LXC_get_NativeConvolutionCallbacks(LXC_CALLBACKS *Callbacks);
LXC_ERROR_CODE LXC_get_NativeBufferCallbacks(LXC_BUFFER_CALLBACKS *Buffer);
LXC_ERROR_CODE LXC_get_NativeRingbufferCallbacks(LXC_RINGBUFFER_CALLBACKS *Ringbuffer);

LXC_ERROR_CODE LXC_NativeCpxMul(const uint Size, void *X, void *H, void *Y);
LXC_ERROR_CODE LXC_NativeCpxAdd(LXC_BUFFER *ResultBuffer, float ScaleFactor=1.0f);
LXC_ERROR_CODE LXC_NativeFreqCombine2Ch(const uint Size, void *X, void *Y, void *Z);
LXC_ERROR_CODE LXC_NativeFreqSplit2Ch(const uint Size, void *Z, void *X, void *Y);