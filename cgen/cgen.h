#include <assert.h>
#include <stdio.h>
#include <ClassTable.h>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0
/*
class CodeGenerator {
private:
    int classtag;
    int stringclasstag;
    int intclasstag;
    int boolclasstag;
    ostream &str;

    ClassTable* classTable;

    void code_global_data();
    void code_global_text();
    void code_bools(int);
    void code_select_gc();
    void code_constants();
    void code_prototypes();

public:
    CodeGenerator(ostream &_str, ClassTable *pTable);
    void code();

};
 */


