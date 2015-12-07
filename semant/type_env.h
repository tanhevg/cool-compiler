//
// Created by Evgeny Tanhilevich on 07/12/2015.
//

#ifndef COOL_COMPILER_TYPE_ENV_H
#define COOL_COMPILER_TYPE_ENV_H

#include <map>
#include <vector>
#include "stringtab.h"
#include "semant_error.h"
#include "symtab.h"
#include "visitor.h"

using std::map;
using std::pair;
using std::vector;

class ClassTable
{
private:
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



#endif //COOL_COMPILER_TYPE_ENV_H
