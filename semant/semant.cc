#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"

extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////

static Symbol
        arg,
        arg2,
        Bool,
        concat,
        cool_abort,
        copy_,
        Int,
        in_int,
        in_string,
        IO,
        length,
        Main,
        main_meth,
        No_class,
        No_type,
        Object,
        out_int,
        out_string,
        prim_slot,
        self,
        SELF_TYPE,
        Str,
        str_field,
        substr,
        type_name,
        val;

//
// Initializing the predefined symbols.
//
static void initialize_constants(void) {
    arg = idtable.add_string("arg");
    arg2 = idtable.add_string("arg2");
    Bool = idtable.add_string("Bool");
    concat = idtable.add_string("concat");
    cool_abort = idtable.add_string("abort");
    copy_ = idtable.add_string("copy");
    Int = idtable.add_string("Int");
    in_int = idtable.add_string("in_int");
    in_string = idtable.add_string("in_string");
    IO = idtable.add_string("IO");
    length = idtable.add_string("length");
    Main = idtable.add_string("Main");
    main_meth = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    // no need to do any specific checking for these symbols because the lexical structure does not permit
    // symbols starting with underscore
    No_class = idtable.add_string("_No_Class");
    No_type = idtable.add_string("_No_Type");

    Object = idtable.add_string("Object");
    out_int = idtable.add_string("out_int");
    out_string = idtable.add_string("out_string");
    prim_slot = idtable.add_string("_prim_slot");
    self = idtable.add_string("self");
    SELF_TYPE = idtable.add_string("SELF_TYPE");
    Str = idtable.add_string("String");
    str_field = idtable.add_string("_str_field");
    substr = idtable.add_string("substr");
    type_name = idtable.add_string("type_name");
    val = idtable.add_string("_val");
}


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


void program_class::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
    // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.

    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    classes = append_Classes(classes, single_Classes(
            class_(Object,
                   No_class,
                   append_Features(
                           append_Features(
                                   single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
                                   single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
                           single_Features(method(copy_, nil_Formals(), SELF_TYPE, no_expr()))),
                   filename)));

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    classes = append_Classes(classes, single_Classes(
            class_(IO,
                   Object,
                   append_Features(
                           append_Features(
                                   append_Features(
                                           single_Features(method(out_string, single_Formals(formal(arg, Str)),
                                                                  SELF_TYPE, no_expr())),
                                           single_Features(method(out_int, single_Formals(formal(arg, Int)),
                                                                  SELF_TYPE, no_expr()))),
                                   single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
                           single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
                   filename)));

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    classes = append_Classes(classes, single_Classes(
            class_(Int,
                   Object,
                   single_Features(attr(val, prim_slot, no_expr())),
                   filename)));

    //
    // Bool also has only the "val" slot.
    //
    classes = append_Classes(classes, single_Classes(
            class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())), filename)));

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    classes = append_Classes(classes, single_Classes(
            class_(Str,
                   Object,
                   append_Features(
                           append_Features(
                                   append_Features(
                                           append_Features(
                                                   single_Features(attr(val, Int, no_expr())),
                                                   single_Features(attr(str_field, prim_slot, no_expr()))),
                                           single_Features(method(length, nil_Formals(), Int, no_expr()))),
                                   single_Features(method(concat,
                                                          single_Formals(formal(arg, Str)),
                                                          Str,
                                                          no_expr()))),
                           single_Features(method(substr,
                                                  append_Formals(single_Formals(formal(arg, Int)),
                                                                 single_Formals(formal(arg2, Int))),
                                                  Str,
                                                  no_expr()))),
                   filename)));
}


/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant() {
    initialize_constants();
    install_basic_classes();

    SemantError semant_error;
//    InheritanceTableP pInheritanceTable = make_shared<InheritanceTable>(InheritanceTable(pSemantError));

    /* ClassTable constructor may do some semantic analysis */
    ClassTable classtable = ClassTable(classes, semant_error);

    if (semant_error.check_errors()) return;

    InheritanceChecker inheritance_checker = InheritanceChecker(classtable, semant_error);
    traverse_tree(&inheritance_checker);

    if (semant_error.check_errors()) return;

    ObjectEnv object_env;
    MethodEnv method_env(classtable);
    TypeEnv type_env(classtable, object_env, method_env);

    MethodResolver method_resolver(type_env);
    traverse_tree(&method_resolver);

    MethodChecker method_checker(type_env, semant_error);
    traverse_tree(&method_checker);

    if (semant_error.check_errors()) return;

    TypeChecker type_checker = TypeChecker(type_env, semant_error);
    traverse_tree(&type_checker);

    if (semant_error.check_errors()) return;
    /* some semantic analysis code may go here */

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
    if ((type1 == Int || type1 == Str || type1 == Bool || type2 == Int || type2 == Str || type2 == Bool) &&
            type1 != type2) {
        semant_error.semant_error(type_env.current_class, node) << "Comparison arguments should have the same types; found '"
        << type1 << "' and '" << type2 << "'" << endl;
    }
    node->set_type(Bool);
}


void MethodEnv::add_method(Symbol call_site_type, Symbol method_name, Symbol return_type,
                           vector<Symbol> *argument_types) {
    methods[pair<Symbol, Symbol>(call_site_type, method_name)] = pair<Symbol, vector<Symbol>*>(return_type,
                                                                                              argument_types);
}

pair<Symbol, vector<Symbol> *> MethodEnv::get_method_no_parents(Symbol call_site_type, Symbol method_name) {
    return methods[pair<Symbol, Symbol>(call_site_type, method_name)];
}

pair<Symbol, vector<Symbol> *> MethodEnv::get_method(Symbol call_site_type, Symbol method_name) {
    pair<Symbol, vector<Symbol> *> ret = methods[pair<Symbol, Symbol>(call_site_type, method_name)];
    if (ret.first != nullptr) {
        return ret;
    }
    Symbol parent_name = class_table.get_class(call_site_type)->get_parent();
    while(ret.first == nullptr && parent_name != No_class) {
        ret = methods[pair<Symbol, Symbol>(parent_name, method_name)];
        if (ret.first != nullptr) {
            return ret;
        }
        parent_name = class_table.get_parent(parent_name);
    }
    return pair<Symbol, vector<Symbol> *>(nullptr, nullptr);
}

void MethodResolver::before(method_class *node) {
    formal_types = new vector<Symbol>(0);
}

void MethodResolver::after(method_class *node) {
    type_env.method_env.add_method(class_name, node->get_name(), node->get_return_type(), formal_types);
}

void MethodResolver::after(formal_class *node) {
    formal_types->push_back(node->get_type());
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
    pair<Symbol, vector<Symbol>*> signature = type_env.method_env.get_method_no_parents(class_name, method_name);
    while (parent != No_class) {
        pair<Symbol, vector<Symbol> *> parent_signature = type_env.method_env.get_method_no_parents(parent, method_name);
        if (parent_signature.first != nullptr) {
            bool same_signature = true;
            same_signature &= parent_signature.first == signature.first;
            same_signature &= parent_signature.second->size() == signature.second->size();
            if (same_signature) {
                for (unsigned long i = 0; i < signature.second->size(); ++i) {
                    same_signature &= parent_signature.second->at(i) == signature.second->at(i);
                }
            }
            if (!same_signature) {
                semant_error.semant_error(cls->get_filename(), node)
                << "Method '" << method_name << "' in class " << class_name
                << " overrides another method with same name and different signature from class " << parent << endl;
            }
        }
        parent = type_env.class_table.get_parent(parent);
    }
}
