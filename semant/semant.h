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

typedef ClassTable *ClassTableP;

class InheritanceTable;
typedef shared_ptr<InheritanceTable> InheritanceTableP;


//typedef InheritanceNode *InheritanceNodeP;
//typedef pair<Class_, InheritanceNodeP> InheritancePair;


// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.


class ClassTable {
private:
    void install_basic_classes();
    map<Symbol, Class_> class_by_name;

    void add_class(Class_);
    InheritanceTableP m_pInheritanceTable;
    SemantErrorP m_pSemantError;

public:
    ClassTable(Classes, SemantErrorP, InheritanceTableP);
    Class_ get_class(Symbol s);
    void check_inheritance_errors();
};



#endif

