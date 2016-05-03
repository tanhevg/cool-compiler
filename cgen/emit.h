///////////////////////////////////////////////////////////////////////
//
//  Assembly Code Naming Conventions:
//
//     Dispatch table            <classname>_dispTab
//     Method entry point        <classname>.<method>
//     Class init code           <classname>_init
//     Abort method entry        <classname>.<method>.Abort
//     Prototype object          <classname>_protObj
//     Integer constant          int_const<Symbol>
//     String constant           str_const<Symbol>
//
///////////////////////////////////////////////////////////////////////
#ifndef _EMIT_H_
#define _EMIT_H_

#include "stringtab.h"

#define MAXINT  100000000    
#define WORD_SIZE    4
#define LOG_WORD_SIZE 2     // for logical shifts

// Global names
#define CLASSNAMETAB         "class_nameTab"
#define CLASSOBJTAB          "class_objTab"
#define INTTAG               "_int_tag"
#define BOOLTAG              "_bool_tag"
#define STRINGTAG            "_string_tag"
#define HEAP_START           "heap_start"

// Naming conventions
#define DISPTAB_SUFFIX       "_dispTab"
#define METHOD_SEP           "."
#define CLASSINIT_SUFFIX     "_init"
#define PROTOBJ_SUFFIX       "_protObj"
#define OBJECTPROTOBJ        "Object_protObj"
#define INTCONST_PREFIX      "int_const"
#define STRCONST_PREFIX      "str_const"
#define BOOLCONST_PREFIX     "bool_const"

#define OBJECT_COPY          "Object.copy"


#define EMPTYSLOT            0
#define LABEL                ":\n"

#define STRINGNAME (char *) "String"
#define INTNAME    (char *) "Int"
#define BOOLNAME   (char *) "Bool"
#define MAINNAME   (char *) "Main"

//
// information about object headers
//
#define DEFAULT_OBJFIELDS 3
#define TAG_OFFSET 0
#define SIZE_OFFSET 1
#define DISPTABLE_OFFSET 2

#define STRING_SLOTS      1
#define INT_SLOTS         1
#define BOOL_SLOTS        1

#define GLOBAL        "\t.globl\t"
#define ALIGN         "\t.align\t2\n"
#define WORD          "\t.word\t"

//
// register names
//
#define ZERO "$zero"		// Zero register 
#define ACC  "$a0"		// Accumulator 
#define A1   "$a1"		// For arguments to prim funcs 
#define A2   "$a2"
#define A3   "$a3"
#define SELF "$s0"		// Ptr to self (callee saves)
#define T0   "$t0"		// Temporary 0
#define T1   "$t1"		// Temporary 1
#define T2   "$t2"		// Temporary 2
#define T3   "$t3"		// Temporary 3 
#define T4   "$t4"		// Temporary 4
#define T5   "$t5"		// Temporary 5
#define SP   "$sp"		// Stack pointer
#define FP   "$fp"		// Frame pointer
#define RA   "$ra"		// Return address

//
// Opcodes
//
#define JALR  "\tjalr\t"
#define JAL   "\tjal\t"
#define JUMP  "\tj\t"
#define RET   "\tjr\t$ra\t"

#define SW    "\tsw\t"
#define LW    "\tlw\t"
#define LI    "\tli\t"
#define LA    "\tla\t"

#define MOVE  "\tmove\t"
#define NEG   "\tneg\t"
#define ADD   "\tadd\t"
#define ADDI  "\taddi\t"
#define ADDU  "\taddu\t"
#define ADDIU "\taddiu\t"
#define DIV   "\tdiv\t"
#define MUL   "\tmul\t"
#define SUB   "\tsub\t"
#define SLL   "\tsll\t"
#define BEQZ  "\tbeqz\t"
#define BRANCH   "\tb\t"
#define BEQ      "\tbeq\t"
#define BNE      "\tbne\t"
#define BLEQ     "\tble\t"
#define BLT      "\tblt\t"
#define BGT      "\tbgt\t"

#define SLT     "\tslt\t"
#define SEQ     "\tseq\t"
#define SLE     "\tsle\t"
#define NOT     "\tnot\t"
#define XORI    "\txori\t"

/**
 * Arguments:
 * $a0 - address of object in case argument
 * $a1 - start of branch table
 * $a2 - last entry in branch table
 *
 * Temporaries
 * $t0 - start of inheritance table
 * $t1 - branch table index
 * $t2 - address in branch table, tag from branch table
 * $t3 - address in inheritance table, tag from inheritance table
 */
#define CASE_SUBROUTINE "\
case_subroutine:\n\
    move    $t1 $zero                               # initialise branch table index to 0 \n\
    move    $t2 $a1                                 # initialise address of tag in branch table to branch table start\n\
    lw      $t3 0($a0)                              # load class tag of case argument (passed in $a0) into $t3\n\
    la      $t0 inheritance_tab                     # load start of inheritance table in $t0\n\
case_subroutine_loop_start:\n\
    beq     $t2 $a2 case_subroutine_not_found       # the new address is at the branch table end\n\
    lw      $t2 0($t2)                              # load tag from branch table\n\
    beq     $t2 $t3 case_subroutine_found           # tag from branch table matches case argument tag -> we found the branch \n\
    addiu   $t1 8                                   # increment branch table index\n\
    addu    $t2 $a1 $t1                             # compute new address of branch table tag by adding the index to branch table start\n\
    j       case_subroutine_loop_start              # repeat the loop wiht the new branch\n\
case_subroutine_not_found:\n\
    beq     $t3 $zero case_subroutine_exception     # nothing found for Object -> abort\n\
    sll     $t3 $t3 2                               # multiply tag of case argument by word size (=4)\n\
    addu    $t3 $t0 $t3                             # compute new address in the inheritance table\n\
    lw      $t3 0($t3)                              # load new tag from inheritance table\n\
    move    $t1 $zero                               # initialise branch table index to 0 \n\
    move    $t2 $a1                                 # initialise address of tag in branch table to branch table start\n\
    j       case_subroutine_loop_start              # repeat the loop with new tag from the inheritance table\n\
case_subroutine_exception:\n\
    j       _case_abort                             # abort ($a0 is preserved)\n\
case_subroutine_found:\n\
    addu    $t2 $a1 $t1                             # compute new address of branch table tag by adding branch table index to branch table start\n\
    lw      $ra 4($t2)                              # load the lable from branch table\n\
    jr      $ra\n\
"


#endif