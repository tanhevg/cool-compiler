#include <assert.h>
#include <stdio.h>
#include <ClassTable.h>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

class ObjectEnvRecord {
private:
    Symbol declaring_type;
    char *reg;
    int offset;
public:
    ObjectEnvRecord(Symbol _declaring_type, char *_reg, int _offset) :
        declaring_type(_declaring_type),
        reg(_reg),
        offset(_offset)
    {}
    Symbol get_declaring_type() { return declaring_type; }
    char* get_reg() {return reg;}
    int get_offset() {return offset;}
    void code_ref(ostream &_str);
    void code_store(ostream &_str);

};

typedef SymbolTable<Symbol, ObjectEnvRecord> ObjectEnv;

class CodeGenerator: public TreeVisitor {
private:
    ObjectEnv object_env; //todo need to delete the naked pointers
    ostream &str;
    void binary_int_op(Binary_Expression_class *expr, int n_temp);
    void unary_int_op(Unary_Expression_class *expr, int n_temp);
public:
    CodeGenerator(ostream& _str):object_env(), str(_str)
                     {}

    void code(assign_class *expr, int n_temp);
    void code(static_dispatch_class *expr, int n_temp);
    void code(dispatch_class *expr, int n_temp);
    void code(cond_class *expr, int n_temp);
    void code(loop_class *expr, int n_temp);
    void code(typcase_class *expr, int n_temp);
    void code(block_class *expr, int n_temp);
    void code(let_class *expr, int n_temp);
    void code(new__class *expr, int n_temp);
    void code(isvoid_class *expr, int n_temp);
    void code(object_class *expr, int n_temp);
    void code(plus_class *expr, int n_temp);
    void code(sub_class *expr, int n_temp);
    void code(mul_class *expr, int n_temp);
    void code(divide_class *expr, int n_temp);
    void code(neg_class *expr, int n_temp);
    void code(lt_class *expr, int n_temp);
    void code(eq_class *expr, int n_temp);
    void code(leq_class *expr, int n_temp);
    void code(comp_class *expr, int n_temp);
    void code(int_const_class *expr, int n_temp);
    void code(string_const_class *expr, int n_temp);
    void code(bool_const_class *expr, int n_temp);
};

