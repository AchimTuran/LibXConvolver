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
#include <string>
using namespace std;

class LXC_CException
{
protected: // protected Variablen
	string str_error;			// Fehlermeldung
	string str_errorModule;		// Module in dem, der Fehler aufgetreten ist
	string str_errorFilename;	// Datei in der, der Fehler aufgetreten ist
	string str_errorFunction;	// Funktion in der, der Fehler aufgetreten ist

	int i_errorLine;			// Zeilennummer bei der, der Fehler aufgetreten ist

	void *v_pointer;			// optionales Argument um Daten noch übergeben zu können

public: // public Funktionen
	LXC_CException(std::string strError, std::string strFilename, std::string strFunction, int iLine, void *vPointer=NULL);
	LXC_CException(std::string strError, std::string strErrorModule, std::string strFilename, std::string strFunction, int iLine, void *vPointer=NULL);
	virtual ~LXC_CException(void);

	virtual void showError() = 0;
};

#define LXC_HANDLE_UNKNOWN_EXCEPTION 	catch(...){ cout << "unknown exception" << endl; }
#define LXC_HANDLE_BAD_ALLOC			catch( bad_alloc &e ){ cout << "bad memory allocation with new: " << e.what() << endl; }
#define LXC_HANDLE_RANGE_EXCEPTION		catch( range_error &e ){ cout << "range error: " << e.what() << endl; }
