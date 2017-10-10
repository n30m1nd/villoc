#define _GNU_SOURCE

// Compile with: 
//   gcc malloc_preload.c -fPIC -shared -ldl -o malloc_preload.so

#define ISINITIALIZED (NULL == real_malloc || NULL == real_free)

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <signal.h>

// LD_PRELOAD Interceptor/Hooks
static void* (*real_malloc)(size_t)=NULL;
static void* (*real_calloc)(size_t, size_t)=NULL;
static void* (*real_realloc)(void*, size_t)=NULL;
static void (*real_free)(void*)=NULL;

void abort_handler(int i)
{
    fprintf(stderr, "<no return ...>\n");
    exit(1);
}

static void mtrace_init(void)
{
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_calloc = dlsym(RTLD_NEXT, "calloc");
    real_realloc = dlsym(RTLD_NEXT, "realloc");
    real_free = dlsym(RTLD_NEXT, "free");
    setenv("MALLOC_CHECK_", "2", 1);
    if (ISINITIALIZED) {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
        abort();
    }

    if (signal(SIGABRT, abort_handler) == SIG_ERR) {
        fprintf(stderr, "Couldn't set signal handler\n");
        exit(1);
    }
}


// Hooking functions

void *malloc(size_t size)
{
    if (ISINITIALIZED) {
        mtrace_init();
    }

    void *p = NULL;
    fprintf(stderr, "malloc(%lu) = ", size);
    p = real_malloc(size);
    if (p)
        fprintf(stderr, "%p\n", p);
    else
        fprintf(stderr, "<error>\n");
    return p;
}

void free(void *p) 
{
    if (p)
    {
        fprintf(stderr, "free(%p) = ", p);
        //fprintf(stderr, "p->fd = %p\n", *(void **)(p));
        real_free(p);
        fprintf(stderr, "<void>\n");
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
            if (real_free == NULL) 
                mtrace_init();
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
    void *ptr = NULL;
    if (real_calloc == NULL)
    {
        ptr = malloc(nmemb*size);
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


