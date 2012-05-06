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

#ifndef CFUNCTIONTABLE_H
#define CFUNCTIONTABLE_H

#include <map>
using std::map;

#include <string>
using std::string;
#include <list>
using std::list;
#include <vector>
using std::vector;

#include "tipos.h"
using util::Tipo2ExpType;

#include <algorithm>

class CFuncArgsType
{
	private:
		Tipo ret;
		vector<Tipo> args;
		int locals;
	public:
		CFuncArgsType(Tipo, list<Tipo>);
		CFuncArgsType() {}
		Tipo getReturnType();
		int getArgCount() { return args.size(); }
		bool checkArgNum(ExpType, int);
		ExpType getArgNum(int);
		void setLocals(int l) { locals = l; }
		void incLocals() { ++locals; }
		int getLocals() { return locals; }
};

class CFunctionTable
{

	private:
		map<string, CFuncArgsType> table;
	public:
		CFunctionTable() {}
		//Agrega una declaracion de funcion
		bool addFunction(Tipo, string, list<Tipo>);

		//Consulta si un identificador corresponde a una funcion declarada
		bool isDeclared(string);

		//Verifica si dos tipos coinciden
		bool checkArgNum(string, ExpType, int);

		//Devuelte el tipo de algun argumento
		ExpType getArgNumType(string, int);

		//Devuelve el tipo de retorno de una funcion
		ExpType getReturnType(string);

		//Devuelve el numero de argumentos que una funcion espera recibir
		int getArgCount(string);

		//Establece el numero de variables locales de una funcion. (en desuso).
		void setLocals(string, int);

		//Incrementa el numero de variables locales de una funcion
		void incLocals(string);

		//Devuelve en numero de variables locales de una funcion
		int getLocals(string);

};

#endif
