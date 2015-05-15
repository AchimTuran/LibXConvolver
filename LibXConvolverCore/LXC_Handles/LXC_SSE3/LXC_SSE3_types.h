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



#include "../../include/LXC_CommonTypes.h"

#define LXC_SSE3_ALIGN			16
#if defined(TARGET_WINDOWS)
  typedef __declspec(align(LXC_SSE3_ALIGN))float LXC_SSE3cpxFloat[2];
  typedef __declspec(align(LXC_SSE3_ALIGN))float LXC_SSE3Float;
#else
  typedef float LXC_SSE3cpxFloat[2];
  typedef float LXC_SSE3Float;
#endif
