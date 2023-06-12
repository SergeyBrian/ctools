#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_MEMORY_DEBUG

#include "ctools.h"

#define MAX_FILE_NAME_LEN 256
#define MAX_COMMENT_LEN 256

#define MAX_ALLOC_LINES 2048
#define DEF_ALLOC_CAPACITY 10

typedef struct {
    void *ptr;
    __uint64_t size;
    char comment[MAX_COMMENT_LEN];
    uint is_freed;
} Allocation;

typedef struct {
    uint line;
    char file[MAX_FILE_NAME_LEN];
    Allocation *allocs;
    uint alloc_count;
    uint alloc_capacity;
} MemAllocLine;

MemAllocLine ct_alloc_lines[MAX_ALLOC_LINES] = {0};
uint alloc_lines_count = 0;

uint ct_total_alloc_size = 0;

void ct_debug_mem_add_alloc(void *ptr, uint size, char *file, uint line) {
    uint i = 0;
    MemAllocLine *alloc_line = ct_alloc_lines;

    ct_total_alloc_size += size;
    uint should_replace = TRUE;

    for (; i < alloc_lines_count; i++) {
        alloc_line = &ct_alloc_lines[i];
        if (alloc_line->line == line && !strcmp(alloc_line->file, file)) {
            should_replace = FALSE;
            break;
        }
    }
    alloc_line += (alloc_lines_count > 0 && should_replace);
    alloc_lines_count++;

    if (alloc_line->alloc_count == 0) {
        alloc_line->alloc_capacity = DEF_ALLOC_CAPACITY;

        alloc_line->allocs = calloc(sizeof(Allocation), alloc_line->alloc_capacity);
    }
    if (alloc_line->alloc_count == alloc_line->alloc_capacity) {
        alloc_line->alloc_capacity *= 2;
        alloc_line->allocs = realloc(alloc_line->allocs, alloc_line->alloc_capacity);
    }
    alloc_line->line = line;
    strncpy(alloc_line->file, file, MAX_FILE_NAME_LEN);

    Allocation *allocation = &alloc_line->allocs[alloc_line->alloc_count++];
    allocation->ptr = ptr;
    allocation->size = size;
    allocation->is_freed = FALSE;
}

Allocation *ct_debug_mem_get_alloc(void *ptr) {
    for (uint i = 0; i < alloc_lines_count; i++) {
        MemAllocLine *line = &ct_alloc_lines[i];
        for (uint j = 0; j < line->alloc_count; j++) {
            if (line->allocs[j].ptr == ptr) return &line->allocs[j];
        }
    }
    printf("ERROR: allocation at %p not found\n", ptr);
    exit(1);
}

void ct_debug_mem_comment(void *ptr, char *comment) {
    Allocation *allocation = ct_debug_mem_get_alloc(ptr);
    strncpy(allocation->comment, comment, MAX_COMMENT_LEN);
}

void *ct_debug_mem_malloc(uint size, char *file, uint line) {
    void *ret_ptr = malloc(size);
    if (ret_ptr == NULL) {
        printf("ERROR: malloc returned NULL at file %s line %d\n", file, line);
        exit(1);
    }
    ct_debug_mem_add_alloc(ret_ptr, size, file, line);

    return ret_ptr;
}

void *ct_debug_mem_calloc(uint size, uint count, char *file, uint line) {
    void *ret_ptr = ct_debug_mem_malloc(size * count, file, line);
    memset(ret_ptr, 0, size);
    return ret_ptr;
}

void *ct_debug_mem_realloc(void *ptr, uint size, char *file, uint line) {
    void *ret_ptr = realloc(ptr, size);
    if (ret_ptr == NULL) {
        printf("ERROR: realloc returned NULL at file %s line %d\n", file, line);
        exit(1);
    }

    Allocation *allocation = ct_debug_mem_get_alloc(ptr);
    ct_total_alloc_size += size - allocation->size;

    allocation->size = size;
    allocation->ptr = ret_ptr;

    return ret_ptr;
}

void ct_debug_mem_free(void *ptr, char *file, uint line) {
    Allocation *allocation = NULL;
    for (uint i = 0; i < alloc_lines_count; i++) {
        MemAllocLine *line = &ct_alloc_lines[i];
        for (uint j = 0; j < line->alloc_count; j++) {
            if (line->allocs[j].ptr != ptr) continue;
            if (line->allocs[j].is_freed) continue;

            allocation = &line->allocs[j];
            break;
        }
    }

    if (!allocation) {
        printf("ERROR: double free at file %s line %d\n", file, line);
        exit(1);
    }
    allocation->is_freed = TRUE;
    free(ptr);
    ct_total_alloc_size -= allocation->size;
}

void ct_debug_mem_print(uint min_alloc) {
    for (uint i = 0; i < alloc_lines_count; i++) {
        MemAllocLine *line = &ct_alloc_lines[i];

        if (line->alloc_count <= min_alloc) continue;

        printf("File: %s Line: %d\n", line->file, line->line);
        for (uint j = 0; j < line->alloc_count; j++) {
            Allocation *allocation = &line->allocs[j];
            printf("\tAllocated %lu bytes at %p %s %s\n", allocation->size,
                   allocation->ptr, allocation->comment,
                   (allocation->is_freed ? "[FREE]" : ""));
        }
    }

    printf("\nTotal memory not freed: %d\n", ct_total_alloc_size);
}

void ct_debug_mem_query(void *ptr, char **file, uint *line, __uint64_t *size) {
    for (uint i = 0; i < alloc_lines_count; i++) {
        MemAllocLine *alloc_line = &ct_alloc_lines[i];
        for (uint j = 0; j < alloc_line->alloc_count; j++) {
            Allocation *allocation = &alloc_line->allocs[j];
            if (allocation->ptr == ptr) {
                *file = alloc_line->file;
                *line = alloc_line->line;
                *size = allocation->size;
                return;
            }
        }
    }
}
