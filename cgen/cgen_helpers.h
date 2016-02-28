//
// Created by Evgeny Tanhilevich on 25/02/2016.
//

#ifndef COOL_COMPILER_CGEN_HELPERS_H_H
#define COOL_COMPILER_CGEN_HELPERS_H_H

#include <cool-tree.h>

void emit_object_header(class__class* cls, int tag, ostream& s);

void emit_disptable_ref(Symbol sym, ostream &s);
void emit_label_ref(int l, ostream &s);
void emit_protobj_ref(Symbol sym, ostream &s);
void emit_method_ref(Symbol classname, Symbol methodname, ostream &s);
void emit_init_ref(Symbol sym, ostream &s);



#endif //COOL_COMPILER_CGEN_HELPERS_H_H
