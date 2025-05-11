/* Tauha Imran, 22i-1239 */
/* Hussain Ali, 22i-0902 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "csv.h"
#include "parser.tab.h"

struct ast_node *root = NULL;
int line = 1;
int column = 1;
char *yytext;

void report_error(const char *msg, int line, int col, const char *context);

/* Initialize scanner and parser */
int main(int argc, char *argv[]) {
    yyscan_t scanner;
    if (yylex_init(&scanner)) {
        fprintf(stderr, "Error: Failed to initialize scanner\n");
        return 1;
    }

    int print_ast_flag = 0;
    char *out_dir = NULL;

    /* Parse command-line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--print-ast") == 0) {
            print_ast_flag = 1;
        } else if (strcmp(argv[i], "--out-dir") == 0) {
            if (i + 1 >= argc) {
                report_error("Missing directory argument for --out-dir", 0, 0, NULL);
            }
            out_dir = argv[++i];
        } else {
            report_error("Invalid command-line argument", 0, 0, argv[i]);
        }
    }

    /* Parse the JSON input */
    if (yyparse(scanner)) {
        /* Parser handles error reporting via yyerror */
        yylex_destroy(scanner);
        return 1;
    }

    /* Process the AST */
    if (print_ast_flag) {
        print_ast(root, 0);
    } else {
        process_ast(root, out_dir);
    }

    /* Clean up resources */
    cleanup();
    yylex_destroy(scanner);
    return 0;
}