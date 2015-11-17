//
// Created by Evgeny Tanhilevich on 15/11/15.
//

#include "inheritance.h"

InheritanceTable::InheritanceTable(ClassTableP _pClassTable): pClassTable(_pClassTable)
{

}

void InheritanceTable::add_node(Class_ _cls, bool can_inherit_from)
{
    cout << "Add inheritance node " << _cls->get_name() << endl;
    InheritanceNodeP pNode = make_shared<InheritanceNode>(InheritanceNode(_cls, can_inherit_from));
    const Symbol class_name = _cls->get_name();
    node_by_name[class_name] = pNode;
    if (idtable.lookup_string("Object") == class_name) {
        object_root = pNode;
    }
    cout << "insert: node_by_name[" << class_name << "] = " << pNode << endl;
}

void InheritanceTable::check_inheritance_errors() {
    if (pClassTable->errors()) {
        return;
    }
    for (auto& ppNode : node_by_name) {
        InheritanceNodeP pNode = ppNode.second;
        if (pNode->get_class()->get_name() != idtable.lookup_string("Object")) {
            InheritanceNodeP pParent = node_by_name[pNode->get_class()->get_parent()];
            cout << "lookup: node_by_name[" << pNode->get_class()->get_parent() << "] = " << pParent << endl;
            if (pParent == nullptr) {
                pClassTable->semant_error(pNode->get_class())
                << pNode->get_class()->get_name()
                << ": undefined parent: "
                << pNode->get_class()->get_parent()
                << endl;
                return;
            }
            if (!pParent->can_inherit_from()) {
                pClassTable->semant_error(pNode->get_class())
                << pNode->get_class()->get_name()
                << ": cannot inherit from "
                << pNode->get_class()->get_parent()
                << endl;
                return;
            }
            pParent->add_child(pNode);
        }
    }
    bool no_cycles = object_root->visit(pClassTable);
    if (no_cycles) {
        for (auto ppNode : node_by_name) {
            InheritanceNodeP pNode = ppNode.second;
            if (!pNode->count_ok()) {
                pClassTable->semant_error(pNode->get_class())
                    << pNode->get_class()->get_name() << ": cyclic inheritance" << endl;
                return;
            }
        }
    }
}

bool InheritanceNode::visit(ClassTableP pClassTable) {
    ++visit_count;
    if (visit_count > 1) {
        pClassTable->semant_error(class_)
            << class_->get_name() << ": cyclic inheritance" << endl;
        return false;
    }
    for (auto pNode = children.begin(); pNode != children.end(); pNode++ ) {
        InheritanceNodeP node = *pNode;
        if (!node->visit(pClassTable)) {
            return false;
        }
    }
    return true;
}