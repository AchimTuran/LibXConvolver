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



#include "LXC_IBenchmark.h"
#include "../LibXConvolverCore/LXC_fftHandles/LXC_fftHandles_types.h"

class LXC_CBenchmark_fftModules : public LXC_IBenchmark
{
public:
  LXC_CBenchmark_fftModules(uint fftSize,
                            LXC_FFT_MODULE fftModule,
                            uint MaxIterations=1);

  virtual ~LXC_CBenchmark_fftModules();

  virtual double RunBenchmark();

private:
  uint m_fftSize;
  uint m_MaxIterations;

  float		*m_InSignal;
  float		*m_OutSignal;

  LXC_FFT_MODULE  m_fftModule;
  LXC_FFT_HANDLE  m_fftHandle;
};
