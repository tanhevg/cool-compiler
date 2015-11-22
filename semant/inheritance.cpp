//
// Created by Evgeny Tanhilevich on 15/11/15.
//

#include "inheritance.h"

void InheritanceTable::add_node(Class_ _cls, bool can_inherit_from)
{
    const Symbol class_name = _cls->get_name();
//    cout << "Add inheritance node " << class_name << " " << (void*)(class_name) <<  endl;
    InheritanceNodeP pNode = make_shared<TreeNode<InheritanceNode>>(TreeNode<InheritanceNode>(
            make_shared<InheritanceNode>(InheritanceNode(_cls, m_pSemantError, can_inherit_from))));
    node_by_name[class_name] = pNode;
    if (idtable.lookup_string("Object") == class_name) {
        object_root = pNode;
    }
//    cout << "insert: node_by_name[" << class_name << " " << (void*)(class_name) << "] = " << pNode << endl;
}


void InheritanceTable::check_inheritance_errors() {
    if (m_pSemantError->errors()) {
        return;
    }
    for (auto& ppNode : node_by_name) {
        InheritanceNodeP pNode = ppNode.second;
        Class_ class_ = pNode->get_data()->get_class();
        Symbol const node_name = class_->get_name();
        if (node_name != idtable.lookup_string("Object")) {
            const Symbol parent_name = class_->get_parent();
            InheritanceNodeP pParent = node_by_name[parent_name];
//            cout << "lookup: node_by_name[" << parent_name << " " << (void*)(parent_name) << "] = " << pParent << endl;
            if (pParent == nullptr) {
                m_pSemantError->semant_error(class_)
                << node_name
                << "undefined parent: "
                << parent_name
                << endl;
                return;
            }
            if (!pParent->get_data()->can_inherit_from()) {
                m_pSemantError->semant_error(class_)
                << node_name
                << "cannot inherit from "
                << parent_name
                << endl;
                return;
            }
            pParent->add_child(pNode);
        }
    }


    bool no_cycles = object_root->traverse_top_if_true([](shared_ptr<InheritanceNode> node) {
        return node->check_cycles();
    });
    if (no_cycles) {
        for (auto ppNode : node_by_name) {
            InheritanceNodeP pNode = ppNode.second;
            if (!pNode->get_data()->count_ok()) {
                m_pSemantError->semant_error(pNode->get_data()->get_class())
                    << pNode->get_data()->get_class()->get_name() << ": cyclic inheritance" << endl;
                return;
            }
        }
    }
}

bool InheritanceNode::check_cycles() {
    ++m_visit_count;
    cout << "check cycles " << get_class()->get_name() << " m_visit_count " << m_visit_count << endl;
    if (m_visit_count > 1) {
        m_pSemantError->semant_error(class_) << get_class()->get_name() << ": cyclic inheritance" << endl;
        return false;
    }
    return true;
}