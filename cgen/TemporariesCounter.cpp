//
// Created by Evgeny Tanhilevich on 12/03/2016.
//

#include "TemporariesCounter.h"

static inline int max(const int a, const int b) {
    return a > b ? a : b;
}

void TemporariesCounter::after(dispatch_class *node) {
    MaxTemporariesCountVisitor counter;
    node->get_actuals()->traverse_tree(&counter);
    node->set_temporaries_count(counter.get_temporaries_count());
}

void TemporariesCounter::after(static_dispatch_class *node) {
    MaxTemporariesCountVisitor counter;
    node->get_actuals()->traverse_tree(&counter);
    node->set_temporaries_count(counter.get_temporaries_count());
}

void TemporariesCounter::after(cond_class *node) {
    int temporaries_count = max(node->get_then()->get_temporaries_count(),
                                node->get_else()->get_temporaries_count());
    temporaries_count = max(temporaries_count, node->get_predicate()->get_temporaries_count());
    node->set_temporaries_count(temporaries_count);

}

void TemporariesCounter::after(block_class *node) {
    MaxTemporariesCountVisitor counter;
    node->traverse_tree(&counter);
    node->set_temporaries_count(counter.get_temporaries_count());
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
    MaxTemporariesCountVisitor counter;
    node->get_cases()->traverse_tree(&counter);
    node->set_temporaries_count(max(node->get_expr()->get_temporaries_count(), counter.get_temporaries_count()));
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



// -----
// todo need to scan just the top level; no need to go too deep
void MaxTemporariesCountVisitor::after_expr(Expression_class *node) {
    temporaries_count = max(temporaries_count, node->get_temporaries_count());
}

void MaxTemporariesCountVisitor::after(branch_class *node) {
    temporaries_count = max(temporaries_count, node->get_temporaries_count());
}

void MaxTemporariesCountVisitor::after(mul_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(divide_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(plus_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(sub_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(assign_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(dispatch_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(static_dispatch_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(cond_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(block_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(no_expr_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(let_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(typcase_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(loop_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(isvoid_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(comp_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(lt_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(leq_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(neg_class *node) { after_expr(node); }

void MaxTemporariesCountVisitor::after(eq_class *node) { after_expr(node); }


void TemporariesCounter::after(class__class *node) {
    MaxTemporariesCountVisitor counter;
    node->get_features()->traverse_tree(&counter);
    node->set_attr_temporaries_count(counter.get_temporaries_count());

}

void MaxTemporariesCountVisitor::after(attr_class *node) {
    after_expr(node->get_initializer());
}
