//
// Created by Evgeny Tanhilevich on 05/12/2015.
//

#ifndef COOL_COMPILER_VISITOR_H
#define COOL_COMPILER_VISITOR_H
#include "cool-tree.h"

class TreeVisitor {
public:
    virtual void before(tree_node* node)  {};
    virtual void after(tree_node* node)  {};

    virtual void before(program_class* node)  {};

    virtual void before(class__class* node)  {};
    virtual void after(class__class* node)  {};

    virtual void before(method_class* node)  {};
    virtual void after(method_class* node)  {};

    virtual void before(attr_class* node)  {};
    virtual void after(attr_class* node)  {};

    virtual void after(formal_class* node)  {};

    virtual void after(int_const_class* node)  {};
    virtual void after(string_const_class* node)  {};
    virtual void after(bool_const_class* node)  {};

    virtual void after(object_class* node)  {};
    virtual void after(mul_class* node)  {};
    virtual void after(divide_class* node)  {};
    virtual void after(plus_class* node)  {};
    virtual void after(sub_class* node)  {};
    virtual void after(assign_class* node) {};
    virtual void after(new__class *node) {};
    virtual void after(dispatch_class *node) {};
    virtual void after(static_dispatch_class *node){};
    virtual void after(cond_class *node) {};
    virtual void after(block_class *node) {};
    virtual void after(no_expr_class *node) {};
    virtual void before(let_class *node) {};
    virtual void after(let_class *node) {};
    virtual void before(branch_class *node) {};
    virtual void after(branch_class *node) {};
    virtual void before(typcase_class *node) {};
    virtual void after(typcase_class *node) {};
    virtual void after(loop_class *node) {};
    virtual void after(isvoid_class *node) {};
    virtual void after(comp_class *node) {};
    virtual void after(lt_class *node) {};
    virtual void after(leq_class *node) {};
    virtual void after(neg_class *node) {};
    virtual void after(eq_class *node) {};
};


#endif //COOL_COMPILER_VISITOR_H
