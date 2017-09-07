#define _GNU_SOURCE

// Compile with: 
//   gcc malloc_preload.c -fPIC -shared -ldl -o malloc_preload.so

#define INITIALIZED (NULL == real_malloc || NULL == real_free || NULL == real_calloc)

#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

// LD_PRELOAD Interceptor/Hooks
static void* (*real_malloc)(size_t)=NULL;
static void* (*real_calloc)(size_t, size_t)=NULL;
static void* (*real_realloc)(void*, size_t)=NULL;
static void (*real_free)(void*)=NULL;

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
    fprintf(stderr, "malloc(%lu) = ", size);
    p = real_malloc(size);
    fprintf(stderr, "%p\n", p);
    return p;
}

void free(void *p) 
{
    real_free(p);
    if (p)
    {
        fprintf(stderr, "free(%p) = <void>\n", p);
        fprintf(stderr, "p->fd = %p\n", *(void **)(p));
    }
}


// From: https://stackoverflow.com/a/10008252
void *realloc(void *ptr, size_t size)
{
    void *nptr = NULL;
    if (real_realloc == NULL)
    {
        nptr = real_malloc(size);
        if (nptr && ptr)
        {
            memmove(nptr, ptr, size);
            real_free(ptr);
        }
    }
    else
    {
        nptr = real_realloc(ptr, size);
    }
    fprintf(stderr, "realloc(%p, %lu) = %p\n", ptr, size, nptr);
    return nptr;
}

void *calloc(size_t nmemb, size_t size)
{
    if (INITIALIZED) {
        mtrace_init();
    }
    void *ptr = NULL;
    if (real_calloc == NULL)
    {
        ptr = real_malloc(nmemb*size);
        if (ptr)
            memset(ptr, 0, nmemb*size);
    }
    else
    {
        ptr = real_calloc(nmemb, size);
    }
    fprintf(stderr, "calloc(%lu, %lu) = %p\n", nmemb, size, ptr);
    return ptr;
}


