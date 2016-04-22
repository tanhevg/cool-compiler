#include <assert.h>
#include <stdio.h>
#include <ClassTable.h>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"
#include "cgen_helpers.h"


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

    template <typename... Ts> ostream & code_store(ostream &str, int line_no, Ts... logs) {
        return emit_store(ACC, offset, reg, str, line_no, logs...);
    }

    template <typename... Ts> ostream & code_load(ostream &str, int line_no, Ts... logs) {
        return emit_load(ACC, offset, reg, str, line_no, logs...);
    }

};

typedef SymbolTable<Symbol, ObjectEnvRecord> ObjectEnv;

class CodeGenerator: public TreeVisitor {
private:
    ObjectEnv object_env; //todo need to delete the naked pointers
    ClassTable *class_table;
    ostream &str;
    void binary_int_bool_op(Binary_Expression_class *expr, char *opcode, const char *op_string, int n_temp,
                            Symbol result_type);
    void unary_int_bool_op(Unary_Expression_class *expr, char *opcode, const char *op_string, int n_temp,
                               Symbol result_type);
    int condition_count;
    int loop_count;

    /**
     * used for indexing attributes within a class, and formals of a method
     * method arguments are referenced via $fp + index; $fp points to return address;
     * so the first method argument is $fp + 1; so the method arguments index is 1-based:
     * we start with 1, and the number of arguments is last_index - 1
     */
    int scope_index;
    class__class *current_class;
    method_class *current_method;
    void emit_function_entry(int tmp_count, int line_no);
    void emit_function_exit(int tmp_count, int parameter_count, int line_no);
    void dispatch(int line_no, Expression callee, Symbol type, Symbol name, Expressions actuals, int n_temp);
    void code_new(Symbol type_name, int line_no);

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
    void after(formal_class *node);

    void before(program_class *node);

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

};

