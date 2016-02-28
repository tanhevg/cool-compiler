//
// Created by Evgeny Tanhilevich on 05/12/2015.
//

#include "visitor.h"

void program_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    classes->traverse_tree(visitor);
    visitor->after(this);
}

void class__class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    features->traverse_tree(visitor);
    visitor->after(this);
}

void attr_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    init->traverse_tree(visitor);
    visitor->after(this);
}

void formal_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    visitor->after(this);
}

void method_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    formals->traverse_tree(visitor);
    expr->traverse_tree(visitor);
    visitor->after(this);
}

void branch_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    expr->traverse_tree(visitor);
    visitor->after(this);
}

void assign_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    expr->traverse_tree(visitor);
    visitor->after(this);
}

void static_dispatch_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    expr->traverse_tree(visitor);
    actual->traverse_tree(visitor);
    visitor->after(this);
}

void dispatch_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    expr->traverse_tree(visitor);
    actual->traverse_tree(visitor);
    visitor->after(this);
}

void cond_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    pred->traverse_tree(visitor);
    then_exp->traverse_tree(visitor);
    else_exp->traverse_tree(visitor);
    visitor->after(this);
}

void loop_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    pred->traverse_tree(visitor);
    body->traverse_tree(visitor);
    visitor->after(this);
}

void typcase_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    expr->traverse_tree(visitor);
    cases->traverse_tree(visitor);
    visitor->after(this);
}

void block_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    body->traverse_tree(visitor);
    visitor->after(this);
}

void let_class::traverse_tree(TreeVisitor *visitor) {
    /*
     We process the initializer ahead of before() for let, to make sure that the variable
     is not introduced in scope for the initializer.
     In other words, let x <- init in expr will not compile if init references x
     (unless x is defined elsewhere in outer scope)
     */
    init->traverse_tree(visitor);
    visitor->before(this);
    body->traverse_tree(visitor);
    visitor->after(this);
}

void plus_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    e1->traverse_tree(visitor);
    e2->traverse_tree(visitor);
    visitor->after(this);
}

void sub_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    e1->traverse_tree(visitor);
    e2->traverse_tree(visitor);
    visitor->after(this);
}

void mul_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    e1->traverse_tree(visitor);
    e2->traverse_tree(visitor);
    visitor->after(this);
}

void divide_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    e1->traverse_tree(visitor);
    e2->traverse_tree(visitor);
    visitor->after(this);
}

void leq_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    e1->traverse_tree(visitor);
    e2->traverse_tree(visitor);
    visitor->after(this);
}

void lt_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    e1->traverse_tree(visitor);
    e2->traverse_tree(visitor);
    visitor->after(this);
}

void eq_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    e1->traverse_tree(visitor);
    e2->traverse_tree(visitor);
    visitor->after(this);
}

void neg_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    e1->traverse_tree(visitor);
    visitor->after(this);
}

void comp_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    e1->traverse_tree(visitor);
    visitor->after(this);
}

void int_const_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    visitor->after(this);
}

void string_const_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    visitor->after(this);
}

void bool_const_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    visitor->after(this);
}

void new__class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    visitor->after(this);
}

void isvoid_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    e1->traverse_tree(visitor);
    visitor->after(this);
}

void object_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    visitor->after(this);
}

void no_expr_class::traverse_tree(TreeVisitor *visitor) {
    visitor->before(this);
    visitor->after(this);
}
