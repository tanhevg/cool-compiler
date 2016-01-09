#include <assert.h>
#include <stdio.h>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

class CodeGenerator {
private:
    int classtag;
    int stringclasstag;
    int intclasstag;
    int boolclasstag;
    ostream &str;

    void code_global_data();
    void code_global_text();
    void code_bools(int);
    void code_select_gc();
    void code_constants();

public:
    CodeGenerator(ostream& _str);
    void code();
};

class BoolConst
{
 private: 
  int val;
 public:
  BoolConst(int);
  void code_def(ostream&, int boolclasstag);
  void code_ref(ostream&) const;
};

