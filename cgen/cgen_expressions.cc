#include <cool-tree.h>
#include <extern_symbols.h>
#include <string>
#include "cgen.h"
#include "cgen_helpers.h"

using std::string;


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wwritable-strings"
static ObjectEnvRecord *stack_entry(Symbol declaring_type, int offset) {
    return new ObjectEnvRecord(declaring_type, FP, offset);
}

static ObjectEnvRecord *heap_entry(Symbol declaring_type,
                                   int offset) { // todo use attribute AST node instead of offset?
    return new ObjectEnvRecord(declaring_type, SELF, offset);
}


/**
 * Operators dealing with Int's and Bool's deal with them in object form.
 * Temporary objects are created on heap for result of each expression
 * Expression result in $a0 is the address of the newly created object on the heap
 */
void CodeGenerator::binary_int_bool_op(Binary_Expression_class *expr, char *opcode, const char *op_string, int n_temp,
                                       Symbol result_type) {
    expr->get_e1()->code(this, n_temp);
    emit_store(ACC, -n_temp, FP, str)                << "\t# store address of the first operand of "
                                                    << op_string << " in temporary " << n_temp << endl;
    expr->get_e2()->code(this, n_temp + 1);
    emit_load(T1, -n_temp, FP, str)                  << "\t# load address of the first operand result of "
                                                    << op_string << " from temporary " << n_temp << endl;
    // Both operands come in object form, therefore we need to fetch their values into respective registers to proceed
    emit_fetch_int(T1, T1, str)                     << "\t# fetch first operand value" << endl;
    emit_fetch_int(T2, ACC, str)                    << "\t# fetch second operand value" << endl;
    str << opcode << T5 << " " << T1 << " " << T2   << "\t# code " << op_string
                                                    << "; use $t5 because $t1-$t4 are clobbered by Object.copy" << endl;
    emit_new(result_type, str)                      << "\t# create (new) the result of " << op_string << " on the heap" << endl;
    emit_store_int(T5, ACC, str)                    << "\t# store the " << op_string << " value" << endl;
}

void CodeGenerator::unary_int_bool_op(Unary_Expression_class *expr, char *opcode, const char *op_string, int n_temp,
                                      Symbol result_type) {
    expr->get_e1()->code(this, n_temp);
    emit_fetch_int(T1, ACC, str)        << "\t# fetch value of first operand of " << op_string << endl;
    str << opcode << T5 << " " << T1    << "\t# code " << op_string
                                        << "; use $t5 because $t1-$t4 are clobbered by Object.copy" << endl;;
    emit_new(result_type, str)          << "\t# create (new) the result of " << op_string << " on the heap" << endl;
    emit_store_int(T5, ACC, str)        << "\t# store the " << op_string << " value" << endl;
}

void CodeGenerator::code(plus_class *expr, int n_temp) {
    binary_int_bool_op(expr, ADD, "'+'", n_temp, Int);
}

void CodeGenerator::code(sub_class *expr, int n_temp) {
    binary_int_bool_op(expr, SUB, "'-'", n_temp, Int);
}

void CodeGenerator::code(mul_class *expr, int n_temp) {
    binary_int_bool_op(expr, MUL, "'*'", n_temp, Int);
}

void CodeGenerator::code(divide_class *expr, int n_temp) {
    binary_int_bool_op(expr, DIV, "'/'", n_temp, Int);
}

void CodeGenerator::code(neg_class *expr, int n_temp) {
    unary_int_bool_op(expr, NEG, "'~'", n_temp, Int);
}

void CodeGenerator::code(lt_class *expr, int n_temp) {
    binary_int_bool_op(expr, SLT, "'<'", n_temp, Bool);
}

void CodeGenerator::code(eq_class *expr, int n_temp) {
    binary_int_bool_op(expr, SEQ, "'='", n_temp, Bool);
}

void CodeGenerator::code(leq_class *expr, int n_temp) {
    binary_int_bool_op(expr, SLE, "'<='", n_temp, Bool);
}

void CodeGenerator::code(comp_class *expr, int n_temp) {
    expr->get_e1()->code(this, n_temp);
    emit_fetch_int(T1, ACC, str)        << "\t# fetch value of first operand of 'not'" << endl;
    str << XORI << T5 << " " << T1 << " 1" << "\t# code 'not'"
                                        << "; use $t5 because $t1-$t4 are clobbered by Object.copy" << endl;;
    emit_new(Bool, str)                 << "\t# create (new) the result of 'not' on the heap" << endl;
    emit_store_int(T5, ACC, str)        << "\t# store the 'not' value" << endl;
    //unary_int_bool_op(expr, NOT, "'not'", n_temp, Bool);
}

void CodeGenerator::code(int_const_class *expr, int n_temp) {
    emit_load_int(ACC, inttable.lookup_string(expr->get_token()->get_string()), str)
    << "\t# load int const " << expr->get_token() << endl;
}

void CodeGenerator::code(string_const_class *expr, int n_temp) {
    emit_load_string(ACC, stringtable.lookup_string(expr->get_token()->get_string()), str)
    << "\t# load string const " << expr->get_token() << endl;
}

void CodeGenerator::code(bool_const_class *expr, int n_temp) {
    emit_load_bool(ACC, BoolConst(expr->get_val()), str)        << "\t# load boolean const" << endl;
}

void CodeGenerator::code(assign_class *expr, int n_temp) {
    expr->get_expr()->code(this, n_temp);
    object_env.lookup(expr->get_name())->code_store(str)        << "\t # assign to " << expr->get_name() << endl;
}

void CodeGenerator::dispatch(Expression callee, Symbol type, Symbol name, Expressions actuals, int n_temp) {
    emit_push(FP, str)                          << "\t# store old frame pointer before calling " << type << '.' << name << endl;
    for (int i = 0; i < actuals->len(); i++) {
        int idx = actuals->len() - i - 1;
        actuals->nth(idx)->code(this, n_temp);
        emit_push(ACC, str)                     << "\t# push actual parameter #" << idx << " of " << type << '.' << name << endl;
    }
    callee->code(this, n_temp);
    // todo runtime error if callee evaluates to void
    emit_load(T1, 2, ACC, str)                  << "\t# load pointer to dispatch table" << endl;
    int method_offset = class_table->get_method_offset(type, name);
    if (method_offset > 0) {
        emit_load(T1, method_offset, T1, str)   << "\t# load address of " << type << '.' << name << endl;
    }
    emit_jalr(T1, str)                          << "\t# call " << type << '.' << name << endl;

}

void CodeGenerator::code(static_dispatch_class *expr, int n_temp) {
    dispatch(expr->get_callee(), expr->get_type_name(), expr->get_name(), expr->get_actuals(), n_temp);
}

void CodeGenerator::code(dispatch_class *expr, int n_temp) {
    dispatch(expr->get_callee(), expr->get_callee()->get_type(), expr->get_name(), expr->get_actuals(), n_temp);
}

void CodeGenerator::code(cond_class *expr, int n_temp) {
    expr->get_predicate()->code(this, n_temp);
    emit_fetch_int(ACC, ACC, str)                           << "\t# conditional predicate: fetch boolean value from boolean object" << endl;
    str << BEQZ << ACC << " " << "false" << condition_count << "\t# branch to false lable if predicate is zero" << endl;
    expr->get_then()->code(this, n_temp);
    str << BRANCH << "condition_end" << condition_count     << "\t# jump to the end of the conditional after executing true (then) branch" << endl;
    str << "false" << condition_count << ':' << endl;
    expr->get_else()->code(this, n_temp);
    str << "condition_end" << condition_count << ':' << endl;
    condition_count++;
}

void CodeGenerator::code(loop_class *expr, int n_temp) {
    str << "loop_start" << loop_count << ':' << endl;
    expr->get_predicate()->code(this, n_temp);
    emit_fetch_int(ACC, ACC, str)    << "\t# loop predicate: fetch boolean value from boolean object" << endl;
    str << BEQZ << ACC << " " << "loop_end" << loop_count << endl;
    expr->get_body()->code(this, n_temp);
    str << BRANCH << "loop_start" << loop_count << endl;
    str << "loop_end" << loop_count << ':' << endl;
    emit_move(ACC, ZERO, str)        << "\t# while returns void" << endl;
    loop_count++;
}

void CodeGenerator::code(typcase_class *expr, int n_temp) {
//todo
}

void CodeGenerator::code(block_class *block, int n_temp) {
    Expressions body = block->get_body();
    body->traverse([this, n_temp](Expression expr) {
        expr->code(this, n_temp);
    });
}

void CodeGenerator::code(let_class *expr, int n_temp) {
    if (expr->get_init()->is_empty()) {
        code_new(expr->get_type_decl());
    } else {
        expr->get_init()->code(this, n_temp);
    }
    object_env.enterscope();
    ObjectEnvRecord *temp_record = stack_entry(expr->get_type_decl(), -n_temp);
    object_env.addid(expr->get_identifier(), temp_record);
    temp_record->code_store(str) << "\t# store the address returned by the initaliser in temp #" << n_temp << endl;
    expr->get_body()->code(this, n_temp+1);
    object_env.exitscope();
}

void CodeGenerator::code(new__class *expr, int n_temp) {
    code_new(expr->get_type_name());
}

void CodeGenerator::code_new(Symbol type_name) {
    emit_new(type_name, str) << "\t# new " << type_name << "()" << endl;
    emit_push(FP, str) << "\t# store old frame pointer before calling new "
    << type_name << "()" << endl;
    str << "\tjal\t" << type_name << "_init" << "\t# jump to initializer of " << type_name << endl;
}

void CodeGenerator::code(isvoid_class *expr, int n_temp) {
    expr->get_e1()->code(this, n_temp);
    str << SEQ << T5 << " " << ACC << " " << ZERO   << "\t# set $t5 if the result of 'isvoid' body is 0 ($zero)"
                                                    << "; use $t5 because $t1-$t4 are clobbered by Object.copy" << endl;
    emit_new(Bool, str)                             << "\t# create (new) the result of 'isvoid' on the heap" << endl;
    emit_store_int(T5, ACC, str)                    << "\t# store the 'isvoid' value" << endl;
}

void CodeGenerator::code(object_class *expr, int n_temp) {
    if (self == expr->get_name()) { //todo ugly if
        emit_move(ACC, SELF, str)                               << "\t# object " << expr->get_name() << endl;
    } else {
        object_env.lookup(expr->get_name())->code_load(str)     << "\t# object " << expr->get_name() << endl;
    }
}


void ObjectEnvRecord::code_ref(ostream &str) {
    str << offset * WORD_SIZE << "(" << reg << ")";
}

ostream & ObjectEnvRecord::code_store(ostream &str) {
    return emit_store(ACC, offset, reg, str);
}

ostream & ObjectEnvRecord::code_load(ostream &str) {
    return emit_load(ACC, offset, reg, str);
}

void CodeGenerator::before(program_class *node) {
    object_env.enterscope();
    object_env.addid(self, heap_entry(SELF_TYPE, 0));
}

void CodeGenerator::before(class__class *node) {
    object_env.enterscope();
    scope_index = DEFAULT_OBJFIELDS + 1;
    class_table->visit_ordered_attrs_of_class(node->get_name(),
        [this](IndexedRecord<attr_class> *ar) {
            object_env.addid(ar->get_ref()->get_name(), heap_entry(ar->get_ref()->get_type(), scope_index++));
        }
    );
    current_class = node;
}

void CodeGenerator::emit_function_entry(int tmp_count) {
    string name;
    if (current_method) {
        name = string(current_class->get_name()->get_string()) + string(".") + string(current_method->get_name()->get_string());
    } else {
        name = string(current_class->get_name()->get_string()) + string("_init");
    }
    emit_move(FP, SP, str)      << "\t# set up stack pointer for " << name << endl;
    emit_push(RA, str)          << endl;
    emit_push(SELF, str)        << endl;
    emit_move(SELF, ACC, str)   << endl;
    if (tmp_count > 0) {
        emit_addiu(SP, SP, -4 * tmp_count, str)     << "\t# push " << tmp_count << " temporaries" << endl;
    }

}

void CodeGenerator::emit_function_exit(int tmp_count, int parameter_count) {
    string name;
    if (current_method) {
        name = string(current_class->get_name()->get_string()) + string(".") + string(current_method->get_name()->get_string());
    } else {
        name = string(current_class->get_name()->get_string()) + string("_init");
    }
    emit_load(RA, 0, FP, str)               << "\t# exit from " << name << ": load return address" << endl;
    emit_load(SELF, -1, FP, str)            << "\t# restore previous self" << endl;;
    int frame_size = 4 * parameter_count +
                     4 * tmp_count +
                     8 + // return address and old self
                     4; // old fp
    emit_addiu(SP, SP, frame_size, str)     << "\t# pop the frame" << endl;
    emit_load(FP, 0, SP, str)               << "\t# restore previous $fp" << endl;
    emit_return(str)                        << "#\treturn from " << name << endl;

}

void CodeGenerator::after(class__class *node) {
    current_method = nullptr;
    int tmp_count = 0;
    bool is_empty = true;
    class_table->visit_ordered_attrs_of_class(current_class->get_name(), [&tmp_count, &is_empty](AttrRecord *ar) {
        if (ar->get_ref()->get_temporaries_count() > tmp_count) {
            tmp_count = ar->get_ref()->get_temporaries_count();
        }
        is_empty &= ar->get_ref()->get_initializer()->is_empty();
    });
    str << current_class->get_name() << "_init:" << endl;
    if (is_empty) {
        emit_return(str)    << "\t# all attribute initializers for " << node->get_name() << " are empty; return straight away" << endl;
        return;
    }
    emit_function_entry(tmp_count);
    class_table->visit_ordered_attrs_of_class(current_class->get_name(), [this](IndexedRecord<attr_class> *ar) {
        if (ar->get_ref()->get_initializer()->is_empty()) {
            return;
        }
        // $fp points at return address; immediatelly below it is the saved previous self
        // therefore the space for the first available temporary is 2 words below $fp
        Symbol name = ar->get_ref()->get_name();
        ar->get_ref()->get_initializer()->code(this, 2);
        object_env.lookup(name)->code_store(str) << "\t# assign initial value to " << name << endl;
    });
    emit_move(ACC, SELF, str) << "\t# restore self in $a0" << endl;
    emit_function_exit(tmp_count, 0);
    object_env.exitscope();
}

void CodeGenerator::before(method_class *node) {
    if (node->is_implemented()) {
        return;
    }
    object_env.enterscope();
    scope_index = 1;
    current_method = node;
}

void CodeGenerator::after(method_class *node) {
    if (node->is_implemented()) {
        return;
    }
    Expression body = node->get_body();
    int tmp_count = body->get_temporaries_count();
    str << current_class->get_name() << '.' << node->get_name() << ':' << endl;
    emit_function_entry(tmp_count);
    // $fp points at return address; immediatelly below it is the saved previous self
    // therefore the space for the first available temporary is 2 words below $fp
    body->code(this, 2);
    emit_function_exit(tmp_count, scope_index - 1);
    object_env.exitscope();
}

void CodeGenerator::after(formal_class *node) {
    object_env.addid(node->get_name(), stack_entry(node->get_type(), scope_index++));
}

void CodeGenerator::code(no_expr_class *expr, int n_temp) {
    emit_move(ACC, ZERO, str) << "\t# non-op" << endl;
}




#pragma clang diagnostic pop