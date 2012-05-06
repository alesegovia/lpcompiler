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

# ifndef CPILA_DE_SIMBOLOS
# define CPILA_DE_SIMBOLOS

///////////////////////////// Dependencias Externas/Internas /////////////////////////////////

# include <list>

using std::list;

# include "stacknode.h"

//////////////////////////////////// Definición de Clase /////////////////////////////////////

/**
 *
 * @class CPilaDeSimbolos CPilaDeSimbolos.h
 * @brief Implementa una pila para almacenar las definiciones y alcances de las variables.
 *
 * .
 *
 */

class CPilaDeSimbolos {

	private:
		
		// Lista para almacenar las definiciones y marcas de alcanse.
		list< CNodoPila > m_lista;
		void FinMarcador(TipoNodo);
	
	public:

		// Constructor...
		CPilaDeSimbolos() {}
		~CPilaDeSimbolos() {}

		void NuevaFuncion( Tipo tipo, string strID );
		void NuevoAlcance();
		void NuevaDefinicion( Tipo tipoDato, string strID );

		void FinFuncion();
		void FinAlcance();

		bool EstaVarDefinida( string strID );

		bool EstaVarEnAlcance( string strID );
		
		int GetAsmID( string strID );

		ExpType getTipo( string strID );

		/** Devuelve el tipo de retorno de la funcion actual en la pila **/
		ExpType getCurrFuncType();

		string getCurrFuncID();
		
		list< CNodoPila >& getLista() { return m_lista; }

		inline CNodoPila GetTope() {
			
			return m_lista.front();
		}

		inline void Vaciar() {
			
			m_lista.clear();
		}
};

//////////////////////////////////////////////////////////////////////////////////////////////

# endif // CPILA_DE_SIMBOLOS
