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



#include <string>
#include "samplerate.h"
#include "../../LibXConvolverCore/include/LXC_CommonTypes.h"

typedef enum
{
  SINC_BEST_QUALITY = 0,
  SINC_MEDIUM_QUALITY = 1,
  SINC_FASTEST = 2,
  ZERO_ORDER_HOLD = 3,
  LINEAR = 4,
}LXC_ResamplingQuality;

class LXC_CResampler
{
public:
  LXC_CResampler(uint MaxFrames, uint InSampleRate, uint OutSampleRate);
	~LXC_CResampler();
	//void SetResamplingQuality(resamplingQuality ResamplingQuality);
  uint GetMaxOutputSamples();
  std::string GetLastError();
  static uint CalcMaxOutputSamples(uint MaxFrames, uint InSampleRate, uint OutSampleRate);

	LXC_ERROR_CODE Resample(float *In, float *Out);
	LXC_ERROR_CODE Resample(float *In, uint MaxFrames, uint InSampleRate, uint OutSampleRate, float *Out);

protected:
	void ResetResampler();			// before we can use the upsampling and downsampling objects we need to feed them with zeros!
	long m_MaxResampledFrames;
	long m_MaxFrames;

	int m_srcError;
	SRC_DATA m_ResamplingPlan;
	SRC_STATE *m_ResamplingObj;
	LXC_ResamplingQuality m_ResamplingQuality;
};