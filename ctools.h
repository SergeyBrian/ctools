#ifndef CTOOLS_CTOOLS_H
#define CTOOLS_CTOOLS_H

#ifdef _WIN32
typdef unsigned int uint;
#else
#include <sys/types.h>
#endif

#define TRUE 1
#define FALSE 0

#ifndef NO_MEMORY_DEBUG
#define CT_MEMORY_DEBUG
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

#endif //CTOOLS_CTOOLS_H
