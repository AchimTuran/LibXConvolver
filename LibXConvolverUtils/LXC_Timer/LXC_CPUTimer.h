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



#if defined(TARGET_WINDOWS)
	#include <windows.h>
	#include <time.h>
#else
	#include <sys/time.h>
#endif

class LXC_CCPUTimer
{
public:
    LXC_CCPUTimer();
    ~LXC_CCPUTimer();

    void start_Timer();
    void stop_Timer();
    bool is_TimerRunning();
    void pause_Timer();
    double get_ElapsedTime();

    void print_ElapsedTime();

private:
#if defined(TARGET_WINDOWS)
	LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_t1;
	LARGE_INTEGER m_t2;
    double m_elapsedTime;
#else
	timespec m_startTime;
    timespec m_endTime;
#endif

    volatile bool m_activeTimer;
    bool m_completeMeasure;
};
