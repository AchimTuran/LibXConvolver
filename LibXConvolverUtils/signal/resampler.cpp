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



#include <iostream>
using namespace std;

#include "resampler.h"
#include "../../LibXConvolverCore/include/common_types.h"
#include "../../LibXConvolverCore/include/LXC_Core.h"

CResampler::CResampler(uint MaxFrames, uint InSampleRate, uint OutSampleRate)
{
  if (MaxFrames && InSampleRate && OutSampleRate)
	{
    m_ResamplingQuality = SINC_BEST_QUALITY;

    m_ResamplingPlan.data_in = NULL;
    m_ResamplingPlan.data_out = NULL;
    m_ResamplingPlan.src_ratio = ((double)OutSampleRate) / ((double)InSampleRate);

    m_MaxFrames = (long)MaxFrames;
    m_MaxResampledFrames = (long)ceil(((double)m_MaxFrames)*m_ResamplingPlan.src_ratio);

    m_ResamplingPlan.input_frames = m_MaxFrames;
    m_ResamplingPlan.output_frames = m_MaxResampledFrames;
    m_ResamplingPlan.input_frames_used = 0;
    m_ResamplingPlan.output_frames_gen = 0;
    m_ResamplingPlan.end_of_input = 0;

    m_srcError = 0;
		m_ResamplingObj = src_new(m_ResamplingQuality, 1, &m_srcError);
    if (!m_ResamplingObj)
		{
			//ToDo: throw Error!
		}

    if (m_srcError)
    {
      //ToDo: throw Error!
    }

		ResetResampler();
	}
	else
	{
		// ToDo: throw Error!
	}
}

CResampler::~CResampler()
{
  if (m_ResamplingObj)
	{
    src_delete(m_ResamplingObj);
    m_ResamplingObj = NULL;
	}
}

LXC_ERROR_CODE CResampler::Resample(float *In, uint MaxFrames, uint InSampleRate, uint OutSampleRate, float *Out)
{
	if( !In || !Out )
	{
		return LXC_ERR_INVALID_INPUT;
	}

	m_ResamplingPlan.data_in = In;
  m_ResamplingPlan.data_out = Out;

  m_ResamplingPlan.src_ratio = ((double)OutSampleRate) / ((double)InSampleRate);

  m_MaxFrames = (long)MaxFrames;
  m_MaxResampledFrames = (long)ceil(((double)m_MaxFrames)*m_ResamplingPlan.src_ratio);

  m_ResamplingPlan.input_frames = m_MaxFrames;
  m_ResamplingPlan.output_frames = m_MaxResampledFrames;
  m_ResamplingPlan.input_frames_used = 0;
  m_ResamplingPlan.output_frames_gen = 0;
  m_ResamplingPlan.end_of_input = 0;

	//src_simple( &m_DownsamplingPlan, m_ResamplingQuality, 1);
  m_srcError = src_process(m_ResamplingObj, &m_ResamplingPlan);

  if (m_srcError)
  {
    return LXC_ERR_RESAMPLER;
  }

	return LXC_NO_ERR;
}

LXC_ERROR_CODE CResampler::Resample(float *In, float *Out)
{
  if (!In || !Out)
  {
    return LXC_ERR_INVALID_INPUT;
  }

  m_ResamplingPlan.data_in = In;
  m_ResamplingPlan.data_out = Out;

  //src_simple( &m_DownsamplingPlan, m_ResamplingQuality, 1);
  m_srcError = src_process(m_ResamplingObj, &m_ResamplingPlan);

  if (m_srcError)
  {
    return LXC_ERR_RESAMPLER;
  }

  return LXC_NO_ERR;
}

std::string CResampler::GetLastError()
{
  return string(src_strerror(m_srcError));
}

//void CResampler::SetResamplingQuality(resamplingQuality ResamplingQuality)
//{
//  m_ResamplingQuality = ResamplingQuality;
//}

void CResampler::ResetResampler()
{
	float *inResampling = new float[m_ResamplingPlan.input_frames];
  float *outResampling = new float[m_ResamplingPlan.output_frames];

  if (!inResampling || !outResampling)
  {
    // ToDo: throw error
  }

  for (long ii = 0; ii < m_ResamplingPlan.input_frames; ii++)
	{
    inResampling[ii] = 0.0f;
	}

  for (long ii = 0; ii < m_ResamplingPlan.output_frames; ii++)
  {
    outResampling[ii] = 0.0f;
  }

	for(unsigned int ii = 0; ii < 2; ii++)
	{
    m_srcError = Resample(inResampling, outResampling);
    if (m_srcError)
    {
      // ToDo: throw error
    }
	}

  if (inResampling)
	{
    delete[] inResampling;
    inResampling = NULL;
	}
  if (outResampling)
	{
    delete[] outResampling;
    outResampling = NULL;
	}
}

uint CResampler::GetMaxOutputSamples()
{
  return m_MaxResampledFrames;
}

uint CResampler::CalcMaxOutputSamples(uint MaxFrames, uint InSampleRate, uint OutSampleRate)
{
  double ratio = ((double)OutSampleRate) / ((double)InSampleRate);
  return (uint)(ceil(((double)MaxFrames)*ratio));
}