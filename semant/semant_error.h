//
// Created by Evgeny Tanhilevich on 22/11/15.
//

#ifndef COOL_COMPILER_SEMANT_ERROR_H
#define COOL_COMPILER_SEMANT_ERROR_H
#include <iostream>
#include "cool-tree.h"
#include "stringtab.h"


class SemantError {
private:
    int semant_errors;
    ostream &error_stream;
public:
    SemantError() : semant_errors(0), error_stream(cerr) {}
    int errors() { return semant_errors; }
    ostream &semant_error();
    ostream &semant_error(Class_ c);
    ostream &semant_error(Symbol filename, tree_node *t);
    ostream &semant_error(Class_ cls, tree_node *t);
    int check_errors() const;
};



#endif //COOL_COMPILER_SEMANT_ERROR_H
