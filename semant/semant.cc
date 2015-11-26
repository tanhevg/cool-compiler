

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"
#include "inheritance.h"


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
        copy,
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
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}

class InheritanceChecker : public TreeVisitor
{
    ClassTable& class_table;
    SemantError& semant_error;
    vector<Symbol> no_inheritance;
public:
    virtual void before(class__class* node) const override
    {
        if (node->get_name() == Object) {
            return;
        }
        for (Symbol s:no_inheritance) {
            if (node->get_parent() == s) {
                semant_error.semant_error(node) << "Inheritance from " << s << " is not allowed" << endl;
                return;
            }
        }
        Class_ parent = node;
        while(parent->get_name() != Object) {
            parent = class_table.get_class(parent->get_parent());
            if (parent->get_name() == node->get_name()) {
                semant_error.semant_error(node) << "Cyclic inheritance" << endl;
                break;
            }
        }
    };

    InheritanceChecker(ClassTable& _class_table, SemantError& _semant_error):
            class_table(_class_table), semant_error(_semant_error)
    {
        no_inheritance.push_back(Bool);
        no_inheritance.push_back(Int);
        no_inheritance.push_back(Str);
    }
};

class AttributeResolver : public TreeVisitor
{
private:
    TypeEnv& type_env;
    SemantError& semant_error;
//    AttributeResolverContext *context;
public:
    AttributeResolver(TypeEnv& _type_env, SemantError&  _semant_error) :
            type_env(_type_env),
            semant_error(_semant_error)//,
//            context(new AttributeResolverContext())
    {}

    virtual void before(class__class* node) const override
    {
        Symbol parent = node->get_parent();
        if (parent != No_class) {
            type_env.class_table.get_class(parent)->traverse_tree(*this);
        }
    }

    virtual void before(attr_class* node) const override
    {
        Symbol addr_type = node->get_type();
        Symbol name = node->get_name();
        if (!type_env.object_env.probe_and_add(name, &addr_type)) {
            semant_error.semant_error(type_env.current_class)
            << "'" << name << "' overrides attribute with the same name" << endl;
        }
    }

};


class TypeChecker: public TreeVisitor
{
    TypeEnv *type_env;
    SemantError& semant_error;
    AttributeResolver attribute_resolver;
public:
    TypeChecker(TypeEnv *_type_env, SemantError& _semant_error):
            type_env(_type_env),
            semant_error(_semant_error),
            attribute_resolver(*type_env, semant_error)
    { }


    virtual void before(class__class *node) const override
    {
        type_env->current_class = type_env->class_table.get_class(node->get_name());
        type_env->object_env.enterscope();
        node->traverse_tree(attribute_resolver);
    }
    virtual void after(class__class *node) const override
    {
        type_env->object_env.exitscope();
    }
};

class MethodResolver : public TreeVisitor{};

//class AttributeResolverContext
//{
//private:
//    int m_scope_count;
//public:
//    AttributeResolverContext() : m_scope_count(0) {}
//    void inc_scope_count() {++m_scope_count;}
//    void dec_scope_count() {--m_scope_count;}
//    int scope_count() { return m_scope_count;}
//};


ClassTable::ClassTable(Classes classes, SemantError& _semant_error) : semant_error(_semant_error)
{

    install_basic_classes();
    for(int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ cls = classes->nth(i);
        add_class(cls);
//        m_pInheritanceTable->add_node(cls);
    }

}

void ClassTable::add_class(Class_ cls)
{
    Class_ &old_cls = class_by_name[cls->get_name()];
    if (old_cls != nullptr) {
        semant_error.semant_error(cls) << "Duplicate class" << endl;
        return;
    }
    old_cls = cls;
}

Class_ ClassTable::get_class(Symbol s)
{
    return class_by_name[s];
}

bool ClassTable::is_subtype(Symbol sub, Symbol super)
{
    Class_ sub_class = get_class(sub);
    do {
        if (sub_class->get_name() == super) {
            return true;
        }
        sub_class = get_class(sub_class->get_parent());
    } while(sub_class->get_name() != Object);
    return false;
}


void ClassTable::install_basic_classes() {

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

    Class_ Object_class =
	class_(Object,
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class =
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
	       filename);

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int,
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
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
	       filename);

    add_class(Object_class);
    add_class(Bool_class);
    add_class(Str_class);
    add_class(Int_class);
    add_class(IO_class);
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
void program_class::semant()
{
    initialize_constants();

    SemantError semant_error;
//    InheritanceTableP pInheritanceTable = make_shared<InheritanceTable>(InheritanceTable(pSemantError));

    /* ClassTable constructor may do some semantic analysis */
    ClassTable classtable = ClassTable(classes, semant_error);

    semant_error.check_errors();

    traverse_tree(InheritanceChecker(classtable, semant_error));

    semant_error.check_errors();

    ObjectEnv object_env;
    MethodEnv method_env;
    TypeEnv type_env(classtable, object_env, method_env);

    traverse_tree(TypeChecker(&type_env, semant_error));

    semant_error.check_errors();
    /* some semantic analysis code may go here */

}

void program_class::traverse_tree(const TreeVisitor &visitor)
{
    visitor.before(this);
    classes->traverse_tree(visitor);
    visitor.after(this);
}

void class__class::traverse_tree(const TreeVisitor &visitor)
{
    visitor.before(this);
    features->traverse_tree(visitor);
    visitor.after(this);
}
void attr_class::traverse_tree(const TreeVisitor &visitor)
{
    visitor.before(this);
    init->traverse_tree(visitor);
    visitor.after(this);
}
void formal_class::traverse_tree(const TreeVisitor &visitor)
{
    visitor.before(this);
    visitor.after(this);
}
void method_class::traverse_tree(const TreeVisitor &visitor)
{
    visitor.before(this);
    formals->traverse_tree(visitor);
    visitor.after(this);
}

