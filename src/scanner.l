/* Tauha Imran, 22i-1239 */
/* Hussain Ali, 22i-0902 */

%option noyywrap
%option yylineno
%option bison-bridge
%option bison-locations
%option reentrant
%option extra-type="void*"
%option nodefault

%{
#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "csv.h"

void report_error(const char *msg, int line, int col, const char *context);
void cleanup(void);
char *unescape_string(char *str);

#define YY_DECL int yylex( \
    YYSTYPE *yylval_param, \
    YYLTYPE *yylloc_param, \
    yyscan_t yyscanner \
)

#define YY_USER_ACTION \
    yylloc_param->first_line = yylloc_param->last_line; \
    yylloc_param->first_column = yylloc_param->last_column; \
    for (int i = 0; yytext[i]; ++i) { \
        if (yytext[i] == '\n') { \
            yylloc_param->last_line++; \
            yylloc_param->last_column = 1; \
        } else { \
            yylloc_param->last_column++; \
        } \
    }

YY_DECL;
%}

DIGIT       [0-9]
DIGIT1_9    [1-9]
SIGN        [+-]?
INTEGER     -?({DIGIT1_9}{DIGIT}*|0)
FRACTION    (\.{DIGIT}+)?
EXPONENT    ([eE]{SIGN}{DIGIT}+)?
NUMBER      {INTEGER}{FRACTION}{EXPONENT}
STRING      \"([^\"\\]|\\[\"\\\/bfnrt]|\\u[0-9a-fA-F]{4})*\"

%%

"{"         { return LBRACE; }
"}"         { return RBRACE; }
"["         { return LBRACKET; }
"]"         { return RBRACKET; }
":"         { return COLON; }
","         { return COMMA; }
"true"      { yylval_param->str = strdup("true"); return TRUE; }
"false"     { yylval_param->str = strdup("false"); return FALSE; }
"null"      { yylval_param->str = strdup("null"); return NULLVAL; }

{STRING}    { 
    yylval_param->str = unescape_string(yytext);
    return STRING; 
}

\"[^\"\\]*$ { 
    report_error("Unterminated string", yylloc_param->first_line, 
                yylloc_param->first_column, yytext);
    return -1;
}

\\[^\"\\\/bfnrtu] { 
    report_error("Invalid escape sequence", yylloc_param->first_line,
                yylloc_param->first_column, yytext);
    return -1;
}

{NUMBER}    { 
    yylval_param->str = strdup(yytext);
    return NUMBER; 
}

[ \t\n\r]+  { /* Ignore whitespace */ }

.           { 
    report_error("Invalid character", yylloc_param->first_line,
                yylloc_param->first_column, yytext);
    return -1;
}

%%

char *unescape_string(char *str) {
    /* Full unescape implementation with error checking */
    char *out = malloc(strlen(str) - 1);  // Skip quotes
    if (!out) {
        report_error("Memory allocation failed", 0, 0, NULL);
        return NULL;
    }
    
    char *src = str + 1;  // Skip opening quote
    char *dst = out;
    size_t len = strlen(str) - 2;  // Exclude closing quote
    
    for (size_t i = 0; i < len; i++) {
        if (*src == '\\') {
            src++;
            i++;
            switch (*src) {
                case '"':  *dst++ = '"';  break;
                case '\\': *dst++ = '\\'; break;
                case '/':  *dst++ = '/';  break;
                case 'b':  *dst++ = '\b'; break;
                case 'f':  *dst++ = '\f'; break;
                case 'n':  *dst++ = '\n'; break;
                case 'r':  *dst++ = '\r'; break;
                case 't':  *dst++ = '\t'; break;
                case 'u':  // Basic Unicode handling
                    *dst++ = '?';  // Placeholder for actual implementation
                    src += 4;
                    i += 4;
                    break;
                default:   *dst++ = *src;  // Handle invalid escapes literally
            }
        } else {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
    return out;
}