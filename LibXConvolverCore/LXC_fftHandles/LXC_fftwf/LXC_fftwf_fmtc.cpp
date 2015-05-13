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



#include "LXC_fftwf_fmtc.h"
#include "LXC_fftwfHandle_types.h"
#include "../../LXC_Handles/LXC_Native/LXC_Native_types.h"
#if defined(USE_LXC_SSE3)
	#include "../../LXC_Handles/LXC_SSE3/LXC_SSE3_types.h"
	#include <xmmintrin.h>
#endif

// -------------------------------------
// ---- format conversion functions ----
// -------------------------------------
LXC_ERROR_CODE LXC_fmtc_float_TO_fftwf(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size)
{
	if(!In || !fftHandle_out || Size > fftHandle_out->LXC_fftSize)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	// cast pointers
	fftwf_complex *out = ((LXC_FFTWF_HANDLE*)fftHandle_out->LXC_specific_fftPlan)->x_in;
	float *in = (float*)In;
	const uint inputFramelength = fftHandle_out->LXC_maxInputFrameLength;

	if(Size == inputFramelength)
	{
		fftwf_complex *oldIn = out + fftHandle_out->LXC_maxInputFrameLength;
		for(uint ii=0; ii < fftHandle_out->LXC_fftZeros; ii++)
		{
			out[ii][0] = oldIn[ii][0];
			out[ii][1] = oldIn[ii][1];
		}
		
		out = ((LXC_FFTWF_HANDLE*)fftHandle_out->LXC_specific_fftPlan)->x_in + fftHandle_out->LXC_fftZeros;
	}

	// copy new samples into fft buffer
	for(uint ii=0; ii < Size; ii++)
	{
		out[ii][0] = in[ii];
		out[ii][1] = 0.0f;
	}

	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_fmtc_fftwf_TO_float(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size)
{
	if(!fftHandle_in || !Out || fftHandle_in->LXC_fftSize < Size)
	{
		return LXC_ERR_INVALID_INPUT;
	}
	
	// cast pointers
	fftwf_complex *in = ((LXC_FFTWF_HANDLE*)fftHandle_in->LXC_specific_fftPlan)->x_out;
	float *out = (float*)Out;
	const uint inputFramelength = fftHandle_in->LXC_maxInputFrameLength;

	// copy samples to external format
	if(Size == inputFramelength)
	{
		in += fftHandle_in->LXC_fftZeros;
		for(uint ii=0; ii < inputFramelength; ii++)
		{
			out[ii] = in[ii][0];
		}
	}
	else
	{
		for(uint ii=0; ii < Size; ii++)
		{
			out[ii] = in[ii][0];
		}
	}

	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_fmtc_float_TO_fftwf_2Ch(void *In1, void *In2, LXC_FFT_PLAN *fftHandle_out, uint Size)
{
	if(!In1 || !In2 || !fftHandle_out || fftHandle_out->LXC_fftSize < Size)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	// cast pointers
	fftwf_complex *out = ((LXC_FFTWF_HANDLE*)fftHandle_out->LXC_specific_fftPlan)->x_in;
	float *in1 = (float*)In1;
	float *in2 = (float*)In2;
	const uint inputFramelength = fftHandle_out->LXC_maxInputFrameLength;

	if(Size == inputFramelength)
	{
		fftwf_complex *oldIn = out + fftHandle_out->LXC_maxInputFrameLength;
		for(uint ii=0; ii < fftHandle_out->LXC_fftZeros; ii++)
		{
			out[ii][0] = oldIn[ii][0];
			out[ii][1] = oldIn[ii][1];
		}
		
		out = ((LXC_FFTWF_HANDLE*)fftHandle_out->LXC_specific_fftPlan)->x_in + fftHandle_out->LXC_fftZeros;
	}

	// copy new samples into fft buffer
	for(uint ii=0; ii < Size; ii++)
	{
		out[ii][0] = in1[ii];
		out[ii][1] = in2[ii];
	}



	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_fmtc_fftwf_TO_float_2Ch(LXC_FFT_PLAN *fftHandle_in, void *Out1, void *Out2, uint Size)
{
	if(!fftHandle_in || !Out1 || !Out2 || fftHandle_in->LXC_fftSize < Size)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	// cast pointers
	fftwf_complex *in = ((LXC_FFTWF_HANDLE*)fftHandle_in->LXC_specific_fftPlan)->x_out;
	float *out1 = (float*)Out1;
	float *out2 = (float*)Out2;
	const uint inputFramelength = fftHandle_in->LXC_maxInputFrameLength;	

	// copy samples to external format
	if(Size == inputFramelength)
	{
		in += fftHandle_in->LXC_fftZeros;
		for(uint ii=0; ii < inputFramelength; ii++)
		{
			out1[ii] = in[ii][0];
			out2[ii] = in[ii][1];
		}
	}
	else
	{
		for(uint ii=0; ii < Size; ii++)
		{
			out1[ii] = in[ii][0];
			out2[ii] = in[ii][1];
		}
	}

	return LXC_NO_ERR;
}

#if defined(USE_LXC_NATIVE)
LXC_ERROR_CODE LXC_fmtc_LXCcpxFloat_TO_fftwf(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size)
{
	if(!In || !fftHandle_out || fftHandle_out->LXC_fftSize < Size)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	LXC_cpxFloat *in = (LXC_cpxFloat*)In;
	fftwf_complex *out = ((LXC_FFTWF_HANDLE*)fftHandle_out->LXC_specific_fftPlan)->X;
	for(uint ii=0; ii < Size; ii++)
	{
		out[ii][0] = in[ii][0];
		out[ii][1] = in[ii][1];
	}

	return LXC_NO_ERR;
}
#endif


#if defined(USE_LXC_NATIVE)
LXC_ERROR_CODE LXC_fmtc_fftwf_TO_LXCcpxFloat(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size)
{
	if(!fftHandle_in || !Out || fftHandle_in->LXC_fftSize < Size)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	fftwf_complex *in = ((LXC_FFTWF_HANDLE*)fftHandle_in->LXC_specific_fftPlan)->X;
	LXC_cpxFloat *out = (LXC_cpxFloat*)Out;
	for(uint ii=0; ii < Size; ii++)
	{
		out[ii][0] = in[ii][0];
		out[ii][1] = in[ii][1];
	}

	return LXC_NO_ERR;
}
#endif


#if defined(USE_LXC_SSE3)
LXC_ERROR_CODE LXC_fmtc_LXCcpxSSE3Float_TO_fftwf(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size)
{
	if(!In || !fftHandle_out || fftHandle_out->LXC_fftSize < Size)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	Size = Size*2;
	float *in = (float*)In;
	float *out = (float*)((LXC_FFTWF_HANDLE*)fftHandle_out->LXC_specific_fftPlan)->X;
	for(uint ii=0; ii < Size; ii+=4)
	{
		//out[ii][0] = in[ii][0];
		//out[ii][1] = in[ii][1];
		_mm_store_ps(&out[ii], _mm_load_ps(&in[ii]));
	}

	return LXC_NO_ERR;
}
#endif


#if defined(USE_LXC_SSE3)
LXC_ERROR_CODE LXC_fmtc_fftwf_TO_LXCcpxSSE3Float(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size)
{
	if(!fftHandle_in || !Out || fftHandle_in->LXC_fftSize < Size)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	Size = Size*2;
	float *in = (float*)((LXC_FFTWF_HANDLE*)fftHandle_in->LXC_specific_fftPlan)->X;
	float *out = (float*)Out;
	for(uint ii=0; ii < Size; ii+=4)
	{
		//out[ii][0] = in[ii][0];
		//out[ii][1] = in[ii][1];
		_mm_store_ps(&out[ii], _mm_load_ps(&in[ii]));
	}

	return LXC_NO_ERR;
}
#endif

#if defined(USE_LXC_SSE3)
LXC_ERROR_CODE LXC_fmtc_LXCcpxSSE3Float_TO_fftwf_SSE3_K2(void *In, LXC_FFT_PLAN *fftHandle_out, uint Size)
{
	if(!In || !fftHandle_out || fftHandle_out->LXC_fftSize < Size)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	Size = Size*2;
	float *in = (float*)In;
	float *out = (float*)((LXC_FFTWF_HANDLE*)fftHandle_out->LXC_specific_fftPlan)->X;
	for(uint ii=0; ii < Size; ii+=4)
	{
		//out[ii][0] = in[ii][0];
		//out[ii][1] = in[ii][1];
		//out[ii+1][0] = in[ii+1][0];
		//out[ii+1][1] = in[ii+1][1];
		_mm_store_ps(&out[ii], _mm_load_ps(&in[ii]));
	}

	return LXC_NO_ERR;
}
#endif


#if defined(USE_LXC_SSE3)
LXC_ERROR_CODE LXC_fmtc_fftwf_TO_LXCcpxSSE3Float_SSE3_K2(LXC_FFT_PLAN *fftHandle_in, void *Out, uint Size)
{
	if(!fftHandle_in || !Out || fftHandle_in->LXC_fftSize < Size)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	Size = Size*2;
	float *in = (float*)((LXC_FFTWF_HANDLE*)fftHandle_in->LXC_specific_fftPlan)->X;
	float *out = (float*)Out;
	for(uint ii=0; ii < Size; ii+=4)
	{
		//out[ii][0] = in[ii][0];
		//out[ii][1] = in[ii][1];
		//out[ii+1][0] = in[ii+1][0];
		//out[ii+1][1] = in[ii+1][1];
		_mm_store_ps(&out[ii], _mm_load_ps(&in[ii]));
	}

	return LXC_NO_ERR;
}
#endif
