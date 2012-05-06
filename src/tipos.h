/*
 *  LPC - The L+ Compiler. A compiler for a toy language similar to C.
 *  Copyright (C) 2011  Alejandro Segovia and Emilio Pombo
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIPOS_H
#define TIPOS_H

#include <fstream>
using std::ostream;

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <cstdlib>

typedef enum { TBOOL, TINT, TFLOAT, TSTRING, TVOID } Tipo;
typedef enum { ET_BOOL, ET_ARIT, ET_INT, ET_FLOAT, ET_STRING, ET_VAR, ET_FCALL, ET_VOID, ET_UNKNOWN} ExpType;

namespace util
{

	ExpType Tipo2ExpType(Tipo);

	void indent(ostream&, int);

	void prettyPrint(ostream&, Tipo, int);

	string compile(ExpType);

	string compileInst(ExpType);
}

#endif
