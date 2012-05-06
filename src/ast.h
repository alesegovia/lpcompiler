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

#ifndef AST_H
#define AST_H

#include <string>
using std::string;

#include <list>
using std::list;

#include <algorithm>
using std::pair;

#include <exception>
using std::exception;

#include <cassert>

#include <cstdlib>

#include <fstream>
using std::ofstream;

#include <iostream>
using std::cout;
using std::endl;
using std::ostream;

#include "tipos.h"
#include "symbolstack.h"
#include "functiontable.h"

//
//Constantes
//
const int tabSize = 4;
const int maxStack = 10;
const string className = "Main";
//
//Enums
//
typedef enum { OP_PAR, OP_ASSIGN, OP_PLUS, OP_MINUS, OP_TIMES, OP_DIVIDEBY, 
	OP_NOT, OP_EQUALS, OP_NOTEQUAL, OP_OR, OP_AND, OP_LESS, OP_LESSEQ,  
	OP_GREATER, OP_GREATEREQ} EOperator;

//
//Clases
//

class Exp;

/** 
 * @class Node
 * @brief Clase abstracta padre de todos los nodos.
 **/
class Node
{
	protected:
		int linenum;
	public:
		virtual void prettyPrint(ostream&, int) = 0;
		virtual void checkSemantics(CPilaDeSimbolos&, CFunctionTable&) {}
		virtual Exp* optimize() { return NULL; }
		virtual void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&) = 0;
		virtual ~Node() {}
	
};

/**
 * @class Statement 
 * @brief Representa un Statement, extiende Node y es abstracta.
 **/
class Statement : public Node
{
	
};


/**
 * @class RootNode
 * @brief Clase que modela la raiz del AST. Extiende Node.
 **/
class RootNode : public Node
{
	private:
		list<Node*>* children;
	public:
		RootNode();
		void prettyPrint(ostream&, int);
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);
		virtual Exp* optimize();
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);
		void append(Node *);
		~RootNode();
};

/**
 * @class Exp
 * @brief Clase abstracta padre de todas las expresiones. Extiende Node.
 **/
class Exp : public Node
{
	public:
		virtual ExpType getType(CPilaDeSimbolos&, CFunctionTable&) = 0;
		/** Evalua si una expresion es un numero, de asi serlo, devuelve true y el numero al que evalua. **/		 
		virtual bool isNum(float&) { return false; }
};

/** 
 * @class StmtIf
 * @brief Representa Statements de tipo if, extiende Statement.
 **/
class StmtIf : public Statement
{
	private:
		Exp *exp;
		Statement* trueStmt;
		Statement* falseStmt;
	public:
		StmtIf(Exp *, Statement*, Statement*);
		void prettyPrint(ostream&, int);
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);
		virtual Exp* optimize();
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);
		~StmtIf();
};

/** 
 * @class StmtWhile
 * @brief Representa Statements del tipo while, extiende Statement.
 **/
class StmtWhile : public Statement
{
	private:
		Exp *exp;
		Statement *stmt;
	public:
		StmtWhile(Exp *, Statement *);
		void prettyPrint(ostream&, int);
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);
		virtual Exp* optimize();
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);
		~StmtWhile();
};

/**
 * @class StmtReturn
 * @brief Representa un Statement del tipo return, extiende Statement.
 **/
class StmtReturn : public Statement
{
	private:
		Exp *exp;
	public:
		StmtReturn(Exp *);
		void prettyPrint(ostream&, int);
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);
		virtual Exp* optimize();
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);
		~StmtReturn();
};



/**
 * @class BinExp
 * @brief Clase que representa expresiones binarias. Extiende Exp.
 **/
class BinExp : public Exp
{
	private:
		EOperator op;
		Exp *exp1;
		Exp *exp2;
	public:
		/** 
		 * Crea una nueva instancia de BinExp
		 * @param EOperator Operador de la Expresion
		 * @param Exp Expresion 1
		 * @param Exp Expresion 2
		 **/
		BinExp(EOperator, Exp *, Exp *);

		/** 
		 * Imprime recursivamente esta expresion y sus miembros.
		 * @param ostream flujo de salida donde imprimir.
		 * @param nivel de identacion.
		 **/
		void prettyPrint(ostream&, int);

		/**
		 * Realiza un analisis semantico de esta expresion.
		 * @param CPilaDeSimbolos pila conteniendo los simbolos definidos hasta el momento.
		 **/
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);
		
		/**
		 * Devuelve el tipo de esta expresion.
		 **/
		ExpType getType(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Calcula a que tipo se evaluara la expresion
		 **/
		ExpType evalType();

		/**
		 * Determina si esta expresion evalua a un numero
		 **/
		bool isNum(float&);

		/**
		 * Aplica constant folding a la expresion binaria
		 **/
		virtual Exp* optimize();

		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);		

		/**
		 * Destructor.
		 **/
		~BinExp();
};

/**
 * @class UnExp
 * @brief Representa expresiones unarias. Extiende Exp.
 **/
class UnExp : public Exp
{
	private: 
		EOperator op;
		Exp *exp;
	public:
		/**
		 * Crea una nueva instancia de UnExp.
		 * @param EOperator operador de esta expresion.
		 * @param Exp expresion asociada.
		 **/
		UnExp(EOperator, Exp *);
		
		/**
		 * Imprime esta expresion.
		 * @param ostream flujo de salida donde imprimir.
		 * @param int nivel de profundidad de identacion.
		 **/
		void prettyPrint(ostream&, int);

		/**
		 * Realiza un analisis semantico sobre esta expresion y quienes la componen..
		 **/
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);
		
		/**
		 * Devuelve el tipo de esta expresion
		 **/
		ExpType getType(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Determina si esta expresion evalua a un numero
		 **/
		bool isNum(float&);

		/**
		 * Envia a optimizar sus hijos
		 **/
		virtual Exp* optimize();

		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);

		/** Destructor **/
		~UnExp();
};

/**
 * @class IDNode
 * @brief Representa un ID. Extiende Exp.
 **/
class IDNode : public Exp
{
	private:
		string id;
	public:
		/** 
		 * Crea una nueva instancia de IDNode.
		 * @param char* string conteniendo el nombre del identificador.
		**/
		IDNode(char *);
		/**
		 * Imprime este nodo.
		 * @param ostream flujo de salida donde imprimir.
		 * @param int nivel de identacion.
		 **/
		void prettyPrint(ostream&, int);

		/**
		 * Revisa la semantica de esta expresion.
		 **/
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Devuelve una representacion literal de este identificador.
		 * @return una representacion literal de este identificador como string.
		 **/
		string toString() { return id; }

		/**
		 * Devuelve el tipo de esta expresion.
		 **/
		ExpType getType(CPilaDeSimbolos&, CFunctionTable&);
};

/**
 * @class IntConst
 * @brief Representa una Constante de Entero (int). Extiende Exp.
 **/
class IntConst : public Exp
{
	private:
		int i;
	public:
		/**
		 * Crea una nueva instancia de IntConst
		 * @param int entero representado.
		 **/
		IntConst(int i);
		/**
		 * Imprime este nodo.
		 * @param ostream flujo de salida donde imprimir.
		 * @param int nivel de identacion.
		 **/
		void prettyPrint(ostream&, int);
		/**
		 * Stub. Una constante entera siempre es valida en el contexto de una expresion.
		**/
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Determina si esta expresion evalua a un numero
		 **/
		bool isNum(float& num) { num = i; return true; }

		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);

		/** Devuelve el tipo asociado a esta constante, TINT.**/
		ExpType getType(CPilaDeSimbolos&, CFunctionTable&);
};

/**
 * @class FloatConst
 * @brief Representa una constante de Real (float). Extiende Exp.
 **/
class FloatConst : public Exp
{
	private:
	       	float f;
	public:
		/**
		 * Crea una nueva instanca de FloatConst.
		 * @param float el punto flotante representado.
		 **/
		FloatConst(float f);
		/**
		 * Imprime este nodo.
		 * @param ostream flujo de salida donde imprimir.
		 * @param int nivel de identacion.
		 **/
		void prettyPrint(ostream&, int);
		/**
		 * Stub. Una constante float siempre es valida en el contexto de una expresion.
		**/
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Determina si esta expresion evalua a un numero
		 **/
		bool isNum(float& num) { num = f; return true; }

		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);

		/** Devuelve el tipo asociado a esta constante, TFLOAT.**/
		ExpType getType(CPilaDeSimbolos&, CFunctionTable&);
};

/**
 * @class StringConst
 * @brief Representa una constante de String. Extiende Exp.
 **/
class StringConst : public Exp
{
	private:
		string s;
	public:
		/**
		 * Crea una nueva instancia de StringConst.
		 * @param string literal representado.
		 **/
		StringConst(string s);
		/**
		 * Imprime este nodo.
		 * @param ostream flujo de salida donde imprimir.
		 * @param int nivel de identacion.
		**/
		void prettyPrint(ostream&, int);
		/**
		 * Stub. Una constante literal siempre es valida en el contexto de una expresion.
		 **/
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);

		/** Devuelve el tipo asociado a esta constante, TSTRING. **/
		ExpType getType(CPilaDeSimbolos&, CFunctionTable&);
};

/**
 * @class BoolConst
 * @brief Representa constantes booleanas. Extiende Exp.
 **/
class BoolConst : public Exp
{
	private: 
		bool b;
	public:
		/**
		 * Crea una nueva instacia de BoolConst.
		 * @param bool valor boolean representado.
		 **/
		BoolConst(bool b);
		/**
		 * Imprime este nodo.
		 * @param ostream flujo de salida donde imprimir.
		 * @param int nivel de identacion.
		**/
		void prettyPrint(ostream&, int);
		/**
		 * Stub. Una constante booleana siempre es valida en el contexto de una expresion.
		 **/
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Devuelve el tipo asociado a esta constante, TBOOL.
		**/
		ExpType getType(CPilaDeSimbolos&, CFunctionTable&);
};

/**
 * @class ArgsNode
 * @brief Es un nodo compuesto de una lista de pares (Tipo,id). Extiende Node.
 **/
class ArgsNode : public Node
{
	private:
		list< pair<Tipo,IDNode*> >* args;
	public:
		ArgsNode(list< pair<Tipo,IDNode*> >*);

		list< pair<Tipo,IDNode*> >* getList();

		void prettyPrint(ostream&, int);

		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);

		list<Tipo> getTypeList();

		~ArgsNode();
		
};

/** 
 * @class BodyNode
 * @brief Este nodo es un cuerpo.
**/
class BodyNode : public Statement
{
	private:
		list<Statement*>* stmts;
	public:
		BodyNode(list<Statement*>*);

		void prettyPrint(ostream&, int);

		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Envia optimizar a sus hijos
		 **/
		virtual Exp* optimize();

		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);

		~BodyNode();
};

/**
 * @class FuncDeclNode
 * @brief Representa una declaracion de funcion.
 **/
class FuncDeclNode : public Node
{
	private:
		//Tipo de Retorno
		Tipo retType;
		//Nombre de la funcion
		IDNode *id;
		//Lista de argumentos
		ArgsNode* args;
		//Cuerpo
		BodyNode* body;
	public:
		FuncDeclNode(Tipo, IDNode*, ArgsNode*, BodyNode*);

		void prettyPrint(ostream&, int);

		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Envia a optimizar a sus hijos
		 **/
		virtual Exp* optimize();

		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);

		~FuncDeclNode();
		
};




/** 
 * @class FCallNode
 * @brief Este nodo representa una llamada a funcion, con sus argumentos
**/
class FCallNode : public Exp, public Statement
{
	private:
		IDNode* id;
		bool isStmt;
		list<Exp*>* args;
	public:
		/** 
		 * Crea una nueva intancia de FCallNode.
		 * @param IDNode* identificador de la funcion llamada.
		 * @param list<Exp*>* lista de parametros.
		 * @param bool determina si esta funcion esta siendo utilizada como Statement para el PrettyPrint.
		 **/
		FCallNode(IDNode*, list<Exp*>*, bool isStmt = true);
		/**
		 * Imprime este nodo.
		 * @param ostream flujo de salida donde imprimir.
		 * @param int nivel de identacion.
		 **/
		void prettyPrint(ostream&, int);
		/**
		 * Realiza validaciones semanticas sobre la llamada.
		 **/
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);
		/**
		 * Aplica constant folding a los parametros a pasar
		 **/
		virtual Exp* optimize();
		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);
		/**
		 * Cambia el uso, como Statement o Expresion
		 **/
		void setStmt(bool);
		/**
		 * Devuelve el tipo de esta llamada (sera el tipo de retorno de la funcion invocada).
		 **/
		ExpType getType(CPilaDeSimbolos&, CFunctionTable&);

		/**
		 * Devuelve el identificador asociado
		 **/
		string getId() { return id->toString(); }
		/** Destructor **/
		~FCallNode();
};

/** 
 * @class DeclNode 
 * @brief Este nodo representa una declaracion de variable. 
**/
class DeclNode : public Statement
{
	private:
		/** Tipo de la variable **/
		Tipo t;
		/**Identificador declarado**/
		IDNode* id;
		/** Expresion o funcion a la cual se inicializa.**/
		Exp* exp;
	public:
		DeclNode(Tipo, IDNode*, Exp*);
		void prettyPrint(ostream&, int);
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);
		/**
		 * Optimiza la expresion.
		 **/
		virtual Exp* optimize();
		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);
		~DeclNode();

};

class AssignNode : public Statement
{
	private:
		IDNode* id;
		Exp* exp;
	public:
		AssignNode(IDNode*, Exp*);
		void prettyPrint(ostream&, int);
		void checkSemantics(CPilaDeSimbolos&, CFunctionTable&);
		/**
		 * Aplica constant folding a la expresion.
		 **/
		virtual Exp* optimize();
		/**
		 * Compila esta expresion a jasmin.
		 * @param ostream flujo de salida.
		 * @param CPilaDeSimbolos pila de simbolos.
		 * @param CFunctionTable tabla de funciones.
		 **/
		void compile(ostream&, CPilaDeSimbolos&, CFunctionTable&);
		~AssignNode();
};

#endif
