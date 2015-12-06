#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>
#include <map>
#include <utility>
#include <memory>
#include <vector>

#include "stringtab.h"
#include "inheritance.h"
#include "semant_error.h"
#include "symtab.h"
#include "visitor.h"

using std::map;
using std::pair;
using std::unique_ptr;
using std::shared_ptr;
using std::vector;
using std::make_pair;
using std::make_unique;
using std::make_shared;
using std::get;


#define TRUE 1
#define FALSE 0

class ClassTable;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.


class ClassTable
{
private:
    void install_basic_classes();
    map<Symbol, Class_> class_by_name;

    SemantError& semant_error;

public:
    ClassTable(Classes, SemantError&);
    void add_class(Class_);
    Class_ get_class(Symbol s);
    Symbol get_parent(Symbol class_name);
    bool is_subtype(Symbol sub, Symbol super);

    Symbol join(const vector<Symbol> &type1);
};

typedef SymbolTable<Symbol, Entry> ObjectEnv;


class MethodEnv
{
    ClassTable& class_table;
    map<pair<Symbol, Symbol>, pair<Symbol, vector<Symbol>*>> methods;
public:
    MethodEnv(ClassTable& _class_table):
            class_table(_class_table),
            methods(map<pair<Symbol, Symbol>, pair<Symbol, vector<Symbol>*>>())
    {}
    bool check_method_dynamic(Symbol call_site_type, Symbol method_name, Symbol return_type, vector<Symbol> argument_types)
    {return false;}
    bool check_method_static(Symbol call_site_type, Symbol method_name, Symbol return_type, vector<Symbol> argument_types)
    {return false;}
    pair<Symbol, vector<Symbol>*> get_method_no_parents(Symbol call_site_type, Symbol method_name);
    pair<Symbol, vector<Symbol>*> get_method(Symbol call_site_type, Symbol method_name);
    void add_method(Symbol call_site_type, Symbol method_name, Symbol return_type, vector<Symbol>* argument_types);
};

struct TypeEnv {
public:
    TypeEnv(ClassTable& _class_table, ObjectEnv& _object_env, MethodEnv& _method_env) :
        class_table(_class_table),
        object_env(_object_env),
        method_env(_method_env),
        current_class(nullptr)
    {}

    ClassTable& class_table;
    ObjectEnv& object_env;
    MethodEnv& method_env;
    Class_ current_class;

};

class InheritanceChecker : public TreeVisitor
{
    ClassTable& class_table;
    SemantError& semant_error;
    vector<Symbol> no_inheritance;
public:
    virtual void before(class__class* node) override;
    InheritanceChecker(ClassTable& _class_table, SemantError& _semant_error);
};

class AttributeResolver : public TreeVisitor
{
private:
    TypeEnv& type_env;
    SemantError& semant_error;
public:
    AttributeResolver(TypeEnv& _type_env, SemantError&  _semant_error) :
            type_env(_type_env),
            semant_error(_semant_error)
    {}

    virtual void before(class__class* node) override;
    virtual void before(attr_class* node) override;
};


class TypeChecker: public TreeVisitor
{
    TypeEnv& type_env;
    SemantError& semant_error;
    AttributeResolver attribute_resolver;
    void check_arith_type(Binary_Expression_class *node);
    void check_formals(tree_node *node, Symbol name, Symbol callee_type, vector<Symbol> *formals, Expressions actuals);
    void check_comparison(Binary_Expression_class *node);
public:
    TypeChecker(TypeEnv &_type_env, SemantError& _semant_error):
            type_env(_type_env),
            semant_error(_semant_error),
            attribute_resolver(type_env, semant_error)
    { }

    virtual void before(class__class *node) override;

    virtual void after(class__class *node) override;

    virtual void after(int_const_class* node)  override;
    virtual void after(string_const_class* node) override;
    virtual void after(bool_const_class* node) override;

    virtual void after(object_class* node) override;
    virtual void after(mul_class* node) override;
    virtual void after(divide_class* node) override;
    virtual void after(plus_class* node) override;
    virtual void after(sub_class* node) override;

    virtual void after(assign_class* node) override;

    virtual void after(new__class *node) override;

    virtual void before(method_class *node) override;

    virtual void after(method_class *node) override;

    virtual void after(formal_class *node) override;

    virtual void after(dispatch_class *node) override;

    virtual void after(static_dispatch_class *node) override;

    virtual void after(cond_class *node) override;

    virtual void after(block_class *node) override;

    virtual void after(no_expr_class *node) override;

    virtual void before(let_class *node) override;

    virtual void after(let_class *node) override;

    virtual void before(branch_class *node) override;

    virtual void after(branch_class *node) override;

    virtual void after(typcase_class *node) override;

    virtual void after(loop_class *node) override;

    virtual void after(isvoid_class *node) override;

    virtual void after(comp_class *node) override;

    virtual void after(lt_class *node) override;

    virtual void after(leq_class *node) override;

    virtual void after(neg_class *node) override;

    virtual void after(eq_class *node) override;
};

class MethodResolver : public TreeVisitor
{
    TypeEnv& type_env;
    vector<Symbol> *formal_types;
    Symbol class_name;
public:
    MethodResolver(TypeEnv& _type_env) :
            type_env(_type_env),
            formal_types(nullptr)
    {}


    virtual void before(class__class *node) override;
    virtual void before(method_class *node) override;

    virtual void after(method_class *node) override;
    virtual void after(formal_class *node) override;
};

class MethodChecker : public TreeVisitor {
    TypeEnv &type_env;
    Symbol class_name;
    SemantError &semant_error;
public:
    MethodChecker(TypeEnv &_type_env, SemantError &_semant_error) :
            type_env(_type_env),
            semant_error(_semant_error) { }
    virtual void before(class__class *node) override;
    virtual void before(method_class *node) override;

};


#endif

