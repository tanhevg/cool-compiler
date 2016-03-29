//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include "cgen.h"
#include "cgen_gc.h"
#include "extern_symbols.h"
#include "cgen_helpers.h"
#include "PrototypeCoder.h"
#include "TemporariesCounter.h"

extern void emit_string_constant(ostream &str, char *s);

extern int cgen_debug;

IntEntryP zero_int;
StringEntryP empty_string;


char *gc_init_names[] =
        {"_NoGC_Init", "_GenGC_Init", "_ScnGC_Init"};
char *gc_collect_names[] =
        {"_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect"};


//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

static void code_global_text(ostream&);
static void code_global_data(ostream&);
static void code_select_gc(ostream&);
static void code_bools(ostream&, int);

void program_class::cgen(ostream &os) {
    // spim wants comments to start with '#'
    os << "# start of generated code\n";

//    CodeGenerator cg(os, classtable);
    if (cgen_debug) cout << "coding global data" << endl;
    code_global_data(os);

    //
    // We also need to know the tag of the Int, String, and Bool classes
    // during code generation.
    //
    classtable->index_attributes();
    int intclasstag = classtable->get_class_tag(Int);
    os << INTTAG << LABEL
    << WORD << intclasstag << endl;
    int boolclasstag = classtable->get_class_tag(Bool);
    os << BOOLTAG << LABEL
    << WORD << boolclasstag << endl;
    int stringclasstag = classtable->get_class_tag(Str);
    os << STRINGTAG << LABEL
    << WORD << stringclasstag << endl;


    if (cgen_debug) cout << "choosing gc" << endl;
    code_select_gc(os);

    if (cgen_debug) cout << "coding constants" << endl;

    empty_string = stringtable.add_string("");
    zero_int = inttable.add_string("0");

    stringtable.code_string_table(os, stringclasstag);
    inttable.code_string_table(os, intclasstag);
    code_bools(os, boolclasstag);

    PrototypeCoder prototypeCoder(os, classtable);
    traverse_tree(&prototypeCoder);

    TemporariesCounter temporariesCounter;
    traverse_tree(&temporariesCounter);

//                 Add your code to emit
//                   - prototype objects
//                   - class_nameTab
//                   - dispatch tables
//

    if (cgen_debug) cout << "coding global text" << endl;
    code_global_text(os);

//                 Add your code to emit
//                   - object initializer
//                   - the class methods
//                   - etc...

    os << "\n# end of generated code\n";
}



///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(ostream &s) {
    s << STRCONST_PREFIX << index;
}

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(ostream &s, int stringclasstag) {
    IntEntryP lensym = inttable.add_int(len);

    // Add -1 eye catcher
    s << WORD << "-1" << endl;

    code_ref(s);
    s << LABEL                                             // label
    << WORD << stringclasstag << endl                                 // tag
    << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len + 4) / 4) << endl; // size

    s << WORD;
    emit_disptable_ref(Str, s);
    s << endl;
    s << WORD;
    lensym->code_ref(s);
    s << endl;            // string length
    emit_string_constant(s, str);                                // ascii string
    s << ALIGN;                                                 // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//
void StrTable::code_string_table(ostream &s, int stringclasstag) {
    for (List<StringEntry> *l = tbl; l; l = l->tl())
        l->hd()->code_def(s, stringclasstag);
}

//
// Ints
//
void IntEntry::code_ref(ostream &s) {
    s << INTCONST_PREFIX << index;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(ostream &s, int intclasstag) {
    // Add -1 eye catcher
    s << WORD << "-1" << endl;

    code_ref(s);
    s << LABEL                                // label
    << WORD << intclasstag << endl                      // class tag
    << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl;  // object size
    s << WORD;
    emit_disptable_ref(Int, s);
    s << endl;                                          // dispatch table
    s << WORD << str << endl;                           // integer value
}


//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//
void IntTable::code_string_table(ostream &s, int intclasstag) {
    for (List<IntEntry> *l = tbl; l; l = l->tl())
        l->hd()->code_def(s, intclasstag);
}


//
// Bools
//
BoolConst::BoolConst(int i) : val(i) { assert(i == 0 || i == 1); }

void BoolConst::code_ref(ostream &s) const {
    s << BOOLCONST_PREFIX << val;
}

//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(ostream &s, int boolclasstag) {
    // Add -1 eye catcher
    s << WORD << "-1" << endl;

    code_ref(s);
    s << LABEL                                  // label
    << WORD << boolclasstag << endl                       // class tag
    << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl;   // object size
    s << WORD;
    emit_disptable_ref(Bool, s);
    s << endl;                                            // dispatch table
    s << WORD << val << endl;                             // value (0 or 1)
}

static ObjectEnvRecord *stack_entry(Symbol declaring_type, int offset) {
    return new ObjectEnvRecord(declaring_type, FP, offset);
}

static ObjectEnvRecord *heap_entry(Symbol declaring_type, int offset) { // todo use attribute AST node instead of offset?
    return new ObjectEnvRecord(declaring_type, SELF, offset);
}

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void code_global_data(ostream &str) {
    Symbol main = idtable.lookup_string(MAINNAME);
    Symbol string = idtable.lookup_string(STRINGNAME);
    Symbol integer = idtable.lookup_string(INTNAME);
    Symbol boolc = idtable.lookup_string(BOOLNAME);

    str << "\t.data\n" << ALIGN;
    //
    // The following global names must be defined first.
    //
    str << GLOBAL << CLASSNAMETAB << endl;
    str << GLOBAL;
    emit_protobj_ref(main, str);
    str << endl;
    str << GLOBAL;
    emit_protobj_ref(integer, str);
    str << endl;
    str << GLOBAL;
    emit_protobj_ref(string, str);
    str << endl;
    str << GLOBAL;
    falsebool.code_ref(str);
    str << endl;
    str << GLOBAL;
    truebool.code_ref(str);
    str << endl;
    str << GLOBAL << INTTAG << endl;
    str << GLOBAL << BOOLTAG << endl;
    str << GLOBAL << STRINGTAG << endl;

}


//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void code_global_text(ostream &str) {
    str << GLOBAL << HEAP_START << endl
    << HEAP_START << LABEL
    << WORD << 0 << endl
    << "\t.text" << endl
    << GLOBAL;
    emit_init_ref(idtable.add_string("Main"), str);
    str << endl << GLOBAL;
    emit_init_ref(idtable.add_string("Int"), str);
    str << endl << GLOBAL;
    emit_init_ref(idtable.add_string("String"), str);
    str << endl << GLOBAL;
    emit_init_ref(idtable.add_string("Bool"), str);
    str << endl << GLOBAL;
    emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), str);
    str << endl;
}

void code_bools(ostream& str, int boolclasstag) {
    falsebool.code_def(str, boolclasstag);
    truebool.code_def(str, boolclasstag);
}

void code_select_gc(ostream &str) {
    //
    // Generate GC choice constants (pointers to GC functions)
    //
    str << GLOBAL << "_MemMgr_INITIALIZER" << endl;
    str << "_MemMgr_INITIALIZER:" << endl;
    str << WORD << gc_init_names[cgen_Memmgr] << endl;
    str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
    str << "_MemMgr_COLLECTOR:" << endl;
    str << WORD << gc_collect_names[cgen_Memmgr] << endl;
    str << GLOBAL << "_MemMgr_TEST" << endl;
    str << "_MemMgr_TEST:" << endl;
    str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
}


//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

void assign_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void static_dispatch_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void dispatch_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void cond_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void loop_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void typcase_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void block_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void let_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

/**
 * Operators dealing with Int's and Bool's deal with them in object form.
 * Temporary objects are created on heap for result of each expression
 * Expression result in $a0 is the address of the newly created object on the heap
 */
void CodeGenerator::binary_int_op(Binary_Expression_class *expr, char *opcode, int n_temp, Symbol result_type) {
    expr->get_e1()->code(this, n_temp);
    emit_store(ACC, n_temp, FP, str);
    expr->get_e2()->code(this, n_temp + 1);
    emit_load(T1, n_temp, FP, str);
    // Both operands come in object form, therefore we need to fetch their values into respective registers to proceed
    emit_fetch_int(T1, T1, str);
    emit_fetch_int(T2, ACC, str);
    str << opcode << T1 << " " << T1 << " " << T2 << endl;
    emit_new(result_type, str); // create (new) the result on the heap; its address is now in $a0
    emit_store_int(T1, ACC, str); // NB: this works for both ints and bools
}

void CodeGenerator::unary_int_op(Unary_Expression_class *expr, char *opcode, int n_temp) {
    expr->get_e1()->code(this, n_temp);
    emit_fetch_int(T1, ACC, str);
    str << opcode << T1 << " " << T1 << endl;
    emit_new(Int, str);
    emit_store_int(T1, ACC, str);
}

void CodeGenerator::code(plus_class *expr, int n_temp) {
    binary_int_op(expr, ADD, n_temp, Int);
}

void plus_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(sub_class *expr, int n_temp) {
    binary_int_op(expr, SUB, n_temp, Int);
}

void sub_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(mul_class *expr, int n_temp) {
    binary_int_op(expr, MUL, n_temp, Int);
}

void mul_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(divide_class *expr, int n_temp) {
    binary_int_op(expr, DIV, n_temp, Int);
}

void divide_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(neg_class *expr, int n_temp) {
    unary_int_op(expr, NEG, n_temp);
}

void neg_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(lt_class *expr, int n_temp) {
    binary_int_op(expr, SLT, n_temp, Bool);
}

void lt_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(eq_class *expr, int n_temp) {
    binary_int_op(expr, SEQ, n_temp, Bool);
}

void eq_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(leq_class *expr, int n_temp) {
    binary_int_op(expr, SLE, n_temp, Bool);
}

void leq_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(comp_class *expr, int n_temp) {
    unary_int_op(expr, NOT, n_temp);
}

void comp_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(int_const_class *expr, int n_temp) {
    //
    // Need to be sure we have an IntEntry , not an arbitrary Symbol
    //
    emit_load_int(ACC, inttable.lookup_string(expr->get_token()->get_string()), str);
}

void int_const_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(string_const_class *expr, int n_temp) {
    emit_load_string(ACC, stringtable.lookup_string(expr->get_token()->get_string()), str);
}

void string_const_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(bool_const_class *expr, int n_temp) {
    emit_load_bool(ACC, BoolConst(expr->get_val()), str);
}

void bool_const_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void new__class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void isvoid_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void no_expr_class::code(CodeGenerator *cgen, int n_temp) {
}

void object_class::code(CodeGenerator *cgen, int n_temp) {
    cgen->code(this, n_temp);
}

void CodeGenerator::code(assign_class *expr, int n_temp) {
//todo
}
void CodeGenerator::code(static_dispatch_class *expr, int n_temp) {
//todo
}
void CodeGenerator::code(dispatch_class *expr, int n_temp) {
    emit_push(FP,str);
    Expressions actuals = expr->get_actuals();
    for (int i = 0; i < actuals->len(); i++) {
        actuals->nth(actuals->len() - i - 1)->code(this, n_temp);
        emit_push(ACC, str);
    }
    Expression callee = expr->get_callee();
    callee->code(this, n_temp);

//todo
}
void CodeGenerator::code(cond_class *expr, int n_temp) {
//todo
}
void CodeGenerator::code(loop_class *expr, int n_temp) {
//todo
}
void CodeGenerator::code(typcase_class *expr, int n_temp) {
//todo
}
void CodeGenerator::code(block_class *expr, int n_temp) {
    Expressions body = expr->get_body();
    for (int i = 0; i < body->len(); i++) { // todo revisit
        body->nth(i)->code(this, n_temp);
    }
}
void CodeGenerator::code(let_class *expr, int n_temp) {
//todo
}
void CodeGenerator::code(new__class *expr, int n_temp) {
//todo
}
void CodeGenerator::code(isvoid_class *expr, int n_temp) {
//todo
}
void CodeGenerator::code(object_class *expr, int n_temp) {
}


void ObjectEnvRecord::code_ref(ostream &str) {
    str << offset * WORD_SIZE << "(" << reg << ")";
}

void ObjectEnvRecord::code_store(ostream &str) {
    emit_store(ACC, offset, reg, str);
}

void CodeGenerator::before(class__class *node) {
    object_env.enterscope();
    scope_index = DEFAULT_OBJFIELDS + 1;
}

void CodeGenerator::after(class__class *node) {
    object_env.exitscope();
}

void CodeGenerator::before(method_class *node) {
    object_env.enterscope();
    scope_index = 1;
}

void CodeGenerator::after(method_class *node) {
    emit_move(FP, SP, str);
    emit_push(RA, str);
    emit_push(SELF, str);
    emit_move(SELF, ACC, str);
    Expression body = node->get_body();
    int tmp_count = body->get_temporaries_count();
    emit_addiu(SP, SP, -4 * tmp_count, str);

    // $fp points at return address; immediatelly below it is the saved previous self
    // therefore the space for the first available temporary is 2 words below $fp
    body->code(this, 2);
    emit_load(RA, 0, FP, str);
    emit_load(SELF, -4, FP, str);
    int frame_size = 4 * (scope_index - 1) +  // parameters
            4 * tmp_count +
            8; // return address and old self
    emit_addiu(SP, SP, frame_size, str);
    emit_load(FP, 0, SP, str);
    emit_return(str);
    object_env.exitscope();
}

void CodeGenerator::after(formal_class *node) {
    object_env.addid(node->get_name(), stack_entry(node->get_type(), scope_index++));
}

void CodeGenerator::after(attr_class *node) {
    object_env.addid(node->get_name(), heap_entry(node->get_type(), scope_index++));
}
