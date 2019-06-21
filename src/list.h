#ifndef WASM_INTERPRETER_LIST_H
#define WASM_INTERPRETER_LIST_H

#include <stdlib.h>
#include "util.h"
#include "type.h"

typedef struct node {
    struct node *next;
    void *data;
} node_t;

void list_init(node_t **head_ref);

void insert_first(node_t **head_ref, void *new_data, uint32_t data_size);

void insert_last(node_t **head_ref, void *new_data, uint32_t data_size);

void delete_first(node_t **head_ref);

void *get_at(node_t **head_ref, uint32_t idx);

size_t length(node_t **head_ref);

#endif //WASM_INTERPRETER_LIST_H
