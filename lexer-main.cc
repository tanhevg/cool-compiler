#include <iostream>
#include <cool-parse.h>

extern int cool_yylex(void);

extern FILE *yyin;
extern int yylineno;
int lex_verbose;
YYSTYPE cool_yylval;
int cool_yydebug;        // for the parser
char *curr_filename;

int main(int argc, char **argv) {
    lex_verbose = 1;
    if (argc >= 0) {
        const char *filename = argv[1];
        yyin = fopen(filename, "r");
        if (!yyin) {
            printf("Could not open %s\n", filename);
            return 1;
        }
        // cout << "Opened " << filename << "\n";
    }
    while (cool_yylex()) { }
    cout << "line count: " << yylineno << "\n";
    return 0;
}
