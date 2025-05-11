#ifndef AST_H
#define AST_H

/* Enum for node types */
enum node_type {
    NODE_OBJECT,  /* JSON object: {key: value, ...} */
    NODE_ARRAY,   /* JSON array: [value, ...] */
    NODE_STRING,  /* JSON string: "text" */
    NODE_NUMBER,  /* JSON number: 123, 12.34, 1e-4 */
    NODE_BOOL,    /* JSON true/false */
    NODE_NULL,    /* JSON null */
    NODE_PAIR     /* Key-value pair in an object */
};

/* AST node structure */
struct ast_node {
    enum node_type type;
    union {
        struct { struct ast_node *pairs; } object;      /* Object's key-value pairs */
        struct { struct ast_node *elements; } array;    /* Array's elements */
        char *string;                                   /* String value */
        char *number;                                   /* Number value */
        char *boolean;                                  /* True/false value */
        char *null;                                     /* Null value */
        struct { char *key; struct ast_node *value; } pair; /* Key-value pair */
    } data;
    struct ast_node *next; /* For linking pairs or elements in a list */
};

/* Function prototypes */
struct ast_node *create_object_node(struct ast_node *pairs);
struct ast_node *create_array_node(struct ast_node *elements);
struct ast_node *create_string_node(char *value);
struct ast_node *create_number_node(char *value);
struct ast_node *create_bool_node(char *value);
struct ast_node *create_null_node(char *value);
struct ast_node *create_pair_node(char *key, struct ast_node *value);
struct ast_node *append_pair(struct ast_node *pair, struct ast_node *next);
struct ast_node *append_element(struct ast_node *element, struct ast_node *next);
void print_ast(struct ast_node *node, int indent);
void free_ast(struct ast_node *node);

#endif