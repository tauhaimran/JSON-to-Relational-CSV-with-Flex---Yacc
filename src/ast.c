// Tauha Imran, 22i-1239
// Hussain Ali, 22i-0902

#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Create an object node */
struct ast_node *create_object_node(struct ast_node *pairs) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    node->type = NODE_OBJECT;
    node->data.object.pairs = pairs;
    node->next = NULL;
    return node;
}

/* Create an array node */
struct ast_node *create_array_node(struct ast_node *elements) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    node->type = NODE_ARRAY;
    node->data.array.elements = elements;
    node->next = NULL;
    return node;
}

/* Create a string node */
struct ast_node *create_string_node(char *value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    node->type = NODE_STRING;
    node->data.string = value; /* Parser allocates, we take ownership */
    node->next = NULL;
    return node;
}

/* Create a number node */
struct ast_node *create_number_node(char *value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    node->type = NODE_NUMBER;
    node->data.number = value; /* Parser allocates, we take ownership */
    node->next = NULL;
    return node;
}

/* Create a boolean node */
struct ast_node *create_bool_node(char *value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    node->type = NODE_BOOL;
    node->data.boolean = value; /* Parser allocates, we take ownership */
    node->next = NULL;
    return node;
}

/* Create a null node */
struct ast_node *create_null_node(char *value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    node->type = NODE_NULL;
    node->data.null = value; /* Parser allocates, we take ownership */
    node->next = NULL;
    return node;
}

/* Create a key-value pair node */
struct ast_node *create_pair_node(char *key, struct ast_node *value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    node->type = NODE_PAIR;
    node->data.pair.key = key; /* Parser allocates, we take ownership */
    node->data.pair.value = value;
    node->next = NULL;
    return node;
}

/* Append a pair to a pair list */
struct ast_node *append_pair(struct ast_node *pair, struct ast_node *next) {
    pair->next = next;
    return pair;
}

/* Append an element to an element list */
struct ast_node *append_element(struct ast_node *element, struct ast_node *next) {
    element->next = next;
    return element;
}

/* Print AST with indentation */
void print_ast(struct ast_node *node, int indent) {
    if (!node) return;

    /* Print indentation */
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    /* Print node based on type */
    switch (node->type) {
        case NODE_OBJECT:
            printf("OBJECT\n");
            print_ast(node->data.object.pairs, indent + 1);
            break;
        case NODE_ARRAY:
            printf("ARRAY\n");
            print_ast(node->data.array.elements, indent + 1);
            break;
        case NODE_STRING:
            printf("STRING: %s\n", node->data.string);
            break;
        case NODE_NUMBER:
            printf("NUMBER: %s\n", node->data.number);
            break;
        case NODE_BOOL:
            printf("BOOL: %s\n", node->data.boolean);
            break;
        case NODE_NULL:
            printf("NULL\n");
            break;
        case NODE_PAIR:
            printf("PAIR: %s\n", node->data.pair.key);
            print_ast(node->data.pair.value, indent + 1);
            break;
    }

    /* Print next node in list */
    print_ast(node->next, indent);
}

/* Free AST and all associated memory */
void free_ast(struct ast_node *node) {
    if (!node) return;

    /* Free next node in list */
    free_ast(node->next);

    /* Free node-specific data */
    switch (node->type) {
        case NODE_OBJECT:
            free_ast(node->data.object.pairs);
            break;
        case NODE_ARRAY:
            free_ast(node->data.array.elements);
            break;
        case NODE_STRING:
            free(node->data.string);
            break;
        case NODE_NUMBER:
            free(node->data.number);
            break;
        case NODE_BOOL:
            free(node->data.boolean);
            break;
        case NODE_NULL:
            free(node->data.null);
            break;
        case NODE_PAIR:
            free(node->data.pair.key);
            free_ast(node->data.pair.value);
            break;
    }

    /* Free the node itself */
    free(node);
}