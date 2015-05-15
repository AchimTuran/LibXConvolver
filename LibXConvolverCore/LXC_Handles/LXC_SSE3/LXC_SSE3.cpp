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



#include "../../include/LXC_Core_types.h"
#include "../../include/LXC_CommonTypes.h"
#include <pmmintrin.h>
#include "LXC_SSE3_types.h"
#include "LXC_SSE3Buffer.h"
#include "LXC_SSE3RingBuffer.h"
#include "LXC_SSE3.h"

#if defined(TARGET_LINUX)
  #pragma GCC target("sse3")
#endif

// Macro definitions
#define LXC_MM_SHUFFLE(fp0,fp1,fp2,fp3) (((fp3) << 6) | ((fp2) << 4) | ((fp1) << 2) | ((fp0)))

// Function implementations
LXC_ERROR_CODE LXC_get_SSE3AllCallbacks(LXC_HANDLE *LXCHandle)
{
	if(!LXCHandle)
	{
		// ToDo: show some error message
		return LXC_ERR_INVALID_INPUT;
	}
	LXC_ERROR_CODE err = LXC_NO_ERR;

	// get LXC callbacks
	err = LXC_get_SSE3ConvolutionCallbacks(&(LXCHandle->LXC_callbacks));
	if(err != LXC_NO_ERR)
	{
		return err;
	}

	// get buffer callbacks
	err = LXC_get_SSE3BufferCallbacks(&(LXCHandle->LXC_Handle.LXC_bufferCallbacks));
	if(err != LXC_NO_ERR)
	{
		return err;
	}

	// get ringbuffer callbacks
	err = LXC_get_SSE3RingbufferCallbacks(&(LXCHandle->LXC_Handle.LXC_ringbufferCallbacks));
	if(err != LXC_NO_ERR)
	{
		return err;
	}

	return LXC_NO_ERR;
}

LXC_ERROR_CODE LXC_get_SSE3ConvolutionCallbacks(LXC_CALLBACKS *Callbacks)
{
  if (!Callbacks)
  {
    // ToDo: show some error message
    return LXC_ERR_INVALID_INPUT;
  }

  // get LXC callbacks
  Callbacks->LXC_CpxMul = LXC_SSE3CpxMul_K2;
  Callbacks->LXC_CpxAdd = LXC_SSE3CpxAdd;
  Callbacks->LXC_FreqCombine2Ch = LXC_SSE3FreqCombine2Ch;
  Callbacks->LXC_FreqSplit2Ch = LXC_SSE3FreqSplit2Ch;

  return LXC_NO_ERR;
}

LXC_ERROR_CODE LXC_get_SSE3BufferCallbacks(LXC_BUFFER_CALLBACKS *Buffer)
{
	if(!Buffer)
	{
		// ToDo: show some error message
		return LXC_ERR_INVALID_INPUT;
	}

	//// get buffer callbacks
	Buffer->LXC_Buffer_getPart = LXC_SSE3Buffer_getPart;
	Buffer->LXC_Buffer_create = LXC_SSE3Buffer_create;
	Buffer->LXC_Buffer_destroy = LXC_SSE3Buffer_destroy;

	return LXC_NO_ERR;
}

LXC_ERROR_CODE LXC_get_SSE3RingbufferCallbacks(LXC_RINGBUFFER_CALLBACKS *Ringbuffer)
{
	if(!Ringbuffer)
	{
		// ToDo: show some error message
		return LXC_ERR_INVALID_INPUT;
	}

	// get ringbuffer callbacks
	Ringbuffer->LXC_Ringbuffer_getPart = LXC_SSE3Ringbuffer_getPart;
	Ringbuffer->LXC_Ringbuffer_getNextPart = LXC_SSE3Ringbuffer_getNextPart;
	Ringbuffer->LXC_Ringbuffer_create = LXC_SSE3Ringbuffer_create;
	Ringbuffer->LXC_Ringbuffer_destroy = LXC_SSE3Ringbuffer_destroy;

	return LXC_NO_ERR;
}

//-----------------------------------------------------------------------------------------
// SSE3 complex multiplication with different kernel sizes
//-----------------------------------------------------------------------------------------
// SSE3 2x complex multiplication (for details see example 6-9 in Intel 64 and IA-32 Architectures Optimization Reference Manual)
// complex multiplication is defined as: (a+jb)*(c+jd) = a*c - b*d + j(a*d + b*c)
// z1 = a1*c1 - b1*d1 + j(a1*d1 + b1*c1)
// z2 = a2*c2 - b2*d2 + j(a2*d2 + b2*c2)
// A = { a1, jb1, c1, jd1 }
// B = { a2, jb2, c2, jd2 }
// C = { Re{z1}, Im{z1}, Re{z2}, Im{z2} } = { a1*c1 - b1*d1, j(a1*d1 + b1*c1), a2*c2 - b2*d2, j(a2*d2 + b2*c2) }
LXC_ERROR_CODE LXC_SSE3CpxMul_K2(uint Size, void *X, void *H, void *Z)
{
	if(!X || !H || !Z)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	Size = Size*2;
	float *m_X = (float*)X;
	float *m_H = (float*)H;
	float *m_Z = (float*)Z;
	for(uint ii=0; ii < Size; ii+=4)
	{
		// local variables
		__m128 val1;
		__m128 val2;
		//__m128 val3;
		//__m128 val4;

		// load values into __m128
		val1 = _mm_load_ps(&m_X[ii]);			// _mm_load_ps:		src{ a1, b1, a2, b2 } --> val1 { a1, b1, a2, b2 }
		val2 = _mm_load_ps(&m_H[ii]);			// _mm_load_ps:		src{ c1, d1, c2, d2 } --> val2 { c1, d1, c2, d2 }
	
		// add/subtract, scale and store operations
		// duplicate values
		// _A1 = _mm_moveldup_ps: src{ a1, b1, a2, b2 } --> val2 { a1, a1, a2, a2 }
		// _A2 = _mm_movehdup_ps:	src{ a1, b1, a2, b2 } --> val3 { b1, b1, b2, b2 }
		// a = calc { a1*c1, a1*d1, a2*c2, a2*d2 } --> sse3 multiply
		// b = reorder im and re numbers { c1, d1, c2, d2 } --> { d1, c1, d2, c2 } and multiply { b1*d1, b1*c1, b2*d2, b2*c2 }
		// A = _mm_addsub_ps: ret { a1*c1 - b1*d1, j(a1*d1 + b1*c1), a2*c2 - b2*d2, j(a2*d2 + b2*c2) }
		// _mm_store_ps: C[0] = result0, C[1] = result1, C[2] = result2, C[3] = result3
		_mm_store_ps(&m_Z[ii], _mm_addsub_ps(_mm_mul_ps(_mm_moveldup_ps(val1), val2), _mm_mul_ps(_mm_movehdup_ps(val1), _mm_shuffle_ps(val2, val2, _MM_SHUFFLE(2,3,0,1)))));


		// old loop
		//// local variables
		//__m128 val1;
		//__m128 val2;
		//__m128 val3;
		//__m128 val4;

		//// load values into __m128
		//val1 = _mm_load_ps(&m_X[ii]);			// _mm_load_ps:		src{ a1, b1, a2, b2 } --> val1 { a1, b1, a2, b2 }
		//val2 = _mm_load_ps(&m_H[ii]);			// _mm_load_ps:		src{ c1, d1, c2, d2 } --> val2 { c1, d1, c2, d2 }

		//// duplicate values
		//val3 = _mm_moveldup_ps(val1);			// _mm_moveldup_ps: src{ a1, b1, a2, b2 } --> val2 { a1, a1, a2, a2 }
		//val4 = _mm_movehdup_ps(val1);			// _mm_movehdup_ps:	src{ a1, b1, a2, b2 } --> val3 { b1, b1, b2, b2 }

		//// sse3 multiply
		//val1 = _mm_mul_ps(val3, val2);			// calc { a1*c1, a1*d1, a2*c2, a2*d2 }
		//// reorder im and re numbers { c1, d1, c2, d2 } --> { d1, c1, d2, c2 } and multiply { b1*d1, b1*c1, b2*d2, b2*c2 }
		//val3 = _mm_mul_ps(val4, _mm_shuffle_ps(val2, val2, _MM_SHUFFLE(2,3,0,1)));
	
		//// add/subtract, scale and store operations
		//val3 = _mm_addsub_ps(val1, val3);		// _mm_addsub_ps: ret { a1*c1 - b1*d1, j(a1*d1 + b1*c1), a2*c2 - b2*d2, j(a2*d2 + b2*c2) }
		//_mm_store_ps(&m_Z[ii], val3);			// _mm_store_ps: C[0] = result0, C[1] = result1, C[2] = result2, C[3] = result3
	}

	return LXC_NO_ERR;
}

//LXC_ERROR_CODE LXC_SSE3CpxMul_K4(uint Size, void *X, void *H, void *Z)
//{
//	if(!X || !H || !Z)
//	{
//		return LXC_ERR_INVALID_INPUT;
//	}
//
//	LXC_SSE3cpxFloat *m_X = (LXC_SSE3cpxFloat*)X;
//	LXC_SSE3cpxFloat *m_H = (LXC_SSE3cpxFloat*)H;
//	LXC_SSE3cpxFloat *m_Z = (LXC_SSE3cpxFloat*)Z;
//	for(uint ii=0; ii < Size; ii+=4)
//	{
//		float* m_fX = (float*)&m_X[ii];
//		float* m_fH = (float*)&m_H[ii];
//		float* m_fZ = (float*)&m_Z[ii];
//
//		LXC_SSE3HelperCpxMul(m_fX, m_fH, m_fZ);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 4);
//	}
//
//	return LXC_NO_ERR;
//}
//
//LXC_ERROR_CODE LXC_SSE3CpxMul_K8(uint Size, void *X, void *H, void *Z)
//{
//	if(!X || !H || !Z)
//	{
//		return LXC_ERR_INVALID_INPUT;
//	}
//
//	LXC_SSE3cpxFloat *m_X = (LXC_SSE3cpxFloat*)X;
//	LXC_SSE3cpxFloat *m_H = (LXC_SSE3cpxFloat*)H;
//	LXC_SSE3cpxFloat *m_Z = (LXC_SSE3cpxFloat*)Z;
//	for(uint ii=0; ii < Size; ii+=8)
//	{
//		float* m_fX = (float*)&m_X[ii];
//		float* m_fH = (float*)&m_H[ii];
//		float* m_fZ = (float*)&m_Z[ii];
//
//		LXC_SSE3HelperCpxMul(m_fX, m_fH, m_fZ);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 4);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 8);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 12);
//	}
//
//	return LXC_NO_ERR;
//}
//
//LXC_ERROR_CODE LXC_SSE3CpxMul_K16(uint Size, void *X, void *H, void *Z)
//{
//	if(!X || !H || !Z)
//	{
//		return LXC_ERR_INVALID_INPUT;
//	}
//
//	LXC_SSE3cpxFloat *m_X = (LXC_SSE3cpxFloat*)X;
//	LXC_SSE3cpxFloat *m_H = (LXC_SSE3cpxFloat*)H;
//	LXC_SSE3cpxFloat *m_Z = (LXC_SSE3cpxFloat*)Z;
//	for(uint ii=0; ii < Size; ii+=16)
//	{
//		float* m_fX = (float*)&m_X[ii];
//		float* m_fH = (float*)&m_H[ii];
//		float* m_fZ = (float*)&m_Z[ii];
//
//		LXC_SSE3HelperCpxMul(m_fX, m_fH, m_fZ);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 4);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 8);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 12);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 16);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 20);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 24);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 28);
//	}
//
//	return LXC_NO_ERR;
//}
//
//LXC_ERROR_CODE LXC_SSE3CpxMul_K32(uint Size, void *X, void *H, void *Z)
//{
//	if(!X || !H || !Z)
//	{
//		return LXC_ERR_INVALID_INPUT;
//	}
//
//	LXC_SSE3cpxFloat *m_X = (LXC_SSE3cpxFloat*)X;
//	LXC_SSE3cpxFloat *m_H = (LXC_SSE3cpxFloat*)H;
//	LXC_SSE3cpxFloat *m_Z = (LXC_SSE3cpxFloat*)Z;
//	for(uint ii=0; ii < Size; ii+=32)
//	{
//		float* m_fX = (float*)&m_X[ii];
//		float* m_fH = (float*)&m_H[ii];
//		float* m_fZ = (float*)&m_Z[ii];
//
//		LXC_SSE3HelperCpxMul(m_fX, m_fH, m_fZ);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 4);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 8);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 12);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 16);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 20);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 24);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 28);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 32);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 36);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 40);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 44);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 48);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 52);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 56);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 60);
//	}
//
//	return LXC_NO_ERR;
//}
//
//LXC_ERROR_CODE LXC_SSE3CpxMul_K64(uint Size, void *X, void *H, void *Z)
//{
//	if(!X || !H || !Z)
//	{
//		return LXC_ERR_INVALID_INPUT;
//	}
//
//	LXC_SSE3cpxFloat *m_X = (LXC_SSE3cpxFloat*)X;
//	LXC_SSE3cpxFloat *m_H = (LXC_SSE3cpxFloat*)H;
//	LXC_SSE3cpxFloat *m_Z = (LXC_SSE3cpxFloat*)Z;
//	for(uint ii=0; ii < Size; ii+=64)
//	{
//		float* m_fX = (float*)&m_X[ii];
//		float* m_fH = (float*)&m_H[ii];
//		float* m_fZ = (float*)&m_Z[ii];
//
//		LXC_SSE3HelperCpxMul(m_fX, m_fH, m_fZ);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 4);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 8);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 12);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 16);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 20);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 24);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 28);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 32);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 36);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 40);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 44);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 48);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 52);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 56);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 60);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 64);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 68);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 72);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 76);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 80);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 84);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 88);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 92);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 96);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 100);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 104);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 108);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 112);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 116);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 120);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 124);
//	}
//
//	return LXC_NO_ERR;
//}
//
//LXC_ERROR_CODE LXC_SSE3CpxMul_K128(uint Size, void *X, void *H, void *Z)
//{
//	if(!X || !H || !Z)
//	{
//		return LXC_ERR_INVALID_INPUT;
//	}
//
//	LXC_SSE3cpxFloat *m_X = (LXC_SSE3cpxFloat*)X;
//	LXC_SSE3cpxFloat *m_H = (LXC_SSE3cpxFloat*)H;
//	LXC_SSE3cpxFloat *m_Z = (LXC_SSE3cpxFloat*)Z;
//	for(uint ii=0; ii < Size; ii+=128)
//	{
//		float* m_fX = (float*)&m_X[ii];
//		float* m_fH = (float*)&m_H[ii];
//		float* m_fZ = (float*)&m_Z[ii];
//
//		LXC_SSE3HelperCpxMul(m_fX, m_fH, m_fZ);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 4);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 8);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 12);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 16);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 20);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 24);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 28);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 32);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 36);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 40);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 44);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 48);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 52);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 56);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 60);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 64);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 68);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 72);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 76);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 80);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 84);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 88);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 92);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 96);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 100);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 104);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 108);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 112);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 116);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 120);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 124);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 128);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 132);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 136);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 140);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 144);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 148);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 152);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 156);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 160);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 164);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 168);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 172);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 176);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 180);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 184);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 188);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 192);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 196);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 200);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 204);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 208);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 212);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 216);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 220);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 224);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 228);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 232);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 236);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 240);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 244);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 248);
//		LXC_SSE3HelperCpxMul_offset(m_fX, m_fH, m_fZ, 252);
//	}
//
//	return LXC_NO_ERR;
//}

LXC_ERROR_CODE LXC_SSE3CpxAdd(LXC_BUFFER *ResultBuffer, float ScaleFactor)
{
	if(!ResultBuffer)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	const uint partSize = ResultBuffer->maxFilterPartLength;
	const uint maxParts = ResultBuffer->maxFilterParts;
	const uint size = partSize*2;

	float *Z0 = (float*)LXC_SSE3Buffer_getPart(ResultBuffer, 0);
	for(uint part=1; part < maxParts; part++)
	{
		float *Zi = (float*)LXC_SSE3Buffer_getPart(ResultBuffer, part);
		for(uint ii=0; ii < size; ii+=4)
		{
			//Z0[ii][0] += Zi[ii][0];
			//Z0[ii][1] += Zi[ii][1];
			//__m128 _Zi = _mm_load_ps(&Zi[ii]);
			//__m128 _Z0 = _mm_load_ps(&Z0[ii]);
			//_mm_store_ps(&Z0[ii], _mm_add_ps(_Z0, _Zi));
			
			//__m128 _Zi = _mm_load_ps(&Zi[ii]);
			//__m128 _Z0 = _mm_load_ps(&Z0[ii]);
			_mm_store_ps(&Z0[ii], _mm_add_ps(_mm_load_ps(&Z0[ii]), _mm_load_ps(&Zi[ii])));
		}
	}

  if (ScaleFactor != 1.0f)
	{
		//const LXC_SSE3Float scaleFactor = 1.0f / ((float)partSize);
    const LXC_SSE3Float scaleFactor = ScaleFactor;
		__m128 _scale = _mm_load1_ps(&scaleFactor);
		for(uint ii=0; ii < size; ii+=4)
		{
			//Z0[ii][0] *= scaleFactor;
			//Z0[ii][1] *= scaleFactor;
			__m128 _Z0 = _mm_load_ps(&Z0[ii]);
			_mm_store_ps(&Z0[ii], _mm_mul_ps(_Z0, _scale));
		}
	}


	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_SSE3FreqCombine2Ch(uint Size, void *X, void *Y, void *Z)
{
	if(!Size || !X || !Y || !Z)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	float *m_X = (float*)X;
	float *m_Y = (float*)Y;
	float *m_Z = (float*)Z;

#if defined(TARGET_WINDOWS)
	const __declspec(align(LXC_SSE3_ALIGN)) float  scaleFactor = 1.0f / ((float)Size);
#else
	const float  scaleFactor = 1.0f / ((float)Size);
#endif
	Size = Size*2;
	__m128 _scale = _mm_load1_ps(&scaleFactor);

	for(uint ii = 0; ii < Size; ii+=4)
	{
		//m_Z[ii][0] = (m_X[ii][0] - m_Y[ii][1])*scaleFactor;
		//m_Z[ii][1] = (m_X[ii][1] + m_Y[ii][0])*scaleFactor;
		//m_Z[ii][0] = (m_X[ii+1][0] - m_Y[ii+1][1])*scaleFactor;
		//m_Z[ii][1] = (m_X[ii+1][1] + m_Y[ii+1][0])*scaleFactor;
		//__m128 A = _mm_load_ps(&m_X[ii]);
		__m128 B = _mm_load_ps(&m_Y[ii]);
		B = _mm_shuffle_ps(B, B, LXC_MM_SHUFFLE(1,0,3,2));
		__m128 addRes = _mm_addsub_ps (_mm_load_ps(&m_X[ii]), B);
		_mm_store_ps(&m_Z[ii], _mm_mul_ps(addRes, _scale));
	}


	return LXC_NO_ERR;
}


LXC_ERROR_CODE LXC_SSE3FreqSplit2Ch(uint Size, void *Z, void *X, void *Y)
{
	if(!Size || !Z || !X || !Y)
	{
		return LXC_ERR_INVALID_INPUT;
	}

	float *m_X = (float*)X;
	float *m_Y = (float*)Y;
	float *m_Z = (float*)Z;
	
	Size = Size*2;
#if defined(TARGET_WINDOWS)
	const __declspec(align(LXC_SSE3_ALIGN)) float  scaleFactor = 0.5f;
#else
	const float  scaleFactor = 0.5f;
#endif
	__m128 scale_05 = _mm_load1_ps(&scaleFactor);
	__m128 XY0 = _mm_setr_ps(m_Z[0], 0.0f, m_Z[1], 0.0f);
	// [0]=Z[0][0], [1]=0.0f, [2]=Z[0][1], [3]=0.0f

	__m128 _m128Z = _mm_load_ps(&m_Z[0]);
	__m128 _m128Z_Size = _mm_loadl_pi(_m128Z, (__m64*)&m_Z[Size-2]);
	// [0]=Z[Size-1][0], [1]=Z[Size-1][1], [2]=Z[1][0], [3]=Z[1][1]

	__m128 leftNumbers = _mm_shuffle_ps(_m128Z_Size, _m128Z_Size, LXC_MM_SHUFFLE(3,2,0,3));
	// [0]=Z[1][1], [1]=Z[1][0], [2]=Z[Size-1][0], [3]=Z[1][1]

	__m128 rightNumbers = _mm_shuffle_ps(_m128Z_Size, _m128Z_Size, LXC_MM_SHUFFLE(1,0,2,1));
	// [0]=Z[Size-1][1], [1]=Z[Size-1][0], [2]=Z[1][0], [3]=Z[Size-1][1]

	__m128 mulAddSubRes = _mm_mul_ps(_mm_addsub_ps(leftNumbers, rightNumbers), scale_05);
	// [0]=(Z[1][1] - Z[Size-1][1])*0.5f=X[1][1]
	// [1]=(Z[1][0] + Z[Size-1][0])*0.5f=X[1][0]
	// [2]=(Z[Size-1][0] - Z[1][0])*0.5f=Y[1][1]
	// [3]=(Z[1][1] + Z[Size-1][1])*0.5f=Y[1][0]

	_mm_store_ps(&m_X[0], _mm_shuffle_ps(XY0, mulAddSubRes, LXC_MM_SHUFFLE(0,1,1,0)));
	// [0]=X[0][0]=Z[0][0]
	// [1]=X[0][1]=0.0f
	// [2]=X[1][0]=(m_Z[kk][0] + m_Z[L_minus_K][0])*0.5f
	// [3]=X[1][1]=(m_Z[kk][1] - m_Z[L_minus_K][1])*0.5f

	_mm_store_ps(&m_Y[0], _mm_shuffle_ps(XY0, mulAddSubRes, LXC_MM_SHUFFLE(2,3,3,2)));
	// [0]=Y[0][0]=Z[0][1]
	// [1]=Y[0][1]=0.0f
	// [2]=Y[1][0]=(m_Z[kk][0] + m_Z[L_minus_K][0])*0.5f
	// [3]=Y[1][1]=(m_Z[kk][1] - m_Z[L_minus_K][1])*0.5f

	for(uint kk = 4; kk < Size; kk+=4)
	{
		//__m128 _Z = {0.0f,1.0f,2.0f,3.0f};
		//__m128 L = {4.0f,5.0f,6.0f,7.0f};
		
		__m128 _Z = _mm_load_ps(&m_Z[kk]);
		// [0]=Z[kk][0], [1]=Z[kk][1], [2]=Z[kk+1][0], [3]=Z[kk+1][1]
		__m128 _ZShuffle = _mm_shuffle_ps(_Z, _Z, LXC_MM_SHUFFLE(1,0,3,2));
		// [0]=Z[kk][1], [1]=Z[kk][0], [2]=Z[kk+1][1], [3]=Z[kk+1][0]
		__m128 _ZSize = _mm_loadu_ps(&(m_Z[Size - kk - 2]));
		// [0]=Z[Size-kk-1][0], [1]=Z[Size-kk-1][1], [2]=Z[Size-kk][0], [3]=Z[Size-kk][1]

		// calculate X signal
		__m128 _ZSizeShuffle = _mm_shuffle_ps(_ZSize, _ZSize, LXC_MM_SHUFFLE(3,2,1,0));
		// [0]=Z[Size-kk][1], [1]=Z[Size-kk][0], [2]=Z[Size-kk-1][1], [3]=Z[Size-kk-1][0]
		__m128 result = _mm_mul_ps(_mm_addsub_ps(_ZShuffle, _ZSizeShuffle), scale_05);
		// [0]=(Z[kk][1] - Z[Size-kk][1])*0.5f=X[kk][1]
		// [1]=(Z[kk][0] + Z[Size-kk][0])*0.5f=X[kk][0]
		// [0]=(Z[kk+1][1] - Z[Size-kk-1][1])*0.5f=X[kk+1][1]
		// [1]=(Z[kk+1][0] + Z[Size-kk-1][0])*0.5f=X[kk+1][0]
		_mm_store_ps(&m_X[kk], _mm_shuffle_ps(result, result, LXC_MM_SHUFFLE(1,0,3,2)));
		// [0]=X[kk][0]  =(Z[kk][0] + Z[Size-kk][0])*0.5f
		// [1]=X[kk][1]  =Z[kk][1] - Z[Size-kk][1])*0.5f
		// [2]=X[kk+1][0]=(Z[kk+1][1] + Z[Size-kk-1][1])*0.5f
		// [3]=X[kk+1][1]=(Z[Size-kk-1][0] - Z[kk+1][0])*0.5f
		
		// calculate Y signal
		__m128 left = _mm_shuffle_ps(_Z, _ZSize, LXC_MM_SHUFFLE(1,3,2,0));
		// [0]=Z[kk][1], [1]=Z[kk+1][1], [2]=Z[Size-kk][0], [3]=Z[Size-kk-1][0]
		left = _mm_shuffle_ps(left, left, LXC_MM_SHUFFLE(2,0,3,1));
		// [0]=Z[Size-kk][0], [1]=Z[kk][1], [2]=Z[Size-kk-1][0], [3]=Z[kk+1][1] 

		__m128 right = _mm_shuffle_ps(_Z, _ZSize, LXC_MM_SHUFFLE(0,2,3,1));
		// [0]=Z[kk][0], [1]=Z[kk+1][0], [2]=Z[Size-kk][1], [3]=Z[Size-kk-1][1]
		right = _mm_shuffle_ps(right, right, LXC_MM_SHUFFLE(0,2,1,3));
		// [0]=Z[kk][0], [1]=Z[Size-kk][1], [2]=Z[kk+1][0], [3]=Z[Size-kk-1][1] 
		
		result = _mm_mul_ps(_mm_addsub_ps(left, right), scale_05);
		// [0]=Y[kk][1]  = 0.5f*(m_Z[Size-kk][0] - m_Z[kk][0]);
		// [1]=Y[kk][0]  = 0.5f*(m_Z[kk][1] + m_Z[Size-kk][1]);
		// [2]=Y[kk+1][1]= 0.5f*(m_Z[Size-kk-1][0] - m_Z[kk+1][0]);
		// [3]=Y[kk+1][0]= 0.5f*(m_Z[kk+1][1] + m_Z[Size-kk-1][1]);

		_mm_store_ps(&m_Y[kk], _mm_shuffle_ps(result, result, LXC_MM_SHUFFLE(1,0,3,2)));
		// [0]=Y[kk][0]  = 0.5f*(m_Z[kk][1] + m_Z[Size-kk][1]);
		// [1]=Y[kk][1]  = 0.5f*(m_Z[Size-kk][0] - m_Z[kk][0]);
		// [2]=Y[kk+1][0]= 0.5f*(m_Z[kk+1][1] + m_Z[Size-kk-1][1]);
		// [3]=Y[kk+1][1]= 0.5f*(m_Z[Size-kk-1][0] - m_Z[kk+1][0]);
	}

	return LXC_NO_ERR;
}
