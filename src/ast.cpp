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

#include "ast.h"

//
//Variables Globales
//

// Contador de etiquetas:
int label = 0;
//Linea actual en el fuente L+
extern int line;
//Nombre del archivo de entrada
extern const char *filename;
//Bandera que indica si el analisis semantico fue exitoso
extern bool semantics;


/** Implementacion de la Clase RootNode **/
RootNode::RootNode()
{
	linenum = line;
	children = new list<Node *>();
}

RootNode::~RootNode()
{
	while (!(children->empty()))
	{
		delete children->front();
		children->pop_front();
	}
	delete children;
}

void RootNode::prettyPrint(ostream& out, int depth)
{	
	for (list<Node*>::iterator it = children->begin(); it != children->end(); it++)
	{		
		(*it)->prettyPrint(out, depth);
	}
}

void RootNode::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	for (list<Node*>::iterator it = children->begin(); it != children->end(); it++)
	{		
		(*it)->checkSemantics(stack, functable);
	}
}

Exp* RootNode::optimize()
{
	for (list<Node*>::iterator it = children->begin(); it != children->end(); it++)
	{		
		(*it)->optimize();
	}
	return NULL;
}

void RootNode::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	label = 0;

	//Compilar instrucciones
	for (list<Node*>::iterator it = children->begin(); it != children->end(); it++)
	{		
		(*it)->compile(out, stack, functable);
	}
}

void RootNode::append(Node *node)
{
	children->push_front(node);
}

/** Implementacion de Clase StmtIf **/
StmtIf::StmtIf(Exp *e, Statement* trueStmt, Statement* falseStmt)
{
	linenum = line;
	//La expresion a evaluar
	this->exp = e;
	//Las listas podria llegar a contener 1 solo elemento: un body.
	this->trueStmt = trueStmt;
	this->falseStmt = falseStmt;
}

void StmtIf::prettyPrint(ostream& out, int depth) 
{
	util::indent(out,depth);
	out << "if ( "; exp->prettyPrint(out, depth); out << " )\n";

	//imprimir el cuerpo del if
	//for (list<Statement*>::iterator it = trueStmts->begin(); it != trueStmts->end(); it++)
	//{
		trueStmt->prettyPrint(out, depth + tabSize); 
	//}

	//imprimir el cuerpo del else, si no es null
	if (falseStmt != NULL)
	{		
		util::indent(out, depth);
		out << "else\n";
		//for (list<Statement*>::iterator it = falseStmts->begin(); it != falseStmts->end(); it++)
		//{
			falseStmt->prettyPrint(out, depth); 
		//}
	}
}

void StmtIf::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Revisar que la expresion sea booleana
	if (exp->getType(stack, functable) != ET_BOOL)
	{
		cout << "Error: line[" << linenum << "]: Conditional expression is not Boolean" << endl;		
		semantics = false;
	}
	//Revisar Declaraciones:
	exp->checkSemantics(stack, functable);
	//chequear el cuerpo del if

	trueStmt->checkSemantics(stack, functable); 

	//chequear el cuerpo del else, si no es null
	if (falseStmt != NULL)
	{		
		falseStmt->checkSemantics(stack, functable); 
	}
}

Exp* StmtIf::optimize()
{
	//Optimizar la expresion
	Exp *opt = exp->optimize();
	if (opt != NULL)
	{
		delete exp;
		exp = opt;
	}

	//Mandar a optimizar los statements de true
	trueStmt->optimize();

	//Mandar a optimizar los statements de false, si no son NULL
	if (falseStmt != NULL)
	{
		falseStmt->optimize();
	}
	return NULL;
}

void StmtIf::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Compilar expresion
	exp->compile(out, stack, functable);

	//Decidir si vamos por if o else
	int lblCmp = label++; //label de la comparacion
	out << "\tifeq Label" << lblCmp << "\n"; //ifeq salta si la condicion es falsa

	//Compilar cuerpo true
	trueStmt->compile(out, stack, functable); 

	//Label para saltar el "else"
	int lblNoElse = label++;
	out << "\tgoto Label" << lblNoElse << "\n";

	//Label para condicion falsa:
	out << "Label" << lblCmp << ":\n";

	//Compilar el cuerpo del else, si no es null
	if (falseStmt != NULL)
	{		
		falseStmt->compile(out, stack, functable); 
	}
	out << "Label" << lblNoElse << ":\n";
}

StmtIf::~StmtIf()
{
	delete exp;
	delete trueStmt;

	//Eliminar else si existe
	if (falseStmt != NULL)
	{
		delete falseStmt;
	}
	
}

/** Implementacion de Clase StmtWhile **/
StmtWhile::StmtWhile(Exp *exp, Statement* stmt)
{ 
	linenum = line;
	this->exp = exp;
	this->stmt = stmt;
}

void StmtWhile::prettyPrint(ostream& out, int depth)
{
	util::indent(out,depth);
	out << "while ( "; exp->prettyPrint(out, depth); out << " )\n";
	stmt->prettyPrint(out, depth);	
}

void StmtWhile::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Revisar Expresion Condicional:
	if (exp->getType(stack, functable) != ET_BOOL)
	{
		cout << "Error: Conditional expression in \"while\" construct is not Boolean" << endl;
		semantics = false;
	}
	//Revisar declaraciones:
	exp->checkSemantics(stack, functable);
	stmt->checkSemantics(stack, functable);
}

Exp* StmtWhile::optimize()
{
	//Optimizar la expresion
	Exp* opt = exp->optimize();
	if (opt != NULL)
	{
		delete exp;
		exp = opt;
	}

	//Mandar optimizar statements
	stmt->optimize();
	return NULL;
}

void StmtWhile::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Etiqueta para la comparacion
	int lblCmp = label++; //etiqueta para comparar
	out << "Label" << lblCmp << ":\n";

	//Compilar condicion
	exp->compile(out, stack, functable);

	//Compilar el salto
	int lblJmp = label++; //etiqueta luego del cuerpo
	out << "\tifeq Label" << lblJmp << "\n";

	//Compilar el cuerpo
	stmt->compile(out, stack, functable);
	
	out << "\tgoto Label" << lblCmp << "\n";
	out << "Label" << lblJmp << ":\n";
}

StmtWhile::~StmtWhile()
{
	delete exp;
	delete stmt;
}

/** Implementacion de Clase StmtReturn **/
StmtReturn::StmtReturn(Exp* exp) 
{
	linenum = line;
	//exp podria llegar a ser NULL en el caso de "devolver void"
	this->exp = exp;
}

void StmtReturn::prettyPrint(ostream& out, int depth)
{ 
	util::indent(out, depth);
	out << "return ";
	if (exp != NULL)
	{
		exp->prettyPrint(out, depth);
	}
	out << ";\n";
}

void StmtReturn::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	if (exp != NULL)
	{
		exp->checkSemantics(stack, functable);
		//Comparar con el tipo de retorno de la funcion
		ExpType funcType = stack.getCurrFuncType();
		ExpType expType = exp->getType(stack, functable);
		bool ok = false;
		switch (funcType)
		{
			case ET_INT: if (expType == ET_INT) ok = true; break;
			case ET_BOOL: if (expType == ET_BOOL) ok = true; break;
			case ET_STRING: if (expType == ET_STRING) ok = true; break;
			case ET_FLOAT: if (expType == ET_INT || expType == ET_FLOAT) ok = true; break;
			case ET_VOID: cout << "Error: line[" << linenum << "]: The function returns void\n"; semantics = false; break;
			default: ok = false;
		}
		if (!ok)
		{
			cout << "Error: line[" << linenum << "]: Return expression does not match declared return type: ";
			semantics = false;
			this->prettyPrint(cout, 0);
			cout << endl;
		}
	}
}

Exp* StmtReturn::optimize()
{
	if (exp != NULL)
	{
		Exp* opt = exp->optimize();
		if (opt != NULL)
		{
			delete exp;
			exp = opt;
		}
	}
	return NULL;
}

void StmtReturn::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Si la expresion no es nula, la compilamos
	if (exp != NULL)
	{
		exp->compile(out, stack, functable);
		ExpType funcType = stack.getCurrFuncType();
		out << "\t";
		switch (funcType)
		{
			case ET_INT: out << "i"; break;
			case ET_BOOL: out << "i"; break;
			case ET_STRING: out << "a"; break;
			case ET_FLOAT: out << "f"; break;
			default: cout << "\nFatal: this function cannot return void\n"; abort(); //No deberia entrar aqui
		}
		out << "return\n";		
	}
	else
	{
		out << "\treturn\n";
	}
}

StmtReturn::~StmtReturn()
{
	if (exp != NULL)
	{
		delete exp;
	}
}

/** Implementacion de Clase BinExp **/
BinExp::BinExp(EOperator op, Exp *exp1, Exp *exp2)
{ 
	linenum = line;
	this->op = op;
	this->exp1 = exp1;
	this->exp2 = exp2;
}

void BinExp::prettyPrint(ostream& out, int depth)
{ 
	exp1->prettyPrint(out, depth);
	switch (op)
	{
		case OP_PLUS: out << " + "; break;
		case OP_MINUS: out << " - "; break;
		case OP_TIMES: out << " * "; break;
		case OP_DIVIDEBY: out << " / "; break;
		case OP_EQUALS: out << " == "; break;
		case OP_NOTEQUAL: out << " != "; break;
		case OP_OR: out << " || "; break;
		case OP_AND: out << " && "; break;
		case OP_LESS: out << " < "; break;
		case OP_LESSEQ: out << " <= "; break;
		case OP_GREATER: out << " > "; break;
		case OP_GREATEREQ: out << " >= "; break;
		default: out << " No deberia estar aqui! ";
	}
	exp2->prettyPrint(out, depth);
}

void BinExp::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{	
	//Validacion de variables:
	exp1->checkSemantics(stack, functable);
	exp2->checkSemantics(stack, functable);

	//Obtener tipos de las expresiones:
	ExpType tipoExp1 = exp1->getType(stack, functable);
	ExpType tipoExp2 = exp2->getType(stack, functable);

	//Bandera para marcar errores.
	bool ok = true;
	if (tipoExp1 != ET_VOID && tipoExp2 != ET_VOID && tipoExp1 != ET_UNKNOWN && tipoExp2 != ET_UNKNOWN)
	{
		switch (op)
		{
			case OP_PLUS: if (tipoExp1 == ET_BOOL || tipoExp2 == ET_BOOL) ok = false; break;
			case OP_TIMES :
			case OP_DIVIDEBY:
			case OP_MINUS: if (tipoExp1 == ET_BOOL || tipoExp1 == ET_STRING || tipoExp2 == ET_BOOL || tipoExp2 == ET_STRING) ok = false; break;
			case OP_EQUALS : 
			case OP_NOTEQUAL: ok = true; break;
			case OP_OR: 
			case OP_AND: if (tipoExp1 != ET_BOOL || tipoExp2 != ET_BOOL) ok = false; break;
			case OP_LESS: 
			case OP_LESSEQ: 
			case OP_GREATER: 
			case OP_GREATEREQ: if (tipoExp1 == ET_BOOL || tipoExp1 == ET_STRING || tipoExp2 == ET_BOOL || tipoExp2 == ET_STRING) ok = false; break;
			default: ok = false;
		}
	}
	else
	{
		ok = false;
		cout << "Error: line[" << linenum << "]: One of the members in binary expression evaluates to void" << endl;
		semantics = false;
	}

	if (!ok)
	{
		cout << "Error: line[" << linenum << "]: Invalid type used in: \""; this->prettyPrint(cout, 0); cout << "\""<< endl;
		semantics = false;
	}
	return;
	
}

Exp* BinExp::optimize()
{
	float f1, f2;
	bool isNumExp1 = exp1->isNum(f1);
	bool isNumExp2 = exp2->isNum(f2);

	//Si ambas son numeros, sustituimos todo el nodo por una constante
	if (isNumExp1 && isNumExp2 && this->evalType() == ET_ARIT)
	{		
		//Evaluamos el tipo pasando una pila y tabla falsos ya que en este punto 
		//sabemos que no se precisan para evaluar los tipos, al ser constantes.
		CPilaDeSimbolos dummyStack;
		CFunctionTable dummyTable;
		ExpType exp1Type = exp1->getType(dummyStack, dummyTable);
		ExpType exp2Type = exp2->getType(dummyStack, dummyTable);
		
		//Evaluar el operador
		float result;
		switch(op)
		{
			case OP_PLUS: result = f1 + f2; break;
			case OP_MINUS: result = f1 - f2; break; 
			case OP_TIMES: result = f1 * f2; break;
			case OP_DIVIDEBY: result = f1 / f2; break;
			default: break; //No deberia entrar aqui
		}

		if (exp1Type == ET_INT && exp2Type == ET_INT)
		{
			return new IntConst((int)result);
		}
		return new FloatConst(result);
	}
	else
	{
		//Si la constante numerica es la primer expresion, la remplazamos
		if (isNumExp1)
		{
			Exp* opt = exp1->optimize();
			if (opt != NULL)
			{
				delete exp1;
				exp1 = opt;
				return NULL; //El padre no podra optimizar mas
			}
		}
		//Si la constante numerica era la segunda expresion, la remplazamos
		if (isNumExp2)
		{
			Exp* opt = exp2->optimize();
			if (opt != NULL)
			{
				delete exp2;
				exp2 = opt;
				return NULL; //idem
			}
		}
	}
	return NULL;
}

bool BinExp::isNum(float& num)
{
	float e1, e2;
	if (exp1->isNum(e1) && exp2->isNum(e2))
	{
		switch (op)
		{
			case OP_PLUS: num = e1 + e2; break;
			case OP_MINUS: num = e1 - e2; break;
			case OP_TIMES: num = e1 * e2; break;
			case OP_DIVIDEBY: num = e1 / e2; break;
			default: break; //Error: No deberia entrar aqui
		}
		return true;
	}
	return false;
}

void BinExp::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	if (this->evalType() == ET_ARIT)
	{
		//Obtiene el tipo de las 2 expresiones
		ExpType expType1 = exp1->getType(stack, functable);
		ExpType expType2 = exp2->getType(stack, functable);
		
		//Verificar si es concatenacion
		if ((op == OP_PLUS) && (expType1 == ET_STRING) && (expType2 == ET_STRING))
		{
			exp1->compile(out, stack, functable);
			exp2->compile(out, stack, functable);
			out << "\tinvokevirtual java/lang/String/concat(Ljava/lang/String;)Ljava/lang/String;\n";
			return;
		}

		//Compila la 1er expresion
		exp1->compile(out, stack, functable);
		//Si es entera y la otra float, castea
		if (expType1 == ET_INT && expType2 == ET_FLOAT)
		{
			out << "\ti2f\n";
		}
		//Idem para la segunda
		exp2->compile(out, stack, functable);
		if (expType1 == ET_FLOAT && expType2 == ET_INT)
		{
			out << "\ti2f\n";
		}
		//determinar el tipo de las intrucciones
		string sType("\ti");
		if (expType1 == ET_FLOAT || expType2 == ET_FLOAT)
		{
			sType = "\tf";
		}
		
		switch(op)
		{
			case OP_PLUS: out << sType << "add\n"; break;
			case OP_MINUS: out << sType << "sub\n"; break;
			case OP_TIMES: out << sType << "mul\n"; break;
			case OP_DIVIDEBY: out << sType << "div\n"; break;
			default: cout << "Fatal: invalid arithmetic operator" << endl;
			abort();
		}
		return;
	}

	//Operacion booleana:
	if (op == OP_AND || op == OP_OR)
	{

		exp1->compile(out, stack, functable);
		exp2->compile(out, stack, functable);

		if (op == OP_AND)
		{
			out << "\tiand\n";
		}
		else
		{
			out << "\tior\n";
		}
		return;

	}

	//Operacion de comparacion de operandos

	//Determinar los tipos
	ExpType expType1 = exp1->getType(stack, functable);
	ExpType expType2 = exp2->getType(stack, functable);

	//Compilamos primero la segunda expresion, por como funciona fcmpl
	exp1->compile(out, stack, functable);
	if (expType1 == ET_INT)
	{
		out << "\ti2f\n";
	}

	exp2->compile(out, stack, functable);
	if (expType2 == ET_INT)
	{
		out << "\ti2f\n";
	}

	string cmpCond;
	switch (op)
	{
		case OP_EQUALS: cmpCond = "ifeq"; break;
		case OP_NOTEQUAL: cmpCond = "ifne"; break;
		case OP_LESS: cmpCond = "iflt"; break;
		case OP_LESSEQ: cmpCond = "ifle"; break;
		case OP_GREATER: cmpCond = "ifgt"; break;
		case OP_GREATEREQ: cmpCond = "ifge"; break;
		default: cout << "Error: Invalid operator in boolean expression " << endl; abort();
	}

	//Labels:
	int lblTrue = label++;
	int lblEnd = label++;

	out << "\tfcmpl\n"; 
	out << "\t" << cmpCond << " Label" << lblTrue << "\n";
	out << "\tldc 0\n"; //la condicion no se cumplio, ponemos false en la pila
	out << "\tgoto Label" << lblEnd << "\n";
	out << "Label" << lblTrue << ":\n";
	out << "\tldc 1\n"; //la condicion se cumplio, ponemos true en la pila
	out << "Label" << lblEnd << ":\n";
}

ExpType BinExp::evalType()
{
	switch (op)
	{
		case OP_PLUS: 
		case OP_MINUS: 
		case OP_TIMES: 
		case OP_DIVIDEBY: return ET_ARIT; //Operacion aritmetica
		case OP_EQUALS: 
		case OP_NOTEQUAL:
		case OP_OR: 
		case OP_AND:
		case OP_LESS:
		case OP_LESSEQ: 
		case OP_GREATER: 
		case OP_GREATEREQ: return ET_BOOL; //Operacion booleana
		default: cout << " Compiler Bug: Should never enter here! ";
	}
	cout << "Compiler Bug: line[" << linenum << "]: Control should never reach this point" << endl;
	semantics = false;
	return ET_UNKNOWN;
}

ExpType BinExp::getType(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	ExpType exp1Type = exp1->getType(stack, functable);
	ExpType exp2Type = exp2->getType(stack, functable);

	if (evalType() == ET_ARIT)
	{
		//Si ambos tipos son iguales, lo devolvemos
		if (exp1Type == exp2Type)
		{
			return exp1Type;
		}
	
		//Si llega aqui, no son iguales, "promocionar" si son float e int
		if ((exp1Type == ET_INT && exp2Type == ET_FLOAT) || (exp1Type == ET_FLOAT && exp2Type == ET_INT))
		{
			return ET_FLOAT;
		}
	}
	else
	{
		if (evalType() == ET_BOOL)
		{
			return ET_BOOL;
		}
	}

	//Centinela
	return ET_UNKNOWN;
}

BinExp::~BinExp()
{
	delete exp1;
	delete exp2;
}

/** Implementacion de Clase UnExp **/
UnExp::UnExp(EOperator op, Exp *exp)
{ 
	linenum = line;
	this->op = op;
	this->exp = exp;
}
void UnExp::prettyPrint(ostream& out, int depth) 
{ 
	if (op == OP_NOT)
	{
		out << "!"; exp->prettyPrint(out, depth);
	}
	else
	{
		if (op == OP_PAR)
		{
			out << "( "; exp->prettyPrint(out,depth); out << " )";
		}
		else
		{
			out << " Error: linea[" << linenum << "]: Operador Unario no reconocido en: "; exp->prettyPrint(out, depth);
			semantics = false;
		}
	}
}

void UnExp::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Validar Expresion
	exp->checkSemantics(stack, functable);
	//Validar operacion
	if (op == OP_NOT)
	{
		//Obtener tipo de la expresion
		ExpType expType = exp->getType(stack, functable);
		
		//Si el operador es NOT, la expresion debe ser booleana
		if (expType != ET_BOOL)
		{
			cout << "Error: line[" << linenum << "]: The given expression is not Boolean: \""; this->prettyPrint(cout, 0); cout << "\"" << endl;
			semantics = false;
		}
	}
	//En otro caso, damos como valido.
}

Exp* UnExp::optimize()
{
	if (op != OP_NOT)
	{
		Exp* opt = exp->optimize();
		if (opt != NULL)
		{
			delete exp;
			exp = opt;
		}
	}
	return NULL;
}

bool UnExp::isNum(float& num)
{
	if (op != OP_NOT && exp->isNum(num))
	{
		return true;
	}
	return false;
}

void UnExp::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	exp->compile(out, stack, functable);
	if (op == OP_NOT)
	{
		out << "\ticonst_1\n";
		out << "\tisub\n";
	}
}

ExpType UnExp::getType(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	return exp->getType(stack, functable);
}

UnExp::~UnExp()
{
	delete exp;
}

/** Implementacion de la Clase IDNode **/
IDNode::IDNode(char *name) : id(name)
{ 
	linenum = line;
}

void IDNode::prettyPrint(ostream& out, int depth) 
{ 
	out << id;
}

void IDNode::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	if (!(stack.EstaVarDefinida(id)))
	{
		cout << "Error: line[" << linenum << "]: Undefined identifier: " << id << endl;
		semantics = false;
	}
}

void IDNode::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{	
	int asmId = stack.GetAsmID(id);
	if (asmId >= 0) //la variable es local
	{
		ExpType type = stack.getTipo(id);
		out << "\t" << util::compileInst(type) << "load_" << asmId << "\n";
		return;
	}
	if (asmId == -1) //la variable es global
	{
		out << "\tgetstatic " << className << "/" << id << " " << util::compile(stack.getTipo(id)) << "\n";
		return;
	}

	//No deberia entrar aqui.
	cout << "Fatal: variable " << id << " undeclared" << endl;
	abort();
}

ExpType IDNode::getType(CPilaDeSimbolos& stack, CFunctionTable&)
{
	return stack.getTipo(id);
}

/** Implementacion de la Clase IntConst **/
IntConst::IntConst(int i) 
{ 
	linenum = line;
	this->i = i;
}
void IntConst::prettyPrint(ostream& out, int depth) 
{ 
	out << i;
}

void IntConst::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	return;
}

void IntConst::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	out << "\tldc " << i << "\n";
}

ExpType IntConst::getType(CPilaDeSimbolos& stack, CFunctionTable&)
{
	return ET_INT; //Constante Entera.
}

/** Implementacion de la Clase FloatConst**/
FloatConst::FloatConst(float f) 
{ 
	linenum = line;
	this->f = f;
}
void FloatConst::prettyPrint(ostream& out, int depth) 
{ 
	out << f;
}

void FloatConst::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	return;
}

void FloatConst::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//No perdamos los decimales al compilar!
	if ((f - (int)f) == 0)
	{
		out << "\tldc " << f << ".0\n";
	}
	else
	{
		out << "\tldc " << f << "\n";
	}
}

ExpType FloatConst::getType(CPilaDeSimbolos& stack, CFunctionTable&)
{
	return ET_FLOAT; //Constante de punto flotante.
}

/** Implementacion de la Clase StringConst**/
StringConst::StringConst(string s) 
{ 
	linenum = line;
	this->s = s;
}
void StringConst::prettyPrint(ostream& out, int depth) 
{ 
	out << s;
}

void StringConst::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	return;
}

void StringConst::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	out << "\tldc " << s << "\n";
}

ExpType StringConst::getType(CPilaDeSimbolos& stack, CFunctionTable&)
{
	return ET_STRING; //Constante literal.
}

/** Implementacion de la Clase BoolConst **/
BoolConst::BoolConst(bool b) 
{ 
	linenum = line;
	this->b = b;
}

void BoolConst::prettyPrint(ostream& out, int depth) 
{ 
	if (b)
	{
		out << "true";
	}
	else
	{
		out << "false";
	}
}

void BoolConst::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	return;
}

void BoolConst::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	if (b)
	{
		out << "\tldc 1\n";
	}
	else
	{
		out << "\tldc 0\n";
	}
}

ExpType BoolConst::getType(CPilaDeSimbolos& stack, CFunctionTable&)
{
	return ET_BOOL; //Constante booleana.
}

/** Implementacion de la Clase ArgsNode **/
ArgsNode::ArgsNode(list< pair<Tipo,IDNode*> >* args) 
{ 
	linenum = line;
	this->args = args;
}

list< pair<Tipo,IDNode*> >* ArgsNode::getList()
{
	return args;
}
void ArgsNode::prettyPrint(ostream& out, int depth) 
{ 
	int i = 0;
	for (list< pair<Tipo, IDNode*> >::iterator it = args->begin(); it != args->end(); it++)
	{
		pair<Tipo, IDNode*> p = (*it);
		util::prettyPrint(out, p.first,0);
		p.second->prettyPrint(out, depth);
		if (i < args->size() -1)
			out << ", ";
		++i;
	}
}

void ArgsNode::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	for (list< pair<Tipo, IDNode*> >::iterator it = args->begin(); it != args->end(); it++)
	{
		pair<Tipo, IDNode*> p = (*it);
		stack.NuevaDefinicion(p.first, (p.second)->toString());
		functable.incLocals(stack.getCurrFuncID()); //Incrementar numero de variables locales de la funcion
	}
}

void ArgsNode::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	for (list< pair<Tipo, IDNode*> >::iterator it = args->begin(); it != args->end(); it++)
	{
		pair<Tipo, IDNode*> p = (*it);
		stack.NuevaDefinicion(p.first, (p.second)->toString()); //NuevaDefinicion le asigna identificadores en asm		
	}
}

list<Tipo> ArgsNode::getTypeList()
{	
	list<Tipo> ret;
	if (args != NULL)
	{
		for (list< pair<Tipo, IDNode*> >::iterator it = args->begin(); it != args->end(); it++)
		{
			ret.push_back((*it).first);
		}
	}
	return ret;
}

ArgsNode::~ArgsNode()
{
	//esta lista puede ser nula
	if (args != NULL)
	{
		while (!(args->empty()))
		{
			pair<Tipo, IDNode*> p = args->front();
			delete p.second;
			args->pop_front();
		}
		delete args;
	}
}

/** Implementacion de la Clase BodyNode	**/
BodyNode::BodyNode(list<Statement*>* l) 
{ 
	linenum = line;
	this->stmts = l;
}
void BodyNode::prettyPrint(ostream& out, int depth) 
{ 
	int localDepth = depth; //(depth - tabSize < 0)? 0 : depth - tabSize;
	util::indent(out, localDepth);
	out << "{\n";
	//Imprimir la lista de Statements si exsite.
	if (stmts != NULL)
	{
		for (list<Statement*>::iterator it = stmts->begin(); it != stmts->end(); it++)
		{
			(*it)->prettyPrint(out, depth+tabSize);
		}
	}
	out << "\n";
	util::indent(out, localDepth);
	out << "}\n";
}

void BodyNode::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Crear nuevo Alcance
	stack.NuevoAlcance();
	if (stmts != NULL)
	{
		for (list<Statement*>::iterator it = stmts->begin(); it != stmts->end(); it++)
		{
			(*it)->checkSemantics(stack, functable);
		}
	}
	//Fin de Alcance
	stack.FinAlcance();
}

Exp* BodyNode::optimize()
{
	if (stmts != NULL)
	{
		for (list<Statement*>::iterator it = stmts->begin(); it != stmts->end(); it++)
		{
			(*it)->optimize();
		}
	}
	return NULL;
}

void BodyNode::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Crear alcance
	stack.NuevoAlcance();

	if (stmts != NULL)
	{
		//cout << "-------------ini body--------\n";
		for (list<Statement*>::iterator it = stmts->begin(); it != stmts->end(); it++)
		{
			(*it)->compile(out, stack, functable);
			//debug
			
		//	(*it)->prettyPrint(cout, 0);
		}
		//cout << "---------------fin body---------\n";
	}

	//Cerrar alcance
	stack.FinAlcance();
}

BodyNode::~BodyNode()
{
	//el body puede ser nulo
	if (stmts != NULL)
	{
		while (!(stmts->empty()))
		{
			delete stmts->front();
			stmts->pop_front();
		}
		delete stmts;
	}
}

/** Implementacion de la Clase FuncDeclNode **/
FuncDeclNode::FuncDeclNode(Tipo t, IDNode *id, ArgsNode* args, BodyNode* body) 
{ 
	linenum = line;
	this->retType = t;
	this->id = id;
	this->args = args;
	this->body = body;
}
void FuncDeclNode::prettyPrint(ostream& out, int depth) 
{ 
	util::prettyPrint(out, retType, depth);
	id->prettyPrint(out,depth);
	out << " ( "; 
	//Imprimir los argumentos, si existen
	if (args != NULL)
	{
		args->prettyPrint(out, depth); 
	}
	out << " )\n";
	body->prettyPrint(out, depth);
	out << "\n";
}

void FuncDeclNode::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Recordar declaracion de funcion
	stack.NuevaFuncion(retType, id->toString());
	
	//Obtener lista de tipos
	list<Tipo> tmp;
	if (args != NULL)
	{
		tmp = args->getTypeList();
	}

	//Agregar nueva funcion
	if (!(functable.addFunction(retType, id->toString(), tmp)))
	{
		cout << "Error: line[" << linenum << "]: Function redefinition: " << id->toString() << " in: ";
		semantics = false;
		this->prettyPrint(cout,0);
		cout << endl;
	}

	//Chequear firma de main
	if (id->toString() == "main" && args != NULL)
	{
		cout << "Error: line[" << linenum << "]: Invalid signature for function \"main\": ";	
		//Imprimir firma
		util::prettyPrint(cout, retType, 0);
		cout << "main (";
		args->prettyPrint(cout, 0);
		cout << ")\nAvailable candidates are: void main()" << endl;
		//Semantica invalida
		semantics = false;		
	}
	
	//Chequear argumentos
	if (args != NULL)
	{
		args->checkSemantics(stack, functable);
	}
	


	//Chequear cuerpo
	body->checkSemantics(stack, functable);

	//Fin de la funcion actual
	stack.FinFuncion();
}

Exp* FuncDeclNode::optimize()
{
	body->optimize();
	return NULL;
}

void FuncDeclNode::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	stack.NuevaFuncion(retType, id->toString());

	//Imprimir: .method public static F ( ARGS )TIPO
	out << "\n.method public static " << id->toString() << "(";
	if ((args != NULL) && (id->toString() != "main"))
	{
		args->compile(out, stack, functable);
		//Imprimir los tipos para la llamada
		for (list< pair<Tipo, IDNode*> >::iterator it = args->getList()->begin(); it != args->getList()->end(); it++)
		{
			out << util::compile(util::Tipo2ExpType( (*it).first ) );
		}
	}
	else
	{
		if (id->toString() == "main")
		{
			out << "[Ljava/lang/String;";
			functable.incLocals(id->toString());
		}
	}
	out << ")" << util::compile(util::Tipo2ExpType(retType)) << "\n";

	//Imprimir limite de stack y variables
	out << "\t.limit stack " << maxStack << "\n";
	out << "\t.limit locals " << functable.getLocals(id->toString()) << "\n"; 

	//Compilar el cuerpo de la funcion
	body->compile(out, stack, functable);

	//Si el tipo es void, agregamos un return
	if (retType == TVOID)
	{	
		out << "\treturn\n";
	}
	else
	{
		//Mini validacion que tenga almenos un return la funcion
		out << "\t";
		util::compileInst(util::Tipo2ExpType(retType));
		out << "return\n";
	}

	//Fin metodo
	out << ".end method\n";

	stack.FinFuncion();
}

FuncDeclNode::~FuncDeclNode()
{
	delete id;
	//args puede ser null
	if (args != NULL)
	{
		delete args;
	}
	//body puede ser null
	if (body != NULL)
	{
		delete body;
	}
}

/** Implementacion de la Clase FCallNode **/
FCallNode::FCallNode(IDNode* id, list<Exp*>* params, bool isStmt) 
{ 
	Exp::linenum = line;
	this->id = id;
	this->isStmt = isStmt;
	this->args = params;
}

void FCallNode::prettyPrint(ostream& out, int depth) 
{ 
	util::indent(out, depth);
	id->prettyPrint(out, depth);
	//Imprimir los parametros, si existen
	out << "( ";
	if (args != NULL)
	{
		int i = 0;
		for (list<Exp*>::iterator it = args->begin(); it != args->end(); it++)
		{
			(*it)->prettyPrint(out, 0);
			if (i < args->size() -1)
				out << ", ";
			++i;
		}
	}
	out << " )";
	if (isStmt)
	{
		out << ";\n";
	}
}

void FCallNode::setStmt(bool b)
{
	isStmt = b;
}

void FCallNode::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Chequear que la funcion este definida	
	if ( !(functable.isDeclared(id->toString())) )
	{
		cout << "Error: line[" << Exp::linenum << "]: Undeclared function: " << id->toString() << " in: ";
		this->prettyPrint(cout, 0);
		cout << endl;
		semantics = false;
	}
	//Chequear argumentos
	if (args != NULL)
	{
		//Comparar la cantidad de argumentos a pasar con los que se espera recibir
		if (functable.getArgCount(id->toString()) != args->size())
		{
			cout << "Error: line[" << Exp::linenum << "]: Number of arguments does not match function signature in: ";
			this->prettyPrint(cout, 0);
			cout << "\n";
			semantics = false;
		}

		//Comparar los tipos pasados en la llamada
		int i = 0;
		for (list<Exp*>::iterator it = args->begin(); it != args->end(); it++)
		{
			//Revisar si el parametro es correcto (un id debe estar defindio, una expresion bien formada, etc)
			(*it)->checkSemantics(stack, functable);

			//Comparar los tipos de los parametros uno a uno con los declarados en la firma de la funcion
			if (!(functable.checkArgNum(id->toString(), (*it)->getType(stack, functable), i++)))
			{
				cout << "Error: line[" << Exp::linenum << "]: Parameter\'s " << i << " type does not match the signature of function: " << id->toString() << " in: ";
				semantics = false;
				this->prettyPrint(cout, 0);
				cout << endl;
			}
		}
	}
	else
	{
		//No pasamos argumentos
		if (functable.getArgCount(id->toString()) != 0)
		{
			cout << "Error: line[" << Exp::linenum << "]: Number of arguments does not match the signature of function in: ";
			this->prettyPrint(cout, 0);
			semantics = false;
		}
	}
}

Exp* FCallNode::optimize()
{
	if (args != NULL)
	{
		for (list<Exp *>::iterator it = args->begin(); it != args->end(); it++)
		{
			Exp* opt = (*it)->optimize();
			if (opt != NULL)
			{
				delete (*it);
				(*it) = opt;
			}
		}
	}
	return NULL;
}

void FCallNode::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	if (args != NULL)
	{
		int i = 0;
		//Compilar los parametros, implica poner en la pila cada expresion
		for (list<Exp *>::iterator it = args->begin(); it != args->end(); it++)
		{
			(*it)->compile(out, stack, functable);
			if (((*it)->getType(stack,functable) == ET_INT) && (functable.getArgNumType(id->toString(), i) == ET_FLOAT))
			{
				//Si estamos pasando un int y la funcion recibe float, casteamos
				out << "\ti2f\n";
			}
			++i;
		}
	}
	
	out << "\tinvokestatic " << className << "/" << id->toString() << "(";

	//Imprimir los tipos para la llamada
	if (args != NULL)
	{
		//for (list<Exp *>::iterator it = args->begin(); it != args->end(); it++)
		for (int i = 0; i < args->size(); i++)
		{
			//out << util::compile((*it)->getType(stack, functable));
			out << util::compile(functable.getArgNumType(id->toString(), i));
		}
	}

	out << ")" << util::compile(functable.getReturnType(id->toString())) << "\n";
	
	//Si la llamada se usa como statement (no se asigna) y devuelve algo, lo quitamos de la pila
	ExpType ret = functable.getReturnType(id->toString());
	if (isStmt && ret != ET_VOID && ret != ET_UNKNOWN)
	{
		out << "\tpop\n";
	}

}

ExpType FCallNode::getType(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Si este metodo se invoca, la llamada se esta usando como expresion.
	assert(!isStmt);
	//return stack.getTipo(id->toString());
	return functable.getReturnType(id->toString());
}

FCallNode::~FCallNode()
{
	delete id;
	//los parametros pueden ser null
	if (args != NULL)
	{
		while (!(args->empty()))
		{
			delete args->front();
			args->pop_front();
		}
		delete args;
	}
}

/** Implementacion de la Clase DeclNode **/
DeclNode::DeclNode(Tipo t, IDNode* id, Exp* exp) 
{ 
	linenum = line;
	this->t = t;
	this->id = id;
	this->exp = exp;
}

void DeclNode::prettyPrint(ostream& out, int depth) 
{ 	
	util::indent(out, depth);
	util::prettyPrint(out, t, 0);
	id->prettyPrint(out, depth);

	//Imprimir asignacion, si existe.
	if (exp != NULL)
	{
		out << " = ";
		exp->prettyPrint(out, 0);
	}
	out << ";\n";
}

void DeclNode::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	if (stack.EstaVarEnAlcance(id->toString()))
	{
		cout << "Error: line[" << linenum << "]: Identifier redeclared: " << id->toString() << endl;
		semantics = false;
	}
	else
	{
		//Agregar definicion nueva
		stack.NuevaDefinicion(t,id->toString());

		//Obtener el identificador de la funcion actual
		string currFunc = stack.getCurrFuncID();

		//Si la variable no es global, aumentamos el numero de locales
		if (currFunc != "__GLOBAL__")
		{
			functable.incLocals(currFunc);
		}
		else
		{
			//Si la variable es global, no puede tener inicializacion
			if (exp != NULL)
			{
				cout << "Error: line[" << linenum << "]: Global variable initialization is illegal: " << id->toString() << endl;
				semantics = false;
			}
		}
		
		//Si exp no es NULL, es una declaracion con asignacion.
		if (exp != NULL)
		{
			//Validar asignacion
			exp->checkSemantics(stack, functable);

			//Obtener el tipo declarado para id de la pila, sabemos que el id es de tipo ET_VAR, 
			//por eso llamamos al stack directamente
			ExpType idType = stack.getTipo(id->toString());

			//Obtener el tipo de la expresion
			ExpType expType = exp->getType(stack, functable);			

			bool ok = false;
			//Revisar el match
			switch (idType)
			{
				case ET_BOOL: if (expType == ET_BOOL) ok = true; break;
				case ET_INT: if (expType == ET_INT) ok = true; break;
				case ET_FLOAT: if (expType  == ET_FLOAT || expType == ET_INT) ok = true; break;
				case ET_STRING: if (expType == ET_STRING) ok = true; break;
				default: ok = false; //Incluye el caso de ET_VOID, que no es un tipo valido para lvalue
			}
			if (!ok)
			{
				cout << "Error: line[" << linenum << "]: Type mismatch in assignment: "; this->prettyPrint(cout, 0); cout << endl;
				semantics = false;
			}
		}
		
	}
}

Exp* DeclNode::optimize()
{
	if (exp != NULL)
	{
		Exp* opt = exp->optimize();
		if (opt != NULL)
		{
			delete exp;
			exp = opt;
		}
	}
	return NULL;
}

void DeclNode::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Nueva definicion
	stack.NuevaDefinicion(t, id->toString());

	if (stack.getCurrFuncID() != "__GLOBAL__")
	{
		//La variable es local
		if (exp != NULL)
		{
			//Compilar expresion
			exp->compile(out, stack, functable);
			//Guardar tope de la pila en id
			string underscore("_");
			if (t == TSTRING)
			{
				underscore = string(" ");
			}
			out << "\t" << util::compileInst(util::Tipo2ExpType(t)) << "store" << underscore << stack.GetAsmID(id->toString()) << "\n";
		}
	}
}

DeclNode::~DeclNode()
{
	delete id;
	//la expresion puede ser null
	if (exp != NULL)
	{
		delete exp;
	}
}

/** Implementacion de la Clase AssignNode **/
AssignNode::AssignNode(IDNode* id, Exp* exp)
{ 
	linenum = line;
	this->id = id;
	this->exp = exp;
}
void AssignNode::prettyPrint(ostream& out, int depth) 
{ 
	util::indent(out, depth);
	id->prettyPrint(out, depth);
	out << " = ";
	exp->prettyPrint(out, 0);
	out << " ;\n";
}

void AssignNode::checkSemantics(CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Validar variable declarada
	if (stack.EstaVarDefinida(id->toString()))
	{
		//Validar expresion
		exp->checkSemantics(stack, functable);
		
		//Obtener el tipo declarado para id de la pila, sabemos que el id es de tipo ET_VAR, 
		//por eso llamamos al stack directamente
		ExpType idType = stack.getTipo(id->toString());

		//Obtener el tipo de la expresion
		ExpType expType = exp->getType(stack, functable);

		bool ok = false;
		//Revisar el match
		switch (idType)
		{
			case ET_BOOL: if (expType == ET_BOOL) ok = true; break;
			case ET_INT: if (expType == ET_INT) ok = true; break;
			case ET_FLOAT: if (expType  == ET_FLOAT || expType == ET_INT) ok = true; break;
			case ET_STRING: if (expType == ET_STRING) ok = true; break;
			default: ok = false; //Incluye el caso de ET_VOID, que no es un tipo valido para lvalue
		}
		if (!ok)
		{
			cout << "Error: line[" << linenum << "]: Type mismatch in assignment: "; this->prettyPrint(cout, 0); cout << endl;
			semantics = false;
		}
	}
	else
	{
		cout << "Error: linea[" << linenum << "]: Undeclared identifier: " << id->toString() << endl;
		semantics = false;
	}
}

Exp* AssignNode::optimize()
{
	Exp* opt = exp->optimize();
	if (opt != NULL)
	{
		delete exp;
		exp = opt;
	}
	return NULL;
}

void AssignNode::compile(ostream& out, CPilaDeSimbolos& stack, CFunctionTable& functable)
{
	//Compilamos la expresion para que quede en en tope de la pila
	exp->compile(out, stack, functable);

	ExpType t = stack.getTipo(id->toString());

	if (stack.GetAsmID(id->toString()) != -1)
	{
		//Guardar tope de la pila en id
		string underscore("_");
		if (t == ET_STRING)
		{
			underscore = string(" ");
		}
		out << "\t" << util::compileInst(t) << "store" << underscore << stack.GetAsmID(id->toString()) << "\n";
	}
	else
	{
		out << "\tputstatic " << className << "/" << id->toString() << " " << util::compile(t) << "\n";	
	}
}

AssignNode::~AssignNode()
{
	delete id;
	delete exp;
}

