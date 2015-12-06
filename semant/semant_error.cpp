//
// Created by Evgeny Tanhilevich on 22/11/15.
//

#include "semant_error.h"



////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& SemantError::semant_error(Class_ c)
{
    return semant_error(c->get_filename(),c) << "class " << c->get_name() << ": ";
}

ostream& SemantError::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& SemantError::semant_error(Class_ cls, tree_node *t)
{
    return semant_error(cls->get_filename(), t);
}

ostream& SemantError::semant_error()
{
    semant_errors++;
    return error_stream;
}

void SemantError::check_errors() const {
    if (semant_errors) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
}



