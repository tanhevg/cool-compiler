//
// See copyright.h for copyright notice and limitation of liability
// and disclaimer of warranty provisions.
//
#include "copyright.h"

//////////////////////////////////////////////////////////////////////////////
//
//  parser-phase.cc
//
//  Reads a COOL token stream from a file and builds the abstract syntax tree.
//
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>     // for Linux system

#include "cool-io.h"  //includes iostream
#include "cool-tree.h"

#include "cool-parse.h"

#include <memory>

using namespace std;

//
// These globals keep everything working.
//
FILE *token_file = stdin;        // we read from this file
extern Classes parse_results;     // list of classes; used for multiple files
extern Program ast_root;     // the AST produced by the parse

char *curr_filename = nullptr;


extern int omerrs;             // a count of lex and parse errors

extern int cool_yyparse();

extern void handle_flags(int argc, char *argv[]);

extern FILE *yyin;
extern char *out_filename;

typedef unique_ptr<ostream, function<void(ostream*)>> stream_ptr;

int main(int argc, char *argv[]) {
    handle_flags(argc, argv);
    cout << "Parser phase " << curr_filename << endl;
    yyin = fopen(curr_filename, "r");
    if (!yyin) {
        cout << "Could not open " << curr_filename << endl;
    }
    cool_yyparse();
    stream_ptr out(&cout, [](ostream*){});
    if (out_filename) {
        cout << "Writing to file " << out_filename << endl;
        out = stream_ptr(new ofstream(out_filename), default_delete<ostream>());
    }
    ast_root->dump_with_types(*out, 0);
    if (omerrs != 0) {
        cerr << "Compilation halted due to lex and parse errors\n";
        return 1;
    }
    return 0;
}

