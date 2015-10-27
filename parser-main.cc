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

#include "cool-io.h"  //includes iostream
#include "cool-tree.h"

#include "cool-parse.h"


using std::unique_ptr;
using std::function;
using std::default_delete;

//
// These globals keep everything working.
//
FILE *token_file = stdin;        // we read from this file
extern Classes parse_results;     // list of classes; used for multiple files
extern Program ast_root;     // the AST produced by the parse

extern char *curr_filename;


extern int omerrs;             // a count of lex and parse errors

extern int cool_yyparse();

extern void handle_flags(int argc, char *argv[]);

extern FILE *yyin;
extern char *out_filename;

typedef unique_ptr<ostream, function<void(ostream *)>> stream_ptr;

int main(int argc, char *argv[]) {
    ast_root = NULL;
    handle_flags(argc, argv);
    if (curr_filename) {
        cout << "Parsing " << curr_filename << endl;
        yyin = fopen(curr_filename, "r");
        if (!yyin) {
            cerr << "Could not open " << curr_filename << endl;
            return 2;
        }
    } else {
        cout << "Parsing standard input" << endl;
        curr_filename = "<stdin>";// used in parser; if left null will cause a seg fault
    }
    cool_yyparse();
    stream_ptr out(&cout, [](ostream *) { });
    if (out_filename) {
        cout << "Writing to file " << out_filename << endl;
        out = stream_ptr(new ofstream(out_filename), default_delete<ostream>());
    }
    if (ast_root) {
        ast_root->dump_with_types(*out, 0);
    }
    if (omerrs != 0) {
        cerr << "Compilation halted due to lex and parse errors\n";
        return 1;
    }
    return 0;
}

