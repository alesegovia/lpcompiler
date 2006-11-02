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

///////////////////////////// Dependencias Externas/Internas /////////////////////////////////

# include "stacknode.h"

//////////////////////////////// Implementación de Clase /////////////////////////////////////

///////////////
// CNodoPila //
///////////////

/********************************************************************************************/

/**
 *
 * Constructor de CNodoPila.
 *
 * @param tipoDato Da el tipo de datos de la correspondiente entrada en la pila.
 * @param strID Un nómbre único para identificar a la variable.
 *
 */

CNodoPila::CNodoPila(TipoNodo tipoNodo, Tipo tipoDato, string strID, int asmID) : m_strID( strID ), m_asmID(asmID) {

	m_tipoDato = tipoDato;
	m_tipoNodo = tipoNodo;

}

/********************************************************************************************/

/**
 *
 * Constructor de CNodoPila.
 *
 * @param tipoDato Da el tipo de datos de la correspondiente entrada en la pila.
 * @param strID Un nómbre único para identificar a la variable.
 *
 */

CNodoPila::CNodoPila( TipoNodo tipo ) : m_strID("__ALCANCE__")
{ 
	m_tipoNodo = tipo; 
}

/********************************************************************************************/
