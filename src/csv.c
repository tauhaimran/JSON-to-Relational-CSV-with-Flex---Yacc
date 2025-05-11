#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv.h"

/* Global list of tables */
static struct table *tables = NULL;

/* Create a new table */
struct table *create_table(const char *name, const const char *out_dir) {
    struct table *table = malloc(sizeof(struct table));
    if (!table) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    table->name = strdup(name);
    table->columns = NULL;
    table->next_id = 1;
    table->next = tables;
    tables = table;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s.csv", out_dir ? out_dir : ".", name);
    table->file = fopen(path, "w");
    if (!table->file) {
        fprintf(stderr, "Error: Cannot open %s\n", path);
        exit(1);
    }
    return table;
}

/* Find a table by name */
struct table *find_table(const char *name) {
    for (struct table *t = tables; t; t = t->next) {
        if (strcmp(t->name, name) == 0) {
            return t;
        }
    }
    return NULL;
}

/* Add a column to a table */
void add_column(struct table *table, const char *name) {
    struct column *col = malloc(sizeof(struct column));
    if (!col) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    col->name = strdup(name);
    col->next = table->columns;
    table->columns = col;
}

/* Write the CSV header for a table */
void write_header(struct table *table) {
    fprintf(table->file, "id");
    for (struct column *col = table->columns; col; col = col->next) {
        fprintf(table->file, ",%s", col->name);
    }
    fprintf(table->file, "\n");
}

/* Process an array node */
void process_array(struct ast_node *node, struct table *parent, const char *table_name, int parent_id, const char *out_dir) {
    struct table *table = create_table(table_name, out_dir);
    add_column(table, "parent_id");
    add_column(table, "index");

    struct ast_node *elem = node->data.array.elements;
    if (elem && elem->type == NODE_OBJECT) {
        add_column(table, "seq");
        for (struct ast_node *pair = elem->data.object.pairs; pair; pair = pair->next) {
            add_column(table, pair->data.pair.key);
        }
        write_header(table);
        int seq = 0;
        for (; elem; elem = elem->next) {
            int id = table->next_id++;
            fprintf(table->file, "%d,%d,%d", id, parent_id, seq++);
            for (struct ast_node *pair = elem->data.object.pairs; pair; pair = pair->next) {
                fprintf(table->file, ",");
                struct ast_node *value = pair->data.pair.value;
                if (value->type == NODE_STRING) {
                    fprintf(table->file, "\"%s\"", value->data.string);
                } else if (value->type == NODE_NUMBER) {
                    fprintf(table->file, "%s", value->data.number);
                } else if (value->type == NODE_BOOL) {
                    fprintf(table->file, "%s", value->data.boolean);
                } else if (value->type == NODE_NULL) {
                    /* Empty field for NULL */
                }
            }
            fprintf(table->file, "\n");
        }
    } else {
        add_column(table, "value");
        write_header(table);
        int index = 0;
        for (; elem; elem = elem->next) {
            int id = table->next_id++;
            fprintf(table->file, "%d,%d,%d,", id, parent_id, index++);
            if (elem->type == NODE_STRING) {
                fprintf(table->file, "\"%s\"", elem->data.string);
            } else if (elem->type == NODE_NUMBER) {
                fprintf(table->file, "%s", elem->data.number);
            } else if (elem->type == NODE_BOOL) {
                fprintf(table->file, "%s", elem->data.boolean);
            } else if (elem->type == NODE_NULL) {
                /* Empty field for NULL */
            }
            fprintf(table->file, "\n");
        }
    }
    fflush(table->file);
}

/* Process an object node */
void process_object(struct ast_node *node, struct table *parent, const char *table_name, int parent_id, const char *out_dir) {
    struct table *table = find_table(table_name);
    if (!table) {
        table = create_table(table_name, out_dir);
        if (parent) {
            add_column(table, "parent_id");
        }
        for (struct ast_node *pair = node->data.object.pairs; pair; pair = pair->next) {
            add_column(table, pair->data.pair.key);
        }
        write_header(table);
    }

    int id = table->next_id++;
    fprintf(table->file, "%d", id);
    if (parent) {
        fprintf(table->file, ",%d", parent_id);
    }
    for (struct ast_node *pair = node->data.object.pairs; pair; pair = pair->next) {
        fprintf(table->file, ",");
        struct ast_node *value = pair->data.pair.value;
        if (value->type == NODE_STRING) {
            fprintf(table->file, "\"%s\"", value->data.string);
        } else if (value->type == NODE_NUMBER) {
            fprintf(table->file, "%s", value->data.number);
        } else if (value->type == NODE_BOOL) {
            fprintf(table->file, "%s", value->data.boolean);
        } else if (value->type == NODE_NULL) {
            /* Empty field for NULL */
        } else if (value->type == NODE_OBJECT) {
            char child_name[256];
            snprintf(child_name, sizeof(child_name), "%s_%s", table_name, pair->data.pair.key);
            process_object(value, table, child_name, id, out_dir);
            fprintf(table->file, "%d", table->next_id - 1);
        } else if (value->type == NODE_ARRAY) {
            char child_name[256];
            snprintf(child_name, sizeof(child_name), "%s_%s", table_name, pair->data.pair.key);
            process_array(value, table, child_name, id, out_dir);
        }
    }
    fprintf(table->file, "\n");
    fflush(table->file);
}

/* Process the entire AST */
void process_ast(struct ast_node *node, const char *out_dir) {
    if (!node) return;
    if (node->type == NODE_OBJECT) {
        process_object(node, NULL, "root", 0, out_dir);
    } else if (node->type == NODE_ARRAY) {
        process_array(node, NULL, "root", 0, out_dir);
    }
}

/* Free all tables and their resources */
void free_tables(void) {
    while (tables) {
        struct table *next = tables->next;
        while (tables->columns) {
            struct column *col = tables->columns;
            tables->columns = col->next;
            free(col->name);
            free(col);
        }
        fclose(tables->file);
        free(tables->name);
        free(tables);
        tables = next;
    }
}