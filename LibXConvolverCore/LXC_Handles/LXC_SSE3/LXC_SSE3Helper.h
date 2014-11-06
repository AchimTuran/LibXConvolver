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



#include <intrin.h>		//SSE types
#include <mmintrin.h>	//SSE Intrinsics
//-----------------------------------------------------------------------------------------
// SSE Intrinsic functions
//-----------------------------------------------------------------------------------------
static inline void LXC_SSE3HelperCpxMul(float *A, float *B, float *C);
static inline void LXC_SSE3HelperCpxMul_offset(float *A, float *B, float *C, uint Offset);

// SSE3 2x complex multiplication (for details see example 6-9 in Intel 64 and IA-32 Architectures Optimization Reference Manual)
// complex multiplication is defined as: (a+jb)*(c+jd) = a*c - b*d + j(a*d + b*c)
// z1 = a1*c1 - b1*d1 + j(a1*d1 + b1*c1)
// z2 = a2*c2 - b2*d2 + j(a2*d2 + b2*c2)
// A = { a1, jb1, c1, jd1 }
// B = { a2, jb2, c2, jd2 }
// C = { Re{z1}, Im{z1}, Re{z2}, Im{z2} } = { a1*c1 - b1*d1, j(a1*d1 + b1*c1), a2*c2 - b2*d2, j(a2*d2 + b2*c2) }
static inline void LXC_SSE3HelperCpxMul(float *A, float *B, float *C)
{
	// local variables
	__m128 val1;
	__m128 val2;
	__m128 val3;
	__m128 val4;

	// load values into __m128
	val1 = _mm_load_ps(A);				// _mm_load_ps:		src{ a1, b1, a2, b2 } --> val1 { a1, b1, a2, b2 }
	val2 = _mm_load_ps(B);				// _mm_load_ps:		src{ c1, d1, c2, d2 } --> val2 { c1, d1, c2, d2 }

	// duplicate values
	val3 = _mm_moveldup_ps(val1);		// _mm_moveldup_ps: src{ a1, b1, a2, b2 } --> val2 { a1, a1, a2, a2 }
	val4 = _mm_movehdup_ps(val1);		// _mm_movehdup_ps:	src{ a1, b1, a2, b2 } --> val3 { b1, b1, b2, b2 }

	// sse3 multiply
	val1 = _mm_mul_ps(val3, val2);		// calc { a1*c1, a1*d1, a2*c2, a2*d2 }
	// reorder im and re numbers { c1, d1, c2, d2 } --> { d1, c1, d2, c2 } and multiply { b1*d1, b1*c1, b2*d2, b2*c2 }
	val3 = _mm_mul_ps(val4, _mm_shuffle_ps(val2, val2, _MM_SHUFFLE(2,3,0,1)));
	
	// add/subtract, scale and store operations
	val3 = _mm_addsub_ps(val1, val3);	// _mm_addsub_ps: ret { a1*c1 - b1*d1, j(a1*d1 + b1*c1), a2*c2 - b2*d2, j(a2*d2 + b2*c2) }
	_mm_store_ps(C, val3);				// _mm_store_ps: C[0] = result0, C[1] = result1, C[2] = result2, C[3] = result3
}


// SSE3 2x complex multiplication (for details see example 6-9 in Intel 64 and IA-32 Architectures Optimization Reference Manual)
// complex multiplication is defined as: (a+jb)*(c+jd) = a*c - b*d + j(a*d + b*c)
// z1 = a1*c1 - b1*d1 + j(a1*d1 + b1*c1)
// z2 = a2*c2 - b2*d2 + j(a2*d2 + b2*c2)
// A = { a1, jb1, c1, jd1 }
// B = { a2, jb2, c2, jd2 }
// C = { Re{z1}, Im{z1}, Re{z2}, Im{z2} } = { a1*c1 - b1*d1, j(a1*d1 + b1*c1), a2*c2 - b2*d2, j(a2*d2 + b2*c2) }
static inline void LXC_SSE3HelperCpxMul_offset(float *A, float *B, float *C, uint Offset)
{
	// local variables
	__m128 val1;
	__m128 val2;
	__m128 val3;
	__m128 val4;

	// load values into __m128
	val1 = _mm_load_ps(A + Offset);		// _mm_load_ps:		src{ a1, b1, a2, b2 } --> val1 { a1, b1, a2, b2 }
	val2 = _mm_load_ps(B + Offset);		// _mm_load_ps:		src{ c1, d1, c2, d2 } --> val2 { c1, d1, c2, d2 }

	// duplicate values
	val3 = _mm_moveldup_ps(val1);		// _mm_moveldup_ps: src{ a1, b1, a2, b2 } --> val2 { a1, a1, a2, a2 }
	val4 = _mm_movehdup_ps(val1);		// _mm_movehdup_ps:	src{ a1, b1, a2, b2 } --> val3 { b1, b1, b2, b2 }

	// sse3 multiply
	val1 = _mm_mul_ps(val3, val2);		// calc { a1*c1, a1*d1, a2*c2, a2*d2 }
	// reorder im and re numbers { c1, d1, c2, d2 } --> { d1, c1, d2, c2 } and multiply { b1*d1, b1*c1, b2*d2, b2*c2 }
	val3 = _mm_mul_ps(val4, _mm_shuffle_ps(val2, val2, _MM_SHUFFLE(2,3,0,1)));
	
	// add/subtract and store operations
	val3 = _mm_addsub_ps(val1, val3);	// _mm_addsub_ps: ret { a1*c1 - b1*d1, j(a1*d1 + b1*c1), a2*c2 - b2*d2, j(a2*d2 + b2*c2) }
	_mm_store_ps(C + Offset, val3);		// _mm_store_ps: C[0] = result0, C[1] = result1, C[2] = result2, C[3] = result3
}