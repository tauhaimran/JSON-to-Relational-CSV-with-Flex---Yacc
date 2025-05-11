%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int line, column;
extern void yyerror(const char *s);
extern int yylex(void);

/* Global AST root */
struct ast_node *root = NULL;
%}

/* Union for token and non-terminal values */
%union {
    char *str;
    struct ast_node *node;
}

/* Token declarations from scanner.l */
%token LBRACE RBRACE LBRACKET RBRACKET COLON COMMA TRUE FALSE NULLVAL
%token <str> STRING NUMBER

/* Non-terminal types */
%type <node> json value object pair pair_list array elements
%type <str> TRUE FALSE NULLVAL

/* Start symbol */
%start json

%%

json: value { root = $1; }
    ;

value: object       { $$ = $1; }
     | array        { $$ = $1; }
     | STRING       { $$ = create_string_node($1); }
     | NUMBER       { $$ = create_number_node($1); }
     | TRUE         { $$ = create_bool_node($1); }
     | FALSE        { $$ = create_bool_node($1); }
     | NULLVAL      { $$ = create_null_node($1); }
     ;

object: LBRACE pair_list RBRACE { $$ = create_object_node($2); }
      ;

pair_list: /* empty */          { $$ = NULL; }
         | pair                { $$ = $1; }
         | pair COMMA pair_list { $$ = append_pair($1, $3); }
         ;

pair: STRING COLON value { $$ = create_pair_node($1, $3); }
    ;

array: LBRACKET elements RBRACKET { $$ = create_array_node($2); }
     ;

elements: /* empty */          { $$ = NULL; }
        | value               { $$ = $1; }
        | value COMMA elements { $$ = append_element($1, $3); }
        ;

%%

/* Error handling */
void yyerror(const char *s) {
    fprintf(stderr, "Error: %s at line %d, column %d\n", s, line, column);
    exit(1);
}