/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2010  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -----------------------------------------------------------------------------*/
 
/* created: 22/12/2006
   updated: 14/05/2010 */

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEMemory.h"

/**
 * \file SCEMemory.c
 * \copydoc memory
 * \brief Memory managment
 * 
 * \file SCEMemory.h
 * \copydoc memory
 * \brief Memory managment
 */

/**
 * \defgroup memory Memory managment
 * Wrapper around libc functions for memory managment
 * \ingroup utils
 */

/** @{ */

#define SCE_USE_MEMORY_MANAGER 1

#define SCE_NUM_MEMORY_ARRAYS 128
/* number of allocs per block */
#define SCE_ARRAY_BLOCK_SIZE 128

/**
 * \brief Stores metadata about memory blocks
 *
 * Used to know where a block was allocated.
 */
typedef struct SCE_SMemAlloc {
    const char *file;
    unsigned int line;
    size_t size;
    void *block; /* hack */
    struct SCE_SMemAlloc *next, *prev;
} SCE_SMemAlloc;

/** \brief Table of all allocations */
static SCE_SMemAlloc allocs = {
    "root allocations list", 0, 0, NULL, NULL, NULL
};
static pthread_mutex_t allocs_m;
static pthread_mutexattr_t allocs_mattr;


typedef struct SCE_SMemArrayBlock {
    SCE_SMemAlloc *allocs[SCE_ARRAY_BLOCK_SIZE];
    SCE_SMemAlloc *freeallocs[SCE_ARRAY_BLOCK_SIZE];
    int nfree;         /* number of free allocs in 'allocs' */
    struct SCE_SMemArrayBlock *next, *prev;
} SCE_SMemArrayBlock;

/* structure d'un tableau contenant une suite d'allocations de la meme taille */
typedef struct SCE_SMemArray {
    size_t alloc_size;         /* size of one allocation */
    SCE_SMemArrayBlock *root;  /* root block */
    SCE_SMemArrayBlock *last;  /* last block */
} SCE_SMemArray;

static SCE_SMemArray arrays[SCE_NUM_MEMORY_ARRAYS];
static pthread_mutex_t arrays_m = PTHREAD_MUTEX_INITIALIZER;

#define SCE_Mem_For(i) for ((i) = allocs.next; (i); (i) = (i)->next)

static void SCE_Mem_InitArray (SCE_SMemArray *a)
{
    a->alloc_size = 1; /* hop */
    a->root = NULL;
    a->last = NULL;
}

int SCE_Init_Mem (void)
{
    size_t i;
    for (i = 0; i < SCE_NUM_MEMORY_ARRAYS; i++) {
        SCE_Mem_InitArray (&arrays[i]);
        arrays[i].alloc_size = i + 1;
    }

#if 0
    /* lol pthread_mutex_recursive is not defined. */
    pthread_mutexattr_init (&allocs_mattr);
    pthread_mutexattr_settype (&allocs_mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init (&allocs_m, &allocs_mattr);
#else
    pthread_mutex_init (&allocs_m, NULL);
#endif

    return SCE_OK;
}
void SCE_Quit_Mem (void)
{
    /* tell the user: be sure the mutexes are unlocked */
    pthread_mutex_destroy (&allocs_m);
    pthread_mutex_destroy (&arrays_m);
    /* safe further re-init, keep the data in the state we got them
       at initialization */
    pthread_mutex_init (&allocs_m, NULL);
    pthread_mutex_init (&arrays_m, NULL);
}

static void SCE_Mem_InitAlloc (SCE_SMemAlloc *m)
{
    m->file = NULL;
    m->line = 1;
    m->size = 0;
    m->block = NULL;
    m->next = m->prev = NULL;
}

static void SCE_Mem_InitBlock (SCE_SMemArrayBlock *b)
{
    unsigned int i;
    for (i = 0; i < SCE_ARRAY_BLOCK_SIZE; i++) {
        b->allocs[i] = NULL;
        b->freeallocs[i] = NULL;
    }
    b->nfree = SCE_ARRAY_BLOCK_SIZE;
    b->next = b->prev = NULL;
}

/* functions managing the arrays */

static SCE_SMemArrayBlock* SCE_Mem_CreateBlock (size_t size)
{
    SCE_SMemArrayBlock *b = malloc (sizeof *b);
    if (!b)
        SCEE_Log (SCE_OUT_OF_MEMORY);
    else {
        size_t i;
        void *address = NULL;
        SCE_Mem_InitBlock (b);
        /* allocate the data */
        size += sizeof (SCE_SMemAlloc);
        b->allocs[0] = malloc (size * SCE_ARRAY_BLOCK_SIZE);
        if (!b->allocs[0]) {
            SCEE_Log (SCE_OUT_OF_MEMORY);
            free (b);
            return NULL;
        }
        SCE_Mem_InitAlloc (b->allocs[0]);
        b->allocs[0]->block = b;
        for (i = 1; i < SCE_ARRAY_BLOCK_SIZE; i++) {
            address = b->allocs[i-1];
            address = (void*)((size_t)address + size);
            b->freeallocs[i] = b->allocs[i] = address;
            SCE_Mem_InitAlloc (b->allocs[i]);
            b->allocs[i]->block = b;
        }
    }
    return b;
}

static void SCE_Mem_DeleteBlock (SCE_SMemArrayBlock *b)
{
    if (b) {
        free (b->allocs[0]);
        free (b);
    }
}

static int SCE_Mem_AddNewBlock (SCE_SMemArray *a)
{
    if (pthread_mutex_lock (&arrays_m) == 0) {
        if (!a->last) {
            a->last = a->root = SCE_Mem_CreateBlock (a->alloc_size);
            if (!a->last)
                return -1;
        } else {
            a->last->next = SCE_Mem_CreateBlock (a->alloc_size);
            if (!a->last->next)
                return -1;
            a->last->next->prev = a->last;
            a->last = a->last->next;
        }
        pthread_mutex_unlock (&arrays_m);
    }
    return 0;
}

static void SCE_Mem_EraseBlock (SCE_SMemArray *a, SCE_SMemArrayBlock *b)
{
    if (pthread_mutex_lock (&arrays_m) == 0) {
        if (b->prev)
            b->prev->next = b->next;
        else
            a->root = b->next;
        if (b->next)
            b->next->prev = b->prev;
        else
            a->last = b->prev;
        SCE_Mem_DeleteBlock (b);
        pthread_mutex_unlock (&arrays_m);
    }
}

static SCE_SMemAlloc* SCE_Mem_GetNextAllocInBlock (SCE_SMemArrayBlock *b)
{
#if 0
    return (b->nfree ? b->freeallocs[--b->nfree] : NULL);
#else
    /* not secure, but this function is (for now) only called if nfree > 0 */
    return b->freeallocs[--b->nfree];
#endif
}

static SCE_SMemAlloc* SCE_Mem_GetNextAlloc (SCE_SMemArray *a)
{
    SCE_SMemAlloc *alloc = NULL;
    SCE_SMemArrayBlock *b = NULL;
    b = a->last;
    if (pthread_mutex_lock (&arrays_m) == 0) {
        while (b && !b->nfree)
            b = b->prev;
        if (b)
            alloc = SCE_Mem_GetNextAllocInBlock (b);
        /* TODO: use recursive mutex */
        pthread_mutex_unlock (&arrays_m);
    }
    return NULL;
}

static SCE_SMemAlloc* SCE_Mem_NewAllocFromArray (size_t size)
{
    SCE_SMemAlloc *m = NULL;
    /* considers this function is called with a good size */
    SCE_SMemArray *a = &arrays[size-1];
    if (!(m = SCE_Mem_GetNextAlloc (a))) {
        if (!(SCE_Mem_AddNewBlock (a) < 0))
            m = SCE_Mem_GetNextAlloc (a); /* can't fail */
    }
    return m;
}

static void SCE_Mem_EraseAllocFromArray (SCE_SMemAlloc *m)
{
    SCE_SMemArrayBlock *b = m->block;
    b->freeallocs[b->nfree++] = m;
    if (b->nfree >= SCE_ARRAY_BLOCK_SIZE)
        SCE_Mem_EraseBlock (&arrays[m->size-1], b);
    SCE_Mem_InitAlloc (m);
}

/* functions managing the table of allocations */

static SCE_SMemAlloc* SCE_Mem_NewAlloc (size_t size)
{
    SCE_SMemAlloc *m = NULL;
#if 0
    if (size <= SCE_NUM_MEMORY_ARRAYS)
        m = SCE_Mem_NewAllocFromArray (size);
    else
#endif
    {
        /* make one allocation for all: descriptor and demanded block */
        m = malloc (sizeof *m + size);
        if (!m)
            SCEE_Log (SCE_OUT_OF_MEMORY);
        else
            SCE_Mem_InitAlloc (m);
    }
    return m;
}

static void SCE_Mem_DeleteAlloc (SCE_SMemAlloc *m)
{
    free (m);
}

#define SCE_Mem_GetAllocAddress(m) ((void*)&((SCE_SMemAlloc*)(m))[1])


static SCE_SMemAlloc* SCE_Mem_LocateAllocFromPointer (void *p)
{
    SCE_SMemAlloc *i = NULL;
    SCE_SMemAlloc *al = p;
    if (pthread_mutex_lock (&allocs_m) == 0) {
        al = &al[-1];
        SCE_Mem_For (i) {
            if (i == al) {
                pthread_mutex_unlock (&allocs_m);
                return i;
            }
        }
        pthread_mutex_unlock (&allocs_m);
    }
    return NULL;
}


int SCE_Mem_IsValid (void *p)
{
    return (SCE_Mem_LocateAllocFromPointer (p) != NULL);
}


static void SCE_Mem_AddAlloc (SCE_SMemAlloc *m)
{
    m->prev = &allocs;
    /* FIXME: errors not checked! */
    if (pthread_mutex_lock (&allocs_m) == 0) {
        m->next = allocs.next;
        if (m->next)
            m->next->prev = m;
        allocs.next = m;
        pthread_mutex_unlock (&allocs_m);
    }
}

static void SCE_Mem_EraseAlloc (SCE_SMemAlloc *m)
{
    if (pthread_mutex_lock (&allocs_m) == 0) {
        m->prev->next = m->next;
        if (m->next)
            m->next->prev = m->prev;
#if 0
        if (m->size <= SCE_NUM_MEMORY_ARRAYS)
            SCE_Mem_EraseAllocFromArray (m);
        else
#endif
        SCE_Mem_DeleteAlloc (m);
        pthread_mutex_unlock (&allocs_m);
    }
}


/**
 * \brief SCE's malloc wrapper
 * \param s Size wanted for the block
 * \param file, line Where the block is asked
 * \returns a pointer to a newly allocated block on succes, NULL on failure
 * 
 * You will generally want to call SCE_malloc() that wraps this function.
 * 
 * \see SCE_malloc()
 */
void* SCE_Mem_Alloc (const char *file, unsigned int line, size_t s)
{
#if !SCE_USE_MEMORY_MANAGER
    void *p = NULL;
    (void)file;
    (void)line;
    p = malloc (s);
    if (!p)
        SCEE_Log (SCE_OUT_OF_MEMORY);
    return p;
#else
    SCE_SMemAlloc *mem = NULL;

    mem = SCE_Mem_NewAlloc (s);
    if (!mem) {
        SCEE_LogSrc ();
        return NULL;
    }

    mem->file = file;
    mem->line = line;
    mem->size = s;

    SCE_Mem_AddAlloc (mem);

    return SCE_Mem_GetAllocAddress (mem);
#endif
}

/**
 * \brief Callocs wrapper
 * \param s Size of one item
 * \param n Num of item required
 * \param file, line Where the block is asked
 * \returns A pointer to a newly allocated block on success, NULL on failure
 * \sa SCE_Mem_Alloc SCE_Mem_Free
 * 
 * You will generally want to call SCE_calloc() that wraps this function.
 * 
 * \see SCE_calloc()
 */
void* SCE_Mem_Calloc (const char *file, unsigned int line, size_t s, size_t n)
{
#if !SCE_USE_MEMORY_MANAGER
    void *p = NULL;
    (void)file;
    (void)line;
    p = calloc (s, n);
    if (!p)
        SCEE_Log (SCE_OUT_OF_MEMORY);
    return p;
#else
    void *p = SCE_Mem_Alloc (file, line, s * n);
    if (p)
        memset (p, 0, s * n);
    return p;
#endif
}

/**
 * \brief Reallocs wrapper
 * \param file File from which the block is requested
 * \param line Line at which the block is requested
 * \param p Old pointer
 * \param s New size wanted for the block
 * \returns A pointer to the reallocated block on success, NULL on failure
 * \sa SCE_Mem_Alloc SCE_Mem_Free
 * 
 * You will generally want to call SCE_realloc() that wraps this function.
 * 
 * \see SCE_realloc()
 * \todo this function doesn't work
 * \warning this function doesn't work, do not use it
 */
void* SCE_Mem_Realloc (const char *file, unsigned int line, void *p, size_t s)
{
#if !SCE_USE_MEMORY_MANAGER
    (void)file;
    (void)line;
    p = realloc (p, s);
    if (!p)
        SCEE_Log (SCE_OUT_OF_MEMORY);
    return p;
#else
    SCE_SMemAlloc *mem = NULL;

    if (!p)
        return SCE_Mem_Alloc (file, line, s); /* nouvelle allocation */
    else {
        mem = SCE_Mem_LocateAllocFromPointer (p);
        if (!mem) {
            SCEE_Log (SCE_INVALID_POINTER);
            return NULL;
        }
        mem = realloc (mem, sizeof *mem + s);
        if (!mem) {
            /* en cas d'echec realloc conserve la memoire deja alloue,
               donc on ne libere aucune memoire */
            SCEE_Log (SCE_OUT_OF_MEMORY);
            return NULL;
        }
    }

    return SCE_Mem_GetAllocAddress (mem);
#endif
}

/**
 * \brief Frees a pointer allocated by SCE_Mem_Alloc
 * \param file File from which the block is requested
 * \param line Line at which the block is requested
 * \param p Pointer to free
 * 
 * You will generally want to call SCE_free() that wraps this function.
 * 
 * \see SCE_free
 */
void SCE_Mem_Free (const char *file, int line, void *p)
{
#if !SCE_USE_MEMORY_MANAGER
    free (p);
#else
    if (p) {
        SCE_SMemAlloc *m = SCE_Mem_LocateAllocFromPointer (p);
        if (m && m != &allocs)
            SCE_Mem_EraseAlloc (m);
        else
            SCEE_SendMsg ("SCE_Mem_Free(): trying to free an invalid pointer %p"
                          " at %s(%d).\n", p, file, line);
    }
#endif
}


/**
 * \brief Duplicates allocated memory and copies its content
 * \param p the memory to duplicate
 * \param size allocated size of \p p, in bytes
 * \returns the newly allocated memory
 */
void* SCE_Mem_Dup (const void *p, size_t size)
{
    void *new = SCE_malloc (size);
    if (new)
        memcpy (new, p, size);
    else
        SCEE_LogSrc ();
    return new;
}


/*#ifdef SCE_DEBUG*/
void SCE_Mem_List (void)
{
    unsigned int n = 0;
    SCE_SMemAlloc *a = NULL;
    SCE_Mem_For (a) {
        SCEE_SendMsg ("- allocation in %s (%u): %zu bytes.\n",
                      a->file, a->line, a->size);
        n++;
    }
    SCEE_SendMsg ("you have %u non-freeds allocations.\n", n);
}


/**
 * \brief Get the size used by a pointer
 */
size_t SCE_Mem_GetSize (void *p)
{
    SCE_SMemAlloc *m = SCE_Mem_LocateAllocFromPointer (p);
    if (m)
        return m->size;
    return 0;
}

/**
 * \brief Get the line of the call to SCE_Mem_Alloc
 */
unsigned int SCE_Mem_GetLine (void *p)
{
    SCE_SMemAlloc *m = SCE_Mem_LocateAllocFromPointer (p);
    if (m)
        return m->line;
    return 0;
}
/**
 * \brief Get the file which allocated the pointer
 */
const char* SCE_Mem_GetFile (void *p)
{
    SCE_SMemAlloc *m = SCE_Mem_LocateAllocFromPointer (p);
    if (m)
        return m->file;
    return NULL;
}
/*#endif*/

/** @} */
