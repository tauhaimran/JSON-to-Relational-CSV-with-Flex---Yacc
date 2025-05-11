/* Tauha Imran, 22i-1239 */
/* Hussain Ali, 22i-0902 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "csv.h"
#include "parser.tab.h"

void report_error(const char *msg, int line, int col, const char *context);
#ifndef YYSCANNER_T
#define YYSCANNER_T
typedef void* yyscan_t;
#endif
extern char *yytext;
extern struct ast_node *root;

// Prototypes for reentrant parser
extern int yylex(YYSTYPE *yylval_param, YYLTYPE *yylloc_param, yyscan_t yyscanner);
extern void yyerror(YYLTYPE *loc, yyscan_t scanner, const char *s);
%}

%locations
%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { yyscan_t scanner }

%union {
    char *str;
    struct ast_node *node;
}

%token LBRACE RBRACE LBRACKET RBRACKET COLON COMMA
%token <str> STRING NUMBER TRUE FALSE NULLVAL
%type <node> json value object pair pair_list array elements

%start json

%%

json: value { root = $1; };

value: object       { $$ = $1; }
     | array        { $$ = $1; }
     | STRING       { $$ = create_string_node($1); }
     | NUMBER       { $$ = create_number_node($1); }
     | TRUE         { $$ = create_bool_node($1); }
     | FALSE        { $$ = create_bool_node($1); }
     | NULLVAL      { $$ = create_null_node($1); }
     ;

object: LBRACE pair_list RBRACE { $$ = create_object_node($2); }
      | LBRACE error RBRACE { 
          report_error("Invalid object", @2.first_line, @2.first_column, "");
          YYABORT;
        }
      ;

pair_list: /* empty */          { $$ = NULL; }
         | pair                { $$ = $1; }
         | pair COMMA pair_list { $$ = append_pair($1, $3); }
         | pair error          { 
             report_error("Missing comma in object", @2.first_line, @2.first_column, NULL);
             YYABORT;
           }
         ;

pair: STRING COLON value { $$ = create_pair_node($1, $3); }
    | STRING error       { 
        report_error("Missing colon after key", @2.first_line, @2.first_column, NULL);
        YYABORT;
      }
    ;

array: LBRACKET elements RBRACKET { $$ = create_array_node($2); }
     | LBRACKET error RBRACKET { 
         report_error("Invalid array", @2.first_line, @2.first_column, NULL);
         YYABORT;
       }
     ;

elements: /* empty */          { $$ = NULL; }
        | value               { $$ = $1; }
        | value COMMA elements { $$ = append_element($1, $3); }
        | value error         { 
            report_error("Missing comma in array", @2.first_line, @2.first_column, NULL);
            YYABORT;
          }
        ;

%%

void yyerror(YYLTYPE *loc, yyscan_t scanner, const char *s) {
    report_error(s, loc->first_line, loc->first_column, yytext);
}