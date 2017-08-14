#define _GNU_SOURCE

// Compile with: 
//   gcc malloc_preload.c -fPIC -shared -ldl -o malloc_preload.so

#define INITIALIZED (NULL == real_malloc || NULL == real_free || NULL == real_calloc || NULL == real_realloc)

#include <stdio.h>
#include <dlfcn.h>

static void* (*real_malloc)(size_t)=NULL;
static void (*real_free)(void*)=NULL;
static void* (*real_calloc)(size_t, size_t)=NULL;
static void* (*real_realloc)(void*, size_t)=NULL;

static void mtrace_init(void)
{
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_calloc = dlsym(RTLD_NEXT, "calloc");
    real_realloc = dlsym(RTLD_NEXT, "realloc");
    real_free = dlsym(RTLD_NEXT, "free");
    if (INITIALIZED) {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    }
}

void *malloc(size_t size)
{
    if (INITIALIZED) {
        mtrace_init();
    }

    void *p = NULL;
    fprintf(stderr, "malloc(%d) = ", size);
    p = real_malloc(size);
    fprintf(stderr, "%p\n", p);
    return p;
}


void *calloc(size_t nmbem, size_t size)
{
    if (INITIALIZED) {
        mtrace_init();
    }

    void *p = NULL;
    fprintf(stderr, "calloc(%d, %d) = ", nmbem, size);
    p = real_calloc(nmbem, size);
    fprintf(stderr, "%p\n", p);
    return p;
}

void *realloc(void *p, size_t size)
{
    if (INITIALIZED) {
        mtrace_init();
    }

    void *ptr = NULL;
    fprintf(stderr, "realloc(%p, %d) = ", p, size);
    ptr = real_realloc(p, size);
    fprintf(stderr, "%p\n", p);
    return p;
}

void free(void *p) 
{
    if (INITIALIZED) {
        mtrace_init();
    }

    real_free(p);
    if (p)
    {
        fprintf(stderr, "free(%p) = <void>\n", p);
        fprintf(stderr, "p->fd = %p\n", *(void **)(p));
    }
}
