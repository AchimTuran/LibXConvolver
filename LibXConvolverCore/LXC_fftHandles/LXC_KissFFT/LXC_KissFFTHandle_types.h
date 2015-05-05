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



#include <kiss_fft.h>

typedef struct
{
  kiss_fft_cpx *x_in;	  // time domain buffer
  kiss_fft_cpx *x_out;	// time domain buffer
  kiss_fft_cpx *X;		  // frequency domain buffer

	kiss_fft_cfg fft_cfg;
	kiss_fft_cfg ifft_cfg;
}LXC_KISSFFT_HANDLE;

#define INVERSE_KISS_FFT  1
#define FORWARD_KISS_FFT  0
