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
    void code_ref(ostream &str);
    void code_store(ostream &str);

    void code_load(ostream &str);
};

typedef SymbolTable<Symbol, ObjectEnvRecord> ObjectEnv;

class CodeGenerator: public TreeVisitor {
private:
    ObjectEnv object_env; //todo need to delete the naked pointers
    ClassTable *class_table;
    ostream &str;
    void binary_int_op(Binary_Expression_class *expr, char *opcode, int n_temp, Symbol result_type);
    void unary_int_op(Unary_Expression_class *expr, char *opcode, int n_temp);
    int condition_count;
    int loop_count;
    int scope_index; // used for indexing attributes within a class, and formals of a method
    class__class *current_class;
    method_class *current_method;
public:
    CodeGenerator(ClassTable *_class_table, ostream& _str):
            class_table(_class_table), object_env(), str(_str),
            condition_count(0), loop_count(0), scope_index(0),
            current_class(nullptr), current_method(nullptr)
            {}

    void before(class__class *node);
    void after(class__class *node);

    void before(method_class *node);
    void after(method_class *node);

//    void after(attr_class *node);
    void after(formal_class *node);

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
    void code(no_expr_class *expr, int n_temp);

    void before(program_class *node);
};

