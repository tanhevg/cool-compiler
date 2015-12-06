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
virtual void semant() = 0;						\
virtual void dump_with_types(ostream&, int) = 0;



#define program_EXTRAS                          \
void semant();									\
void traverse_tree(TreeVisitor *visitor);\
void dump_with_types(ostream&, int);

#define Class__EXTRAS                   \
virtual Symbol get_filename() = 0;      \
virtual void dump_with_types(ostream&,int) = 0; \
virtual Symbol get_name() = 0;					\
virtual Symbol get_parent() = 0;


#define class__EXTRAS                                 \
Symbol get_filename() { return filename; }             \
void traverse_tree(TreeVisitor *visitor);        \
void dump_with_types(ostream&,int);                    \
Symbol get_name();										\
Symbol get_parent();


#define Feature_EXTRAS                                        \
virtual void dump_with_types(ostream&,int) = 0; 


#define Feature_SHARED_EXTRAS                                       \
void dump_with_types(ostream&,int);    

#define attr_EXTRAS	            		            	\
void traverse_tree(TreeVisitor *visitor);        \
Symbol get_name();										\
Symbol get_type();

#define method_EXTRAS	            		            	\
void traverse_tree(TreeVisitor *visitor);	            	\
Symbol get_return_type();	                				\
Symbol get_name();	            				        	\
Expression get_body();



#define Formal_EXTRAS                              \
virtual void dump_with_types(ostream&,int) = 0;


#define formal_EXTRAS                           \
void traverse_tree(TreeVisitor *visitor);        \
Symbol get_type();	    		            	\
Symbol get_name();	    		            	\
void dump_with_types(ostream&,int);

#define object_EXTRAS	        			    \
Symbol get_name();	            				\
void traverse_tree(TreeVisitor *visitor);

#define Case_EXTRAS                             \
virtual Expression get_expr() = 0;\
virtual Symbol get_type_decl() = 0;\
virtual void dump_with_types(ostream& ,int) = 0;


#define branch_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);        \
Symbol get_name();\
Symbol get_type_decl();\
Expression get_expr();\
void dump_with_types(ostream& ,int);

#define assign_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);	            \
Symbol get_name();  			        		    	\
Expression get_expr();

#define static_dispatch_EXTRAS                                   \
Expression get_callee();        			            \
Expressions get_actuals();      		                \
Symbol get_name();  			        		    	\
Symbol get_type_name();  			        		    	\
void traverse_tree(TreeVisitor *visitor);

#define dispatch_EXTRAS                                   \
Expression get_callee();        			            \
Expressions get_actuals();      		                \
Symbol get_name();  			        		    	\
void traverse_tree(TreeVisitor *visitor);

#define cond_EXTRAS                                   \
Expression get_predicate();\
Expression get_else();\
Expression get_then();\
void traverse_tree(TreeVisitor *visitor);

#define loop_EXTRAS                                   \
Expression get_predicate();\
Expression get_body();\
void traverse_tree(TreeVisitor *visitor);

#define typcase_EXTRAS                                   \
Cases get_cases();\
void traverse_tree(TreeVisitor *visitor);

#define block_EXTRAS                                   \
Expressions get_body();\
void traverse_tree(TreeVisitor *visitor);

#define let_EXTRAS                                   \
Symbol get_identifier();\
Symbol get_type_decl();\
Expression get_init();\
Expression get_body();\
void traverse_tree(TreeVisitor *visitor);

#define plus_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define sub_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define mul_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define divide_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define neg_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define lt_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define eq_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define leq_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define comp_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define bool_const_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define string_const_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define int_const_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define new__EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);	        \
Symbol get_type_name();

#define isvoid_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define no_expr_EXTRAS                                   \
void traverse_tree(TreeVisitor *visitor);

#define Expression_EXTRAS                    \
Symbol type;                                 \
Symbol get_type() { return type; }           \
Expression set_type(Symbol s) { type = s; return this; } \
virtual void dump_with_types(ostream&,int) = 0;  \
void dump_type(ostream&, int);               \
Expression_class() { type = (Symbol) NULL; }



#define Expression_SHARED_EXTRAS           \
void dump_with_types(ostream&,int); 


#endif
