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

%{

#include "ast.h"
RootNode* raiz = new RootNode();
bool semantics = true;
const char *filename;

%}

%code requires{

/*Version 1.3, cambios varios*/
#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
using std::ofstream;
using std::ifstream;

#include "ast.h"
extern RootNode* raiz;
extern const char *filename;
extern bool semantics;

extern FILE* yyin;
extern int line;

int yyerror(char*);
int yylex(void);
int yyparse(void);


}

%union{
	Statement* statement;
	StmtIf* stmtIf;
	StmtWhile* stmtWhile;
	StmtReturn* stmtReturn;
	Exp* expr;
	BinExp* binexpr;
	UnExp* unexpr;
	IDNode* id;
	IntConst* intConst;
	FloatConst* floatConst;
	StringConst* strConst;
	BoolConst* boolConst;
	ArgsNode* funcDeclArgs;
	FuncDeclNode *funcDecl;
	BodyNode* bodyNode;
	FCallNode* fcallNode;
	DeclNode* varDecl;
	list<Exp*>* lparams;
	list<Statement*>* lstmt;
	Tipo tipo;
	int ival;
	float fval;
	char* cpval;
	bool bval;
}

%type <statement> stmt
%type <expr> exp
%type <tipo> type
%type <funcDeclArgs> args
%type <funcDecl> function
%type <bodyNode> body
%type <fcallNode> fcall
%type <varDecl> declaration
%type <lparams> params
%type <lstmt> stmtlst
%type <ival> NUM
%type <fval> REAL
%type <cpval> ID 
%type <cpval> STRING
%type <bval> TRUE FALSE

%start prog
%left 	LPAR RPAR
%token  BOOL NUM REAL INT FLOAT STRING VOID
%token TRUE FALSE
%left	NOT
%left	TIMES DIVIDEBY
%left	PLUS MINUS
%right	ASSIGN
%left	OR AND
%left	EQUALS NOTEQUAL LESS LESSEQ GREATER GREATEREQ
%token	IF ELSE WHILE RETURN
%left	COLON
%left	LCURL RCURL
%left	SEMICOLON
%token	ID

%%

prog	:	function				{ raiz->append($1); }
	|	function prog				{ raiz->append($1); }
	|	declaration				{ raiz->append($1); }
	|	declaration prog				{ raiz->append($1); }

function :	type ID LPAR args RPAR body		{ $$ = new FuncDeclNode($1,new IDNode($2),$4,$6); delete[] $2;}
	 |	type ID LPAR RPAR body			{ $$ = new FuncDeclNode($1,new IDNode($2),NULL,$5); delete[] $2; }

declaration :	type ID SEMICOLON			{ $$ = new DeclNode($1,new IDNode($2),NULL); delete[] $2; }
	    |	type ID ASSIGN exp SEMICOLON		{ $$ = new DeclNode($1,new IDNode($2),$4); delete[] $2; }

args	:	type ID					{  list< pair<Tipo,IDNode*> >* l = new list< pair<Tipo,IDNode*> >();
							   pair<Tipo,IDNode*> p; p.first = $1; p.second = new IDNode($2);
							   l->push_front(p);
							   $$ = new ArgsNode(l); delete[] $2;
							}
	|	type ID COLON args			{  pair<Tipo, IDNode*> p; p.first = $1; p.second = new IDNode($2);
							   $4->getList()->push_front(p);
							   $$ = $4; delete[] $2;
							}

params	:	exp					{ list<Exp*>* l = new list<Exp*>(); l->push_front($1); $$ = l;}
	|	exp COLON params			{ $3->push_front($1); $$ = $3; }


exp	:	NUM					{ $$ = new IntConst($1); }
	|	REAL					{ $$ = new FloatConst($1); }
	|	LPAR exp RPAR				{ $$ = new UnExp(OP_PAR, $2); }
	| 	exp PLUS exp				{ $$ = new BinExp(OP_PLUS, $1, $3); }
	|	exp MINUS exp				{ $$ = new BinExp(OP_MINUS, $1, $3); }
	|	exp TIMES exp				{ $$ = new BinExp(OP_TIMES, $1, $3); }
	|	exp DIVIDEBY exp			{ $$ = new BinExp(OP_DIVIDEBY, $1, $3); }
	|	TRUE					{ $$ = new BoolConst($1); }
	|	FALSE					{ $$ = new BoolConst($1); }
	|	NOT exp				{ $$ = new UnExp(OP_NOT, $2); }
	|	exp EQUALS exp			{ $$ = new BinExp(OP_EQUALS, $1, $3); }
	|	exp NOTEQUAL exp			{ $$ = new BinExp(OP_NOTEQUAL, $1, $3); }
	|	exp OR exp				{ $$ = new BinExp(OP_OR, $1, $3); }
	|	exp AND exp				{ $$ = new BinExp(OP_AND, $1, $3); }
	|	exp LESS exp				{ $$ = new BinExp(OP_LESS, $1, $3); }
	|	exp LESSEQ exp			{ $$ = new BinExp(OP_LESSEQ, $1, $3); }
	|	exp GREATER exp			{ $$ = new BinExp(OP_GREATER, $1, $3); }
	|	exp GREATEREQ exp			{ $$ = new BinExp(OP_GREATEREQ, $1, $3); }
	|	ID					{ $$ = new IDNode($1); delete[] $1;}
	|	STRING					{ $$ = new StringConst($1); delete[] $1;}
	|	fcall					{ FCallNode* fcalln = $1; fcalln->setStmt(false); $$ = $1; }


stmtlst :	/* vacia */				{ $$ = new list< Statement* >(); }
	|	stmt stmtlst 				{ $2->push_front($1); $$ = $2; }

stmt	:	declaration				{ $$ = $1; }
	|	IF LPAR exp RPAR stmt			{ $$ = new StmtIf($3,$5, NULL); }
	|	IF LPAR exp RPAR stmt ELSE stmt	{ $$ = new StmtIf($3,$5,$7); }
	|	WHILE LPAR exp RPAR stmt		{ $$ = new StmtWhile($3,$5); }
	|	body					{ $$ = $1; }
	|	fcall SEMICOLON			{ $$ = $1; }
	|	ID ASSIGN exp SEMICOLON		{ $$ = new AssignNode(new IDNode($1),$3); delete[] $1;}
	|	RETURN exp SEMICOLON		{ $$ = new StmtReturn($2); }
	|	RETURN SEMICOLON			{ $$ = new StmtReturn(NULL); }

body	:	LCURL stmtlst RCURL			{ $$ = new BodyNode($2); }
	|	LCURL RCURL				{ $$ = new BodyNode(NULL); }

fcall	:	ID LPAR params RPAR			{ $$ = new FCallNode(new IDNode($1),$3); delete[] $1;}
	|	ID LPAR RPAR				{ $$ = new FCallNode(new IDNode($1),NULL); delete[] $1;}

type	:	BOOL					{ $$ = TBOOL; }
	|	INT					{ $$ = TINT; }
	|	FLOAT					{ $$ = TFLOAT; }
	|	STRING					{ $$ = TSTRING; }
	|	VOID					{ $$ = TVOID; }

%%
void peephole(string, string);
bool isSubStr(string str, string subStr);
string subStrFromCh(string str, char ch);

int yyerror(char *s)
{
	printf("Grammar error: line[%d]\n", line);
	return 0;
}

int main(int argc, char* argv[])
{
	
	//Tomar el primer argumento como entrada, sino stdin.
	if (argc > 1)
	{
		cout << "Batch mode. taking as input: " << argv[1] << endl;
		yyin = fopen(argv[1],"r+");
		filename = argv[1];
	}
	else
	{
		cout << "Interactive mode. Enter EOF or a grammar error to quit.\n"
		     << "----------------------------------------------" << endl;
		yyin = stdin;
		filename = "stdin";
	}
	/*
	int result = 0;
	//Parsear entrada
	for (int i = 1; i < argc; i++)
	{
		yyin = fopen(argv[i],"r+");
		int tmp = yyparse();
		if (result == 0)
		{
			result = tmp;
		}
		fclose(yyin);
	}*/
	int result = yyparse();

	//Imprimir mensaje de salida
	if (result == 0)
	{
		cout << "Parsing successful" << endl;
	}
	else
	{
		cout << "Parse error, code: " << result << "\nAbort..." << endl;
		abort();
	}

	//Pretty Print
	cout << "Pretty Printing result. Saving as \"out.txt\"" << endl;
	ofstream out("out.txt",std::ios::out);
	raiz->prettyPrint(out,0);
	out.close();
	
	//Chequear Semantica
	cout << "Begin semantic validation..." << endl;
	CPilaDeSimbolos stack;
	CFunctionTable functable;
	
	//Optimizar
	cout << "Optimizing..." << endl;
	raiz->optimize();

	//Pretty Print Optimizado
	cout << "Pretty Printing optimized code. Saving as \"opt.txt\"" << endl;
	ofstream outopt("opt.txt",std::ios::out);
	raiz->prettyPrint(outopt,0);
	out.close();

	//Funciones predefinidas del lenguaje
	list<Tipo> args;
	args.push_back(TSTRING);
	functable.addFunction(TVOID, "print", args);

	args.clear();
	args.push_back(TFLOAT);
	functable.addFunction(TINT, "trunc", args);

	args.clear();
	args.push_back(TFLOAT);
	functable.addFunction(TSTRING, "toString", args);

	args.clear();
	args.push_back(TSTRING);
	functable.addFunction(TINT, "toInt", args);

	args.clear();
	functable.addFunction(TSTRING, "read", args);

	raiz->checkSemantics(stack, functable);
	if (semantics)
	{
		cout << "Compiling into file \"comp.j\"" << endl;
		//Crear archivo de salida
		ofstream comp("comp.j", std::ios::out);

		//Crear cabezal de clase estatica generada
		comp << ".source " << filename << "\n";
		comp << ".class " << className << "\n";
		comp << ".super java/lang/Object\n\n";

		//Compilar las variables globales
		for (list< CNodoPila >::iterator it = stack.getLista().begin(); it != stack.getLista().end(); it++)
		{
			comp << ".field public static "<< it->GetID() << " " <<  util::compile(util::Tipo2ExpType(it->GetTipoDato())) << "\n";
		}

		//Copiar biblioteca Estandar
		ifstream in("lpstdlib.j");
		if (!in)
		{
			cout << "Error: standar library (lpstdlib.j) missing.\nAborting..." << endl;
			abort();
		}

		while (!in.eof())
		{
			string s;
			getline(in, s);
			comp << s << "\n";
		}
	
		comp << ";\n; Codigo de usuario\n;\n";

		//Compilar si checksemantics todo ok.
		raiz->compile(comp, stack, functable);

		//Cerrar archivos
		out.close();
		outopt.close();
		comp.close();

		//Optimizacion a nivel de codigo objeto
		cout << "Optimizing object code. Saving as \"compopt.j\"" << endl;
		peephole("comp.j", "compopt.j");

	}
	else
	{
		cout << "Semantic errors detected in source code. Aborting..." << endl;
		result = -1;
	}

	//Liberar memoria
	cout << "Freeing used memory..." << endl;
	delete raiz;

	//out.close();
	//outopt.close();

	return result;
}

void peephole(string infile, string outfile)
{
	ifstream in(infile.c_str());
	ofstream out(outfile.c_str(), std::ios::out);
	
	string oldLine;
	string newLine;

	if (!in.eof())
		getline(in, oldLine);

	if (!in.eof())
		getline(in,newLine);

	while (!in.eof())
	{
		if ( (oldLine.size() > 0) && (newLine.size() > 0) && isSubStr(oldLine, "store") && isSubStr(newLine, "load") )
		{
			//Compara el segundo caracter (el 1ero es el tipo...), de las dos lineas para ver si coinciden.
			if (oldLine[1] == newLine[1])
			{
				string idAssemStore = subStrFromCh(oldLine, ' ');
				string idAssemLoad = subStrFromCh(newLine, '_');

				//Verifica que el store y el load sean sobre la misma variable
				if (idAssemStore == idAssemLoad)
				{
					out << "\tdup\n";
					newLine = "\t;Store optimizado por PeepHole";
				}
			}
		}

		out << oldLine << "\n";

		oldLine = newLine;
		getline(in, newLine);
	}
	out << oldLine << endl;
	out.close();
}

bool isSubStr(string str, string subStr)
{
	if (str.size() < subStr.size())
		return false;

	bool isSub = false;
	for (int i = 0; i < str.size(); i++)
	{
		int j = 0;
		int h = i;
		isSub = true;
		while (h < str.size() && j < subStr.size())
		{
			if (str[h] != subStr[j])
				isSub = false;
			++j;
			++h;
		}
		if (isSub && j == subStr.size())
			return true;
	}
	
	return false;
}

string subStrFromCh(string str, char ch)
{
	bool existCh = false;
	string result("");

	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == ch)
		{
			existCh = true;
			continue;
		}

		if (existCh)
		{
			result.append(&str[i]);
		}
	}

	return result;
}
