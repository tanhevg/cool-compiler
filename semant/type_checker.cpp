//
// Created by Evgeny Tanhilevich on 07/12/2015.
//

#include "type_checker.h"
#include "extern_symbols.h"

void TypeChecker::before(class__class *node) {
    Symbol class_name = node->get_name();
    type_env.current_class = type_env.class_table.get_class(class_name);
    type_env.object_env.enterscope();
    type_env.object_env.addid(self, class_name);
    node->traverse_tree(&attribute_resolver);
}

void TypeChecker::after(class__class *node) {
    type_env.object_env.exitscope();
}

void TypeChecker::after(int_const_class* node)  {
    node->set_type(Int);
};
void TypeChecker::after(string_const_class* node)  {
    node->set_type(Str);
};
void TypeChecker::after(bool_const_class* node)  {
    node->set_type(Bool);
};
void TypeChecker::after(object_class* node)  {
    Symbol type = type_env.object_env.lookup(node->get_name());
    if (type == nullptr) {
        semant_error.semant_error(type_env.current_class, node) << "'" << node->get_name() << "' is not declared" << endl;
    }
    node->set_type(type);
};

void TypeChecker::check_arith_type(Binary_Expression_class *node) {
    if (node->get_e1()->get_type() != Int) {
        semant_error.semant_error(type_env.current_class, node->get_e1()) << "Invalid arithmetic expression argument type : '"
        << node->get_e1()->get_type() << "'; expecting 'Int'"  << endl;
    }
    if (node->get_e2()->get_type() != Int) {
        semant_error.semant_error(type_env.current_class, node->get_e2()) << "Invalid arithmetic expression argument type : '"
        << node->get_e2()->get_type() << "'; expecting 'Int'"  << endl;
    }
    node->set_type(Int);
}

void TypeChecker::after(mul_class* node)  {
    check_arith_type(node);
};

void TypeChecker::after(divide_class* node)  {
    check_arith_type(node);
};

void TypeChecker::after(plus_class* node)  {
    check_arith_type(node);
};

void TypeChecker::after(sub_class* node)  {
    check_arith_type(node);
};

void TypeChecker::after(assign_class* node)  {
    Symbol declared_type = type_env.object_env.lookup(node->get_name());
    if (declared_type == nullptr) {
        semant_error.semant_error(type_env.current_class, node) << "'" << node->get_name() << "' is not declared" << endl;
    }
    Symbol expr_type = node->get_expr()->get_type();
    if (!type_env.class_table.is_subtype(expr_type, declared_type)) {
        semant_error.semant_error(type_env.current_class, node->get_expr()) << "Expression type is '" << expr_type
        << "' should be a subtype of '" << declared_type << "'" << endl;
    }
    node->set_type(expr_type);
};

void TypeChecker::after(new__class* node)  {
    Symbol type = node->get_type_name();
    if (type == SELF_TYPE) {
        type = type_env.current_class->get_name();
    }
    node->set_type(type);
};

void TypeChecker::after(no_expr_class *node) {
    node->set_type(No_type);
}

void TypeChecker::before(method_class *node) {
    type_env.object_env.enterscope();
}
void TypeChecker::after(attr_class *node) {
    if (!type_env.class_table.is_subtype(node->get_initializer()->get_type(), node->get_type())) {
        semant_error.semant_error(type_env.current_class, node) << "Attribute '" << node->get_name()
        << "' initialiser type should be a subtipe of the declared type '" << node->get_type()
        << "'; found '" << node->get_initializer()->get_type() << "'" << endl;
    }
}
void TypeChecker::after(method_class *node) {
    type_env.object_env.exitscope();
    Symbol body_type = node->get_body()->get_type();
    Symbol return_type = node->get_return_type();
    if (return_type == SELF_TYPE) {
        return_type = type_env.current_class->get_name();
    }
    if (!type_env.class_table.is_subtype(body_type, return_type)) {
        semant_error.semant_error(type_env.current_class, node) << "Method '" << node->get_name()
        << "' body type should be a subtype of the declared return type '" << return_type
        << "'; found '" << body_type << "'" << endl;
    }
}
void TypeChecker::after(formal_class *node) {
    Symbol addr_type = node->get_type();
    if (addr_type == SELF_TYPE) {
        addr_type = type_env.current_class->get_name();
    }
    Symbol name = node->get_name();
    if (!type_env.object_env.probe_and_add(name, addr_type)) {
        semant_error.semant_error(type_env.current_class, node) << "Formal parameter with name '"
        << name << "' already defined" << endl;
    }
}

void TypeChecker::check_formals(tree_node *node, Symbol name, Symbol callee_type, vector<Symbol> *formals, Expressions actuals) {
    if (formals->size() != actuals->len()) {
        semant_error.semant_error(type_env.current_class, node) << "Mehtod '" << callee_type << "." << name
        <<"' formal parameter count (" << formals->size()
        << ") does not match actual parameter count (" << actuals->len() << ")" << endl;
    }
    for(int i = actuals->first(); actuals->more(i); i = actuals->next(i)) {
        Expression actual = actuals->nth(i);
        if (i == formals->size()) {
            break;
        }
        Symbol formal = formals->at(i);
        if (formal == SELF_TYPE) {
            formal = callee_type;
        }
        if (!type_env.class_table.is_subtype(actual->get_type(), formal)) {
            semant_error.semant_error(type_env.current_class, node) << "Mehtod '" << callee_type << "." << name
            << "' parameter #" << i << " type mismatch. Actual: '"
            << actual->get_type() << "' should be a subtype of '" << formal << "'" << endl;
        }
    }

}

void TypeChecker::after(dispatch_class *node) {
    Symbol callee_type = node->get_callee()->get_type();
    pair<Symbol, vector<Symbol> *> method_signature = type_env.method_env.get_method(callee_type, node->get_name());
    Symbol return_type = method_signature.first;
    if (return_type == nullptr) {
        semant_error.semant_error(type_env.current_class, node) << "No such method: '"
        << callee_type << "." << node->get_name() << "'" << endl;
        return;
    }
    if (return_type == SELF_TYPE) {
        return_type = callee_type;
    }
    Expressions actuals = node->get_actuals();
    vector<Symbol> *formals = method_signature.second;
    check_formals(node, node->get_name(), callee_type, formals, actuals);
    node->set_type(return_type);
}

void TypeChecker::after(static_dispatch_class *node) {
    Symbol callee_type = node->get_callee()->get_type();
    if (!type_env.class_table.is_subtype(callee_type, node->get_type_name())) {
        semant_error.semant_error(type_env.current_class, node) << "Callee type '" << callee_type
        << "' is not a subtype of '" << node->get_type_name() << "'" << endl;
    }
    pair<Symbol, vector<Symbol> *> method_signature = type_env.method_env.get_method(node->get_type_name(), node->get_name());
    Symbol return_type = method_signature.first;
    if (return_type == nullptr) {
        semant_error.semant_error(type_env.current_class, node) << "No such method: '"
        << callee_type << "." << node->get_name() << "'" << endl;
        return;
    }
    if (return_type == SELF_TYPE) {
        return_type = callee_type;
    }
    Expressions actuals = node->get_actuals();
    vector<Symbol> *formals = method_signature.second;
    check_formals(node, node->get_name(), callee_type, formals, actuals);
    node->set_type(return_type);
}

void TypeChecker::after(cond_class *node) {
    if (node->get_predicate()->get_type() != Bool) {
        semant_error.semant_error(type_env.current_class, node) << "Predicate should be of type 'Bool'" << endl;
    }
    vector<Symbol> branch_types;
    branch_types.push_back(node->get_then()->get_type());
    branch_types.push_back(node->get_else()->get_type());
    node->set_type(type_env.class_table.join(branch_types));
}

void TypeChecker::after(block_class *node) {
    Expressions body = node->get_body();
    node->set_type(body->nth(body->len() - 1)->get_type());
}

void TypeChecker::before(let_class *node) {
    type_env.object_env.enterscope();
    Symbol type_decl = node->get_type_decl();
    if (type_decl == SELF_TYPE) {
        type_decl = type_env.current_class->get_name();
    }
    type_env.object_env.addid(node->get_identifier(), type_decl);
}
void TypeChecker::after(let_class *node) {
    type_env.object_env.exitscope();
    Symbol type_decl = node->get_type_decl();
    if (type_decl == SELF_TYPE) {
        type_decl = type_env.current_class->get_name();
    }
    Symbol init_type = node->get_init()->get_type();
    if (!type_env.class_table.is_subtype(init_type, type_decl)) {
        semant_error.semant_error(type_env.current_class, node) << "Let initializer type '" << init_type
        << "' should be a subtype of declared type '" << type_decl << "'" << endl;
    }
    node->set_type(node->get_body()->get_type());
}
void TypeChecker::before(branch_class *node) {
    type_env.object_env.enterscope();
    type_env.object_env.addid(node->get_name(), node->get_type_decl());
}
void TypeChecker::after(branch_class *node) {
    type_env.object_env.exitscope();
}
void TypeChecker::after(typcase_class *node) {
    map<Symbol, int> branch_declared_types;
    vector<Symbol> branch_expression_types;
    Cases cases = node->get_cases();
    for(int i = cases->first(); cases->more(i); i = cases->next(i)) {
        Case case_ = cases->nth(i);
        Symbol declared_type = case_->get_type_decl();
        if (branch_declared_types[declared_type] == 1) {
            semant_error.semant_error(type_env.current_class, case_) << "Branch with type '" << declared_type
            << "' declared multiple times in the same case statement" << endl;
        } else {
            branch_declared_types[declared_type] = 1;
        }
        branch_expression_types.push_back(case_->get_expr()->get_type());
    }
    node->set_type(type_env.class_table.join(branch_expression_types));
}

void TypeChecker::after(loop_class *node) {
    if (node->get_predicate()->get_type() != Bool) {
        semant_error.semant_error(type_env.current_class, node) << "Invalid predicate type: '"
        << node->get_predicate()->get_type() << "'; expecting 'Bool'" << endl;
    }
    node->set_type(Object);
}

void TypeChecker::after(isvoid_class *node) {
    node->set_type(Bool);
}

void TypeChecker::after(comp_class *node) {
    if (node->get_e1()->get_type() != Bool) {
        semant_error.semant_error(type_env.current_class, node->get_e1()) << "Invalid argument type: '"
        << node->get_e1()->get_type() << "'; expecting 'Bool'" << endl;
    }
    node->set_type(Bool);
}

void TypeChecker::check_comparison(Binary_Expression_class *node) {
    if (node->get_e1()->get_type() != Int) {
        semant_error.semant_error(type_env.current_class, node->get_e1()) << "Invalid comparison argument type: '"
        << node->get_e1()->get_type() << "'; expecting 'Int'" << endl;
    }
    if (node->get_e2()->get_type() != Int) {
        semant_error.semant_error(type_env.current_class, node->get_e2()) << "Invalid comparison argument type: '"
        << node->get_e2()->get_type() << "'; expecting 'Int'" << endl;
    }
    node->set_type(Bool);
}

void TypeChecker::after(lt_class *node) {
    check_comparison(node);
}
void TypeChecker::after(leq_class *node) {
    check_comparison(node);
}
void TypeChecker::after(neg_class *node) {
    if (node->get_e1()->get_type() != Int) {
        semant_error.semant_error(type_env.current_class, node->get_e1()) << "Invalid unary minus argument type: '"
        << node->get_e1()->get_type() << "'; expecting 'Int'" << endl;
    }
    node->set_type(Int);
}

void TypeChecker::after(eq_class *node) {
    Symbol type1 = node->get_e1()->get_type();
    Symbol type2 = node->get_e2()->get_type();
    if ((type1 == Int || type1 == Str || type1 == Bool ||
            type2 == Int || type2 == Str || type2 == Bool) &&
        type1 != type2) {
        semant_error.semant_error(type_env.current_class, node) << "Comparison arguments should have the same types; found '"
        << type1 << "' and '" << type2 << "'" << endl;
    }
    node->set_type(Bool);
}
