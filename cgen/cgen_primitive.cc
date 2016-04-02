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

#include <extern_symbols.h>
#include "emit.h"
#include "cgen_helpers.h"

extern void emit_string_constant(ostream &str, char *s);

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
