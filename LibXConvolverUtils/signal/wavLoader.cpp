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



#include "wavLoader.h"
#include "../../LibXConvolverUtils/errorHandle/errorCout.h"


#include <string>
#include "../../LibXConvolverUtils/logging/LXC_Logging.h"
using namespace std;

CWavLoader::CWavLoader()
{
}

CWavLoader::~CWavLoader()
{
}

unsigned int CWavLoader::openWavFile(string WavFile, WavStruct *WavHandle)
{
	if(WavFile.empty())
	{
		throw EXCEPTION_COUT_HANDLER("WavFile was empty!");
	}

	if(WavHandle == NULL)
	{
		throw EXCEPTION_COUT_HANDLER("WavHandle == NULL! Invalid Input!");
	}

	if(WavHandle->samples)
	{
		deleteWav(WavHandle);
	}

	// open filter file
	const int mode = SFM_READ;
	SndfileHandle wavFile(WavFile.c_str(), mode);
	int sndError = wavFile.error();
	if(sndError != SF_ERR_NO_ERROR)
	{
		string sndErrorStr(sf_error_number(sndError));
		deleteWav(WavHandle);
    // ToDo show some error message
    return 0;
		//throw EXCEPTION_COUT_HANDLER("Wav file " + WavFile + " not found!" + " libsndfile error: " + sndErrorStr);
	}

	// get channels of the file
	const int sndChannels = wavFile.channels();
	if(sndChannels < 1)
	{
		deleteWav(WavHandle);
		throw EXCEPTION_COUT_HANDLER("WavFile " + WavFile + "has < 1 channels! Invalid input file!");
	}
	WavHandle->maxChannels = sndChannels;

	const int sndSampleFrequency = wavFile.samplerate();
	if( sndSampleFrequency <= 0 )
	{
		deleteWav(WavHandle);
		throw EXCEPTION_COUT_HANDLER("Samplerate from the filter file was < 1! Invalid input filter file!");
	}
	WavHandle->sampleFrequency = (uint)sndSampleFrequency;

	// get length (taps) of the filter file
	sf_count_t filterSize = wavFile.frames();
	if( filterSize <= 0 )
	{
		deleteWav(WavHandle);
		throw EXCEPTION_COUT_HANDLER("Filter file " + WavFile + " was empty! No filter taps were loaded!");
	}
	// set filter length to WavHandle
	WavHandle->maxSamples = (unsigned int)filterSize;
	LXC_LOG_DEBUG(	"%s: load wave file with channels: %i, sample frequency: %f Hz, maximum samples: %i", 
					__func__, WavHandle->maxChannels,  WavHandle->sampleFrequency, WavHandle->maxSamples);
	
	// create buffer for the filter taps
	WavHandle->samples = new float[WavHandle->maxChannels * WavHandle->maxSamples];
	if(!(WavHandle->samples))
	{
		deleteWav(WavHandle);
		throw EXCEPTION_COUT_HANDLER("Could not create buffer for filter! Not enough free memory?!?");
	}

	// copy filter taps from file to the new created buffer (WavHandle->samples)
	const sf_count_t sndSamples = wavFile.read(WavHandle->samples, WavHandle->maxChannels * WavHandle->maxSamples);
	if(sndSamples != WavHandle->maxSamples * WavHandle->maxChannels)
	{
		throw EXCEPTION_COUT_HANDLER("calculated filter size was greate than the filter in the file! Is the filter file corrupted?");
	}

	return WavHandle->maxSamples;
}

void CWavLoader::deleteWav(WavStruct *WavHandle)
{
	if(WavHandle)
	{
		if(WavHandle->samples)
		{
			delete [] WavHandle->samples;
		
			// reset WavStruct to default values
			WavHandle->samples = NULL;
			WavHandle->maxChannels = 0;
			WavHandle->maxSamples = 0;
			WavHandle->sampleFrequency = 0;
		}
	}
}

void CWavLoader::reorderChannels( WavStruct *WavHandle )
{
	if(!WavHandle)
	{
		throw EXCEPTION_COUT_HANDLER("WavHandle == NULL! Invalid input!");
	}
	checkWavStruct(WavHandle);

	if(WavHandle->samples)
	{
		float *tempFilter = new float[WavHandle->maxChannels * WavHandle->maxSamples];
		if(!tempFilter)
		{
			throw EXCEPTION_COUT_HANDLER("Could not allocate enough dynamic memory! Is there not enough free memory?");
		}

		const unsigned int maxChannels = WavHandle->maxChannels;
		for(unsigned int ii = 0; ii < WavHandle->maxChannels; ii++)
		{
			const unsigned int arrayOffset = ii * WavHandle->maxSamples;
			for(unsigned int zz = 0; zz < WavHandle->maxSamples; zz++)
			{
				tempFilter[arrayOffset + zz] = WavHandle->samples[ii + zz*maxChannels];
			}
		}

		// delete old filter taps buffer
		delete [] WavHandle->samples;

		// change filter taps buffers
		WavHandle->samples = tempFilter;
	}

}

bool CWavLoader::checkWavStruct(WavStruct *WavHandle)
{
	if(!WavHandle)
	{
		throw EXCEPTION_COUT_HANDLER("WavHandle == NULL! Invalid input!");
	}

	// get channels of the file
	if(WavHandle->maxChannels <= 0)
	{
		deleteWav( WavHandle );
		throw EXCEPTION_COUT_HANDLER("WavFile has <= 0 channels! Invalid input file!");
	}

	if(WavHandle->sampleFrequency <= 0)
	{
		deleteWav(WavHandle);
		throw EXCEPTION_COUT_HANDLER("Samplerate from the filter file was <= 0! Invalid input filter file!");
	}

	// get length (taps) of the filter file
	if(WavHandle->maxSamples <= 0)
	{
		deleteWav(WavHandle);
		throw EXCEPTION_COUT_HANDLER("Filter file was empty! No filter taps were loaded!");
	}	
	LXC_LOG_DEBUG(	"CWavLoader::checkWavStruct channels: %i, sample frequency: %i Hz, max. samples: %i", 
					WavHandle->maxChannels, WavHandle->sampleFrequency, WavHandle->maxSamples);

	if(!WavHandle->samples)
	{
		deleteWav(WavHandle);
		throw EXCEPTION_COUT_HANDLER("There were no filter taps!");
	}

	return true;
}