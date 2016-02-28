//
// Created by Evgeny Tanhilevich on 07/12/2015.
//

#include <utility>
#include "type_env.h"
#include "extern_symbols.h"


InheritanceChecker::InheritanceChecker(ClassTable &_class_table, SemantError &_semant_error) :
        class_table(_class_table), semant_error(_semant_error) {
    no_inheritance.push_back(Bool);
    no_inheritance.push_back(Int);
    no_inheritance.push_back(Str);
}

void InheritanceChecker::before(class__class *node) {
    for (Symbol s:no_inheritance) {
        if (node->get_parent() == s) {
            semant_error.semant_error(node) << "Inheritance from '" << s << "' is not allowed" << endl;
            return;
        }
    }
    Class_ parent = node;
    map<Symbol, int> visit_count;
    Symbol parent_name = parent->get_name();
    while (parent_name != Object) {
        parent_name = parent->get_parent();
        parent = class_table.get_class(parent_name);
        if (!parent) {
            semant_error.semant_error(node) << "Parent class '" << parent_name << "' is not declared" << endl;
            break;
        }
        if (visit_count[parent_name] > 0) {
            semant_error.semant_error(parent) << "Cyclic inheritance" << endl;
            break;
        } else {
            ++visit_count[parent_name];
        }
    }
};

void AttributeResolver::before(class__class *node) {
    Symbol parent = node->get_parent();
    if (parent != No_class) {
        type_env.class_table.get_class(parent)->traverse_tree(this);
    }
}

void AttributeResolver::before(attr_class *node) {
    Symbol attr_type = node->get_type();
    Symbol name = node->get_name();
    if (attr_type != prim_slot && !type_env.class_table.get_class(attr_type)) {
        semant_error.semant_error(type_env.current_class, node)
        << "'" << attr_type << "': no class definition for attribute " << name << endl;
        return;
    }
    if (!type_env.object_env.probe_and_add(name, attr_type)) {
        semant_error.semant_error(type_env.current_class)
        << "'" << name << "' overrides attribute with the same name" << endl;
    }
}


void MethodEnv::add_method(Symbol class_name, Symbol method_name, Symbol return_type, vector<Symbol> formal_parameter_types) {
    methods.emplace(make_pair(make_pair(class_name, method_name),
                                   MethodSignature(class_name, method_name, return_type, move(formal_parameter_types))));
}

const MethodSignature& MethodEnv::get_method_no_parents(Symbol call_site_type, Symbol method_name) {
    return methods[make_pair(call_site_type, method_name)];
}

static MethodSignature MISSING_METHOD_SIGNATURE;

const MethodSignature& MethodEnv::get_method(Symbol call_site_type, Symbol method_name) {
    while(call_site_type != No_class) {
        const MethodSignature& ret = methods[make_pair(call_site_type, method_name)];
        if (ret.get_name()) {
            return ret;
        }
        call_site_type = class_table.get_parent(call_site_type);
    }
    return MISSING_METHOD_SIGNATURE;
}

void MethodResolver::before(method_class *node) {
    formal_types = vector<Symbol>();
}

void MethodResolver::after(method_class *node) {
    type_env.method_env.add_method(class_name, node->get_name(), node->get_return_type(), move(formal_types));
}

void MethodResolver::after(formal_class *node) {
    formal_types.push_back(node->get_type());
}

void MethodResolver::before(class__class *node) {
    class_name = node->get_name();
}

void MethodChecker::before(class__class *node) {
    class_name = node->get_name();
}

void MethodChecker::before(method_class *node) {
    Class_ cls = type_env.class_table.get_class(class_name);
    Symbol parent = cls->get_parent();
    Symbol method_name = node->get_name();
    const MethodSignature& signature = type_env.method_env.get_method_no_parents(class_name, method_name);
    while (parent != No_class) {
        const MethodSignature& parent_signature = type_env.method_env.get_method_no_parents(parent, method_name);
        if (parent_signature.get_name()) {
            bool same_signature = true;
            same_signature &= parent_signature.get_return_type() == signature.get_return_type();
            same_signature &= parent_signature.get_parameter_types().size() == signature.get_parameter_types().size();
            if (same_signature) {
                for (unsigned long i = 0; i < signature.get_parameter_types().size(); ++i) {
                    same_signature &= parent_signature.get_parameter_types()[i] == signature.get_parameter_types()[i];
                }
            }
            if (!same_signature) {
                semant_error.semant_error(cls->get_filename(), node)
                << "Method '" << method_name << "' in class " << class_name
                << " overrides another method with the same name and different signature from class " << parent << endl;
            }
        }
        parent = type_env.class_table.get_parent(parent);
    }
}


