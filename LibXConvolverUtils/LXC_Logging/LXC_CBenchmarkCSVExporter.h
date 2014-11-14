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



#include "../LXC_Exceptions/LXC_CExceptionCout.h"
#include "../../LibXConvolverCore/include/LXC_CommonTypes.h"
#include "../../LibXConvolverCore/include/LXC_Core_types.h"
#include <iostream>
#include <fstream>
#include <string>

class LXC_CBenchmarkCSVExporter
{
public:
	LXC_CBenchmarkCSVExporter(std::string Filename, std::string Header, uint MaxCols);
	~LXC_CBenchmarkCSVExporter();

	LXC_ERROR_CODE WriteElement(double Element);
	LXC_ERROR_CODE WriteElement(uint Element);

private:
	std::ofstream m_OutputFile;
	std::string m_Filename;
	std::string m_FileHeader;
	uint m_MaxCols;
	uint m_CurrentCol;
};