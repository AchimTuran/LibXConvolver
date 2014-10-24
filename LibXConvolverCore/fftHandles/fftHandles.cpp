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



#include "fftwf/fftwfHandle.h"
#include "fftHandles_types.h"
#include "../include/LXC_Core_types.h"
#include <stdio.h>
#include <malloc.h>


LXC_ERROR_CODE LXC_get_fftHandle(LXC_FFT_HANDLE *LXC_fftHandle, LXC_FFT_MODULE fftModule, LXC_OPTIMIZATION_MODULE LXC_module, uint FreqSize, uint TimeSize)
{
	if(!LXC_fftHandle || !FreqSize || !TimeSize)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	if(fftModule <= LXC_fftModule_Min || fftModule >= LXC_fftModule_Max)
	{
		return LXC_ERR_UNSUPPORTED_FFT_MODULE;
	}

	LXC_fftHandle->fftModule = fftModule;
	LXC_ERROR_CODE err = LXC_NO_ERR;
	switch(fftModule)
	{
		case LXC_fftModule_fftwf:
			err = get_fftwfHandleCallbacks(&(LXC_fftHandle->fftCallbacks), LXC_module);
		break;

		default:
			err = LXC_ERR_UNSUPPORTED_FFT_MODULE;
		break;
	}

	if(err != LXC_NO_ERR)
	{
		return err;
	}

	err = LXC_fftHandle->fftCallbacks.create_fft(&(LXC_fftHandle->fftPlan), FreqSize, TimeSize);
	if(err != LXC_NO_ERR || !LXC_fftHandle->fftPlan.specific_fftPlan)
	{
		LXC_fftHandle->fftCallbacks.destroy_fft(&(LXC_fftHandle->fftPlan));
		return err;
	}
	
	return err;
}
