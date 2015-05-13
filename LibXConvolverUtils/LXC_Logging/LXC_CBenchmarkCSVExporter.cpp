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



#include "LXC_CBenchmarkCSVExporter.h"
using namespace std;

LXC_CBenchmarkCSVExporter::LXC_CBenchmarkCSVExporter(string Filename, string FileHeader, uint MaxCols)
{
	if(Filename.empty())
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Invalid filename!");
	}

	if(!MaxCols)
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Invalid input! MaxCols should be >0!");
	}

	m_MaxCols = MaxCols;
	m_CurrentCol = 0;
	m_Filename = Filename;
	m_FileHeader = FileHeader;

#if defined(TARGET_WINDOWs)
	m_OutputFile.open(m_Filename);
#else
	m_OutputFile.open(m_Filename.c_str());
#endif
	if(!m_OutputFile.is_open())
	{
		throw LXC_EXCEPTION_COUT_HANDLER("Could not open benchmark export file: " + Filename);
	}

	m_OutputFile << m_FileHeader << endl;
}

LXC_CBenchmarkCSVExporter::~LXC_CBenchmarkCSVExporter()
{
	if(m_OutputFile.is_open())
	{
		m_OutputFile.close();
	}
}

LXC_ERROR_CODE LXC_CBenchmarkCSVExporter::WriteElement(double Element)
{
	if(!m_OutputFile.is_open())
	{
		return LXC_ERR_NOT_INIT;
	}

	if(m_CurrentCol < m_MaxCols -1)
	{
		m_OutputFile << Element << ";";
		m_CurrentCol++;
	}
	else
	{
		m_OutputFile << Element << endl;
		m_CurrentCol = 0;
	}

	return LXC_NO_ERR;
}

LXC_ERROR_CODE LXC_CBenchmarkCSVExporter::WriteElement(uint Element)
{
	if(!m_OutputFile.is_open())
	{
		return LXC_ERR_NOT_INIT;
	}

	if(m_CurrentCol < m_MaxCols -1)
	{
		m_OutputFile << Element << ";";
		m_CurrentCol++;
	}
	else
	{
		m_OutputFile << Element << endl;
		m_CurrentCol = 0;
	}

	return LXC_NO_ERR;
}
