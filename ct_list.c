#include "ctools.h"
#include <string.h>

#define DEF_LIST_CAPACITY 10
#define DEF_LIST_RESIZE_FACTOR 2
#define MAX_DELETE_BUF_SIZE 1024

List *ct_list_init(uint type_size) {
    return ct_list_init_ex(type_size, DEF_LIST_CAPACITY, DEF_LIST_RESIZE_FACTOR);
}

List *ct_list_init_ex(uint type_size, uint capacity, uint resize_factor) {
    List *list = malloc(sizeof(List));
    list->nodes = malloc(type_size * capacity);
    list->capacity = capacity;
    list->resize_factor = resize_factor;
    list->type_size = type_size;
    list->size = 0;

    return list;
}

void ct_list_resize(List *list) {
    if (list->size < list->capacity)
        return;
    list->capacity *= list->resize_factor;
    char *dest = (char *)list->nodes;
    list->nodes = realloc(dest, list->capacity * list->type_size);
}

void free_func(void *ptr) { free(ptr); }

void ct_list_destroy(List *list) { ct_list_destroy_ex(list, NULL, TRUE); }

void ct_list_destroy_ex(List *list, void (*node_destructor)(void *val),
        uint keep_nodes) {
    if (!node_destructor)
        node_destructor = &free_func;
    if (!keep_nodes) {
        for (uint i = 0; i < list->size; i++) {
            node_destructor(list->nodes[i]);
        }
    }

    free(list->nodes);
    free(list);
}

void ct_list_append(List *list, void *value) {
    list->size++;
    ct_list_resize(list);
    char *dest = (char *)list->nodes;
    dest += (list->size - 1) * list->type_size;
    if (!value) return;
    memcpy(dest, value, list->type_size);
}

void ct_list_insert(List *list, void *value, uint pos) {
    if (pos >= list->size) {
        ct_list_append(list, value);
        return;
    }

    list->size++;
    ct_list_resize(list);
    char *src_pos = (char *)list->nodes + pos * list->type_size;
    char *dest_pos = src_pos + list->type_size;
    memmove(dest_pos, src_pos, list->type_size * (list->size - pos));

    if (!value) return;
    char *dest = (char *)list->nodes;
    dest += pos * list->type_size;
    memcpy(dest, value, list->type_size);
}

void *ct_list_get(List *list, uint pos) {
    char *data = (char *)list->nodes;
    data += pos * list->type_size;
    return (void *)data;
}

void ct_list_replace(List *list, uint pos, void *value) {
    char *dest = (char *)list->nodes;
    dest += pos * list->type_size;
    memcpy(dest, value, list->type_size);
}

void ct_list_remove_pos_ex(List *list, uint pos,
        void (*node_destructor)(void *val),
        uint keep_nodes) {
    list->size--;

    if (!node_destructor) {
        node_destructor = &free_func;
    }

    char *dest = (char *)list->nodes;
    dest += pos * list->type_size;

    if (!keep_nodes) {
        node_destructor((void *)dest);
    }

    memmove(dest, dest + list->type_size, (list->size - pos) * list->type_size);
}

void ct_list_remove_pos(List *list, uint pos) {
    ct_list_remove_pos_ex(list, pos, NULL, TRUE);
}

void ct_list_remove_first(List *list) { ct_list_remove_pos(list, 0); }

void ct_list_remove_last(List *list) {
    ct_list_remove_pos(list, list->size - 1);
}

void ct_list_remove_val(List *list, void *val, uint count) {
    char *dest = (char *)list->nodes;

    uint delete_positions[MAX_DELETE_BUF_SIZE] = {};
    uint delete_count = 0;

    for (uint i = 0; i < list->size && delete_count < count;
            dest += list->type_size, i++) {
        if (memcmp((void *)dest, val, list->type_size))
            continue;
        delete_positions[delete_count] = i - delete_count;
        delete_count++;
    }

    for (uint i = 0; i < delete_count; i++) {
        ct_list_remove_pos(list, delete_positions[i]);
    }
}

int ct_list_find(List *list, void *val) {
    int pos = -1;
    char *item = (char *)list->nodes;
    for (uint i = 0; i < list->size; i++) {
        if (memcmp((void *)item, val, list->type_size)) {
            item += list->type_size;
            continue;
        }
        pos = i;
        break;
    }

    return pos;
}

void ct_list_swap(List *list, uint pos1, uint pos2) {
    char *swap_mem = malloc(list->type_size);
    char *dest = (char *) list->nodes + pos1 * list->type_size;
    char *src = (char *) list->nodes + pos2 * list->type_size;

    memcpy(swap_mem, dest, list->type_size);
    memcpy(dest, src, list->type_size);
    memcpy(src, swap_mem, list->type_size);

    free(swap_mem);
}

uint ct_list_compare(List *list1, List *list2) {
    if (list1->type_size != list2->type_size) return 1;
    if (list1->size != list2->size) return 2;

    char *pos1 = (char *)list1->nodes;
    char *pos2 = (char *)list2->nodes;
    for (uint i = 0; i < list1->size; i++) {
        if (memcmp(pos1, pos2, list1->type_size)) return 3;
        pos1 += list1->type_size;
        pos2 += list2->type_size;
    }

    return 0;
}

void ct_list_cat(List *dest, List *src) {
    uint old_size = dest->size;
    dest->size += src->size;
    ct_list_resize(dest);

    char *dest_pos = ((char *) dest->nodes) + old_size * dest->type_size;
    char *src_pos = (char *) src->nodes;

    for (uint i = 0; i < src->size; i++) {
        memcpy(dest_pos + i * dest->type_size, src_pos + i * dest->type_size, dest->type_size);
    }
}

void ct_list_clear_ex(List *list, void (*node_destructor)(void *), uint keep_nodes) {
    if (!node_destructor) node_destructor = &free_func;
    if (!keep_nodes) {
        char *node = (char *)list->nodes;
        for (uint i = 0; i < list->size; i++) {
            node_destructor((void *)node);
            node += list->type_size;
        }
    }

    char *nodes = (char *)list->nodes;
    memset(nodes, 0, list->size * list->type_size);

    list->size = 0;
}

void ct_list_clear(List *list) {
    ct_list_clear_ex(list, NULL, TRUE);
}


void ct_list_reverse(List *list) {
    uint middle_pos = list->size / 2;
    for (uint i = 0; i < middle_pos; i++) {
        ct_list_swap(list, i, list->size - i - 1);
    }
}

List *ct_list_copy(List *dest, List *src) {
    dest->size = src->size;
    ct_list_resize(dest);
    char *dest_nodes = (char *)dest->nodes;
    char *src_nodes = (char *)src->nodes;
    memcpy(dest_nodes, src_nodes, src->type_size * src->size);
    return dest;
}

void ct_list_map(List *list, void (*func)(void *)) {
    char *node = (char *)list->nodes;
    for (uint i = 0; i < list->size; i++) {
        func(node);
        node += list->type_size;
    }
}

uint ct_list_size(List *list) {
    return list->size * list->type_size;
}

uint ct_list_all(List *list, uint (*func)(void *)) {
    char *node = (char *)list->nodes;
    for (uint i = 0; i < list->size; i++) {
        if (!func(node)) return FALSE;
        node += list->type_size;
    }

    return TRUE;
}

uint ct_list_any(List *list, uint (*func)(void *)) {

    char *node = (char *)list->nodes;
    for (uint i = 0; i < list->size; i++) {
        if (func(node)) return TRUE;
        node += list->type_size;
    }

    return FALSE;
}

uint ct_list_len(List *list) {
    return list->size;
}

void *ct_list_max(List *list, uint (* greater) (void *a, void *b), void *min_possible_value) {
    void *max_node = min_possible_value;
    
    char *node = (char *)list->nodes;
    for (uint i = 0; i < list->size; i++) {
        if (greater(node, max_node)) {
            max_node = node;
        }

        node += list->type_size;
    }

    return max_node;
}

void *ct_list_min(List *list, uint (* less) (void *a, void *b), void *max_possible_value) {
    void *min_node = max_possible_value;
    
    char *node = (char *)list->nodes;
    for (uint i = 0; i < list->size; i++) {
        if (less(node, min_node)) {
            min_node = node;
        }

        node += list->type_size;
    }

    return min_node;
}

void *ct_list_alloc_pos(List *list, uint pos) {
    ct_list_insert(list, NULL, pos);
    return ct_list_get(list, pos);
}
