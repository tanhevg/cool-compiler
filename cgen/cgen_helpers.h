//
// Created by Evgeny Tanhilevich on 25/02/2016.
//

#ifndef COOL_COMPILER_CGEN_HELPERS_H_H
#define COOL_COMPILER_CGEN_HELPERS_H_H

#include "emit.h"
#include <cool-tree.h>

ostream & emit_object_header(class__class* cls, int tag, ostream& s);

void emit_disptable_ref(Symbol sym, ostream &s);
ostream & emit_label_ref(int l, ostream &s);
ostream & emit_label_ref(const char *ls, int ln, ostream &s);
ostream & emit_label_def(const char *ls, int ln, ostream &s);
void emit_protobj_ref(Symbol sym, ostream &s);
void emit_method_ref(Symbol classname, Symbol methodname, ostream &s);
void emit_init_ref(Symbol sym, ostream &s);
ostream & emit_store(char *source_reg, int offset, char *dest_reg, ostream &s);

ostream & emit_load(char *dest_reg, int offset, char *source_reg, ostream &s);
ostream & emit_load_address(char *dest_reg, const char *address, ostream &s);
ostream & emit_load_imm(char *dest_reg, int val, ostream &s);

ostream & emit_move(char *dest_reg, char *source_reg, ostream &s);

ostream & emit_push(char *reg, ostream &str);
ostream & emit_addiu(char *dest, char *src1, int imm, ostream &s);
ostream & emit_jump(const char *dest, ostream &s);

ostream & emit_bne(const char *src1, const char *src2, const char *ls, int ln, ostream &s);

void emit_pop_fp(ostream &str, int line_no);
void emit_abort_file_line(ostream &str, int line_no, const char *label, const char *comment);
void emit_void_dispatch_check(ostream &str, int line_no, int &label_count);

void emit_partial_load_address(char *dest_reg, ostream &s);

template <typename T> ostream & _comment(ostream &os, T arg) {
    return (os << arg);
}

template <typename T, typename... Ts> ostream &  _comment(ostream &os, T arg, Ts... args) {
    os << arg;
    return _comment(os, args...);
}

template <typename... Ts> ostream & comment(ostream &os, int line_no, Ts... args) {
    os << "\t# " << line_no << ": ";
    _comment(os, args...);
    os << endl;
    return os;
}

template <typename... Ts> ostream & emit_store(char *source_reg, int offset, char *dest_reg, ostream &s, int line_no, Ts... comments) {
    emit_store(source_reg, offset, dest_reg, s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_load(char *dest_reg, int offset, char *source_reg, ostream &s, int line_no, Ts... comments) {
    emit_load(dest_reg, offset, source_reg, s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_load_address(char *dest_reg, const char *address, ostream &s, int line_no, Ts... comments) {
    emit_load_address(dest_reg, address, s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_load_imm(char *dest_reg, int val, ostream &s, int line_no, Ts... comments) {
    emit_load_imm(dest_reg, val, s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_fetch_int(char *dest, char *src, ostream &s, int line_no, Ts... comments) {
    emit_load(dest, DEFAULT_OBJFIELDS, src, s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_new(Symbol sym, ostream &s, int line_no, Ts... comments) {
    emit_partial_load_address(ACC, s);
    s << sym << PROTOBJ_SUFFIX << endl;
    s << JAL << OBJECT_COPY;
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_store_int(char *source, char *dest, ostream &s, int line_no, Ts... comments) {
    emit_store(source, DEFAULT_OBJFIELDS, dest, s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_load_bool(char *dest, const BoolConst &b, ostream &s, int line_no, Ts... comments) {
    emit_partial_load_address(dest, s);
    b.code_ref(s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_load_string(char *dest, StringEntry *str, ostream &s, int line_no, Ts... comments) {
    emit_partial_load_address(dest, s);
    str->code_ref(s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_load_int(char *dest, IntEntry *i, ostream &s, int line_no, Ts... comments) {
    emit_partial_load_address(dest, s);
    i->code_ref(s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_push(char *reg, ostream &str, int line_no, Ts... comments) {
    emit_push(reg, str);
    return comment(str, line_no, comments...);
}

template <typename... Ts> ostream & emit_jalr(char *address, ostream &s, int line_no, Ts... comments) {
    s << JALR << address;
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_beqz(const char *source, const char *ls, int ln, ostream &s, int line_no, Ts... comments) {
    s << BEQZ << source << " ";
    emit_label_ref(ls, ln, s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_beqz(const char *source, const char *address, ostream &s, int line_no, Ts... comments) {
    s << BEQZ << source << " " << address;
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_branch(const char *ls, int ln, ostream &s, int line_no, Ts... comments) {
    s << BRANCH;
    emit_label_ref(ls, ln, s);
    return comment(s, line_no, comments...);
}
template <typename... Ts> ostream & emit_bne(const char *source, const char *dest, const char *ls, int ln, ostream &s, int line_no, Ts... comments) {
    emit_bne(source, dest, ls, ln, s);
    return comment(s, line_no, comments...);
}
template <typename... Ts> ostream &  emit_label_def(const char *ls, int ln, ostream &s, int line_no, Ts... comments) {
    emit_label_ref(ls, ln, s) << ':';
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_move(char *dest_reg, char *source_reg, ostream &s, int line_no, Ts... comments) {
    emit_move(dest_reg, source_reg, s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_seq(char *dest, char *src1, char *src2, ostream &s, int line_no, Ts... comments) {
    s << SEQ << " " << src1 << " " << src2;
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_return(ostream &s, int line_no, Ts... comments) {
    s << RET;
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_addiu(char *dest, char *src1, int imm, ostream &s, int line_no, Ts... comments) {
    emit_addiu(dest, src1, imm, s);
    return comment(s, line_no, comments...);
}

template <typename... Ts> ostream & emit_jump(const char *dest, ostream &s, int line_no, Ts... comments) {
    emit_jump(dest, s);
    return comment(s, line_no, comments...);
}


#endif //COOL_COMPILER_CGEN_HELPERS_H_H
