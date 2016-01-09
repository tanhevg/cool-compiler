//
// Created by Evgeny Tanhilevich on 07/12/2015.
//

#ifndef COOL_COMPILER_TYPE_ENV_H
#define COOL_COMPILER_TYPE_ENV_H

#include <map>
#include <vector>
#include <memory>
#include "stringtab.h"
#include "semant_error.h"
#include "symtab.h"
#include "visitor.h"
#include "ClassTable.h"

using std::map;
using std::pair;
using std::vector;
using std::move;
using std::shared_ptr;
using std::make_shared;
using std::make_pair;



typedef SymbolTable<Symbol, Entry> ObjectEnv;

class MethodSignature {
private:
    Symbol declaring_class;
    Symbol return_type;
    Symbol name;
    vector<Symbol> parameter_types;
public:
    MethodSignature(Symbol _declaring_class, Symbol _name, Symbol _return_type, vector<Symbol>&& _parameter_types):
            declaring_class(_declaring_class), name(_name), return_type(_return_type), parameter_types(_parameter_types)
    {}
    MethodSignature():
            declaring_class(nullptr), name(nullptr), return_type(nullptr), parameter_types()
    {}
    MethodSignature(MethodSignature&&other) :
        declaring_class(other.declaring_class),
        return_type(other.return_type),
        name(other.name),
        parameter_types(move(other.parameter_types))
    {
        other.parameter_types = vector<Symbol>();
    }
    MethodSignature& operator=(MethodSignature&& other) {
        declaring_class = other.declaring_class;
        return_type = other.return_type;
        name = other.name;
        parameter_types = move(other.parameter_types);
        other.parameter_types = vector<Symbol>();
        return *this;
    }
    Symbol get_declaring_class() const {return declaring_class; }
    Symbol get_return_type() const {return return_type;}
    Symbol get_name() const {return name;}
    const vector<Symbol>& get_parameter_types() const {return parameter_types;}

};

class MethodEnv
{
    ClassTable& class_table;
    map<pair<Symbol, Symbol>, MethodSignature> methods;
public:
    MethodEnv(ClassTable& _class_table):
            class_table(_class_table),
            methods(map<pair<Symbol, Symbol>, MethodSignature>())
    {}
    const MethodSignature& get_method_no_parents(Symbol call_site_type, Symbol method_name);
    const MethodSignature& get_method(Symbol call_site_type, Symbol method_name);
    void add_method(Symbol class_name, Symbol method_name, Symbol return_type, vector<Symbol> formal_parameter_types);
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
    vector<Symbol> formal_types;
    Symbol class_name;
public:
    MethodResolver(TypeEnv& _type_env) :
            type_env(_type_env),
            formal_types()
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
