//
// Created by Evgeny Tanhilevich on 12/03/2016.
//

#ifndef COOL_COMPILER_TEMPORARIESCOUNTER_H
#define COOL_COMPILER_TEMPORARIESCOUNTER_H


#include <visitor.h>

class TemporariesCounter : public TreeVisitor {
public:

    virtual void after(mul_class *node) override;

    virtual void after(divide_class *node) override;

    virtual void after(plus_class *node) override;

    virtual void after(sub_class *node) override;

    virtual void after(assign_class *node) override;

    virtual void after(dispatch_class *node) override;

    virtual void after(static_dispatch_class *node) override;

    virtual void after(cond_class *node) override;

    virtual void after(block_class *node) override;

    virtual void after(let_class *node) override;

    virtual void after(branch_class *node) override;

    virtual void after(typcase_class *node) override;

    virtual void after(loop_class *node) override;

    virtual void after(isvoid_class *node) override;

    virtual void after(comp_class *node) override;

    virtual void after(lt_class *node) override;

    virtual void after(leq_class *node) override;

    virtual void after(neg_class *node) override;

    virtual void after(eq_class *node) override;

private:
    void after_expr(Unary_Expression_class *node);

    void after_expr(Binary_Expression_class *node);
};


#endif //COOL_COMPILER_TEMPORARIESCOUNTER_H
