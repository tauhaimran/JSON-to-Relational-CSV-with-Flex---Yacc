// Tauha Imran, 22i-1239
// Hussain Ali, 22i-0902

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "csv.h"
#include "parser.tab.h"

extern struct ast_node *root;
extern int yyparse(void);

int main(int argc, char *argv[]) {
    int print_ast_flag = 0;
    char *out_dir = NULL;

    /* Parse command-line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--print-ast") == 0) {
            print_ast_flag = 1;
        } else if (strcmp(argv[i], "--out-dir") == 0 && i + 1 < argc) {
            out_dir = argv[++i];
        } else {
            fprintf(stderr, "Error: Invalid argument '%s'\n", argv[i]);
            return 1;
        }
    }

    /* Parse the JSON input */
    if (yyparse() != 0) {
        /* Parser handles error reporting */
        return 1;
    }

    /* Process the AST */
    if (print_ast_flag) {
        print_ast(root, 0);
    } else {
        process_ast(root, out_dir);
    }

    /* Clean up resources */
    free_ast(root);
    free_tables();

    return 0;
}