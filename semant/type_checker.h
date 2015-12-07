//
// Created by Evgeny Tanhilevich on 07/12/2015.
//

#ifndef COOL_COMPILER_TYPE_CHECKER_H
#define COOL_COMPILER_TYPE_CHECKER_H


#include "visitor.h"
#include "type_env.h"

class TypeChecker: public TreeVisitor
{
    TypeEnv& type_env;
    SemantError& semant_error;
    AttributeResolver attribute_resolver;
    void check_arith_type(Binary_Expression_class *node);
    void check_formals(tree_node *node, Symbol name, Symbol callee_type, vector<Symbol> *formals, Expressions actuals);
    void check_comparison(Binary_Expression_class *node);
public:
    TypeChecker(TypeEnv &_type_env, SemantError& _semant_error):
            type_env(_type_env),
            semant_error(_semant_error),
            attribute_resolver(type_env, semant_error)
    { }

    virtual void before(class__class *node) override;

    virtual void after(class__class *node) override;

    virtual void after(int_const_class* node)  override;
    virtual void after(string_const_class* node) override;
    virtual void after(bool_const_class* node) override;

    virtual void after(object_class* node) override;
    virtual void after(mul_class* node) override;
    virtual void after(divide_class* node) override;
    virtual void after(plus_class* node) override;
    virtual void after(sub_class* node) override;

    virtual void after(assign_class* node) override;

    virtual void after(new__class *node) override;

    virtual void before(method_class *node) override;

    virtual void after(method_class *node) override;

    virtual void after(formal_class *node) override;

    virtual void after(dispatch_class *node) override;

    virtual void after(static_dispatch_class *node) override;

    virtual void after(cond_class *node) override;

    virtual void after(block_class *node) override;

    virtual void after(no_expr_class *node) override;

    virtual void before(let_class *node) override;

    virtual void after(let_class *node) override;

    virtual void before(branch_class *node) override;

    virtual void after(branch_class *node) override;

    virtual void after(typcase_class *node) override;

    virtual void after(loop_class *node) override;

    virtual void after(isvoid_class *node) override;

    virtual void after(comp_class *node) override;

    virtual void after(lt_class *node) override;

    virtual void after(leq_class *node) override;

    virtual void after(neg_class *node) override;

    virtual void after(eq_class *node) override;

    virtual void after(attr_class *node) override;
};


#endif //COOL_COMPILER_TYPE_CHECKER_H
