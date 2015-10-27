/*
 *  cool.y
 *              Parser definition for the COOL language.
 *
 */
%{
#include <iostream>
#include "cool-tree.h"
#include "stringtab.h"
#include "utilities.h"

extern char *curr_filename;

void yyerror(char *s);        /*  defined below; called for each parse error */
extern int yylex();           /*  the entry point to the lexer  */

/************************************************************************/
/*                DONT CHANGE ANYTHING IN THIS SECTION                  */

Program ast_root;	      /* the result of the parse  */
Classes parse_results;        /* for use in semantic analysis */
int omerrs = 0;               /* number of errors in lexing and parsing */
%}

/* A union of all the types that can be the result of parsing actions. */
%union {
  Boolean boolean;
  Symbol symbol;
  Program program;
  Class_ class_;
  Classes classes;
  Feature feature;
  Features features;
  Formal formal;
  Formals formals;
  Case case_;
  Cases cases;
  Expression expression;
  Expressions expressions;
  char *error_msg;
}

/* 
   Declare the terminals; a few have types for associated lexemes.
   The token ERROR is never used in the parser; thus, it is a parse
   error when the lexer returns it.

   The integer following token declaration is the numeric constant used
   to represent that token internally.  Typically, Bison generates these
   on its own, but we give explicit numbers to prevent version parity
   problems (bison 1.25 and earlier start at 258, later versions -- at
   257)
*/
%token CLASS 258 ELSE 259 FI 260 IF 261 IN 262 
%token INHERITS 263 LET 264 LOOP 265 POOL 266 THEN 267 WHILE 268
%token CASE 269 ESAC 270 OF 271 DARROW 272 NEW 273 ISVOID 274
%token <symbol>  STR_CONST 275 INT_CONST 276 
%token <boolean> BOOL_CONST 277
%token <symbol>  TYPEID 278 OBJECTID 279 
%token ASSIGN 280 NOT 281 LE 282 ERROR 283

/*  DON'T CHANGE ANYTHING ABOVE THIS LINE, OR YOUR PARSER WONT WORK       */
/**************************************************************************/
 
   /* Complete the nonterminal list below, giving a type for the semantic
      value of each non terminal. (See section 3.6 in the bison 
      documentation for details). */

/* Declare types for the grammar's non-terminals. */
%type <program> program
%type <classes> class_list
%type <class_> class

%type <feature> feature
%type <features> feature_list

%type <formal> formal
%type <formals> formals_list

%type <expression> expr opt_assign let_expr
%type <expressions> exprs_semi exprs_comma 

%type <case_> case
%type <cases> cases

/* Precedence declarations go here. Higher line number means higher preference. */
%left LET
%right ASSIGN
%left NOT
%nonassoc '<' '=' LE
%left '+' '-' 
%left '*' '/' 
%left ISVOID
%left '~'
%left '@'
%left '.'
%left '(' ')'


%%
/* 
   Save the root of the abstract syntax tree in a global variable.
*/
program	: class_list	{ ast_root = program($1); parse_results = $1; }
        ;

class_list
	: class			/* single class */
		{ $$ = single_Classes($1);  }
	| error ';'
		{ $$ = nil_Classes();  }
	| class_list class	/* several classes */
		{ $$ = append_Classes($1,single_Classes($2)); }
	| class_list error ';'
		{ $$ = append_Classes($1,nil_Classes()); }
	;

/* If no parent is specified, the class inherits from the Object class. */
class	: CLASS TYPEID '{' feature_list '}' ';'
		{ $$ = class_($2,idtable.add_string("Object"),$4,stringtable.add_string(curr_filename)); }
	| CLASS TYPEID '{' '}' ';'
		{ $$ = class_($2,idtable.add_string("Object"),nil_Features(),stringtable.add_string(curr_filename)); } 
	| CLASS TYPEID INHERITS TYPEID '{' feature_list '}' ';'
		{ $$ = class_($2,$4,$6,stringtable.add_string(curr_filename)); }
	| CLASS TYPEID INHERITS TYPEID '{' '}' ';'
		{ $$ = class_($2,$4,nil_Features(),stringtable.add_string(curr_filename)); }
	;

feature_list	: feature ';'
			{ $$ = single_Features($1); }
		| error ';'
			{ $$ = nil_Features(); }
		| feature_list feature ';'
			{ $$ = append_Features($1, single_Features($2)); }
		| feature_list error ';'
			{ $$ = append_Features($1, nil_Features()); }
		;

feature	: OBJECTID '(' formals_list ')' ':' TYPEID '{' expr '}' 
		{ $$ = method($1, $3, $6, $8); }
	| OBJECTID '(' ')' ':' TYPEID '{' expr '}' 
		{ $$ = method($1, nil_Formals(), $5, $7); } 
	| OBJECTID ':' TYPEID opt_assign
		{ $$ = attr($1, $3, $4); }
	;

formal	: OBJECTID ':' TYPEID 
       		{ $$ = formal($1, $3); }
	;

formals_list	: formal 
	     		{ $$ = single_Formals($1); }
	     	| formals_list ',' formal 
			{ $$ = append_Formals($1, single_Formals($3)); }
		;
exprs_comma	: expr
	    		{ $$ = single_Expressions($1); }
		| exprs_comma ',' expr
			{ $$ = append_Expressions($1, single_Expressions($3)); }
		;

exprs_semi	: expr ';'
			{ $$ = single_Expressions($1); }
		| exprs_semi expr ';'
			{ $$ = append_Expressions($1, single_Expressions($2)); }
		;

case	: OBJECTID ':' TYPEID DARROW expr ';'
     		{ $$ = branch($1, $3, $5); }
	;

cases 	: case
       		{ $$ = single_Cases($1); }
	| cases case
		{ $$ = append_Cases($1, single_Cases($2)); }
	;

opt_assign	: /* empty */
		    	{ $$ = no_expr(); }
		| ASSIGN expr
			{ $$ = $2; }
		;

let_expr	: OBJECTID ':' TYPEID opt_assign IN expr 
	 		%prec LET
	 		{ $$ = let($1, $3, $4, $6); }
		| OBJECTID ':' TYPEID opt_assign ',' let_expr
			{ $$ = let($1, $3, $4, $6); }
		| error ',' let_expr
			{ $$ = $3; }
		;

expr	: OBJECTID ASSIGN expr
     		{ $$ = assign($1, $3); }
	| expr '@' TYPEID '.' OBJECTID '(' ')'
		{ $$ = static_dispatch($1, $3, $5, nil_Expressions()); }
	| expr '@' TYPEID '.' OBJECTID '(' exprs_comma ')'
		{ $$ = static_dispatch($1, $3, $5, $7); }
	| OBJECTID '(' ')'
		{ $$ = dispatch(object(idtable.add_string("self")), $1, nil_Expressions()); }
	| OBJECTID '(' exprs_comma ')'
		{ $$ = dispatch(object(idtable.add_string("self")), $1, $3); }
	| expr '.' OBJECTID '(' ')'
		{ $$ = dispatch($1, $3, nil_Expressions()); }
	| expr '.' OBJECTID '(' exprs_comma ')'
		{ $$ = dispatch($1, $3, $5); }
	| IF expr THEN expr ELSE expr FI
		{ $$ = cond($2, $4, $6); }
	| WHILE expr LOOP expr POOL
		{ $$ = loop($2, $4); }
	| '{' exprs_semi '}'
		{ $$ = block($2); }
	| '{' '}'
		{ $$ = no_expr(); }
	| '{' error '}'
		{ $$ = no_expr(); }
	| LET let_expr 
		{ $$ = $2; }
	| CASE expr OF cases ESAC
		{ $$ = typcase($2, $4); }
	| NEW TYPEID
		{ $$ = new_($2); }
	| ISVOID expr
		{ $$ = isvoid($2); }
	| expr '+' expr
		{ $$ = plus($1, $3); }
	| expr '-' expr
		{ $$ = sub($1, $3); }
	| expr '*' expr 
		{ $$ = mul($1, $3); }
	| expr '/' expr 
		{ $$ = divide($1, $3); }
	| '~' expr
		{ $$ = neg($2); }
	| expr '<' expr
		{ $$ = lt($1, $3); }
	| expr LE expr
		{ $$ = leq($1, $3); }
	| expr '=' expr
		{ $$ = eq($1, $3); }
	| NOT expr
		{ $$ = comp($2); }
	| '(' expr ')'
		{ $$ = $2; } 
	| OBJECTID 
     		{ $$ = object($1); }
	| STR_CONST
		{ $$ = string_const($1); }
	| INT_CONST
		{ $$ = int_const($1); }
	| BOOL_CONST
		{ $$ = bool_const($1); } 
	; 

/* end of grammar */
%%

/* This function is called automatically when Bison detects a parse error. */
void yyerror(char *s)
{

  cerr << "\"" << curr_filename << "\", line " << curr_lineno << ": " \
    << s << " at or near ";
  print_cool_token(yychar);
  cerr << endl;
  omerrs++;

  if(omerrs>50) {fprintf(stdout, "More than 50 errors\n"); exit(1);}
}

