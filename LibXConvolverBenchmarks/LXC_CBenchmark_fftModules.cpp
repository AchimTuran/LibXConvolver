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



#include "LXC_CBenchmark_fftModules.h"

#include "../LibXConvolverUtils/LXC_Exceptions/LXC_CExceptionCout.h"
#include "../LibXConvolverCore/LXC_fftHandles/LXC_fftHandles.h"

LXC_CBenchmark_fftModules::LXC_CBenchmark_fftModules( uint fftSize,
                                                      LXC_FFT_MODULE fftModule,
                                                      uint MaxIterations=1) :
  LXC_IBenchmark("LXC combined channels")
{
  if(!fftSize)
  {
    throw LXC_EXCEPTION_COUT_HANDLER("Invalid fft size!");
  }
  m_fftSize = fftSize;

  LXC_ERROR_CODE err = LXC_get_fftHandle(&m_fftHandle, fftModule, LXC_OPT_NATIVE, m_fftSize, m_fftSize);
  if(err != LXC_NO_ERR)
  {
    throw LXC_EXCEPTION_COUT_HANDLER("Failed getting fftHandle!");
  }
}

LXC_CBenchmark_fftModules::~LXC_CBenchmark_fftModules()
{
}

double LXC_CBenchmark_fftModules::RunBenchmark()
{
  double elapsedTime = 0.0;
    uint loopIteration = 0;
    do
    {
      m_Timer.start_Timer();
      LXC_ERROR_CODE err = 0;
      m_Timer.stop_Timer();
      elapsedTime += m_Timer.get_ElapsedTime();
      if(err != LXC_NO_ERR)
      {
        throw LXC_EXCEPTION_COUT_HANDLER("Combined channels Benchmark skipped!");
      }

      loopIteration++;
    }while(loopIteration < m_MaxIterations);

    return elapsedTime/((double)m_MaxIterations);
}
