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



#include <sndfile.hh>
#include "../../LibXConvolverCore/include/LXC_CommonTypes.h"

#include <string>

#define DEFAULT_BLOCKLENGTH 256	// Default length of a block that will be read in openFilterFile

typedef struct LXC_WavStruct
{
	uint sampleFrequency;
	unsigned int maxSamples;
	unsigned int maxChannels;
	float *samples;
  LXC_WavStruct()
  {
    sampleFrequency = 0;
    maxSamples = 0;
    maxChannels = 0;
    samples = NULL;
  }
};

//typedef enum filterDatatype {FLOAT = 0, BIT8, };

class LXC_CWavLoader
{
public:
	LXC_CWavLoader();
	~LXC_CWavLoader();

	// this method tries to load a filter from an wave file.
	// FilterFile: name and path of the filter file.
	// FilterHandle: pointer to the filter buffer. The methods creates the buffer and you can delete it with delete []..., own function or use the deleteFilter(...) method from CWAVFilterLoader-class.
	// returns the number of loaded filter taps.
	static unsigned int openWavFile( std::string WavFile, LXC_WavStruct *WavHandle );

	// optional function to reorder the channels that were readed with libsndfile
	// libsndfile order = { ch1_0, ch2_0, ..., chN_0 }
	// reorderChannels() = { ch1_0, ch1_1, ..., ch1_K, 
	//						 ch2_0, ch2_1, ..., ch2_K,
	//						 chN_0, chN_1, ..., chN_K }
	static void reorderChannels(LXC_WavStruct *WavHandle);
	static bool checkWavStruct(LXC_WavStruct *WavHandle);

	// this method deletes the filter taps
	static void deleteWav(LXC_WavStruct *WavHandle);
};
