//
// Created by Evgeny Tanhilevich on 12/03/2016.
//

#include "TemporariesCounter.h"

static inline int max(const int a, const int b) {
    return a > b ? a : b;
}

static int max_tmp_count(Expressions expressions) {
    int max_tmp = 0;
    expressions->traverse([&max_tmp](Expression expr) {
        max_tmp = max(max_tmp, expr->get_temporaries_count());
    });
    return max_tmp;
}

static int max_tmp_count(Cases cases) {
    int max_tmp = 0;
    cases->traverse([&max_tmp](Case _case) {
        max_tmp = max(max_tmp, _case->get_temporaries_count());
    });
    return max_tmp;
}

void TemporariesCounter::after(dispatch_class *node) {
    node->set_temporaries_count(max(node->get_callee()->get_temporaries_count(), max_tmp_count(node->get_actuals())));
}

void TemporariesCounter::after(static_dispatch_class *node) {
    node->set_temporaries_count(max(node->get_callee()->get_temporaries_count(), max_tmp_count(node->get_actuals())));
}

void TemporariesCounter::after(cond_class *node) {
    int temporaries_count = max(node->get_then()->get_temporaries_count(),
                                node->get_else()->get_temporaries_count());
    temporaries_count = max(temporaries_count, node->get_predicate()->get_temporaries_count());
    node->set_temporaries_count(temporaries_count);

}

void TemporariesCounter::after(block_class *node) {
    node->set_temporaries_count(max_tmp_count(node->get_body()));
}

void TemporariesCounter::after(let_class *node) {
    node->set_temporaries_count(max(
            node->get_init()->get_temporaries_count(),
            node->get_body()->get_temporaries_count() + 1));
}

void TemporariesCounter::after(branch_class *node) {
    node->set_temporaries_count(node->get_expr()->get_temporaries_count());
}

void TemporariesCounter::after(typcase_class *node) {
    node->set_temporaries_count(max(node->get_expr()->get_temporaries_count(), max_tmp_count(node->get_cases())));
}

void TemporariesCounter::after(loop_class *node) {
    node->set_temporaries_count(max(
            node->get_predicate()->get_temporaries_count(),
            node->get_body()->get_temporaries_count()));
}

void TemporariesCounter::after_expr(Unary_Expression_class *node) {
    node->set_temporaries_count(node->get_e1()->get_temporaries_count());
}

void TemporariesCounter::after_expr(Binary_Expression_class *node) {
    node->set_temporaries_count(max(
            node->get_e1()->get_temporaries_count(),
            node->get_e2()->get_temporaries_count() + 1));
}

void TemporariesCounter::after(mul_class *node) {
    after_expr(node);
}

void TemporariesCounter::after(divide_class *node) {
    after_expr(node);
}

void TemporariesCounter::after(plus_class *node) {
    after_expr(node);
}

void TemporariesCounter::after(sub_class *node) {
    after_expr(node);
}

void TemporariesCounter::after(assign_class *node) {
    node->set_temporaries_count(node->get_expr()->get_temporaries_count());
}

void TemporariesCounter::after(isvoid_class *node) { after_expr(node); }

void TemporariesCounter::after(comp_class *node) { after_expr(node); }

void TemporariesCounter::after(lt_class *node) { after_expr(node); }

void TemporariesCounter::after(leq_class *node) { after_expr(node); }

void TemporariesCounter::after(neg_class *node) { after_expr(node); }

void TemporariesCounter::after(eq_class *node) { after_expr(node); }
