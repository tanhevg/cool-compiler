//
// Created by Evgeny Tanhilevich on 07/12/2015.
//

#include <utility>
#include "type_env.h"
#include "extern_symbols.h"


ClassTable::ClassTable(Classes classes, SemantError &_semant_error) : semant_error(_semant_error) {

    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ cls = classes->nth(i);
        add_class(cls);
    }

}

void ClassTable::add_class(Class_ cls) {
    Class_ &old_cls = class_by_name[cls->get_name()];
    if (old_cls != nullptr) {
        semant_error.semant_error(cls) << "Duplicate class" << endl;
        return;
    }
    old_cls = cls;
}

Class_ ClassTable::get_class(Symbol s) {
    return class_by_name[s];
}

Symbol ClassTable::get_parent(Symbol class_name) {
    return get_class(class_name)->get_parent();
}

Symbol ClassTable::join(const vector<Symbol> &types) {
    vector<vector<Symbol>> ancestors;
    for(Symbol s : types) {
        vector<Symbol> v;
        Symbol ss = s;
        while(ss != No_class) {
            v.insert(v.begin(), ss);
            ss = get_class(ss)->get_parent();
        }
        // although vector push_back accepts parameters by reference, it creates copies of values for storing them
        // use std::move to create an rvalue reference, such that move is performed instead of a copy
        ancestors.push_back(move(v));
    }
    int i = 0;
    bool br = false;
    Symbol ret = nullptr, test;
    vector<Symbol>& v0 = ancestors[0];
    do {
        test = v0[i];
        for(vector<Symbol>& v : ancestors) {
            if (i >= v.size() || v[i] != test) {
                br = true;
                break;
            }
        }
        if (!br) {
            ++i;
            ret = test;
        }
    } while (!br && i < v0.size());
    return ret;
}

bool ClassTable::is_subtype(Symbol sub, Symbol super) {
    if (sub == No_type) {
        return true;
    }
    while (sub != No_class) {
        if (sub == super) {
            return true;
        }
        sub = get_class(sub)->get_parent();
    }
    return false;
}


InheritanceChecker::InheritanceChecker(ClassTable &_class_table, SemantError &_semant_error) :
        class_table(_class_table), semant_error(_semant_error) {
    no_inheritance.push_back(Bool);
    no_inheritance.push_back(Int);
    no_inheritance.push_back(Str);
}

void InheritanceChecker::before(class__class *node) {
    if (node->get_name() == Object) {
        return;
    }
    for (Symbol s:no_inheritance) {
        if (node->get_parent() == s) {
            semant_error.semant_error(node) << "Inheritance from '" << s << "' is not allowed" << endl;
            return;
        }
    }
    Class_ parent = node;
    while (parent->get_name() != Object) {
        parent = class_table.get_class(parent->get_parent());
        if (parent->get_name() == node->get_name()) {
            semant_error.semant_error(node) << "Cyclic inheritance" << endl;
            break;
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
    Symbol addr_type = node->get_type();
    Symbol name = node->get_name();
    if (!type_env.object_env.probe_and_add(name, addr_type)) {
        semant_error.semant_error(type_env.current_class)
        << "'" << name << "' overrides attribute with the same name" << endl;
    }
}


void MethodEnv::add_method(MethodSignatureP method_signature) {
    methods[pair<Symbol, Symbol>(method_signature->get_declaring_class(), method_signature->get_name())] =
            method_signature;
}

MethodSignatureP MethodEnv::get_method_no_parents(Symbol call_site_type, Symbol method_name) {
    return methods[pair<Symbol, Symbol>(call_site_type, method_name)];
}

MethodSignatureP MethodEnv::get_method(Symbol call_site_type, Symbol method_name) {
    while(call_site_type != No_class) {
        MethodSignatureP ret = methods[pair<Symbol, Symbol>(call_site_type, method_name)];
        if (ret) {
            return ret;
        }
        call_site_type = class_table.get_parent(call_site_type);
    }
    return nullptr;
}

void MethodResolver::before(method_class *node) {
    formal_types = vector<Symbol>();
}

void MethodResolver::after(method_class *node) {
    type_env.method_env.add_method(make_shared<MethodSignature>(
            MethodSignature(class_name, node->get_name(), node->get_return_type(), move(formal_types))));
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
    MethodSignatureP signature = type_env.method_env.get_method_no_parents(class_name, method_name);
    while (parent != No_class) {
        MethodSignatureP parent_signature = type_env.method_env.get_method_no_parents(parent, method_name);
        if (parent_signature) {
            bool same_signature = true;
            same_signature &= parent_signature->get_return_type() == signature->get_return_type();
            same_signature &= parent_signature->get_parameter_types().size() == signature->get_parameter_types().size();
            if (same_signature) {
                for (unsigned long i = 0; i < signature->get_parameter_types().size(); ++i) {
                    same_signature &= parent_signature->get_parameter_types()[i] == signature->get_parameter_types()[i];
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


