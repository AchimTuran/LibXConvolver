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



#include <iostream>
using namespace std;

#include "CPUTimer.h"
#include "../constants.h"

#if defined(TARGET_WINDOWS)
	#include <windows.h>
#endif

#if defined(TARGET_LINUX)
	#define __FUNCTION__ __func__
#endif

CPUTimer::CPUTimer()
{
    activeTimer = false;
    completeMeasure = false;

#if defined(TARGET_WINDOWS)
	if(!QueryPerformanceFrequency(&frequency))
	{
		cout << "Warning can't initialize timer!" << endl;
	}
#endif
}

CPUTimer::~CPUTimer()
{
}

void CPUTimer::start_Timer()
{
    if( activeTimer )
    {
        cout << "Warning in function " << __FUNCTION__ << " : time measurement is running! Please stop this measurement before running a new one." << endl;
    }
    else
    {
        activeTimer = true;
        if( completeMeasure )
        {
            cout << "Warning in function " << __FUNCTION__ << ": old time measure value lost!" << endl;
        }
        completeMeasure = false;
	#if defined(TARGET_LINUX)
        //gettimeofday( &startTime, 0 );
        clock_gettime( CLOCK_REALTIME, &startTime );
	#endif
	#if defined(TARGET_WINDOWS)
		QueryPerformanceCounter(&t1);
	#endif
    }
}

void CPUTimer::stop_Timer()
{
    if( !activeTimer )
    {
        cout << "Warning in function " << __FUNCTION__ << ": no time measurement was running! Please run a time time measuremnt first" << endl;
    }
    else
    {
        activeTimer = false;
        if( completeMeasure )
        {
            cout << "Warning: old time measure value lost!" << endl;
        }
        completeMeasure = true;

	#if defined(TARGET_LINUX)
        //gettimeofday( &endTime, 0 );
        clock_gettime( CLOCK_REALTIME, &endTime );
	#endif
	#if defined(TARGET_WINDOWS)
		 QueryPerformanceCounter(&t2);
	#endif
    }
}

bool CPUTimer::is_TimerRunning()
{
    return activeTimer;
}

double CPUTimer::get_ElapsedTime()
{
    if( activeTimer )
    {
        cout << "Warning in function " << __FUNCTION__ << ": no complete time measurement availible! Timer measurement is running" << endl;
        return -1.0f;
    }

    if( !completeMeasure )
    {
        cout << "Warning in function " << __FUNCTION__ << ": no complete time measurement availible!" << endl;
        return -1.0f;
    }
    else
    {
        completeMeasure = false;

	#if defined(TARGET_LINUX)
        timespec timeDiff;
        if ( endTime.tv_nsec - startTime.tv_nsec < 0 )
        {
            timeDiff.tv_sec = endTime.tv_sec - startTime.tv_sec -1;
            timeDiff.tv_nsec = 1000000000 + endTime.tv_nsec - startTime.tv_nsec;
        }
        else
        {
            timeDiff.tv_sec = endTime.tv_sec - startTime.tv_sec;
            timeDiff.tv_nsec = endTime.tv_nsec - startTime.tv_nsec;
        }

        return (double)timeDiff.tv_sec + ((double)timeDiff.tv_nsec)*E_m9;
	#endif

	#if defined(TARGET_WINDOWS)
		return ((double)(t2.QuadPart-t1.QuadPart))/((double)(frequency.QuadPart));
	#endif
    }
}

void CPUTimer::pause_Timer()
{
}

void CPUTimer::print_ElapsedTime()
{
    if( !completeMeasure )
    {
        cout << "Warning in function " << __FUNCTION__ << ": no complete time measurement availible!" << endl;
    }
    else
    {
	#if defined(TARGET_LINUX)
        timespec diff;
        if ( endTime.tv_nsec - startTime.tv_nsec < 0 )
        {
            diff.tv_sec = endTime.tv_sec - startTime.tv_sec -1;
            diff.tv_nsec = 1000000000 + endTime.tv_nsec - startTime.tv_nsec;
        }
        else
        {
            diff.tv_sec = endTime.tv_sec - startTime.tv_sec;
            diff.tv_nsec = endTime.tv_nsec - startTime.tv_nsec;
        }

        const double elapsedTime = (double)diff.tv_sec + ((double)diff.tv_nsec)*E_m9;
		cout << "elapsed time: " << elapsedTime << "s" << endl;
	#endif

	#if defined(TARGET_WINDOWS)
        cout << "elapsed time: " << ((double)(t2.QuadPart-t1.QuadPart))/((double)(frequency.QuadPart)) << "s" << endl;
	#endif
    }
}
