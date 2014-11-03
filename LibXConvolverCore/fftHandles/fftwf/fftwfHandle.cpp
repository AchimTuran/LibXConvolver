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



#include "fftwfHandle.h"
#include "../../include/LXC_Core_types.h"
#include "../fftHandles_types.h"
#include "fftwfHandle_types.h"
#include "fftwf_fmtc.h"
#include "../../LXCHandles/LXC_SSE3/LXC_SSE3_types.h"
#include "../../../LibXConvolverUtils/logging/LXC_Logging.h"

#include "fftw3.h"
#include <malloc.h>
#include <string.h>

extern char *g_LXC_HomePath;
char g_FirstPlan = 1;

LXC_ERROR_CODE get_fftwfHandleCallbacks(LXC_FFT_CALLBACKS *Callbacks, LXC_OPTIMIZATION_MODULE LXC_module)
{
	if(!Callbacks)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	switch(LXC_module)
	{
	#if defined(USE_LXC_NATIVE)
		case LXC_OPT_NATIVE:
      LXC_LOG_INFO("Get fftwf module with Native format conversion functions.");
			Callbacks->fmtc_internal_TO_fft  = fmtc_LXCcpxFloat_TO_fftwf;
			Callbacks->fmtc_fft_TO_internal = fmtc_fftwf_TO_LXCcpxFloat;
		break;
	#endif

	#if defined(USE_LXC_SSE3)
		case LXC_OPT_SSE3:
      LXC_LOG_INFO("Get fftwf module with SSE3 format conversion functions.");
			Callbacks->fmtc_internal_TO_fft  = fmtc_LXCcpxSSE3Float_TO_fftwf_SSE3_K2;
			Callbacks->fmtc_fft_TO_internal = fmtc_fftwf_TO_LXCcpxSSE3Float_SSE3_K2;
		break;
	#endif

		default:
      LXC_LOG_ERROR("Selected unsupported format conversion functions!");
			return LXC_ERR_UNSUPPORTED_FFT_FMTC;
		break;
	}

	// get format conversion functions
	Callbacks->fmtc_external_TO_fft		= fmtc_float_TO_fftwf;
	Callbacks->fmtc_fft_TO_external		= fmtc_fftwf_TO_float;
	Callbacks->fmtc_external_TO_fft_2Ch = fmtc_float_TO_fftwf_2Ch;
	Callbacks->fmtc_fft_TO_external_2Ch = fmtc_fftwf_TO_float_2Ch;

	// get fft functions
	Callbacks->create_fft	= create_fftwf;
	Callbacks->destroy_fft	= destroy_fftwf;
	Callbacks->clearBuffers = fftwf_clearBuffers;
	Callbacks->fft			= fftwf_fft;
	Callbacks->ifft			= fftwf_ifft;

	return LXC_NO_ERR;
}


LXC_ERROR_CODE create_fftwf(LXC_FFT_PLAN *fftPlan, uint FreqSize, uint TimeSize)
{
	if(!fftPlan || !FreqSize || !TimeSize)
	{
		return LXC_ERR_INVALID_INPUT;
	}
	fftPlan->specific_fftPlan = NULL;
	fftPlan->fftSize = 0;

	// reset fftw library
  if (g_FirstPlan)
  {
    fftwf_cleanup();
    g_FirstPlan = 0;
  }

	FFTWF_HANDLE* fftwfHandle = (FFTWF_HANDLE*)malloc(sizeof(FFTWF_HANDLE));
	if(!fftwfHandle)
	{
		// ToDo: show some error message
    return LXC_ERR_DYNAMIC_MEMORY;
	}
	fftwfHandle->x_in	  = NULL;
	fftwfHandle->x_out	  = NULL;
	fftwfHandle->X		  = NULL;
	fftwfHandle->fftPlan  = NULL;
	fftwfHandle->ifftPlan = NULL;

	// allocate fftw float complex data
	// for details see http://www.fftw.org/doc/SIMD-alignment-and-fftw_005fmalloc.html
	// fftw_alloc_real(N) and fftw_alloc_complex(N) that are equivalent to 
	// (double*)fftw_malloc(sizeof(double) * N) and (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N)
	fftwf_complex *x_in = fftwf_alloc_complex(FreqSize);
	if(!x_in)
	{
		// ToDo: show some error message
		free(fftwfHandle);
		return LXC_ERR_DYNAMIC_MEMORY;
	}

	fftwf_complex *x_out = fftwf_alloc_complex(FreqSize);
	if(!x_out)
	{
		// ToDo: show some error message
		fftwf_free(x_in);
		free(fftwfHandle);
    return LXC_ERR_DYNAMIC_MEMORY;
	}

	fftwf_complex *X = fftwf_alloc_complex(FreqSize);
	if(!X)
	{
		// ToDo: show some error message
		fftwf_free(x_in);
		fftwf_free(x_out);
		free(fftwfHandle);
    return LXC_ERR_DYNAMIC_MEMORY;
	}

	// initialize fft buffers
	for(uint ii=0; ii < FreqSize; ii++)
	{
		x_in[ii][0] = 0.0f;
		x_in[ii][1] = 0.0f;

		x_out[ii][0] = 0.0f;
		x_out[ii][1] = 0.0f;

		X[ii][0] = 0.0f;
		X[ii][1] = 0.0f;
	}

	char newPlans = 0;

  size_t strHomeLen = strlen(g_LXC_HomePath);
  strHomeLen++;
  size_t strWisdomLen = strlen(FFTW_WISDOM_FILENAME);
  char *wisdomPath = (char*)malloc(sizeof(char)*(strHomeLen + strWisdomLen));
  if (!wisdomPath)
  {
    return LXC_ERR_DYNAMIC_MEMORY;
  }
  strncpy(wisdomPath, g_LXC_HomePath, strHomeLen);
  strcat(wisdomPath, FFTW_WISDOM_FILENAME);

	// init fftw library and plans
	// fftwf wisdom http://www.fftw.org/doc/Words-of-Wisdom_002dSaving-Plans.html
	// save plan to file "int fftw_export_wisdom_to_filename(const char *filename);" returns non 0 on success
	// load plans from file "int fftw_import_wisdom_from_filename(const char *filename);" returns non 0 on success
  if (fftwf_import_wisdom_from_filename(wisdomPath))
	{// create fftw plan with wisdom informations
		// for detailed description of the planar flags see http://www.fftw.org/doc/Planner-Flags.html
		// FFTW_PATIENT use this to get the best algorithm to create the fastest fftw plan
		// FFTW_WISDOM_ONLY use only wisdom data for fftw plans

		fftwfHandle->fftPlan = fftwf_plan_dft_1d( (int)FreqSize, x_in, X, FFTW_FORWARD, FFTW_WISDOM_ONLY | FFTW_PATIENT );
		if( !fftwfHandle->fftPlan )
		{
			fftwfHandle->fftPlan = fftwf_plan_dft_1d( (int)FreqSize, x_in, X, FFTW_FORWARD, FFTW_PATIENT );
			newPlans++;
		}

		fftwfHandle->ifftPlan = fftwf_plan_dft_1d( (int)FreqSize, X, x_out, FFTW_BACKWARD, FFTW_WISDOM_ONLY | FFTW_PATIENT );
		if( !fftwfHandle->ifftPlan )
		{
			fftwfHandle->ifftPlan = fftwf_plan_dft_1d( (int)FreqSize, X, x_out, FFTW_BACKWARD, FFTW_PATIENT );
			newPlans++;
		}

		if( newPlans <= 0 )
		{
			// ToDo: show some log message
			//cout << "fftw plans successfull loaded from wisdom file: " << m_fftwWisdomFile << endl;
		}
	}
	else
	{
		fftwfHandle->fftPlan = fftwf_plan_dft_1d( (int)FreqSize, x_in, X, FFTW_FORWARD, FFTW_PATIENT );
		fftwfHandle->ifftPlan = fftwf_plan_dft_1d( (int)FreqSize, X, x_out, FFTW_BACKWARD, FFTW_PATIENT );
		newPlans += 2;
	}

	if( !fftwfHandle->ifftPlan || !fftwfHandle->ifftPlan )
	{
		// ToDo: show some error message
		//throw EXCEPTION_COUT_HANDLER("fftw plans not created! FATAL ERROR!!!");
		fftwf_free(x_in);
		fftwf_free(x_out);
		fftwf_free(X);
		free(fftwfHandle);
    free(wisdomPath);
    return LXC_ERR_FFTWF_PLAN_CREATION;
	}

	if( newPlans > 0 )
	{// save that measured plans in the file
		// save fftwf plan with wisdom
    if (!fftwf_export_wisdom_to_filename(wisdomPath))
		{
			// ToDo: show some warning

			//cout << "can't save fftw plans to wisdom file: " << FFTW_WISDOM_FILENAME << endl;
			//fftwf_free(x_in);
			//fftwf_free(x_out);
			//fftwf_free(X);
			//free(fftwfHandle);
   //   return LXC_ERR_FFTWF_WISDOM_EXPORT;
		}
		else
		{
			// ToDo: show some log message
			//cout << "fftw new parameters saved to wisdom file: " << FFTW_WISDOM_FILENAME << endl;
		}
	}

	// ToDo: show some log message
	//cout << "successfull initialized fftw for " << m_Name << endl;

	fftwfHandle->x_in = x_in;
	fftwfHandle->x_out = x_out;
	fftwfHandle->X = X;
	fftPlan->specific_fftPlan = (void*)fftwfHandle;
	fftPlan->fftSize = FreqSize;
	fftPlan->maxInputFrameLength = TimeSize;
	fftPlan->fftZeros = FreqSize - fftPlan->maxInputFrameLength;
	fftPlan->scaleFactor = 1.0f/((float)FreqSize);
  free(wisdomPath);

	return LXC_NO_ERR;
}

LXC_ERROR_CODE destroy_fftwf(LXC_FFT_PLAN *fftHandle)
{
	if(!fftHandle || !fftHandle->specific_fftPlan)
	{
		return LXC_ERR_INVALID_INPUT;
	}
	FFTWF_HANDLE* fftwfHandle = (FFTWF_HANDLE*)fftHandle->specific_fftPlan;

	if(fftwfHandle->x_in)
	{
		fftwf_free(fftwfHandle->x_in);
	}
	fftwfHandle->x_in = NULL;

	if(fftwfHandle->x_out)
	{
		fftwf_free(fftwfHandle->x_out);
	}
	fftwfHandle->x_out = NULL;

	if(fftwfHandle->X)
	{
		fftwf_free(fftwfHandle->X);
	}
	fftwfHandle->X = NULL;

	// destroy fftwf plans
	fftwf_destroy_plan(fftwfHandle->fftPlan);
	fftwf_destroy_plan(fftwfHandle->ifftPlan);

	// destroy fftHandle
	if(fftHandle->specific_fftPlan)
	{
		free(fftHandle->specific_fftPlan);
	}
	fftHandle->specific_fftPlan = NULL;

	return LXC_NO_ERR;
}


LXC_ERROR_CODE fftwf_clearBuffers(LXC_FFT_PLAN *fftPlan)
{
	if(!fftPlan || !fftPlan->specific_fftPlan)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	fftwf_complex *x_in = ((FFTWF_HANDLE*)fftPlan->specific_fftPlan)->x_in;
	fftwf_complex *x_out = ((FFTWF_HANDLE*)fftPlan->specific_fftPlan)->x_out;
	fftwf_complex *X = ((FFTWF_HANDLE*)fftPlan->specific_fftPlan)->X;

	if(!X || !x_in || !x_out)
	{
		return LXC_ERR_NULL_POINTER;
	}

	for(uint ii=0; ii < fftPlan->fftSize; ii++)
	{
		x_in[ii][0] = 0.0f;
		x_in[ii][0] = 0.0f;

		x_out[ii][0] = 0.0f;
		x_out[ii][0] = 0.0f;

		X[ii][0] = 0.0f;
		X[ii][0] = 0.0f;
	}

	return LXC_NO_ERR;
}


LXC_ERROR_CODE fftwf_fft(LXC_FFT_PLAN *fftHandle)
{
	if(!fftHandle || !fftHandle->specific_fftPlan)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	FFTWF_HANDLE* fftwfHandle = (FFTWF_HANDLE*)fftHandle->specific_fftPlan;
	fftwf_execute_dft(fftwfHandle->fftPlan, fftwfHandle->x_in, fftwfHandle->X);
	
	return LXC_NO_ERR;
}


LXC_ERROR_CODE fftwf_ifft(LXC_FFT_PLAN *fftHandle)
{
	if(!fftHandle || !fftHandle->specific_fftPlan)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	FFTWF_HANDLE* fftwfHandle = (FFTWF_HANDLE*)fftHandle->specific_fftPlan;
	fftwf_execute_dft(fftwfHandle->ifftPlan, fftwfHandle->X, fftwfHandle->x_out);

	return LXC_NO_ERR;
}