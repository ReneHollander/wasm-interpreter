#include "list.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

void list_init(node_t **head_ref){
    *head_ref = NULL;
}

void insert_first(node_t **head_ref, void *new_data, uint32_t data_size) {
    node_t *new_node = (node_t *) malloc(sizeof(node_t));

    new_node->data = malloc(data_size);
    new_node->next = (*head_ref);

    int i;
    for (i = 0; i < data_size; i++)
        *(byte *) (new_node->data + i) = *(byte *) (new_data + i);

    (*head_ref) = new_node;
}

void insert_last(node_t **head_ref, void *new_data, uint32_t data_size) {
    node_t *new_node = (node_t *) malloc(sizeof(node_t));

    new_node->data = malloc(data_size);
    new_node->next = NULL;

    int i;
    for (i = 0; i < data_size; i++)
        *(byte *) (new_node->data + i) = *(byte *) (new_data + i);

    node_t *cur = *head_ref;
    if (cur == NULL) {
        *head_ref = new_node;
        return;
    }

    for (; cur->next != NULL;) {
        cur = cur->next;
    }
    cur->next = new_node;
}

void delete_first(node_t **head_ref) {
    if (*head_ref == NULL) {
        fprintf(stderr, "tried to delete element from empty list\n");
        exit(EXIT_FAILURE);
    }

    node_t *tmp = *head_ref;
    *head_ref = (*head_ref)->next;
    free(tmp->data);
    free(tmp);
}

void *get_at(node_t **head_ref, uint32_t idx) {
    node_t *cur = *head_ref;

    for (int i = 0; cur != NULL; i++) {
        if (i == idx) {
            return cur->data;
        }
        cur = cur->next;
    }

    return NULL;
}

size_t length(node_t **head_ref) {
    node_t *cur = *head_ref;
    size_t length = 0;

    for (int i = 0; cur != NULL; i++) {
        length++;
        cur = cur->next;
    }

    return length;
}

