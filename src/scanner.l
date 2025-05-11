/* Tauha Imran, 22i-1239 */
/* Hussain Ali, 22i-0902 */

%{
#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>

/* Track line and column for error reporting */
int line = 1, column = 1;

/* Update line and column based on input text */
void update_position(char *text) {
    for (char *c = text; *c; c++) {
        if (*c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
}

/* Handle string unescaping (basic implementation) */
char *unescape_string(char *str) {
    char *result = strdup(str);
    // Strip quotes
    size_t len = strlen(result);
    if (len >= 2) {
        result[len - 1] = '\0';
        memmove(result, result + 1, len - 1);
    }
    // TODO: Implement full escape handling (\n, \t, \uXXXX, etc.)
    return result;
}
%}

%option noyywrap

/* Regular expressions for JSON tokens */
DIGIT       [0-9]
DIGIT1_9    [1-9]
SIGN        [+-]?
INTEGER     -?({DIGIT1_9}{DIGIT}*|0)
FRACTION    (\.{DIGIT}+)?
EXPONENT    ([eE]{SIGN}{DIGIT}+)?
NUMBER      {INTEGER}{FRACTION}{EXPONENT}
STRING      \"([^\"\\]|\\[\"\\\/bfnrt]|\\u[0-9a-fA-F]{4})*\"

%%
"{"         { update_position(yytext); return LBRACE; }
"}"         { update_position(yytext); return RBRACE; }
"["         { update_position(yytext); return LBRACKET; }
"]"         { update_position(yytext); return RBRACKET; }
":"         { update_position(yytext); return COLON; }
","         { update_position(yytext); return COMMA; }
"true"      { update_position(yytext); yylval.str = strdup("true"); return TRUE; }
"false"     { update_position(yytext); yylval.str = strdup("false"); return FALSE; }
"null"      { update_position(yytext); yylval.str = strdup("null"); return NULLVAL; }

{STRING}    { 
    update_position(yytext); 
    yylval.str = unescape_string(yytext); 
    return STRING; 
}

{NUMBER}    { 
    update_position(yytext); 
    yylval.str = strdup(yytext); 
    return NUMBER; 
}

[ \t\n\r]+  { update_position(yytext); /* Ignore whitespace */ }

.           { 
    fprintf(stderr, "Error: Invalid character '%s' at line %d, column %d\n", 
            yytext, line, column); 
    exit(1); 
}
%%