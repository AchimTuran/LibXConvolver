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



// include header files for fftModules
#include "LXC_fftwf/LXC_fftwfHandle.h"
#include "LXC_KissFFT/LXC_KissFFTHandle.h"

// include header files for fftModule handling
#include "LXC_fftHandles_types.h"
#include "../include/LXC_Core_types.h"
#include "../../LibXConvolverUtils/LXC_Logging/LXC_Logging.h"
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

	LXC_fftHandle->LXC_fftModule = fftModule;
	LXC_ERROR_CODE err = LXC_NO_ERR;
	switch(fftModule)
	{
		case LXC_fftModule_fftwf:
      LXC_LOG_INFO("Trying to get LXC fftModule: fftwf.");
			err = LXC_get_fftwfHandleCallbacks(&(LXC_fftHandle->LXC_fftCallbacks), LXC_module);
		break;

    case LXC_fftModule_KissFFT:
      LXC_LOG_INFO("Trying to get LXC fftModule: KissFFT.");
      err = LXC_get_KissFFTHandleCallbacks(&(LXC_fftHandle->LXC_fftCallbacks), LXC_module);
    break;

		default:
      LXC_LOG_ERROR("Selected unsupported LXC fftModule!");
			err = LXC_ERR_UNSUPPORTED_FFT_MODULE;
		break;
	}

	if(err != LXC_NO_ERR)
	{
		return err;
	}

	err = LXC_fftHandle->LXC_fftCallbacks.LXC_create_fft(&(LXC_fftHandle->LXC_fftPlan), FreqSize, TimeSize);
	if(err != LXC_NO_ERR || !LXC_fftHandle->LXC_fftPlan.LXC_specific_fftPlan)
	{
		LXC_fftHandle->LXC_fftCallbacks.LXC_destroy_fft(&(LXC_fftHandle->LXC_fftPlan));
		return err;
	}
	
  LXC_LOG_INFO("Successful initialize LXC_fftModule.");
	return err;
}
