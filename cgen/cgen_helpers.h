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
void emit_store(char *source_reg, int offset, char *dest_reg, ostream &s);
void emit_load_bool(char *dest, const BoolConst &b, ostream &s);
void emit_load_string(char *dest, StringEntry *str, ostream &s) ;
void emit_load_int(char *dest, IntEntry *i, ostream &s);
void emit_load(char *dest_reg, int offset, char *source_reg, ostream &s);
void emit_add(char *dest, char *src1, char *src2, ostream &s);
void emit_fetch_int(char *dest, char *source, ostream &s);
void emit_store_int(char *source, char *dest, ostream &s);
void emit_div(char *dest, char *src1, char *src2, ostream &s);
void emit_mul(char *dest, char *src1, char *src2, ostream &s);
void emit_sub(char *dest, char *src1, char *src2, ostream &s);
void emit_neg(char *dest, char *src1, ostream &s);
void emit_slt(char *dest, char *src1, char *src2, ostream &s);
void emit_seq(char *dest, char *src1, char *src2, ostream &s);
void emit_sle(char *dest, char *src1, char *src2, ostream &s);
void emit_not(char *dest, char *src1, ostream &s);








#endif //COOL_COMPILER_CGEN_HELPERS_H_H
