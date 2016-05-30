#include <cool-tree.h>
#include <extern_symbols.h>
#include <string>
#include "cgen.h"
#include "string_util.h"

using std::string;
using std::to_string;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wwritable-strings"
static ObjectEnvRecord *stack_entry(Symbol declaring_type, int offset) {
    return new ObjectEnvRecord(declaring_type, FP, offset);
}

static ObjectEnvRecord *heap_entry(Symbol declaring_type,
                                   int offset) {
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
    int line_no = expr->get_line_number();
    emit_void_dispatch_check(str, line_no, label_count);
    emit_store(ACC, -n_temp, FP, str, line_no,
               "store address of the first operand of ", op_string, " in temporary ", n_temp);
    expr->get_e2()->code(this, n_temp + 1);
    emit_void_dispatch_check(str, line_no, label_count);
    emit_load(T1, -n_temp, FP, str, line_no,
              "load address of the first operand result of ", op_string, " from temporary ", n_temp);
    // Both operands come in object form, therefore we need to fetch their values into respective registers to proceed
    emit_fetch_int(T1, T1, str, line_no, "fetch first operand value" );
    emit_fetch_int(T2, ACC, str, line_no, "fetch second operand value");
    str << opcode << T5 << " " << T1 << " " << T2;
    comment(str, line_no, "code ", op_string, "; use $t5 because $t1-$t4 are clobbered by Object.copy");
    emit_new(result_type, str, line_no, "create (new) the result of ", op_string, " on the heap");
    emit_store_int(T5, ACC, str, line_no, "store the ", op_string, " value");
}

void CodeGenerator::unary_int_bool_op(Unary_Expression_class *expr, char *opcode, const char *op_string, int n_temp,
                                      Symbol result_type) {
    int line_no = expr->get_line_number();
    expr->get_e1()->code(this, n_temp);
    emit_void_dispatch_check(str, line_no, label_count);
    emit_fetch_int(T1, ACC, str, line_no, "fetch value of first operand of ", op_string);
    str << opcode << T5 << " " << T1;
    comment(str, line_no, "# code ", op_string, "; use $t5 because $t1-$t4 are clobbered by Object.copy");
    emit_new(result_type, str, line_no, "create (new) the result of ", op_string, " on the heap");
    emit_store_int(T5, ACC, str, line_no, " store the ", op_string, " value");
}

void CodeGenerator::code(plus_class *expr, int n_temp) {
    binary_int_bool_op(expr, ADD, "'plus'", n_temp, Int);
}

void CodeGenerator::code(sub_class *expr, int n_temp) {
    binary_int_bool_op(expr, SUB, "'minus'", n_temp, Int);
}

void CodeGenerator::code(mul_class *expr, int n_temp) {
    binary_int_bool_op(expr, MUL, "'star'", n_temp, Int);
}

void CodeGenerator::code(divide_class *expr, int n_temp) {
    binary_int_bool_op(expr, DIV, "'divide'", n_temp, Int);
}

void CodeGenerator::code(neg_class *expr, int n_temp) {
    unary_int_bool_op(expr, NEG, "'unary minus'", n_temp, Int);
}

void CodeGenerator::code(lt_class *expr, int n_temp) {
    binary_int_bool_op(expr, SLT, "'less'", n_temp, Bool);
}

void CodeGenerator::code(eq_class *expr, int n_temp) {
    expr->get_e1()->code(this, n_temp);
    int line_no = expr->get_line_number();
    emit_void_dispatch_check(str, line_no, label_count);
    emit_store(ACC, -n_temp, FP, str, line_no,
               "store address of the first operand of '=' in temporary ", n_temp);
    expr->get_e2()->code(this, n_temp + 1);
    emit_void_dispatch_check(str, line_no, label_count);
    emit_load(T1, -n_temp, FP, str, line_no,
              "load address of the first operand result of '=' from temporary ", n_temp);
    emit_move(T2, ACC, str, line_no, "move the second operand of '='");

    emit_partial_load_address(ACC, str);
    truebool.code_ref(str);
    str << " # " << line_no << " : value in $a0 will be returned for true\n";

    emit_beq(T1, T2, "_pointers_equal", label_count, str, line_no, "skip equality test if pointers are identical");

    emit_partial_load_address(A1, str);
    falsebool.code_ref(str);
    str << " # " << line_no << " : value in $a1 will be returned for false\n";

    emit_jal("equality_test", str, line_no, "");
    emit_label_def("_pointers_equal", label_count, str);
    label_count++;

}

void CodeGenerator::code(leq_class *expr, int n_temp) {
    binary_int_bool_op(expr, SLE, "'<='", n_temp, Bool);
}

void CodeGenerator::code(comp_class *expr, int n_temp) {
    int line_no = expr->get_line_number();
    expr->get_e1()->code(this, n_temp);
    emit_void_dispatch_check(str, line_no, label_count);
    emit_fetch_int(T5, ACC, str, line_no, " fetch value of first operand of 'not'");
    str << ADDI << T5 << " " << T5 << " -1";
    comment(str, line_no, " code 'not'; use 'not(x) = -(x-1)' form to make sure 0->1 and 1->0 translation");
    str << NEG << T5 << " " << T5;
    comment(str, line_no, " use $t5 because $t1-$t4 are clobbered by Object.copy");
    emit_new(Bool, str, line_no, " create (new) the result of 'not' on the heap");
    emit_store_int(T5, ACC, str, line_no, " store the 'not' value");
}

void CodeGenerator::code(int_const_class *expr, int n_temp) {
    string tok = string(expr->get_token()->get_string());
    emit_load_int(ACC, inttable.lookup_string(expr->get_token()->get_string()), str,
                  expr->get_line_number(), "load int const ", rtrim(tok));
}

void CodeGenerator::code(string_const_class *expr, int n_temp) {
    string tok = string(expr->get_token()->get_string());
    tok = erase_newline(tok);
    emit_load_string(ACC, stringtable.lookup_string(expr->get_token()->get_string()), str,
                     expr->get_line_number(), "load string const ", "'", tok,"'");
}

void CodeGenerator::code(bool_const_class *expr, int n_temp) {
    emit_load_bool(ACC, BoolConst(expr->get_val()), str, expr->get_line_number(), " load boolean const");
}

void CodeGenerator::code(assign_class *expr, int n_temp) {
    expr->get_expr()->code(this, n_temp);
    object_env.lookup(expr->get_name())->code_store(str, expr->get_line_number(),  "assign to ", expr->get_name());
}

void CodeGenerator::dispatch(int line_no, Expression callee, Symbol static_type_or_null, Symbol name, Expressions actuals, int n_temp) {
    Symbol type = static_type_or_null ? static_type_or_null : callee->get_type();
    emit_push(FP, str, line_no, "push old frame pointer before calling ", type, '.', name );
    for (int i = 0; i < actuals->len(); i++) {
        // evaluate parameters in the reverse order, such that they appear in the correct order in the stack frame
        // the first parameter should appear at index 0 from $fp, i.e. last in the stack
        int idx = actuals->len() - i - 1;
        actuals->nth(idx)->code(this, n_temp);
        emit_push(ACC, str, line_no, "push actual parameter #", idx, " of ", type, '.', name);
    }
    callee->code(this, n_temp);
    emit_void_dispatch_check(str, line_no, label_count);
    if (static_type_or_null) {
        emit_load_address(T1, (string(static_type_or_null->get_string()) + "_dispTab").c_str(), str, line_no,
                          "load address of static dispatch table");
    } else {
        emit_load(T1, 2, ACC, str, line_no, "load address of dynamic dispatch table");
    }
    int method_offset = class_table->get_method_offset(type, name);
    emit_load(T1, method_offset, T1, str, line_no, "load address of ", type, '.', name);
    emit_jalr(T1, str, line_no, "call ", type, '.', name);
    // code this here instead of emit_function_exit, because functions in trap handler do not pop $fp
    emit_pop(FP, str, line_no);
}

void CodeGenerator::code(static_dispatch_class *expr, int n_temp) {
    dispatch(expr->get_line_number(), expr->get_callee(), expr->get_type_name(), expr->get_name(), expr->get_actuals(), n_temp);
}

void CodeGenerator::code(dispatch_class *expr, int n_temp) {
    dispatch(expr->get_line_number(), expr->get_callee(), nullptr, expr->get_name(), expr->get_actuals(), n_temp);
}

void CodeGenerator::code(cond_class *expr, int n_temp) {
    int line_no = expr->get_line_number();
    int condition_count = this->condition_count++;
    expr->get_predicate()->code(this, n_temp);
    emit_fetch_int(ACC, ACC, str, line_no, "conditional predicate: fetch boolean value from boolean object");
    emit_beqz(ACC,"false",condition_count, str, line_no, "branch to false lable if predicate is zero");
    expr->get_then()->code(this, n_temp);
    emit_branch("condition_end", condition_count, str, line_no, "jump to the end of the conditional after executing true (then) branch");
    emit_label_def("false", condition_count, str, line_no, "false branch");
    expr->get_else()->code(this, n_temp);
    emit_label_def("condition_end", condition_count, str, line_no, "condition end");
}

void CodeGenerator::code(loop_class *expr, int n_temp) {
    int line_no = expr->get_line_number();
    int loop_count = this->loop_count++;
    emit_label_def("loop_start", loop_count, str, line_no, "loop start");
    expr->get_predicate()->code(this, n_temp);
    emit_fetch_int(ACC, ACC, str, line_no, "loop predicate: fetch boolean value from boolean object");
    emit_beqz(ACC, "loop_end", loop_count, str, line_no, "branch to loop end if predicate is false");
    expr->get_body()->code(this, n_temp);
    emit_branch("loop_start", loop_count, str, line_no, "branch to loop start");
    emit_label_def("loop_end", loop_count, str, line_no, "end of loop");
    emit_move(ACC, ZERO, str, line_no, "loop returns void");
}

static const char* case_label(int case_count, const char *label) {
    string str = string("_case_") + to_string(case_count) + '_' + label;
    return str.data();
}

void CodeGenerator::code(typcase_class *expr, int n_temp) {
    int line_no = expr->get_line_number();
    expr->get_expr()->code(this, n_temp);
    int case_count = this->case_count++;
    emit_beqz(ACC, case_label(case_count, "void"), str, line_no, "abort if case argument is void");
    emit_store(ACC, -n_temp, FP, str, line_no, "case: store argument in temporary #", n_temp);
    emit_load_address(A1, case_label(case_count, "tab_start"), str, line_no, "case: load start of branch table into $a1");
    emit_load_address(A2, case_label(case_count, "tab_end"), str, line_no, "case: load end of branch table into $a2");
    emit_addiu(A2, A2, -8, str, line_no, "case: subtract 2 words from branch table end, such that BEQ test can be used");
    emit_jump("_case_subroutine", str, line_no, "jump to case subroutine");
    expr->get_cases()->traverse([this, n_temp, case_count](Case _case){
        object_env.enterscope();
        object_env.addid(_case->get_name(), stack_entry(_case->get_type_decl(), -n_temp));
        str << case_label(case_count, _case->get_type_decl()->get_string()) << LABEL; // branch label
        _case->get_expr()->code(this, n_temp+1);
        emit_jump(case_label(case_count, "end"), str, _case->get_line_number(), "jump to end of case after branch is evaluated");
        object_env.exitscope();
    });
    str << case_label(case_count, "void") << LABEL;
    emit_abort_file_line(str, line_no, "_case_abort2", "void case argument");
    str << case_label(case_count, "end") << LABEL; // end of case label
}

void CodeGenerator::code(block_class *block, int n_temp) {
    Expressions body = block->get_body();
    body->traverse([this, n_temp](Expression expr) {
        expr->code(this, n_temp);
    });
}

void CodeGenerator::code(let_class *expr, int n_temp) {
    int line_no = expr->get_line_number();
    if (expr->get_init()->is_empty()) {
        if (expr->get_type_decl() == Bool) {
            emit_partial_load_address(ACC, str);
            falsebool.code_ref(str);
        } else if (expr->get_type_decl() == Str) {
            emit_partial_load_address(ACC, str);
            empty_string->code_ref(str);
        } else if (expr->get_type_decl() == Int) {
            emit_partial_load_address(ACC, str);
            zero_int->code_ref(str);
        } else {
            str << MOVE << ACC << " " << ZERO;
        }
        comment(str, line_no, "empty initializer for ", expr->get_identifier());
    } else {
        expr->get_init()->code(this, n_temp);
    }
    object_env.enterscope();
    ObjectEnvRecord *temp_record = stack_entry(expr->get_type_decl(), -n_temp);
    object_env.addid(expr->get_identifier(), temp_record);
    temp_record->code_store(str, line_no, "store the address returned by the initalizer for ",
                            expr->get_identifier(), " in temp #",  n_temp);
    expr->get_body()->code(this, n_temp+1);
    object_env.exitscope();
}

void CodeGenerator::code(new__class *expr, int n_temp) {
    Symbol type_name = expr->get_type_name();
    int line_no = expr->get_line_number();
    emit_new(type_name, str, line_no, "new ", type_name, "()");
    emit_push(FP, str, line_no, "store old frame pointer before calling ", type_name, "_init");
    str << "\tjal\t" << type_name << "_init";
    comment(str, line_no, "jump to initializer of ", type_name);
    emit_pop(FP, str, line_no);
}

void CodeGenerator::code(isvoid_class *expr, int n_temp) {
    int line_no = expr->get_line_number();
    expr->get_e1()->code(this, n_temp);
    emit_seq(T5, ACC, ZERO, str, line_no, "set $t5 if the result of 'isvoid' body is 0 ($zero)",
             "; use $t5 because $t1-$t4 are clobbered by Object.copy");
    emit_new(Bool, str, line_no, "new the result of 'isvoid' on the heap");
    emit_store_int(T5, ACC, str, line_no, "store the 'isvoid' value");
}

void CodeGenerator::code(object_class *expr, int n_temp) {
    int line_no = expr->get_line_number();
    if (self == expr->get_name()) {
        emit_move(ACC, SELF, str, line_no, "object 'self'");
    } else {
        object_env.lookup(expr->get_name())->code_load(str, line_no, "object ", expr->get_name());
    }
}


void CodeGenerator::before(program_class *node) {
    object_env.enterscope();
    object_env.addid(self, heap_entry(SELF_TYPE, 0));
}

void CodeGenerator::before(class__class *node) {
    current_attributes.clear();
    object_env.enterscope();
    class_table->visit_ordered_attrs_of_class(node->get_name(),
        [this](IndexedRecord<attr_class> *ar) {
            object_env.addid(ar->get_ref()->get_name(), heap_entry(ar->get_ref()->get_type(), ar->get_index()));
        }
    );
    current_class = node;
}

void CodeGenerator::emit_function_entry(int tmp_count, int line_no) {
    emit_move(FP, SP, str, line_no, "set up stack pointer for ", generate_init_or_method_name());
    emit_push(RA, str)          << endl;
    emit_push(SELF, str)        << endl;
    emit_move(SELF, ACC, str)   << endl;
    if (tmp_count > 0) {
        emit_addiu(SP, SP, -4 * tmp_count, str, line_no, "push ", tmp_count, " temporaries");
    }

}

string CodeGenerator::generate_init_or_method_name() const {
    string name;
    if (current_method) {
        name = string(current_class->get_name()->get_string()) + string(".") + string(current_method->get_name()->get_string());
    } else {
        name = string(current_class->get_name()->get_string()) + string("_init");
    }
    return std::move(name);
}

void CodeGenerator::emit_function_exit(int tmp_count, int parameter_count, int line_no) {
    string name = generate_init_or_method_name();
    emit_load(RA, 0, FP, str, line_no, "exit from ", name, ": load return address");
    emit_load(SELF, -1, FP, str, line_no, "restore previous self");
    int frame_size = 4 * parameter_count +
                     4 * tmp_count +
                     8;// return address and old self
    emit_addiu(SP, SP, frame_size, str, line_no, "pop the frame");
    emit_return(str, line_no, "return from ",  name);

}

void CodeGenerator::after(class__class *node) {
    current_method = nullptr;
    int tmp_count = 0;
    bool is_empty = true;
    int line_no = node->get_line_number();
    for (attr_class *attr : current_attributes) {
        if (attr->get_temporaries_count() > tmp_count) {
            tmp_count = attr->get_temporaries_count();
        }
        is_empty &= attr->get_initializer()->is_empty();
    }
    Symbol class_name = node->get_name();
    str << class_name << "_init:" << endl;
    emit_function_entry(tmp_count, line_no);
    if (node->get_parent() != No_class) {
        emit_push(FP, str, line_no, "store old frame pointer before calling parent initializer ", node->get_parent(), "_init");
        str << JAL << node->get_parent() << "_init" << "\t# call parent initializer\n";
        emit_pop(FP, str, line_no);
    }
    if (!is_empty) {
        for (attr_class *attr : current_attributes) {
            Expression initializer = attr->get_initializer();
            if (!initializer->is_empty()) {
                // $fp points at return address; immediately below it is the saved previous self
                // therefore the space for the first available temporary is 2 words below $fp
                Symbol name = attr->get_name();
                initializer->code(this, 2);
                object_env.lookup(name)->code_store(str, line_no, "assign initial value to ", name);
            }
        }
    }
    emit_move(ACC, SELF, str, line_no, "restore self in $a0");
    emit_function_exit(tmp_count, 0, line_no);
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
    int line_no = node->get_line_number();
    str << current_class->get_name() << '.' << node->get_name() << ':' << endl;
    emit_function_entry(tmp_count, line_no);
    // $fp points at return address; immediately below it is the saved previous self
    // therefore the space for the first available temporary is 2 words below $fp
    body->code(this, 2);
    emit_function_exit(tmp_count, scope_index - 1, line_no);
    object_env.exitscope();
}

void CodeGenerator::after(attr_class *node) {
    current_attributes.push_back(node);
}

void CodeGenerator::after(formal_class *node) {
    object_env.addid(node->get_name(), stack_entry(node->get_type(), scope_index++));
}

void CodeGenerator::code(no_expr_class *expr, int n_temp) {
    emit_move(ACC, ZERO, str, expr->get_line_number(), "non-op");
}

void CaseDataCoder::after(typcase_class *node) {
    str << case_label(case_count, "tab_end") << LABEL;
    case_count++;
}

void CaseDataCoder::after(branch_class *node) {
    str << WORD << class_table->get_class_tag(node->get_type_decl()) << endl;
    str << WORD << case_label(case_count, node->get_type_decl()->get_string()) << endl;

}

void CaseDataCoder::before(typcase_class *node) {
    str << case_label(case_count, "tab_start") << LABEL;
}


#pragma clang diagnostic pop