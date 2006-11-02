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

# include <iostream>
using std::cout;
using std::endl;

# include "symbolstack.h"

//////////////////////////////// Implementación de Clase /////////////////////////////////////

/////////////////////
// CPilaDeSimbolos //
/////////////////////

/********************************************************************************************/

/**
 *
 * Ingresa a la lista un nuevo nodo que marca el inicio del alcance de una nueva función.
 *
 */

void CPilaDeSimbolos::NuevaFuncion(Tipo tipo, string ID) {

	m_lista.push_front( CNodoPila( INI_FUNCION, tipo, ID, -1 ) );
}

/********************************************************************************************/

/**
 *
 * Ingresa a la lista un nuevo nodo que marca el inicio del alcance de una nueva función.
 *
 */

void CPilaDeSimbolos::NuevoAlcance() {

	m_lista.push_front( CNodoPila( INI_ALCANCE ) );
}

/********************************************************************************************/

/**
 *
 * Ingresa a la lista un nuevo nodo que almacena la definición de una nueva variable.
 *
 * @param tipoDato Da el tipo de datos de la correspondiente entrada en la pila.
 * @param strID Un nómbre único para identificar a la variable.
 *
 */

void CPilaDeSimbolos::NuevaDefinicion( Tipo tipoDato, string strID )
{
	//Recorrer hacia atras hasta encontrar INI_FUNCION o DEF_VAR
	list< CNodoPila >::iterator it = m_lista.begin();
	while (it != m_lista.end() && it->GetTipoNodo() == INI_ALCANCE)
	{
		++it;
	}

	if ( it == m_lista.end() || (it->GetTipoNodo() != INI_FUNCION && it->GetAsmID() == -1) ) //Es una variable global, no tiene id en assembler
	{
		m_lista.push_front( CNodoPila( DEF_VAR, tipoDato, strID, -1 ) );
		return;
	}
	if (it->GetTipoNodo() == INI_FUNCION)
	{
		m_lista.push_front( CNodoPila( DEF_VAR, tipoDato, strID, 0 ) );
		return;
	}

	m_lista.push_front( CNodoPila( DEF_VAR, tipoDato, strID, it->GetAsmID() + 1 ) );
}

/********************************************************************************************/


/** Elimina todos los nodos de la pila hasta la ultima definición de funcion ingresada.**/
void CPilaDeSimbolos::FinFuncion()
{
	FinMarcador(INI_FUNCION);
}


/** Elimina todos los nodos de la pila hasta la ultima definición de alcance ingresada.**/
void CPilaDeSimbolos::FinAlcance()
{
	FinMarcador(INI_ALCANCE);
}

/**
 *
 * Elimina todos los nodos de la pila hasta la ultima definición del nodo especificado.
 *
 */
void CPilaDeSimbolos::FinMarcador(TipoNodo mark) 
{
	bool salir = false;
	list<CNodoPila>::iterator it = m_lista.begin();
	//Vamos quitando el frente hasta llegar a INI_ALCANCE o INI_FUNCION
	while (!salir && (it != m_lista.end()))
	{		
		if (it->GetTipoNodo() == mark)
		{
			salir = true;
		}
		m_lista.pop_front();
		it = m_lista.begin();
	}

}

/********************************************************************************************/

/**
 *
 * Determina si una variable está en el alcance actual (de la recorrida del arbol sintactico
 * abstracto) en la pila. El tipo es impostante que sea un parametro de busqueda ya que en
 * diferentes alcances se permite definir variables con iguales ID, pero con diferentes
 * tipos de datos asociados.
 *
 * @param tipoDato El tipo que identifica a la variable que se está buscando.
 * @param strID El nombre que identifica a la variable que se está buscando.
 *
 */

bool CPilaDeSimbolos::EstaVarDefinida( string strID ) {

	std::list< CNodoPila >::iterator i;
	for ( i = m_lista.begin() ; i != m_lista.end() ; i++ ) {

		if ( ( i -> GetID() ) == strID ) {
			
			return true;
		}
	}

	return false;
}

/********************************************************************************************/


bool CPilaDeSimbolos::EstaVarEnAlcance( string strID ) {

	std::list< CNodoPila >::iterator i;
	for ( i = m_lista.begin() ; (i != m_lista.end()) && (i->GetTipoNodo() != INI_ALCANCE) ; i++ ) {

		if ( ( i -> GetID() ) == strID ) {
			
			return true;
		}
	}

	return false;
}


/********************************************************************************************/

ExpType CPilaDeSimbolos::getTipo( string strID ) {

	std::list< CNodoPila >::iterator i;
	for ( i = m_lista.begin() ; i != m_lista.end() ; i++ ) {

		if ( ( i -> GetID() ) == strID ) 
		{	
			return Tipo2ExpType(i->GetTipoDato());
		}
	}

	return ET_UNKNOWN;
}
/********************************************************************************************/
ExpType CPilaDeSimbolos::getCurrFuncType()
{
	//Vamos recorriendo hasta llegar a INI_FUNCION
	for ( list<CNodoPila>::iterator it = m_lista.begin() ; it != m_lista.end() ; it++ )
	{		
		if (it->GetTipoNodo() == INI_FUNCION)
		{			
			return getTipo(it->GetID());
		}

	}
	return ET_UNKNOWN;
}
/********************************************************************************************/

int CPilaDeSimbolos::GetAsmID( string strID )
{
	list< CNodoPila >::iterator it = m_lista.begin();
	while (it != m_lista.end()) //it->GetTipoNodo() != INI_FUNCION
	{
		if (it->GetTipoNodo() == DEF_VAR && it->GetID() == strID)
		{
			return it->GetAsmID();
		}
		++it;
	}
	return -2; //Error
}

/********************************************************************************************/
string CPilaDeSimbolos::getCurrFuncID()
{
	//Vamos recorriendo hasta llegar a INI_FUNCION
	for ( list<CNodoPila>::iterator it = m_lista.begin() ; it != m_lista.end() ; it++ )
	{		
		if (it->GetTipoNodo() == INI_FUNCION)
		{			
			return it->GetID();
		}

	}
	return string("__GLOBAL__");
}

