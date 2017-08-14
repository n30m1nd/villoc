#define _GNU_SOURCE

// Compile with: 
//   gcc malloc_preload.c -fPIC -shared -ldl -o malloc_preload.so

#define INITIALIZED (NULL == real_malloc || NULL == real_free)

#include <stdio.h>
#include <dlfcn.h>

static void* (*real_malloc)(size_t)=NULL;
static void (*real_free)(void*)=NULL;

static void mtrace_init(void)
{
    real_malloc = dlsym(RTLD_NEXT, "malloc");
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
    fprintf(stderr, "malloc(%lu) = ", size);
    p = real_malloc(size);
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
