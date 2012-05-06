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

# ifndef CNODO_PILA
# define CNODO_PILA

///////////////////////////// Dependencias Externas/Internas /////////////////////////////////

# include <string>

using std::string;

# include "tipos.h"
using util::Tipo2ExpType;

////////////////////////////////////// Enumeraciones /////////////////////////////////////////

enum TipoNodo { DEF_VAR, INI_ALCANCE, INI_FUNCION };

//////////////////////////////////// Definición de Clase /////////////////////////////////////

/**
 *
 * @class CNodoPila stacknode.h
 * @brief Almacena los datos básicos a almacenar en el tabla de simbolos.
 *
 * .
 *
 */

class CNodoPila {

	private:
		
		// Tipo de entrada que se está almacenando en el pila.
		TipoNodo m_tipoNodo;

		// Tipo de dato que identifica a la definición de la variable.
		Tipo m_tipoDato;

		// Nombre que identifica al nodo en el caso que se trate de una definición de variable.
		string m_strID;

		// Identificador del numero de variable correspondiente al ID Node en assembler.
		int m_asmID;

	public:

		// Constructores...
		CNodoPila( TipoNodo tipoNodo, Tipo tipoDato, string strID, int asmID );

		CNodoPila( TipoNodo tipo );

		inline TipoNodo GetTipoNodo() {
			
			return m_tipoNodo;
		}

		inline Tipo GetTipoDato() {
			
			return m_tipoDato;
		}

		inline string GetID() {
			
			return m_strID;
		}

		int GetAsmID() {
			return m_asmID;
		}

};

//////////////////////////////////////////////////////////////////////////////////////////////

# endif // CNODO_PILA
