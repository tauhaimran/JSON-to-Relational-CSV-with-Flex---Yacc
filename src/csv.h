#ifndef CSV_H
#define CSV_H

#include "ast.h"

/* Structure for a table column */
struct column {
    char *name;
    struct column *next;
};

/* Structure for a table */
struct table {
    char *name;
    struct column *columns;
    int next_id; /* For assigning primary keys */
    FILE *file;  /* File handle for CSV output */
    struct table *next;
};

/* Function prototypes */
struct table *create_table(const char *name, const char *out_dir);
struct table *find_table(const char *name);
void add_column(struct table *table, const char *name);
void write_header(struct table *table);
void process_array(struct ast_node *node, struct table *parent, const char *table_name, int parent_id, const char *out_dir);
void process_object(struct ast_node *node, struct table *parent, const char *table_name, int parent_id, const char *out_dir);
void process_ast(struct ast_node *node, const char *out_dir);
void free_tables(void);

#endif