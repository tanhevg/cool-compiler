//
// The following include files must come first.

#ifndef COOL_TREE_HANDCODE_H
#define COOL_TREE_HANDCODE_H

#include <iostream>
#include "tree.h"
#include "cool.h"
#include "stringtab.h"
#define yylineno curr_lineno;
extern int yylineno;

inline Boolean copy_Boolean(Boolean b) {return b; }
inline void assert_Boolean(Boolean) {}
inline void dump_Boolean(ostream& stream, int padding, Boolean b)
	{ stream << pad(padding) << (int) b << "\n"; }

void dump_Symbol(ostream& stream, int padding, Symbol b);
void assert_Symbol(Symbol b);
Symbol copy_Symbol(Symbol b);

class Program_class;
typedef Program_class *Program;
class Class__class;
typedef Class__class *Class_;
class Feature_class;
typedef Feature_class *Feature;
class Formal_class;
typedef Formal_class *Formal;
class Expression_class;
typedef Expression_class *Expression;
class Case_class;
typedef Case_class *Case;

typedef list_node<Class_> Classes_class;
typedef Classes_class *Classes;
typedef list_node<Feature> Features_class;
typedef Features_class *Features;
typedef list_node<Formal> Formals_class;
typedef Formals_class *Formals;
typedef list_node<Expression> Expressions_class;
typedef Expressions_class *Expressions;
typedef list_node<Case> Cases_class;
typedef Cases_class *Cases;

#define Program_EXTRAS                          \
virtual int semant() = 0;						\
virtual void cgen(ostream &os) = 0;     	    \
virtual void dump_with_types(ostream&, int) = 0;



#define program_EXTRAS                          \
int semant();									\
void cgen(ostream &os);     			        \
void traverse_tree(TreeVisitor *visitor);\
void install_basic_classes();\
void dump_with_types(ostream&, int);

#define Class__EXTRAS                   \
virtual Symbol get_filename() = 0;      \
virtual void dump_with_types(ostream&,int) = 0; \
virtual Symbol get_name() = 0;					\
virtual Symbol get_parent() = 0;	    \
virtual Features get_features() = 0;


#define class__EXTRAS                                 \
Symbol get_filename() { return filename; }             \
void traverse_tree(TreeVisitor *visitor);        \
void dump_with_types(ostream&,int);                    \
Symbol get_name();										\
Symbol get_parent();	        	                	\
Features get_features();


#define Feature_EXTRAS                                        \
virtual void dump_with_types(ostream&,int) = 0; \
virtual Symbol get_name() = 0;


#define Feature_SHARED_EXTRAS                                       \
void dump_with_types(ostream&,int);	    		        	        \
Symbol get_name();


#define attr_EXTRAS	            		            	\
void traverse_tree(TreeVisitor *visitor);        \
Expression get_initializer();\
Symbol get_type();\
int get_temporaries_count() {return get_initializer() -> get_temporaries_count();}

#define method_EXTRAS	            		            	\
void traverse_tree(TreeVisitor *visitor);	            	\
Symbol get_return_type();	                				\
bool is_implemented();\
Expression get_body();



#define Formal_EXTRAS                              \
virtual void dump_with_types(ostream&,int) = 0;


#define formal_EXTRAS                           \
void traverse_tree(TreeVisitor *visitor);        \
Symbol get_type();	    		            	\
Symbol get_name();	    		            	\
void dump_with_types(ostream&,int);

#define object_EXTRAS	        			    \
Symbol get_name();

#define Case_EXTRAS                             \
int temporaries_count;	    		    	 \
virtual Expression get_expr() = 0;\
virtual Symbol get_type_decl() = 0;\
virtual void dump_with_types(ostream& ,int) = 0;\
int get_temporaries_count() {return temporaries_count;} \
void set_temporaries_count(int _temporaries_count) {temporaries_count = _temporaries_count;}



#define branch_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);        \
Symbol get_name();\
Symbol get_type_decl();\
Expression get_expr();\
void dump_with_types(ostream& ,int);

#define assign_EXTRAS                                   \
Symbol get_name();  			        		    	\
Expression get_expr();

#define static_dispatch_EXTRAS                                   \
Expression get_callee();        			            \
Expressions get_actuals();      		                \
Symbol get_name();  			        		    	\
Symbol get_type_name();

#define dispatch_EXTRAS                                   \
Expression get_callee();        			            \
Expressions get_actuals();      		                \
Symbol get_name();

#define cond_EXTRAS                                   \
Expression get_predicate();\
Expression get_else();\
Expression get_then();

#define loop_EXTRAS                                   \
Expression get_predicate();\
Expression get_body();

#define typcase_EXTRAS                                   \
Cases get_cases();\
Expression get_expr();

#define block_EXTRAS                                   \
Expressions get_body();

#define let_EXTRAS                                   \
Symbol get_identifier();\
Symbol get_type_decl();\
Expression get_init();\
Expression get_body();

#define new__EXTRAS                                   \
Symbol get_type_name();

class CodeGenerator;

#define int_const_EXTRAS \
Symbol get_token() { return token; }

#define string_const_EXTRAS \
Symbol get_token() { return token; }

#define bool_const_EXTRAS \
Boolean get_val() { return val; }

#define Expression_SHARED_EXTRAS           \
void code(CodeGenerator *cgen, int n_temp); \
void traverse_tree(TreeVisitor *visitor);   \
void dump_with_types(ostream&,int);


#endif
