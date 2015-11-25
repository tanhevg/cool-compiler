#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>
#include <map>
#include <utility>
#include <memory>
#include <vector>
#include "cool-tree.h"
#include "stringtab.h"
#include "inheritance.h"
#include "semant_error.h"
#include "symtab.h"

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


class ClassTable {
private:
    void install_basic_classes();
    map<Symbol, Class_> class_by_name;

    SemantError& semant_error;

public:
    ClassTable(Classes, SemantError&);
    void add_class(Class_);
    Class_ get_class(Symbol s);
    bool is_subtype(Symbol sub, Symbol super);
};

typedef SymbolTable<Symbol, Symbol> ObjectEnv;
typedef SymbolTable<pair<Symbol, Symbol>, vector<Symbol>> MethodEnv;

struct TypeEnv {
public:
    ClassTable& class_table;
    ObjectEnv& object_env;
    MethodEnv& method_env;
    Class_ current_class;

};

class TreeVisitor {
public:
    virtual void before(tree_node* node) const {};
    virtual void after(tree_node* node) const {};

    virtual void before(class__class* node) const {};
    virtual void after(class__class* node) const {};

    virtual void before(program_class* node) const {};
    virtual void after(program_class* node) const {};

    virtual void before(method_class* node) const {};
    virtual void after(method_class* node) const {};

    virtual void before(attr_class* node) const {};
    virtual void after(attr_class* node) const {};

    virtual void before(formal_class* node) const {};
    virtual void after(formal_class* node) const {};

    virtual ~TreeVisitor() {}
};

class InheritanceChecker;
class TypeChecker;
class MethodResolver;
class AttributeResolver;


#endif

