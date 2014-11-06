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



#include "LXC_CExceptionCout.h"


LXC_CExceptionCout::LXC_CExceptionCout(string strError, string strFilename, string strFunction, int iLine) :
	LXC_CException(strError, strFilename, strFunction, iLine)
{
}

LXC_CExceptionCout::LXC_CExceptionCout(string strError, string strErrorModule, string strFilename, string strFunction, int iLine) :
	LXC_CException(strError, strErrorModule, strFilename, strFunction, iLine)
{
}


LXC_CExceptionCout::~LXC_CExceptionCout(void)
{
}


void LXC_CExceptionCout::showError()
{
	if(str_errorModule.empty())
	{
		cout << endl << "Error in " << str_errorFilename << ", function " << str_errorFunction << " at line " << i_errorLine << endl;
		cout << "Error message: " << str_error << endl << endl;
	}
	else
	{
		cout << endl << "Error in " << str_errorFilename << ", function " << str_errorFunction << ", in module " << str_errorModule << " at line " << i_errorLine << endl;
		cout << "Error message: " << str_error << endl << endl;
	}
}