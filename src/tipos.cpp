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

#include "tipos.h"

ExpType util::Tipo2ExpType(Tipo tipo)
{
	switch(tipo)
	{
		case TBOOL: return ET_BOOL;
		case TINT: return ET_INT;
		case TFLOAT: return ET_FLOAT;
		case TSTRING: return ET_STRING;
		case TVOID: return ET_VOID;
		default: return ET_UNKNOWN;
	}
}

//
//Utilidades
//


/** Impresion de tipos **/
void util::indent(ostream& out, int depth)
{
	for (int i = 0; i < depth; i++)
	{
		out << " ";
	}
}


void util::prettyPrint(ostream& out, Tipo t, int depth)
{
	util::indent(out, depth);
	switch (t)
	{
		case TVOID : out << "void "; break;
		case TBOOL : out << "bool "; break;
		case TFLOAT: out << "float "; break;
		case TINT: out << "int "; break;
		case TSTRING: out << "string "; break;
		default: out << "ERROR: nodo tipo de retorno invalido!"; //No deberia entrar aqui
	}
}
/** Compilacion de tipos **/
string util::compile(ExpType et)
{
	switch (et)
	{
		case ET_INT: return string("I");
		case ET_BOOL: return string("Z");
		case ET_FLOAT: return string("F");
		case ET_STRING: return string("Ljava/lang/String;");
		case ET_VOID: return string("V");
		default: cout << "Compiler Bug: Impossible to compile given type" << endl; abort(); //No deberia entrar aqui
	}
}

/** Compilacion de tipos para instrucciones **/
string util::compileInst(ExpType et)
{
	switch (et)
	{
		case ET_INT: return string("i");
		case ET_BOOL: return string("i");
		case ET_FLOAT: return string("f");
		case ET_STRING: return string("a");
		default: cout << "Compiler Bug: Impossible to compile given type" << endl; abort(); //No deberia entrar aqui
	}
}
