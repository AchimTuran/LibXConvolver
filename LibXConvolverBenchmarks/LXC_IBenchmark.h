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



#include "../LibXConvolverUtils/LXC_Timer/LXC_CPUTimer.h"
#include <string>

class LXC_IBenchmark
{
public:
	LXC_IBenchmark(std::string Name)
	{
		if(!Name.empty())
		{
			m_Name = Name;
		}
		else
		{
			m_Name = "Unknown Benchmark";
		}
	}

	// Gets the current Benchmark name.
	std::string GetName()
	{
		return m_Name;
	}

	// This method must be implemented by the specific benchmark.
	virtual double RunBenchmark() = 0;

protected:
	LXC_CCPUTimer m_Timer;

private:
	std::string m_Name;
};