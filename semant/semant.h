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

using std::map;
using std::pair;
using std::unique_ptr;
using std::vector;
using std::make_pair;
using std::make_unique;
using std::get;


#define TRUE 1
#define FALSE 0

class ClassTable;

typedef ClassTable *ClassTableP;

class InheritanceTable;

typedef unique_ptr<InheritanceTable> InheritanceTableP;


//typedef InheritanceNode *InheritanceNodeP;
//typedef pair<Class_, InheritanceNodeP> InheritancePair;


// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
    int semant_errors;
    void install_basic_classes();
    ostream &error_stream;
    map<Symbol, Class_> class_by_name;

    void add_class(Class_);
    InheritanceTableP m_pInheritanceTable;

public:
    ClassTable(Classes);
    Class_ get_class(Symbol s);
    template<typename Op> void for_each_class(Op op);
    int errors() { return semant_errors; }
    ostream &semant_error();
    ostream &semant_error(Class_ c);
    ostream &semant_error(Symbol filename, tree_node *t);
    void check_inheritance_errors();
};



#endif

