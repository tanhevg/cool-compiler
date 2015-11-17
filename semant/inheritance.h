#ifndef COOL_COMPILER_INHERITANCE_H
#define COOL_COMPILER_INHERITANCE_H

#include <assert.h>
#include <iostream>
#include <map>
#include <utility>
#include <memory>
#include <vector>
#include "cool-tree.h"
#include "stringtab.h"
#include "semant.h"

using std::map;
using std::pair;
using std::shared_ptr;
using std::vector;
using std::make_pair;
using std::make_shared;
using std::get;
using std::endl;

class ClassTable;
class InheritanceNode;
typedef shared_ptr<InheritanceNode> InheritanceNodeP;

class InheritanceNode {
private:
    int visit_count;
    Class_ class_;
    bool m_can_inherit_from;
    vector<InheritanceNodeP> children;
public:
    InheritanceNode(Class_ _class_, bool p_can_inherit_from) :
            visit_count(0), class_(_class_), m_can_inherit_from{p_can_inherit_from}, children{} { }
    InheritanceNode(InheritanceNode&&) = default;
    void add_child(InheritanceNodeP node) { children.push_back(node);}
    Class_ get_class() { return class_;}

    bool visit(ClassTable*);
    bool count_ok() {return visit_count == 1;}
    bool can_inherit_from() {return m_can_inherit_from; }
};

class InheritanceTable;


class InheritanceTable {
private:
    ClassTable* pClassTable;
    InheritanceNodeP object_root;
    map<Symbol, InheritanceNodeP> node_by_name;
public:
    InheritanceTable(ClassTable* _pClassTable);
    void add_node(Class_ _cls, bool can_inherit_from);
    void check_inheritance_errors() ;
};
#endif //COOL_COMPILER_INHERITANCE_H
