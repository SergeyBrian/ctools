#ifndef CTOOLS_CTOOLS_H
#define CTOOLS_CTOOLS_H

#ifdef _WIN32
typdef unsigned int uint;
#else
#include <sys/types.h>
#endif

#define TRUE 1
#define FALSE 0
#define NULL ((void *) 0)

#ifndef NO_MEMORY_DEBUG
#define CT_MEMORY_DEBUG
#else
#include <stdlib.h>
#endif

#ifdef CT_MEMORY_DEBUG
extern void *ct_debug_mem_malloc(uint size, char *file, uint line);
extern void *ct_debug_mem_realloc(void *ptr, uint size, char *file, uint line);
extern void *ct_debug_mem_calloc(uint size, uint count, char *file, uint line);
extern void ct_debug_mem_free(void *buf, char *file, uint line);

extern void ct_debug_mem_print(uint min_allocs);
extern void ct_debug_mem_comment(void *ptr, char *comment);
extern void ct_debug_mem_reset();
extern void ct_debug_mem_query(void *ptr, char **file, uint *line, __uint64_t *size);

#define malloc(n) ct_debug_mem_malloc(n, __FILE__, __LINE__)
#define calloc(n, m) ct_debug_mem_calloc(n, m, __FILE__, __LINE__)
#define realloc(n, m) ct_debug_mem_realloc(n, m, __FILE__, __LINE__)
#define free(n) ct_debug_mem_free(n, __FILE__, __LINE__)
#endif

typedef struct {
    uint type_size;
    uint size;
    uint capacity;
    uint resize_factor;
    void **nodes;
} List;

extern List *ct_list_init(uint type_size);
extern List *ct_list_init_ex(uint type_size, uint capacity, uint resize_factor);
extern void ct_list_destroy(List *list);
extern void ct_list_destroy_ex(List *list, void (* node_destructor) (void *val), uint keep_nodes);
extern void ct_list_append(List *list, void *value);
extern void ct_list_insert(List *list, void *value, uint pos);
extern void *ct_list_get(List *list, uint pos);
extern void ct_list_replace(List *list, uint pos, void *value);
extern void ct_list_remove_first(List *list);
extern void ct_list_remove_last(List *list);
extern void ct_list_remove_pos(List *list, uint pos);
extern void ct_list_remove_pos_ex(List *list, uint pos, void (* node_destructor) (void *val), uint keep_nodes);
extern void ct_list_remove_val(List *list, void *value, uint count);
extern int ct_list_find(List *list, void *value);
extern void ct_list_swap(List *list, uint pos1, uint pos2);
extern uint ct_list_compare(List *list1, List *list2);
extern void ct_list_merge(List *dest, List *src);
extern void ct_list_cat(List *dest, List *src);
extern void ct_list_clear(List *list);
extern void ct_list_clear_ex(List *list, void (* node_destructor) (void *val), uint keep_nodes);
extern void ct_list_reverse(List *list);
extern List *ct_list_copy(List *dest, List *src);
extern void ct_list_sort(List *list, uint (* func) (void *val), uint desc);
extern List *ct_list_filter(List *list, uint (* func) (void *val));
extern void ct_list_map(List *list, void (* func) (void *val));
extern uint ct_list_size(List *list);
extern uint ct_list_all(List *list, uint (* func) (void *val));
extern uint ct_list_any(List *list, uint (* func) (void *val));
extern uint ct_list_len(List *list);
extern void *ct_list_max(List *list, uint (* greater) (void *a, void *b), void *min_possible_value);
extern void *ct_list_min(List *list, uint (* less) (void *a, void *b), void *max_possible_value);

#endif //CTOOLS_CTOOLS_H
