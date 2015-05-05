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

#include "../LibXConvolverBenchmarks/LXC_CBenchmark_combinedChannels.h"
#include "../LibXConvolverBenchmarks/LXC_CBenchmark_seperateChannels.h"
#include "../LibXConvolverUtils/LXC_Logging/LXC_CBenchmarkCSVExporter.h"

#include "sndfile.hh"

#include "../LibXConvolverUtils/LXC_Timer/LXC_CPUTimer.h"
#include "../LibXConvolverUtils/LXC_Signal/LXC_WavLoader.h"
#include "../LibXConvolverUtils/LXC_Exceptions/LXC_CException.h"
#include "../LibXConvolverUtils/LXC_Constants.h"
#include "../LibXConvolverUtils/LXC_System/LXC_OptimizationTranslator.h"

#include "../LibXConvolverCore/include/LXC_Core.h"
#include "../LibXConvolverCore/LXC_Handles/LXC_SSE3/LXC_SSE3Buffer.h"
#include "../LibXConvolverCore/LXC_Handles/LXC_SSE3/LXC_SSE3.h"
#include "../LibXConvolverCore/LXC_Handles/LXC_SSE3/LXC_SSE3_types.h"
#include "../LibXConvolverCore/LXC_Handles/LXC_Native/LXC_Native.h"
#include "../LibXConvolverCore/LXC_fftHandles/LXC_fftwf/LXC_fftwfHandle.h"


LXC_ERROR_CODE test_LXC_SSE3Kernels(uint MaxSamples, uint MaxLoops, bool ShowAllResults=false);
LXC_ERROR_CODE test_LXC_Convolver_2Ch(LXC_OPTIMIZATION_MODULE Module);
LXC_ERROR_CODE test_LXC_Convolver_1Ch(LXC_OPTIMIZATION_MODULE Module);
double test_LXC_ConvolverWav(string filterFilename, string InSigFilename, 
							 LXC_OPTIMIZATION_MODULE Module, 
							 uint InputFrameLength = 1024);
LXC_ERROR_CODE test_SSE3vsNative(uint inputFramelength);
void benchmark_combinedVSseperate(uint FromInputeFrameLength, 
								  uint ToInputFrameLength, 
								  LXC_OPTIMIZATION_MODULE Module,
								  uint MaxIterations);

LXC_ERROR_CODE test_LXC_Convolver_1Ch(LXC_OPTIMIZATION_MODULE Module)
{
	LXC_ERROR_CODE err = LXC_NO_ERR;

	// create 2 channel test filter
	float h1[] = {1,1,1,1, 1,1,1,1, 1,1,1,1};
	const uint sizeH1 = sizeof(h1)/sizeof(float);
	const uint inputFrameLength = 5;
	LXC_ptrFilterHandle *lxc_filterHandle = LXC_Core_createFilter(h1, sizeH1, 44100);
	if(!lxc_filterHandle)
		return LXC_Core_getLastError();

	// get LXC_Native convolver
	LXC_HANDLE *lxcHandle = LXC_Core_getConvolver(lxc_filterHandle, inputFrameLength, Module, LXC_fftModule_fftwf);
	if(!lxcHandle)
		return LXC_Core_getLastError();

	// convolve with x1 and x2
	float x[] = {1,1,1,1, 1,1,1,1, 0,0,0,0, 0,0,0,0};
	float z1[] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	const uint blocks = sizeof(x)/sizeof(float)/inputFrameLength;

	for(uint ii=0; ii < blocks && err == LXC_NO_ERR; ii++)
	{
		const uint pos = ii*inputFrameLength;
		LXC_Core_convolve(lxcHandle, &x[pos], &z1[pos]);
	}

	printf("z1=");
	for(uint ii=0; ii < sizeof(z1)/sizeof(float) -1; ii++)
	{
		printf("%.1f; ", z1[ii]);
	}
	printf("%.1f\n", z1[sizeof(z1)/sizeof(float)-1]);

	return LXC_Core_destroy(lxcHandle);
}

LXC_ERROR_CODE test_LXC_Convolver_2Ch(LXC_OPTIMIZATION_MODULE Module)
{
	LXC_ERROR_CODE err = LXC_NO_ERR;

	// create 2 channel test filter
	float h1[] = {1,1,1,1, 1,1,1,1, 1,1,1,1};
	const uint sizeH1 = sizeof(h1)/sizeof(float);
	float h2[] = {1,1,1,1, 1,1,1,1};
	const uint sizeH2 = sizeof(h2)/sizeof(float);
	const uint inputFrameLength = 4;
	LXC_ptrFilterHandle *lxc_filterHandle = LXC_Core_createFilter2Ch(h1, sizeH1, h2, sizeH2, 44100);
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

double test_LXC_ConvolverWav(string filterFilename, string InSigFilename, 
							 LXC_OPTIMIZATION_MODULE Module, 
							 uint InputFrameLength)
{
	LXC_CCPUTimer timer;
	double elapsedTime = 0.0;
	LXC_ERROR_CODE err = LXC_NO_ERR;

	string optString = LXC_OptimizationToString(Module);

	// load test filter
	LXC_WavStruct filter;
	filter.samples = NULL;
	if(LXC_CWavLoader::openWavFile(filterFilename, &filter) == 0)
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Can't load " + filterFilename);
	}
	if( filter.maxChannels > 1 )
	{
		cout << "2 channel filter loaded" << endl;
		LXC_CWavLoader::reorderChannels(&filter);
	}

	// load input signal
	LXC_WavStruct inputSignal;
	inputSignal.samples = NULL;
	if(LXC_CWavLoader::openWavFile(InSigFilename, &inputSignal) == 0)
	{
		LXC_CWavLoader::deleteWav(&filter);
		throw LXC_EXCEPTION_COUT_HANDLER("Can't load " + InSigFilename);
	}
	if( inputSignal.maxChannels > 1 )
	{
		LXC_CWavLoader::reorderChannels(&inputSignal);
	}

	// create LXC_[OPT] Convolver
	float *h1 = filter.samples;
	float *h2 = filter.samples + filter.maxSamples;
	const uint inputFrameLength = InputFrameLength;
	LXC_ptrFilterHandle *lxc_filterHandle = LXC_Core_createFilter2Ch(h1, filter.maxSamples, h2, filter.maxSamples, (uint)filter.sampleFrequency);
	if(!lxc_filterHandle)
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Filter setup failed!");
		//return LXC_Core_getLastError();
	}

	// get LXC_Native convolver
	LXC_HANDLE *lxcHandle = LXC_Core_getConvolver(lxc_filterHandle, inputFrameLength, Module, LXC_fftModule_fftwf);
	if(!lxcHandle)
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Can't get " + optString);
	}

	// convolve with x1 and x2 with h1 and h2
	float *z1 = new float[inputFrameLength];
	if(!z1)
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Can't create buffer for z1!");
	}

	float *z2 = new float[inputFrameLength];
	if(!z2)
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Can't create buffer for z2!");
	}

	// open wave file for writting
	SndfileHandle wavFile_z1(string("out_ch1_") + optString + string(".wav"),
							 SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, 1, 
							 (int)inputSignal.sampleFrequency);
	int sndError = wavFile_z1.error();
	if(sndError != SF_ERR_NO_ERROR)
	{
		string sndErrorStr(sf_error_number(sndError));
		throw LXC_EXCEPTION_COUT_HANDLER("libsndfile error: " + sndErrorStr);
	}

	SndfileHandle wavFile_z2(string("out_ch2_") + optString + string(".wav"),
							 SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, 1, 
							 (int)inputSignal.sampleFrequency);
	sndError = wavFile_z2.error();
	if(sndError != SF_ERR_NO_ERROR)
	{
		string sndErrorStr(sf_error_number(sndError));
		throw LXC_EXCEPTION_COUT_HANDLER("libsndfile error: " + sndErrorStr);
	}
	

	const uint blocks = inputSignal.maxSamples/inputFrameLength;
	const double percentageSteps = 5;
	double currentPercentageStep = 1;
	cout << "max samples: " << blocks*inputFrameLength << endl;
	cout << "sample frequency: " << inputSignal.sampleFrequency << "Hz" << endl;
	cout << "input framelength: " << inputFrameLength << endl;
	cout << "I/O-delay: " << ((double)inputFrameLength)/((double)inputSignal.sampleFrequency) << "[s]" << endl;
	cout << "filter taps: " << filter.maxSamples << endl;
	printf("-->processing: [");
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
			LXC_CWavLoader::deleteWav(&inputSignal);
			LXC_CWavLoader::deleteWav(&filter);
			delete [] z1;
			delete [] z2;
			throw LXC_EXCEPTION_COUT_HANDLER("LXC convolution error!");
		}
	}
	printf("]\n");
	cout << "RTI: " << 
		elapsedTime*100.0/(((double)(blocks*inputFrameLength)) / ((double)inputSignal.sampleFrequency)) 
		<< "%" << endl;

	LXC_CWavLoader::deleteWav(&inputSignal);
	LXC_CWavLoader::deleteWav(&filter);
	delete [] z1;
	delete [] z2;

	LXC_Core_destroy(lxcHandle);

	return elapsedTime;
}

LXC_ERROR_CODE test_LXC_SSE3Kernels(uint MaxSamples, uint MaxLoops, bool ShowAllResults)
{
	uint fastesKernel = 0;
	double elapsedMinTime = 10.0f;
	LXC_CCPUTimer timer;
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

LXC_ERROR_CODE test_SSE3vsNative(uint inputFramelength)
{
	LXC_ERROR_CODE err = LXC_NO_ERR;
	cout << "====Testing LXC_Native====" << endl;
	double elapsedTime_Native = test_LXC_ConvolverWav("Noise.wav", "Hitman.wav", LXC_OPT_NATIVE, inputFramelength);
	cout << "Processing time for LXC_Native: " << elapsedTime_Native << "s" << endl;

	cout << endl << "====Testing LXC_SSE3====" << endl;
	double elapsedTime_SSE3 = test_LXC_ConvolverWav("Noise.wav", "Hitman.wav", LXC_OPT_SSE3, inputFramelength);
	cout << "Processing time for LXC_SSE3: " << elapsedTime_SSE3 << "s" << endl;

	cout << endl << "-->Speed up: " << elapsedTime_Native / elapsedTime_SSE3 << endl << endl << endl;


	cout << "====Testing LXC_Native====" << endl;
	elapsedTime_Native = test_LXC_ConvolverWav("tyndall_bruce_ortf.wav", "Hitman.wav", LXC_OPT_NATIVE, inputFramelength);
	cout << "Processing time for LXC_Native: " << elapsedTime_Native << "s" << endl;

	cout << endl << "====Testing LXC_SSE3====" << endl;
	elapsedTime_SSE3 = test_LXC_ConvolverWav("tyndall_bruce_ortf.wav", "Hitman.wav", LXC_OPT_SSE3, inputFramelength);
	cout << "Processing time for LXC_SSE3: " << elapsedTime_SSE3 << "s" << endl;

	cout << endl << "-->Speed up: " << elapsedTime_Native / elapsedTime_SSE3 << endl << endl << endl;

	return err;
}

void benchmark_combinedVSseperate(uint FromInputeFrameLength, 
								  uint ToInputFrameLength, 
								  LXC_OPTIMIZATION_MODULE Module,
								  uint MaxIterations)
{
	if(!MaxIterations)
	{
		cout << "Skipped combined VS. seperate channels Benchmark! Proof that MaxIterations>0!" << endl;
		return;
	}

	if(FromInputeFrameLength>=ToInputFrameLength)
	{
		cout << "Skipped combined VS. seperate channels Benchmark! Proof that FromInputeFrameLength<ToInputFrameLength!" << endl;
		return;
	}

	try
	{
		LXC_CBenchmarkCSVExporter benchmarkCSV("benchmark_combinedVSseperate_" + LXC_OptimizationToString(Module) + ".csv",
											   "InputFrameLength;elapsedTimeCombined;elapsedTimeSeperate;SpeedUp;sampleFrequency",
											   5);

		uint startFrameLength = 4;
		uint endFrameLength = 4;
		LXC_Core_checkPowerOfTwo(FromInputeFrameLength, &startFrameLength);
		LXC_Core_checkPowerOfTwo(ToInputFrameLength,	&endFrameLength);

		cout << endl << " Running combined VS. seperate channels Benchmark with " << MaxIterations << " iterations";
		cout << "from framelength " << startFrameLength << " to " << endFrameLength;
		cout << " in steps of 2^x" << endl;

		for(uint frameLength = startFrameLength; frameLength <= endFrameLength; frameLength*=2)
		{
			try
			{
				// run combined channels benchmark
				double elapsedTimeCombined = 0.0;
				double elapsedTimeSeperate = 0.0;

				LXC_CBenchmark_combinedChannels benchmarkCombined(frameLength, 44100, Module, MaxIterations);
				elapsedTimeCombined = benchmarkCombined.RunBenchmark();

			
				LXC_CBenchmark_seperateChannels benchmarkSeperate(frameLength, 44100, Module, MaxIterations);
				elapsedTimeSeperate = benchmarkSeperate.RunBenchmark();

				// run seperate channels benchmark

				// print results
				cout << endl << "====Benchmark results with " << frameLength << " framelength====" << endl;
				cout << " combined channels elapsed time: " << elapsedTimeCombined << " s" << endl;
				cout << " seperate channels elapsed time: " << elapsedTimeSeperate << " s" << endl;
				cout << " Speed Up: " << elapsedTimeSeperate/elapsedTimeCombined << endl;

				// write results to benchmark file
				benchmarkCSV.WriteElement(frameLength);
				benchmarkCSV.WriteElement(elapsedTimeCombined);
				benchmarkCSV.WriteElement(elapsedTimeSeperate);
				benchmarkCSV.WriteElement(elapsedTimeSeperate/elapsedTimeCombined);
				benchmarkCSV.WriteElement((uint)44100);
			}
			catch(LXC_CException &e)
			{
				// ToDo: show a better error message
				cout << "ERROR: benchmark_combinedVSseperate skipped " << frameLength << " framelength" << endl;
				e.showError();

				#ifdef TARGET_WINDOWS
					system("PAUSE");
				#endif
			}
			LXC_HANDLE_BAD_ALLOC
			LXC_HANDLE_RANGE_EXCEPTION
			LXC_HANDLE_UNKNOWN_EXCEPTION
		}
	}
	catch(LXC_CException &e)
	{
		// ToDo: show a better error message
		cout << "ERROR: benchmark_combinedVSseperate skipped " << endl;
		e.showError();

		#ifdef TARGET_WINDOWS
			system("PAUSE");
		#endif
	}
	LXC_HANDLE_BAD_ALLOC
	LXC_HANDLE_RANGE_EXCEPTION
	LXC_HANDLE_UNKNOWN_EXCEPTION
}

// main function
int main()
{
	try
	{
		// init LXC library
		LXC_ERROR_CODE err = LXC_Core_init("");
		if(err != LXC_NO_ERR)
			return err;

		// get LXC library properties
		LXC_PROPERTIES lxcProperties;
		err = LXC_Core_getProperties(&lxcProperties);
		if(err != LXC_NO_ERR)
			return err;

		cout << "====Testing LXC_Native 1 channel convolution====" << endl;
		err = test_LXC_Convolver_1Ch(LXC_OPT_NATIVE);
		if(err != LXC_NO_ERR)
			return err;

		cout << "====Testing LXC_SSE3 1 channel convolution====" << endl;
		err = test_LXC_Convolver_1Ch(LXC_OPT_SSE3);
		if(err != LXC_NO_ERR)
			return err;

		cout << endl << endl;
		cout << "====Testing LXC_Native 2 channel convolution====" << endl;
		err = test_LXC_Convolver_2Ch(LXC_OPT_NATIVE);
		if(err != LXC_NO_ERR)
			return err;

		cout << "====Testing LXC_SSE3 2 channel convolution====" << endl;
		err = test_LXC_Convolver_2Ch(LXC_OPT_SSE3);
		if(err != LXC_NO_ERR)
			return err;

		cout << endl << endl;
		cout << "====Testing LXC_SSE3 complex multiplication Kernel functions====" << endl;
		err = test_LXC_SSE3();
		if(err != LXC_NO_ERR)
			return err;

		cout << endl << endl;
		cout << "====Testing LXC_SSE3 convolution vs. LXC_Native convolution====" << endl;
		err = test_SSE3vsNative(512);
		if(err != LXC_NO_ERR)
			return err;
	
		cout << endl << endl;
		cout << "====Benchmarking combined vs seperate channels for LXC_Native====" << endl;
		benchmark_combinedVSseperate(4, 8192, LXC_OPT_NATIVE, 20);

		cout << endl << endl;
		cout << "====Benchmarking combined vs seperate channels for LXC_SSE3====" << endl;
		benchmark_combinedVSseperate(4, 8192, LXC_OPT_SSE3, 20);
		

	#ifdef TARGET_WINDOWS
		cout << endl << endl;
		system("PAUSE");
	#endif

		LXC_Core_close();

		return err;
	}
	catch(LXC_CException &e)
	{
		LXC_Core_close();
		e.showError();

	#ifdef TARGET_WINDOWS
		system("PAUSE");
	#endif

		return -1;
	}
	LXC_HANDLE_BAD_ALLOC
	LXC_HANDLE_RANGE_EXCEPTION
	LXC_HANDLE_UNKNOWN_EXCEPTION
}
