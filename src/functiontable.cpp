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

#include "functiontable.h"

//		Tipo ret;
//		list<Tipo> args;

CFuncArgsType::CFuncArgsType(Tipo t, list<Tipo> largs)
{
	ret = t;
	locals = 0; //El minimo de variables locales para jasmin es 1
	for (list<Tipo>::iterator it = largs.begin(); it != largs.end(); it++)
	{
		args.push_back((*it));
	}
}

Tipo CFuncArgsType::getReturnType()
{
	return ret;
}


bool CFuncArgsType::checkArgNum(ExpType expType, int i)
{
	if (i >= 0 && i < args.size())
	{
		ExpType argType = Tipo2ExpType(args[i]);
		if ((argType == expType) || (argType == ET_FLOAT && expType == ET_INT))
		{
			return true;
		}
	}
	return false;
}

ExpType CFuncArgsType::getArgNum(int i)
{
	if (i >= 0 && i < args.size())
	{
		return Tipo2ExpType(args[i]);
	}
	return ET_UNKNOWN;
}

bool CFunctionTable::addFunction(Tipo t, string f, list<Tipo> args)
{
	if (table.find(f) == table.end())
	{
		table.insert( make_pair( f, CFuncArgsType (t, args)) );
		return true;
	}
	return false;
}

bool CFunctionTable::isDeclared(string f)
{
	return (table.find(f) != table.end());
}

bool CFunctionTable::checkArgNum(string f, ExpType t, int i)
{
	if (table.find(f) != table.end())
	{
		return table[f].checkArgNum(t, i);
	}
	return false;
}

int CFunctionTable::getArgCount(string f)
{
	if (table.find(f) != table.end())
	{
		return table[f].getArgCount();
	}
	return -1; //error
}

ExpType CFunctionTable::getArgNumType(string f, int i)
{
	if (table.find(f) != table.end())
	{
		return table[f].getArgNum(i);
	}
	return ET_UNKNOWN;
}

ExpType CFunctionTable::getReturnType(string f)
{
	if (table.find(f) != table.end())
	{
		return Tipo2ExpType(table[f].getReturnType());
	}
	return ET_UNKNOWN;
}

void CFunctionTable::setLocals(string f, int locals)
{
	if (table.find(f) != table.end())
	{
		table[f].setLocals(locals);
	}
}

void CFunctionTable::incLocals(string f)
{
	if (table.find(f) != table.end())
	{
		table[f].incLocals();
	}
}

int CFunctionTable::getLocals(string f)
{
	if (table.find(f) != table.end())
	{
		return table[f].getLocals();
	}
	return -1; //Error, no existe tal funcion.
}
