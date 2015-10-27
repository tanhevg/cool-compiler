%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

#define yylval cool_yylval
#define yylex  cool_yylex


extern int lex_verbose;

#define LOG_LEVEL 1 
#define LOGE \
if (lex_verbose) { \
    cout << "Scanned error: '" << cool_yylval.error_msg << "'\n"; \
}
#define LOGS(token) \
if (lex_verbose) { \
    cout << "Scanned symbol " << #token << ": '" << cool_yylval.symbol << "'\n"; \
}
#define LOGB \
if (lex_verbose) { \
    cout << "Scanned boolean " << cool_yylval.boolean << ": '" << yytext << "'\n"; \
}
#define LOGT(token) \
if (lex_verbose) { \
    cout << "Scanned token " << #token << ": '" << yytext << "'\n"; \
}
/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */



char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern YYSTYPE cool_yylval;
int curr_lineno=0;
int mcomment_count; /* to handle nested multiline comments */
%}

SCOMMENT	--.* 
MCOMMENT	[\(\)\*]|[^\(\)\*]+
MCOMMENT_START	\(\*
MCOMMENT_END	\*\)
NEWLINE		\n
DQUOTE		\"
ESCAPE		\\
ZERO		\0
INTEGER		[0-9]+
WS		[ \r\t\v\f]+
PUNCTUATION	[@;:,\(\)\{\}\+\-\*/=~<\.]
DARROW          =>
LE		<=
ASSIGN		<-
STRING		[^\n\"\\]+
TYPEID		[A-Z][A-Za-z0-9_]*
OBJECTID	[a-z][A-Za-z0-9_]* 
BOOL		t[Rr][Uu][Ee]|f[Aa][Ll][Ss][Ee]
CLASS           [Cc][Ll][Aa][Ss][Ss]
ELSE		[Ee][Ll][Ss][Ee]
FI		[Ff][Ii]
IF		[Ii][Ff]
IN		[Ii][Nn]
INHERITS	[Ii][Nn][Hh][Ee][Rr][Ii][Tt][Ss]
LET		[Ll][Ee][Tt]
LOOP		[Ll][Oo][Oo][Pp]
POOL		[Pp][Oo][Oo][Ll]
THEN		[Tt][Hh][Ee][Nn]
WHILE		[Ww][Hh][Ii][Ll][Ee]
CASE		[Cc][Aa][Ss][Ee]
ESAC		[Ee][Ss][Aa][Cc]
OF		[Oo][Ff]
NEW		[Nn][Ee][Ww]
ISVOID		[Ii][Ss][Vv][Oo][Ii][Dd]
NOT		[Nn][Oo][Tt]

%x MCOMMENT STRING ESCAPE

%%

{CLASS} {
    LOGT(CLASS);
    return CLASS;
}

{ELSE} {
    LOGT(ELSE);
    return ELSE;
}

{FI} {
    LOGT(FI);
    return FI;
}

{IF} {
    LOGT(IF);
    return IF;
}

{IN} {
    LOGT(IN);
    return IN;
}

{INHERITS} {
    LOGT(INHERITS);
    return INHERITS;
}

{LET} {
    LOGT(LET);
    return LET;
}

{LOOP} {
    LOGT(LOOP);
    return LOOP;
}

{POOL} {
    LOGT(POOL);
    return POOL;
}

{THEN} {
    LOGT(THEN);
    return THEN;
}

{WHILE} {
    LOGT(WHILE);
    return WHILE;
}

{CASE} {
    LOGT(CASE);
    return CASE;
}

{ESAC} {
    LOGT(ESAC);
    return ESAC;
}

{OF} {
    LOGT(OF);
    return OF;
}

{NEW} {
    LOGT(NEW);
    return NEW;
}

{ISVOID} {
    LOGT(ISVOID);
    return ISVOID;
}

{NOT} {
    LOGT(NOT);
    return NOT;
}

{BOOL} {
    cool_yylval.boolean = yytext[0] == 't';
    LOGB;
    return BOOL_CONST;
} 

{TYPEID} {
    cool_yylval.symbol = inttable.add_string(yytext,yyleng);
    LOGS(TYPEID);
    return TYPEID;
}

{OBJECTID} {
    cool_yylval.symbol = inttable.add_string(yytext,yyleng);
    LOGS(OBJECTID);
    return OBJECTID;
}

{DQUOTE} {
    LOGT("Start of string");
    BEGIN(STRING);
    string_buf_ptr = string_buf;
}

<STRING>{DQUOTE} {
    BEGIN(INITIAL);
    if (string_buf_ptr < string_buf + MAX_STR_CONST) {
        *string_buf_ptr = '\0';
        cool_yylval.symbol = inttable.add_string(string_buf);
	LOGS(STR_CONST);
        return STR_CONST;
    } else {
        cool_yylval.error_msg = "String constant too long";
	LOGE;
        return ERROR;
    }
}

<STRING>{ESCAPE} {
    BEGIN(ESCAPE);
}

<STRING>{ZERO} {
    cool_yylval.error_msg = "String contains null character";
    LOGE;
    return ERROR;
}

<STRING><<EOF>> {
    cool_yylval.error_msg = "EOF in string constant";
    LOGE;
    return 0;
}

<STRING>{NEWLINE} {
    cool_yylval.error_msg = "Unterminated string constant";
    LOGE;
    curr_lineno++;
    return ERROR;
}

<ESCAPE>. {
    char c = yytext[0];
    if (c == 't') {
        c = '\t';
    } else if (c == 'b') {
        c = '\b';
    } else if (c == 'n') {
        c = '\n';
    } else if (c == 'f') {
        c = '\f';
    }
    if (string_buf_ptr < string_buf + MAX_STR_CONST - 1) {
        *string_buf_ptr++ = c;
    }
    BEGIN(STRING);
}

<ESCAPE>{NEWLINE} {
    curr_lineno++;
    BEGIN(STRING);
}

<STRING>{STRING} {
    int remaining_size = MAX_STR_CONST - (string_buf_ptr - string_buf) - 1;
    int to_copy = yyleng > remaining_size ? remaining_size : yyleng;
    strncpy(string_buf_ptr, yytext, to_copy);
    string_buf_ptr += to_copy;
}
    

{INTEGER} {
    cool_yylval.symbol = inttable.add_string(yytext);
    LOGS(INT_CONST)
    return INT_CONST;
}

{DARROW} {
    LOGT(DARROW);
    return DARROW;
}

{PUNCTUATION} {
    LOGT();
    return yytext[0];
}

{LE} {
    LOGT(LE)
    return LE;
}

{ASSIGN} {
    LOGT(ASSIGN)
    return ASSIGN;
}

{WS} LOGT(WS);

{SCOMMENT} LOGT("Single line comment");

<INITIAL>{MCOMMENT_END} {
    cool_yylval.error_msg = "Unbalanced *)";
    LOGE
    return ERROR;
}

<INITIAL,MCOMMENT>{MCOMMENT_START} {
    if (!mcomment_count) {
        LOGT("Start of multiline comment");
        BEGIN(MCOMMENT);
    } else {
        LOGT("Nested multiline comment start");
    }
    mcomment_count++;
}

<MCOMMENT>{MCOMMENT_END} {
    mcomment_count--;
    if (!mcomment_count) {
        LOGT("End of multiline comment");
        BEGIN(INITIAL);
    } else {
        LOGT("Nested multiline comment end");
    }
}

<MCOMMENT><<EOF>> {
    cool_yylval.error_msg = "EOF in multiline comment";
    LOGE;
    return 0;
}

<MCOMMENT>{MCOMMENT} /* eat up all characters except new line in multi line comments in maximum chunks*/

<INITIAL,MCOMMENT>{NEWLINE} {
	curr_lineno++;
}

. {
    cool_yylval.error_msg = "Unexpected character";
    LOGE;
    return ERROR;
}

%%
/*
int main( int argc, char **argv ) {
    if (argc >= 0) {
        const char *filename = argv[1];
        yyin = fopen(filename, "r");
	if (!yyin) {
	    cerr << "Could not open " << filename << endl;
	    return 1;
        }
	// cout << "Opened " << filename << "\n";
    }
    yylineno = 0;
    while (yylex()) {}
//    cout << "line count: " << curr_lineno << "\n";
}
*/
