//
// Created by Evgeny Tanhilevich on 25/02/2016.
//

#include <cgen_gc.h>
#include "cgen_helpers.h"

extern char *gc_collect_names;

ostream & emit_object_header(class__class* cls, int tag, ostream& s) {
    return s;
}

//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

ostream & emit_load(char *dest_reg, int offset, char *source_reg, ostream &s) {
    s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")";
    return s;
}

ostream & emit_store(char *source_reg, int offset, char *dest_reg, ostream &s) {
    s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")";
    return s;
}

static void emit_load_imm(char *dest_reg, int val, ostream &s) { s << LI << dest_reg << " " << val << endl; }

static void emit_load_address(char *dest_reg, char *address, ostream &s) {
    s << LA << dest_reg << " " << address << endl;
}

void emit_partial_load_address(char *dest_reg, ostream &s) { s << LA << dest_reg << " "; }

ostream & emit_move(char *dest_reg, char *source_reg, ostream &s) {
    s << MOVE << dest_reg << " " << source_reg;
    return s;
}

ostream & emit_addiu(char *dest, char *src1, int imm, ostream &s) {
    s << ADDIU << dest << " " << src1 << " " << imm;
    return s;
}

void emit_div(char *dest, char *src1, char *src2, ostream &s) {
    s << DIV << dest << " " << src1 << " " << src2 << endl;
}

void emit_mul(char *dest, char *src1, char *src2, ostream &s) {
    s << MUL << dest << " " << src1 << " " << src2 << endl;
}

void emit_sub(char *dest, char *src1, char *src2, ostream &s) {
    s << SUB << dest << " " << src1 << " " << src2 << endl;
}

static void emit_sll(char *dest, char *src1, int num, ostream &s) {
    s << SLL << dest << " " << src1 << " " << num << endl;
}

static void emit_gc_assign(ostream &s) { s << JAL << "_GenGC_Assign" << endl; }

void emit_disptable_ref(Symbol sym, ostream &s) { s << sym << DISPTAB_SUFFIX; }

void emit_init_ref(Symbol sym, ostream &s) { s << sym << CLASSINIT_SUFFIX; }

ostream &  emit_label_ref(int l, ostream &s) { s << "label" << l; return s; }

ostream &  emit_label_ref(const char *ls, int ln, ostream &s) { s << ls << ln; return s; }

void emit_protobj_ref(Symbol sym, ostream &s) { s << sym << PROTOBJ_SUFFIX; }

void emit_method_ref(Symbol classname, Symbol methodname, ostream &s) {
    s << classname << METHOD_SEP << methodname;
}

static void emit_label_def(int l, ostream &s) {
    emit_label_ref(l, s);
    s << ":" << endl;
}

static void emit_beq(char *src1, char *src2, int label, ostream &s) {
    s << BEQ << src1 << " " << src2 << " ";
    emit_label_ref(label, s);
}

static void emit_bne(char *src1, char *src2, int label, ostream &s) {
    s << BNE << src1 << " " << src2 << " ";
    emit_label_ref(label, s);
}

static void emit_bleq(char *src1, char *src2, int label, ostream &s) {
    s << BLEQ << src1 << " " << src2 << " ";
    emit_label_ref(label, s);
}

static void emit_blt(char *src1, char *src2, int label, ostream &s) {
    s << BLT << src1 << " " << src2 << " ";
    emit_label_ref(label, s);
}

static void emit_blti(char *src1, int imm, int label, ostream &s) {
    s << BLT << src1 << " " << imm << " ";
    emit_label_ref(label, s);
}

static void emit_bgti(char *src1, int imm, int label, ostream &s) {
    s << BGT << src1 << " " << imm << " ";
    emit_label_ref(label, s);
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//
ostream & emit_push(char *reg, ostream &str) {
    emit_store(reg, 0, SP, str) << endl;
    return emit_addiu(SP, SP, -4, str);
}


static void emit_test_collector(ostream &s) {
    emit_push(ACC, s) << endl;
    emit_move(ACC, SP, s) << endl; // stack end
    emit_move(A1, ZERO, s) << endl; // allocate nothing
    s << JAL << gc_collect_names[cgen_Memmgr] << endl;
    emit_addiu(SP, SP, 4, s) << endl;
    emit_load(ACC, 0, SP, s) << endl;
}

static void emit_gc_check(char *source, ostream &s) {
    if (source != (char *) A1) emit_move(A1, source, s) << endl;
    s << JAL << "_gc_check" << endl;
}


