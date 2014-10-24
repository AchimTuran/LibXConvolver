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

#include "sndfile.hh"

#include "../utils/timer/CPUTimer.h"
#include "../utils/signal/wavLoader.h"
#include "../utils/errorHandle/errorHandle.h"
#include "../utils/errorHandle/errorCout.h"

#include "LXC_Core.h"
#include "../LXCHandles/LXC_SSE3/LXC_SSE3Buffer.h"
#include "../LXCHandles/LXC_SSE3/LXC_SSE3.h"
#include "../LXCHandles/LXC_SSE3/LXC_SSE3_types.h"
#include "../LXCHandles/LXC_Native/LXC_Native.h"
#include "../fftHandles/fftwf/fftwfHandle.h"
#include "../../utils/system/LXC_OptimizationTranslator.h"

LXC_ERROR_CODE test_LXC_SSE3Kernels(uint MaxSamples, uint MaxLoops, bool ShowAllResults=false);
LXC_ERROR_CODE test_LXC_Convolver(LXC_OPTIMIZATION_MODULE Module);
double test_LXC_ConvolverWav(string wavFilename, string InSigFilename, LXC_OPTIMIZATION_MODULE Module);
LXC_ERROR_CODE test_SSE3vsNative();

LXC_ERROR_CODE test_LXC_Convolver(LXC_OPTIMIZATION_MODULE Module)
{
	LXC_ERROR_CODE err = LXC_NO_ERR;

	// create 2 channel test filter
	float h1[] = {1,1,1,1, 1,1,1,1, 1,1,1,1};
	float h2[] = {1,1,1,1, 1,1,1,1};
	const uint inputFrameLength = 4;
	LXC_ptrFilterHandle *lxc_filterHandle = LXC_Core_createFilter2Ch(h1, 12, h2, 8, 44100);
	if(!lxc_filterHandle)
		return LXC_Core_getLastError();

	// get LXC_Native convolver
	LXC_HANDLE *lxcHandle = LXC_Core_getConvolver(lxc_filterHandle, inputFrameLength, Module, LXC_fftModule_fftwf);
	if(!lxcHandle)
		return LXC_Core_getLastError();

	// convolve with x1 and x2
	float x[] = {1,1,1,1, 1,1,1,1, 0,0,0,0, 0,0,0,0};
	float y[] = {1,1,1,1, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	float z1[] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	float z2[] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	const uint blocks = sizeof(x)/sizeof(float)/inputFrameLength;

	for(uint ii=0; ii < blocks && err == LXC_NO_ERR; ii++)
	{
		const uint pos = ii*inputFrameLength;
		LXC_Core_convolve2Ch(lxcHandle, &x[pos], &y[pos], &z1[pos], &z2[pos]);
	}

	printf("z1=");
	for(uint ii=0; ii < sizeof(z1)/sizeof(float) -1; ii++)
	{
		printf("%.1f; ", z1[ii]);
	}
	printf("%.1f\n", z1[sizeof(z1)/sizeof(float)-1]);

	printf("z2=");
	for(uint ii=0; ii < sizeof(z2)/sizeof(float) -1; ii++)
	{
		printf("%.1f; ", z2[ii]);
	}
	printf("%.1f\n", z2[sizeof(z2)/sizeof(float)-1]);

	return LXC_Core_destroy(lxcHandle);
}

double test_LXC_ConvolverWav(string wavFilename, string InSigFilename, LXC_OPTIMIZATION_MODULE Module)
{
	CPUTimer timer;
	double elapsedTime = 0.0;
	LXC_ERROR_CODE err = LXC_NO_ERR;

	string optString = LXC_OptimizationToString(Module);

	// load test filter
	WavStruct filter;
	filter.samples = NULL;
	if(CWavLoader::openWavFile(wavFilename, &filter) == 0)
	{
		throw EXCEPTION_COUT_HANDLER("Can't load " + wavFilename);
	}
	if( filter.maxChannels > 1 )
	{
		CWavLoader::reorderChannels(&filter);
	}

	// load input signal
	WavStruct inputSignal;
	inputSignal.samples = NULL;
	if(CWavLoader::openWavFile(InSigFilename, &inputSignal) == 0)
	{
		CWavLoader::deleteWav(&filter);
		throw EXCEPTION_COUT_HANDLER("Can't load " + InSigFilename);
	}
	if( inputSignal.maxChannels > 1 )
	{
		CWavLoader::reorderChannels(&inputSignal);
	}

	// create LXC_[OPT] Convolver
	float *h1 = filter.samples;
	float *h2 = filter.samples + filter.maxSamples;
	const uint inputFrameLength = 1024;
	LXC_ptrFilterHandle *lxc_filterHandle = LXC_Core_createFilter2Ch(h1, filter.maxSamples, h2, filter.maxSamples, (uint)filter.sampleFrequency);
	if(!lxc_filterHandle)
	{
		throw EXCEPTION_COUT_HANDLER("Filter setup failed!");
		//return LXC_Core_getLastError();
	}

	// get LXC_Native convolver
	LXC_HANDLE *lxcHandle = LXC_Core_getConvolver(lxc_filterHandle, inputFrameLength, Module, LXC_fftModule_fftwf);
	if(!lxcHandle)
	{
		throw EXCEPTION_COUT_HANDLER("Can't get " + optString);
	}

	// convolve with x1 and x2 with h1 and h2
	float *z1 = new float[inputFrameLength];
	if(!z1)
	{
		throw EXCEPTION_COUT_HANDLER("Can't create buffer for z1!");
	}

	float *z2 = new float[inputFrameLength];
	if(!z2)
	{
		throw EXCEPTION_COUT_HANDLER("Can't create buffer for z2!");
	}

	// open wave file for writting
	SndfileHandle wavFile_z1(string("out_ch1_") + optString + string(".wav"),
							 SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, 1, 
							 (int)inputSignal.sampleFrequency);
	int sndError = wavFile_z1.error();
	if(sndError != SF_ERR_NO_ERROR)
	{
		string sndErrorStr(sf_error_number(sndError));
		throw EXCEPTION_COUT_HANDLER("libsndfile error: " + sndErrorStr);
	}

	SndfileHandle wavFile_z2(string("out_ch2_") + optString + string(".wav"),
							 SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, 1, 
							 (int)inputSignal.sampleFrequency);
	sndError = wavFile_z2.error();
	if(sndError != SF_ERR_NO_ERROR)
	{
		string sndErrorStr(sf_error_number(sndError));
		throw EXCEPTION_COUT_HANDLER("libsndfile error: " + sndErrorStr);
	}
	

	const uint blocks = inputSignal.maxSamples/inputFrameLength;
	const double percentageSteps = 5;
	double currentPercentageStep = 1;
	printf("processing: [");
	for(uint ii=0; ii < blocks; ii++)
	{
		const uint pos = ii*inputFrameLength;
		float *x = inputSignal.samples + pos;
		float *y = inputSignal.samples + pos + inputSignal.maxSamples;

		double currentPercentage = ((double)ii)/((double)blocks)*100.0;
		if(	currentPercentageStep*percentageSteps <= currentPercentage &&
			currentPercentage < currentPercentageStep*(percentageSteps+1))
		{
			currentPercentageStep += 1;
			printf("=");
		}

		timer.start_Timer();
			err = LXC_Core_convolve2Ch(lxcHandle, x, y, z1, z2);
		timer.stop_Timer();

		elapsedTime += timer.get_ElapsedTime();

		// write samples to wave file
		wavFile_z1.write(z1, inputFrameLength);
		wavFile_z2.write(z2, inputFrameLength);

		if(err != LXC_NO_ERR)
		{
			LXC_Core_destroy(lxcHandle);
			CWavLoader::deleteWav(&inputSignal);
			CWavLoader::deleteWav(&filter);
			delete [] z1;
			delete [] z2;
			throw EXCEPTION_COUT_HANDLER("LXC convolution error!");
		}
	}
	printf("]\n");

	CWavLoader::deleteWav(&inputSignal);
	CWavLoader::deleteWav(&filter);
	delete [] z1;
	delete [] z2;

	LXC_Core_destroy(lxcHandle);

	return elapsedTime;
}

LXC_ERROR_CODE test_LXC_SSE3Kernels(uint MaxSamples, uint MaxLoops, bool ShowAllResults)
{
	uint fastesKernel = 0;
	double elapsedMinTime = 10.0f;
	CPUTimer timer;
	const uint maxSamples = MaxSamples;
	const uint maxLoops = MaxLoops;
	cout << "using " << maxSamples << " Samples and ";
	cout << maxLoops << " loop iterations" << endl;
	if(ShowAllResults)
	{
		cout << endl << "test begin" << endl;
	}

	LXC_ERROR_CODE err = LXC_NO_ERR;
	LXC_BUFFER_CALLBACKS bufferCallbacks;
	err = LXC_get_SSE3BufferCallbacks(&bufferCallbacks);
	if(err != LXC_NO_ERR)
		return err;

	// create test buffers
	LXC_BUFFER x;
	x.buffer = NULL;
	x.maxFilterLength = maxSamples;
	x.maxFilterPartLength = maxSamples;
	x.maxFilterPartLength_NonZero = maxSamples;
	x.maxFilterParts = 1;
	x.sampleFrequency = 44100;
	err = bufferCallbacks.LXC_Buffer_create(&x);
	if(err != LXC_NO_ERR)
		return err;

	LXC_BUFFER y;
	y.buffer = NULL;
	y.maxFilterLength = maxSamples;
	y.maxFilterPartLength = maxSamples;
	y.maxFilterPartLength_NonZero = maxSamples;
	y.maxFilterParts = 1;
	y.sampleFrequency = 44100;
	err = bufferCallbacks.LXC_Buffer_create(&y);
	if(err != LXC_NO_ERR)
		return err;

	LXC_BUFFER z;
	z.buffer = NULL;
	z.maxFilterLength = maxSamples;
	z.maxFilterPartLength = maxSamples;
	z.maxFilterPartLength_NonZero = maxSamples;
	z.maxFilterParts = 1;
	z.sampleFrequency = 44100;
	err = bufferCallbacks.LXC_Buffer_create(&z);
	if(err != LXC_NO_ERR)
		return err;


	// get buffer pointers
	void *_x = bufferCallbacks.LXC_Buffer_getPart(&x, 0);
	void *_y = bufferCallbacks.LXC_Buffer_getPart(&y, 0);
	void *_z = bufferCallbacks.LXC_Buffer_getPart(&z, 0);

	for(uint ii=0; ii < maxSamples; ii++)
	{
		((LXC_SSE3cpxFloat*)_x)[ii][0] = (float)ii+1;
		((LXC_SSE3cpxFloat*)_x)[ii][1] = (float)ii+1;

		((LXC_SSE3cpxFloat*)_y)[ii][0] = (float)ii+1;
		((LXC_SSE3cpxFloat*)_y)[ii][1] = (float)ii+1;
	}


	double timeMeasureNative = 0.0f;

	// test kernel functions
	for(uint kernelSize=0; kernelSize <= 128; kernelSize +=2)
	{
		double timeMeasureSSE3 = 0.0f;
		for(uint ii=0; ii < maxLoops; ii++)
		{
			switch(kernelSize)
			{
				case 0:
					timer.start_Timer();
					LXC_NativeCpxMul(maxSamples, _x, _y, _z);
					timer.stop_Timer();
					timeMeasureNative += timer.get_ElapsedTime();
				break;

				case 2:
					timer.start_Timer();
					LXC_SSE3CpxMul_K2(maxSamples, _x, _y, _z);
					timer.stop_Timer();

					timeMeasureSSE3 += timer.get_ElapsedTime();
				break;

				case 4:
					timer.start_Timer();
					LXC_SSE3CpxMul_K4(maxSamples, _x, _y, _z);
					timer.stop_Timer();

					timeMeasureSSE3 += timer.get_ElapsedTime();
				break;

				case 8:
					timer.start_Timer();
					LXC_SSE3CpxMul_K8(maxSamples, _x, _y, _z);
					timer.stop_Timer();

					timeMeasureSSE3 += timer.get_ElapsedTime();
				break;

				case 16:
					timer.start_Timer();
					LXC_SSE3CpxMul_K16(maxSamples, _x, _y, _z);
					timer.stop_Timer();

					timeMeasureSSE3 += timer.get_ElapsedTime();
				break;

				case 32:
					timer.start_Timer();
					LXC_SSE3CpxMul_K32(maxSamples, _x, _y, _z);
					timer.stop_Timer();

					timeMeasureSSE3 += timer.get_ElapsedTime();
				break;

				case 64:
					timer.start_Timer();
					LXC_SSE3CpxMul_K64(maxSamples, _x, _y, _z);
					timer.stop_Timer();

					timeMeasureSSE3 += timer.get_ElapsedTime();
				break;

				case 128:
					timer.start_Timer();
					LXC_SSE3CpxMul_K128(maxSamples, _x, _y, _z);
					timer.stop_Timer();

					timeMeasureSSE3 += timer.get_ElapsedTime();
				break;

				default:
				break;
			}
		}

		if(	kernelSize == 2 || kernelSize == 4 || kernelSize == 8 || kernelSize == 16 ||
			kernelSize == 32 || kernelSize == 64 || kernelSize == 128) 
		{
			timeMeasureSSE3 /= (double)maxLoops;
			if(ShowAllResults)
			{
				if(	kernelSize == 4 || kernelSize == 8 || kernelSize == 16 ||
					kernelSize == 16 || kernelSize == 32 || kernelSize == 64)
				{
					cout << endl;
				}
			
				cout << " successful tested SSE3 Kernel with size=" << kernelSize << endl;
				cout << " " << timeMeasureSSE3 << "s for one kernel function" << endl;

				if(	kernelSize == 4 || kernelSize == 8 || kernelSize == 16 ||
					kernelSize == 16 || kernelSize == 32 || kernelSize == 64)
				{
					cout << endl;
				}
			}

			if(timeMeasureSSE3 < elapsedMinTime && timeMeasureSSE3 > 0.0)
			{
				elapsedMinTime = timeMeasureSSE3;
				fastesKernel = kernelSize;
			}
		}
	}

	bufferCallbacks.LXC_Buffer_destroy(&x);
	bufferCallbacks.LXC_Buffer_destroy(&y);
	bufferCallbacks.LXC_Buffer_destroy(&z);

	if(ShowAllResults)
	{
		cout << "test end" << endl << endl;
	}
	timeMeasureNative /= (double)maxLoops;
	cout << "fastest has size=" << fastesKernel << " min. time: " << elapsedMinTime << "s" << endl;
	cout << "Speed up: " << timeMeasureNative/elapsedMinTime << endl;

	return err;
}

LXC_ERROR_CODE test_LXC_SSE3()
{
	LXC_ERROR_CODE err = LXC_NO_ERR;
	uint maxLoops = 1000;
	bool showAllResults = false;

	err = test_LXC_SSE3Kernels(128, 1000, showAllResults);
	if(err != LXC_NO_ERR)
		return err;

	cout << endl;

	err = test_LXC_SSE3Kernels(256, 1000, showAllResults);
	if(err != LXC_NO_ERR)
		return err;

	cout << endl;

	err = test_LXC_SSE3Kernels(512, 1000, showAllResults);
	if(err != LXC_NO_ERR)
		return err;

	cout << endl;

	err = test_LXC_SSE3Kernels(1024, 1000, showAllResults);
	if(err != LXC_NO_ERR)
		return err;

	cout << endl;

	err = test_LXC_SSE3Kernels(2048, 1000, showAllResults);
	if(err != LXC_NO_ERR)
		return err;

	cout << endl;

	err = test_LXC_SSE3Kernels(4096, 1000, showAllResults);
	if(err != LXC_NO_ERR)
		return err;
	
	cout << endl;

	return err;
}

LXC_ERROR_CODE test_SSE3vsNative()
{
	LXC_ERROR_CODE err = LXC_NO_ERR;
	cout << "Testing LXC_Native" << endl;
	double elapsedTime_Native = test_LXC_ConvolverWav("tyndall_bruce_ortf.wav", "Hitman.wav", LXC_OPT_NATIVE);
	cout << "Processing time for LXC_Native: " << elapsedTime_Native << "s" << endl;

	cout << endl << "Testing LXC_SSE3" << endl;
	double elapsedTime_SSE3 = test_LXC_ConvolverWav("tyndall_bruce_ortf.wav", "Hitman.wav", LXC_OPT_SSE3);
	cout << "Processing time for LXC_SSE3: " << elapsedTime_SSE3 << "s" << endl;

	cout << endl << "Speed up: " << elapsedTime_Native / elapsedTime_SSE3 << endl;

	return err;
}

// main function
int main()
{
	try
	{
		// init LXC library
		LXC_ERROR_CODE err = LXC_Core_init();
		if(err != LXC_NO_ERR)
			return err;

		// get LXC library properties
		LXC_PROPERTIES lxcProperties;
		err = LXC_Core_getProperties(&lxcProperties);
		if(err != LXC_NO_ERR)
			return err;

		cout << endl << endl;
		cout << "====Testing LXC_Native convolution====" << endl;
		err = test_LXC_Convolver(LXC_OPT_NATIVE);
		if(err != LXC_NO_ERR)
			return err;

		cout << endl << endl;
		cout << "====Testing LXC_SSE3 convolution====" << endl;
		err = test_LXC_Convolver(LXC_OPT_SSE3);
		if(err != LXC_NO_ERR)
			return err;

		cout << endl << endl;
		cout << "====Testing LXC_SSE3 complex multiplication Kernel functions====" << endl;
		err = test_LXC_SSE3();
		if(err != LXC_NO_ERR)
			return err;

		cout << endl << endl;
		cout << "====Testing LXC_SSE3 convolution vs. LXC_Native convolution====" << endl;
		err = test_SSE3vsNative();
		if(err != LXC_NO_ERR)
			return err;
	

	#ifdef TARGET_WINDOWS
		cout << endl << endl;
		system("PAUSE");
	#endif

		return err;
	}
	catch(errorHandle &e)
	{
		LXC_Core_close();
		e.showError();
		system("PAUSE");
		return -1;
	}
	HANDLE_BAD_ALLOC
	HANDLE_RANGE_EXCEPTION
	HANDLE_UNKNOWN_EXCEPTION
}
