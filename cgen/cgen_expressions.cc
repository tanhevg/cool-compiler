#include <cool-tree.h>
#include <extern_symbols.h>
#include "cgen.h"
#include "cgen_helpers.h"


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
void CodeGenerator::binary_int_op(Binary_Expression_class *expr, char *opcode, int n_temp, Symbol result_type) {
    str << "#\tcode the first operand of binary integer operator" << endl;
    expr->get_e1()->code(this, n_temp);
    str << "#\tstore address of the first operand result in the temporary #" << n_temp << " on the stack frame" << endl;
    emit_store(ACC, n_temp, FP, str);
    str << "#\tcode the second operand of binary integer operator" << endl;
    expr->get_e2()->code(this, n_temp + 1);
    str << "#\tload address of the first operand result into $t1 from temporary #" << n_temp << endl;
    emit_load(T1, n_temp, FP, str);
    // Both operands come in object form, therefore we need to fetch their values into respective registers to proceed
    str << "#\tfetch first operand value into $t1" << endl;
    emit_fetch_int(T1, T1, str);
    str << "#\tfetch second operand value into $t2" << endl;
    emit_fetch_int(T2, ACC, str);
    str << "#\tcode the operation; use $t5 because $t1-$t4 are clobbered by Object.copy" << endl;
    str << opcode << T5 << " " << T1 << " " << T2 << endl;
    str << "#\tcreate (new) the result on the heap" << endl;
    emit_new(result_type, str);
    str << "#\tstore the operation value" << endl;
    emit_store_int(T5, ACC, str);
}

void CodeGenerator::unary_int_op(Unary_Expression_class *expr, char *opcode, int n_temp) {
    str << "#\tcode the operand of unary integer operator" << endl;
    expr->get_e1()->code(this, n_temp);
    str << "#\tfetch first operand value into $t1" << endl;
    emit_fetch_int(T1, ACC, str);
    str << "#\tcode the operation; use $t5 because $t1-$t4 are clobbered by Object.copy" << endl;
    str << opcode << T5 << " " << T1 << endl;
    str << "#\tcreate (new) the result on the heap" << endl;
    emit_new(Int, str);
    str << "#\tstore the operation value" << endl;
    emit_store_int(T5, ACC, str);
}

void CodeGenerator::code(plus_class *expr, int n_temp) {
    str << "#\t'+'" << endl;
    binary_int_op(expr, ADD, n_temp, Int);
}

void CodeGenerator::code(sub_class *expr, int n_temp) {
    str << "#\t'-'" << endl;
    binary_int_op(expr, SUB, n_temp, Int);
}

void CodeGenerator::code(mul_class *expr, int n_temp) {
    str << "#\t'*'" << endl;
    binary_int_op(expr, MUL, n_temp, Int);
}

void CodeGenerator::code(divide_class *expr, int n_temp) {
    str << "#\t'/'" << endl;
    binary_int_op(expr, DIV, n_temp, Int);
}

void CodeGenerator::code(neg_class *expr, int n_temp) {
    str << "#\t'~'" << endl;
    unary_int_op(expr, NEG, n_temp);
}

void CodeGenerator::code(lt_class *expr, int n_temp) {
    str << "#\t'<'" << endl;
    binary_int_op(expr, SLT, n_temp, Bool);
}

void CodeGenerator::code(eq_class *expr, int n_temp) {
    str << "#\t'='" << endl;
    binary_int_op(expr, SEQ, n_temp, Bool);
}

void CodeGenerator::code(leq_class *expr, int n_temp) {
    str << "#\t'<='" << endl;
    binary_int_op(expr, SLE, n_temp, Bool);
}

void CodeGenerator::code(comp_class *expr, int n_temp) {
    str << "#\t'!'" << endl;
    unary_int_op(expr, NOT, n_temp);
}

void CodeGenerator::code(int_const_class *expr, int n_temp) {
    str << "#\tint const " << expr->get_token() << endl;
    emit_load_int(ACC, inttable.lookup_string(expr->get_token()->get_string()), str);
}

void CodeGenerator::code(string_const_class *expr, int n_temp) {
    str << "#\tString const " << expr->get_token() << endl;
    emit_load_string(ACC, stringtable.lookup_string(expr->get_token()->get_string()), str);
}

void CodeGenerator::code(bool_const_class *expr, int n_temp) {
    str << "#\tBoolean const" << endl;
    emit_load_bool(ACC, BoolConst(expr->get_val()), str);
}

void CodeGenerator::code(assign_class *expr, int n_temp) {
    expr->get_expr()->code(this, n_temp);
    str << "#\tAssign to " << expr->get_name() << endl;
    object_env.lookup(expr->get_name())->code_store(str);
}

void CodeGenerator::dispatch(Expression callee, Symbol type, Symbol name, Expressions actuals, int n_temp) {
    str << "#\tcalling " << name << endl;
    emit_push(FP, str);
    for (int i = 0; i < actuals->len(); i++) {
        int idx = actuals->len() - i - 1;
        str << "#\tcoding actual parameter #" << idx << endl;
        actuals->nth(idx)->code(this, n_temp);
        emit_push(ACC, str);
    }
    str << "#\tcallee object" << endl;
    callee->code(this, n_temp);
    str << "#\tload pointer to dispatch table into $t1" << endl;
    emit_load(T1, 2, ACC, str);
    int method_offset = class_table->get_method_offset(type, name);
    if (method_offset > 0) {
        str << "#\tload address of method " << name << " into $t1" << endl;
        emit_load(T1, method_offset, T1, str);
    } else {
        str << "#\tmethod " << name << " has offset 0" << endl;
    }
    emit_jalr(T1, str);

}

void CodeGenerator::code(static_dispatch_class *expr, int n_temp) {
    dispatch(expr->get_callee(), expr->get_type_name(), expr->get_name(), expr->get_actuals(), n_temp);
}

void CodeGenerator::code(dispatch_class *expr, int n_temp) {
    dispatch(expr->get_callee(), expr->get_callee()->get_type(), expr->get_name(), expr->get_actuals(), n_temp);
}

void CodeGenerator::code(cond_class *expr, int n_temp) {
    str << "#\tconditional predicate" << endl;
    expr->get_predicate()->code(this, n_temp);
    str << "#\tconditional predicate: fetch boolean value from boolean object" << endl;
    emit_fetch_int(ACC, ACC, str);
    str << "#\tbranch to false lable if predicate is zero" << endl;
    str << BEQZ << ACC << " " << "false" << condition_count << endl;
    str << "#\ttrue (then) branch of the conditional" << endl;
    expr->get_then()->code(this, n_temp);
    str << "#\tjump to the end of the conditional after executing true (then) branch" << endl;
    str << BRANCH << "condition_end" << condition_count << endl;
    str << "false" << condition_count << ':' << endl;
    expr->get_else()->code(this, n_temp);
    str << "condition_end" << condition_count << ':' << endl;
    condition_count++;
}

void CodeGenerator::code(loop_class *expr, int n_temp) {
    str << "loop_start" << loop_count << ':' << endl;
    str << "#\tloop predicate" << endl;
    expr->get_predicate()->code(this, n_temp);
    str << "#\tloop predicate: fetch boolean value from boolean object" << endl;
    emit_fetch_int(ACC, ACC, str);
    str << BEQZ << ACC << " " << "loop_end" << loop_count << endl;
    expr->get_body()->code(this, n_temp);
    str << BRANCH << "loop_start" << loop_count << endl;
    str << "loop_end" << loop_count << ':' << endl;
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
//todo
}

void CodeGenerator::code(new__class *expr, int n_temp) {
    emit_new(expr->get_type(), str);
    str << "\tjal\t" << expr->get_type() << "_init" << endl;

}

void CodeGenerator::code(isvoid_class *expr, int n_temp) {
//todo
}

void CodeGenerator::code(object_class *expr, int n_temp) {
    str << "#\tobject " << expr->get_name() << endl;
    object_env.lookup(expr->get_name())->code_load(str);
}


void ObjectEnvRecord::code_ref(ostream &str) {
    str << offset * WORD_SIZE << "(" << reg << ")";
}

void ObjectEnvRecord::code_store(ostream &str) {
    emit_store(ACC, offset, reg, str);
}

void ObjectEnvRecord::code_load(ostream &str) {
    emit_load(ACC, offset, reg, str);
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
    str << "#\tset up stack frame; number of temporaries = " << tmp_count << endl;
    emit_move(FP, SP, str);
    emit_push(RA, str);
    emit_push(SELF, str);
    emit_move(SELF, ACC, str);
    if (tmp_count > 0) {
        emit_addiu(SP, SP, -4 * tmp_count, str);
    }

}

void CodeGenerator::emit_function_exit(int tmp_count, int parameter_count) {
    str << "#\tfunction exit: load return address from the frame" << endl;
    emit_load(RA, 0, FP, str);
    str << "#\trestore previous self" << endl;
    emit_load(SELF, -4, FP, str);
    int frame_size = 4 * parameter_count +
                     4 * tmp_count +
                     8; // return address and old self
    str << "#\tpop the frame" << endl;
    emit_addiu(SP, SP, frame_size, str);
    str << "#\trestore previous $fp" << endl;
    emit_load(FP, 0, SP, str);
    str << "#\treturn from " << current_class->get_name();
    if (current_method) {
        str << '.' << current_method->get_name();
    } else {
        str << "_init";
    }
    str << endl;
    emit_return(str);

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
        str << "#\tall attribute initializers are empty; return straight away" << endl;
        emit_return(str);
        return;
    }
    emit_function_entry(tmp_count);
    class_table->visit_ordered_attrs_of_class(current_class->get_name(), [this](IndexedRecord<attr_class> *ar) {
        if (ar->get_ref()->get_initializer()->is_empty()) {
            return;
        }
        // $fp points at return address; immediatelly below it is the saved previous self
        // therefore the space for the first available temporary is 2 words below $fp
        str << "#\tcode initializer for " << ar->get_ref()->get_name() << endl;
        ar->get_ref()->get_initializer()->code(this, 2);
        str << "#\tstore value returned by initializer in the attribute" << endl;
        object_env.lookup(ar->get_ref()->get_name())->code_store(str);
    });
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
    str << "#\tcode method body" << endl;
    body->code(this, 2);
    emit_function_exit(tmp_count, scope_index - 1);
    object_env.exitscope();
}

void CodeGenerator::after(formal_class *node) {
    object_env.addid(node->get_name(), stack_entry(node->get_type(), scope_index++));
}

void CodeGenerator::code(no_expr_class *expr, int n_temp) {
    emit_move(ACC, ZERO, str);
}




#pragma clang diagnostic pop