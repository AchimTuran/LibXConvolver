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



#include "LXC_CBenchmark_cpxMul.h"
#include "../LibXConvolverUtils/LXC_Exceptions/LXC_CExceptionCout.h"
#include "../LibXConvolverCore/include/LXC_Core.h"

void Helper_ResetCallbacks(LXC_BUFFER_CALLBACKS *Buffer_Callbacks, 
						   LXC_RINGBUFFER_CALLBACKS *Ringbuffer_Callbacks, 
						   LXC_CALLBACKS *LXCCallbacks);
void Helper_SetBufferParameters(LXC_BUFFER *Buffer, uint MaxFilterLength,
								uint MaxFilterParthLength, uint MaxFilterPartLength_NonZero,
								uint MaxFilterParts, uint SampleFrequency);


LXC_CBenchmark_cpxMul::LXC_CBenchmark_cpxMul(uint InputFrameLength, 
											 uint SampleFrequency, 
											 LXC_OPTIMIZATION_MODULE OptModule, 
											 uint MaxItererations) :
	LXC_IBenchmark("LXC Complex Addition")
{
	if(!InputFrameLength || !SampleFrequency || !MaxItererations)
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Invalid initialization parameters! Please proof that InputFrameLength>0, SampleFrequency>0 and Itererations>0.");
	}

	m_InputFrameLength = InputFrameLength;
	m_SampleFrequency = SampleFrequency;
	m_MaxIterations = MaxItererations;

	m_OptModule = OptModule;

	Helper_ResetCallbacks(&m_BufferCallbacks, &m_RingbufferCallbacks, &m_LXCCallbacks);

	// get module callbacks
	LXC_ERROR_CODE err = LXC_Core_getModuleCallbacks(&m_LXCCallbacks, &m_BufferCallbacks, &m_RingbufferCallbacks, m_OptModule);
	if(err != LXC_NO_ERR)
	{
		// ToDo: Show LXC Error
		throw LXC_EXCEPTION_COUT_HANDLER("Couldn't get LXC Module Callbacks!");
	}

	

	// create LXC Buffer
	err = m_BufferCallbacks.LXC_Buffer_create(&m_Buffer[0]);
	if(err != LXC_NO_ERR)
	{
		// ToDo: Show LXC Error
		throw LXC_EXCEPTION_COUT_HANDLER("Couldn't create LXC Module Buffer!");
	}

	err = m_BufferCallbacks.LXC_Buffer_create(&m_Buffer[1]);
	if(err != LXC_NO_ERR)
	{
		// ToDo: Show LXC Error
		throw LXC_EXCEPTION_COUT_HANDLER("Couldn't create LXC Module Buffer!");
	}

	err = m_BufferCallbacks.LXC_Buffer_create(&m_Buffer[2]);
	if(err != LXC_NO_ERR)
	{
		// ToDo: Show LXC Error
		throw LXC_EXCEPTION_COUT_HANDLER("Couldn't create LXC Module Buffer!");
	}
}

LXC_CBenchmark_cpxMul::~LXC_CBenchmark_cpxMul()
{
	if(m_BufferCallbacks.LXC_Buffer_destroy)
	{
		m_BufferCallbacks.LXC_Buffer_destroy(&m_Buffer);
	}
}
	
double LXC_CBenchmark_cpxMul::RunBenchmark()
{
	double elapsedTime = 0.0;
	uint loopIteration = 0;
	float scaleFactor = 1.0f/((float)m_InputFrameLength);
	do
	{
		void *X = m_BufferCallbacks.LXC_Buffer_getPart(&m_Buffer[0], 0);
		void *H = m_BufferCallbacks.LXC_Buffer_getPart(&m_Buffer[1], 0);
		void *Y = m_BufferCallbacks.LXC_Buffer_getPart(&m_Buffer[2], 0);
		const uint Size = m_Buffer[0].maxFilterPartLength;

		m_Timer.start_Timer();
			LXC_ERROR_CODE err = m_LXCCallbacks.LXC_CpxMul(Size, X, H, Y);
		m_Timer.stop_Timer();
		elapsedTime += m_Timer.get_ElapsedTime();

		if(err != LXC_NO_ERR)
		{
			throw LXC_EXCEPTION_COUT_HANDLER(this->GetName() + " Benchmark skipped!");
		}

		loopIteration++;
	}while(loopIteration < m_MaxIterations);

	return elapsedTime/((double)m_MaxIterations);
}

void Helper_ResetCallbacks(LXC_BUFFER_CALLBACKS *Buffer_Callbacks, 
						   LXC_RINGBUFFER_CALLBACKS *Ringbuffer_Callbacks, 
						   LXC_CALLBACKS *LXCCallbacks)
{
	if(Buffer_Callbacks)
	{
		Buffer_Callbacks->LXC_Buffer_create = NULL;
		Buffer_Callbacks->LXC_Buffer_destroy = NULL;
		Buffer_Callbacks->LXC_Buffer_getPart = NULL;
	}

	if(Ringbuffer_Callbacks)
	{
		Ringbuffer_Callbacks->LXC_Ringbuffer_create = NULL;
		Ringbuffer_Callbacks->LXC_Ringbuffer_destroy = NULL;
		Ringbuffer_Callbacks->LXC_Ringbuffer_getNextPart = NULL;
		Ringbuffer_Callbacks->LXC_Ringbuffer_getPart = NULL;
	}

	if(LXCCallbacks)
	{
		LXCCallbacks->LXC_CpxAdd = NULL;
		LXCCallbacks->LXC_CpxMul = NULL;
		LXCCallbacks->LXC_FreqCombine2Ch = NULL;
		LXCCallbacks->LXC_FreqSplit2Ch = NULL;
	}
}


void Helper_SetBufferParameters(LXC_BUFFER *Buffer, uint MaxFilterLength,
								uint MaxFilterParthLength, uint MaxFilterPartLength_NonZero,
								uint MaxFilterParts, uint SampleFrequency)
{
	if(!Buffer)
	{
		// ToDo: show some error message!
		throw LXC_EXCEPTION_COUT_HANDLER("Invalid Input!");
	}


	Buffer->buffer = NULL;
	Buffer->maxFilterLength = MaxFilterLength;
	Buffer->maxFilterPartLength = MaxFilterParthLength;
	Buffer->maxFilterPartLength_NonZero = MaxFilterPartLength_NonZero;
	Buffer->maxFilterParts = MaxFilterLength;
	Buffer->sampleFrequency = SampleFrequency;
}
