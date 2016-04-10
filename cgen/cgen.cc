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
    classtable->index_features();
    int intclasstag = classtable->get_class_tag(Int);
    os << INTTAG << LABEL
    << WORD << intclasstag << endl;
    int boolclasstag = classtable->get_class_tag(Bool);
    os << BOOLTAG << LABEL
    << WORD << boolclasstag << endl;
    int stringclasstag = classtable->get_class_tag(Str);
    os << STRINGTAG << LABEL
    << WORD << stringclasstag << endl;

    os << CLASSNAMETAB << LABEL;
    classtable->visit_classes_ordered_by_tag([&os](Class_ cls) {
        os << WORD;
        stringtable.add_string(cls->get_name()->get_string())->code_ref(os);
        os << endl;
    });

    os << CLASSOBJTAB << LABEL;
    classtable->visit_classes_ordered_by_tag([&os](Class_ cls) {
        os << WORD;
        Symbol class_name = cls->get_name();
        emit_protobj_ref(class_name, os);
        os << endl;
        os << WORD;
        os << class_name << "_init" << endl;
    });

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

    DispTableCoder dispTableCoder(os, classtable);
    traverse_tree(&dispTableCoder);

    TemporariesCounter temporariesCounter;
    traverse_tree(&temporariesCounter);

//                 Add your code to emit
//                   - prototype objects
//                   - class_nameTab
//                   - dispatch tables
//

    if (cgen_debug) cout << "coding global text" << endl;
    code_global_text(os);

    CodeGenerator codeGen(classtable, os);
    traverse_tree(&codeGen);

//                 Add your code to emit
//                   - object initializer
//                   - the class methods
//                   - etc...

    os << "\n# end of generated code\n";
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
//    Symbol boolc = idtable.lookup_string(BOOLNAME);

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
    str << GLOBAL << "_MemMgr_INITIALIZER" << endl;    str << "_MemMgr_INITIALIZER:" << endl;
    str << WORD << gc_init_names[cgen_Memmgr] << endl;
    str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
    str << "_MemMgr_COLLECTOR:" << endl;
    str << WORD << gc_collect_names[cgen_Memmgr] << endl;
    str << GLOBAL << "_MemMgr_TEST" << endl;
    str << "_MemMgr_TEST:" << endl;
    str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
}


