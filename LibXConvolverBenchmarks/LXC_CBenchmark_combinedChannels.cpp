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



#include "LXC_CBenchmark_combinedChannels.h"
#include "../LibXConvolverUtils/LXC_Exceptions/LXC_CExceptionCout.h"
#include "../LibXConvolverCore/include/LXC_Core.h"
#include "../LibXConvolverCore/LXC_fftHandles/LXC_fftHandles.h"

LXC_CBenchmark_combinedChannels::LXC_CBenchmark_combinedChannels(uint InputFrameLength, 
														 uint SampleFrequency, 
														 LXC_OPTIMIZATION_MODULE OptModule, 
														 uint MaxItererations,
														 LXC_FFT_MODULE fftModule) :
	LXC_IBenchmark("LXC combined channels")
{
	if(!InputFrameLength || !SampleFrequency || !MaxItererations)
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Invalid initialization parameters! Please proof that InputFrameLength>0, SampleFrequency>0 and Itererations>0.");
	}

	m_InputFrameLength = InputFrameLength;
	m_SampleFrequency = SampleFrequency;
	m_MaxIterations = MaxItererations;

	m_OptModule = OptModule;
	m_fftModule = fftModule;

	// initialize filter
	m_InSignal[0] = new float[m_InputFrameLength];
	if(!m_InSignal[0])
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Could not create dynamic buffer! Not enough free dynamic memory?");
	}
	m_InSignal[1] = new float[m_InputFrameLength];
	if(!m_InSignal[1])
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Could not create dynamic buffer! Not enough free dynamic memory?");
	}
	m_OutSignal[0] = new float[m_InputFrameLength];
	if(!m_OutSignal[0])
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Could not create dynamic buffer! Not enough free dynamic memory?");
	}
	m_OutSignal[1] = new float[m_InputFrameLength];
	if(!m_OutSignal[1])
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Could not create dynamic buffer! Not enough free dynamic memory?");
	}

	for(uint ii=0; ii < m_InputFrameLength; ii++)
	{
		m_InSignal[0][ii] = 1.0f;
		m_InSignal[1][ii] = 1.0f;

		m_OutSignal[0][ii] = 0.0f;
		m_OutSignal[1][ii] = 0.0f;
	}

	// create LXC filter
	LXC_ptrFilterHandle *filter = LXC_Core_createFilter2Ch(m_InSignal[0], m_InputFrameLength, 
														   m_InSignal[1], m_InputFrameLength,
														   m_SampleFrequency);
	if(!filter)
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Could not create 2 channel filter! Not enough free dynamic memory?");
	}

	// get LXC convolver
	m_lxcHandle = LXC_Core_getConvolver(filter, m_InputFrameLength, m_OptModule, m_fftModule);
	if(!m_lxcHandle)
	{
		// Todo: LXC error string
		throw LXC_EXCEPTION_COUT_HANDLER("Could not create LXC convolver");
	}
	
	LXC_Core_destroyFilter(&filter);
}

LXC_CBenchmark_combinedChannels::~LXC_CBenchmark_combinedChannels()
{
	if(m_InSignal[0])
	{
		delete [] m_InSignal[0];
		m_InSignal[0] = NULL;
	}

	if(m_InSignal[1])
	{
		delete [] m_InSignal[1];
		m_InSignal[1] = NULL;
	}

	if(m_OutSignal[0])
	{
		delete [] m_OutSignal[0];
		m_OutSignal[0] = NULL;
	}

	if(m_OutSignal[1])
	{
		delete [] m_OutSignal[1];
		m_OutSignal[1] = NULL;
	}

	if(m_lxcHandle)
	{
		LXC_Core_destroy(m_lxcHandle);
		m_lxcHandle = NULL;
	}
}
	
double LXC_CBenchmark_combinedChannels::RunBenchmark()
{
	double elapsedTime = 0.0;
	uint loopIteration = 0;
	do
	{
		m_Timer.start_Timer();
		LXC_ERROR_CODE err = LXC_Core_convolve2Ch(m_lxcHandle, 
												  m_InSignal[0], m_InSignal[1],
												  m_OutSignal[0], m_OutSignal[1]);
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