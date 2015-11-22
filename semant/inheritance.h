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
#include "semant_error.h"
#include "tree_node.h"



using std::map;
using std::pair;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::make_pair;
using std::make_shared;
using std::get;
using std::endl;

class InheritanceNode;
typedef shared_ptr<TreeNode<InheritanceNode>> InheritanceNodeP;

class InheritanceNode {
private:
    int m_visit_count;
    Class_ class_;
    bool m_can_inherit_from;
    SemantErrorP m_pSemantError;
public:
    InheritanceNode(Class_ _class_, SemantErrorP _pSemantError, bool p_can_inherit_from) :
            m_visit_count(0), class_(_class_), m_pSemantError(_pSemantError), m_can_inherit_from(p_can_inherit_from) { }
    Class_ get_class() { return class_;}

    bool check_cycles();
    bool count_ok() {return m_visit_count == 1;}
    bool can_inherit_from() {return m_can_inherit_from; }
};

class InheritanceTable;


class InheritanceTable {
private:
    SemantErrorP m_pSemantError;
    InheritanceNodeP object_root;
    map<Symbol, InheritanceNodeP> node_by_name;
public:
    InheritanceTable(SemantErrorP _pClassTable) : m_pSemantError(_pClassTable){}
    void add_node(Class_ _cls, bool can_inherit_from = true);
    void check_inheritance_errors() ;
};
#endif //COOL_COMPILER_INHERITANCE_H
