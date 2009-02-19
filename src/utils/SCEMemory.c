/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2009  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
   updated: 20/11/2008 */

#include <stdlib.h>
#include <string.h>
#include <SCE/SCEGLee.h>

#include <SCE/utils/SCEError.h>
#include <SCE/utils/SCEMemory.h>

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

#define SCE_NUM_MEMORY_ARRAYS 128
/* number of allocs per block */
#define SCE_ARRAY_BLOCK_SIZE 128

/**
 * \brief Stores metadata about memory blocks
 *
 * Used to know where a block was allocated.
 */
typedef struct SCE_SMemAlloc
{
    const char *file;
    unsigned int line;
    size_t size;
    void *block; /* hack */
    struct SCE_SMemAlloc *next, *prev;
} SCE_SMemAlloc;

/*! Table of all allocations */
static SCE_SMemAlloc allocs =
{
    "root allocations list", 0, 0, NULL, NULL, NULL
};


typedef struct SCE_SMemArrayBlock
{
    SCE_SMemAlloc *allocs[SCE_ARRAY_BLOCK_SIZE];
    SCE_SMemAlloc *freeallocs[SCE_ARRAY_BLOCK_SIZE];
    int nfree;         /* number of free allocs in 'allocs' */
    struct SCE_SMemArrayBlock *next, *prev;
} SCE_SMemArrayBlock;

/* structure d'un tableau contenant une suite d'allocations de la meme taille */
typedef struct SCE_SMemArray
{
    size_t alloc_size;         /* size of one allocation */
    SCE_SMemArrayBlock *root;  /* root block */
    SCE_SMemArrayBlock *last;  /* last block */
} SCE_SMemArray;

static SCE_SMemArray arrays[SCE_NUM_MEMORY_ARRAYS];

#define SCE_Mem_For(i) for (i = allocs.next; i; i = i->next)

static void SCE_Mem_InitArray (SCE_SMemArray *a)
{
    a->alloc_size = 1; /* hop */
    a->root = NULL;
    a->last = NULL;
}

int SCE_Init_Mem (void)
{
    size_t i;
    for (i=0; i<SCE_NUM_MEMORY_ARRAYS; i++)
    {
        SCE_Mem_InitArray (&arrays[i]);
        arrays[i].alloc_size = i + 1;
    }
    return SCE_OK;
}

static void SCE_Mem_InitAlloc (SCE_SMemAlloc *m)
{
    m->file = NULL;
    m->line = 1;
    m->size = 0;
    m->next = m->prev = NULL;
}

static void SCE_Mem_InitBlock (SCE_SMemArrayBlock *b)
{
    unsigned int i;
    for (i=0; i<SCE_ARRAY_BLOCK_SIZE; i++)
    {
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
        Logger_Log (SCE_OUT_OF_MEMORY);
    else
    {
        size_t i;
        void *address = NULL;
        SCE_Mem_InitBlock (b);
        /* allocate the data */
        size += sizeof (SCE_SMemAlloc);
        b->allocs[0] = malloc (size * SCE_ARRAY_BLOCK_SIZE);
        if (!b->allocs[0])
        {
            Logger_Log (SCE_OUT_OF_MEMORY);
            free (b);
            return NULL;
        }
        SCE_Mem_InitAlloc (b->allocs[0]);
        b->allocs[0]->block = b;
        for (i=1; i<SCE_ARRAY_BLOCK_SIZE; i++)
        {
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
    if (b)
    {
        free (b->allocs[0]);
        free (b);
    }
}

static int SCE_Mem_AddNewBlock (SCE_SMemArray *a)
{
    if (!a->last)
    {
        a->last = a->root = SCE_Mem_CreateBlock (a->alloc_size);
        if (!a->last)
            return -1;
    }
    else
    {
        a->last->next = SCE_Mem_CreateBlock (a->alloc_size);
        if (!a->last->next)
            return -1;
        a->last->next->prev = a->last;
        a->last = a->last->next;
    }
    return 0;
}

static void SCE_Mem_EraseBlock (SCE_SMemArray *a, SCE_SMemArrayBlock *b)
{
    if (b->prev)
        b->prev->next = b->next;
    else
        a->root = b->next;
    if (b->next)
        b->next->prev = b->prev;
    else
        a->last = b->prev;
    SCE_Mem_DeleteBlock (b);
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
    SCE_SMemArrayBlock *b = NULL;
    b = a->last;
    while (b && !b->nfree)
        b = b->prev;
    if (b)
        return SCE_Mem_GetNextAllocInBlock (b);
    return NULL;
}

static SCE_SMemAlloc* SCE_Mem_NewAllocFromArray (size_t size)
{
    SCE_SMemAlloc *m = NULL;
    /* considers this function is called with a good size */
    SCE_SMemArray *a = &arrays[size-1];
    if (!(m = SCE_Mem_GetNextAlloc (a)))
    {
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
    if (size <= SCE_NUM_MEMORY_ARRAYS)
        m = SCE_Mem_NewAllocFromArray (size);
    else
    {
        /* make one allocation for all: descriptor and demanded block */
        m = malloc (sizeof *m + size);
        if (!m)
            Logger_Log (SCE_OUT_OF_MEMORY);
        else
            SCE_Mem_InitAlloc (m);
    }
    return m;
}

static void SCE_Mem_DeleteAlloc (SCE_SMemAlloc *m)
{
    free (m);
}

#define SCE_Mem_GetAllocAddress(m) ((void*)&m[1])


static SCE_SMemAlloc* SCE_Mem_LocateAllocFromPointer (void *p)
{
    SCE_SMemAlloc *i = &allocs;

    while (i->next && SCE_Mem_GetAllocAddress (i) != p)
        i = i->next;

    if (SCE_Mem_GetAllocAddress (i) == p)
        return i;

    return NULL;
}


int SCE_Mem_IsValid (void *p)
{
    return (SCE_Mem_LocateAllocFromPointer (p) != NULL);
}


static void SCE_Mem_AddAlloc (SCE_SMemAlloc *m)
{
    m->prev = &allocs;
    m->next = allocs.next;
    if (m->next)
        m->next->prev = m;
}

static void SCE_Mem_EraseAlloc (SCE_SMemAlloc *m)
{
    m->prev->next = m->next;
    if (m->next)
        m->next->prev = m->prev;
    if (m->size <= SCE_NUM_MEMORY_ARRAYS)
        SCE_Mem_EraseAllocFromArray (m);
    else
        SCE_Mem_DeleteAlloc (m);
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
    SCE_SMemAlloc *mem = NULL;

    mem = SCE_Mem_NewAlloc (s);
    if (!mem) /* pas de LogSrc, car cette fonction alloue de la memoire */
        return NULL;

    mem->file = file;
    mem->line = line;
    mem->size = s;

    SCE_Mem_AddAlloc (mem);

    return SCE_Mem_GetAllocAddress (mem);
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
    void *p = SCE_Mem_Alloc (file, line, s * n);
    if (p)
        memset (p, 0, s * n);
    return p;
}

/**
 * \brief Reallocs wrapper
 * \param aloc Old pointer
 * \param size New size wanted for the block
 * \param file, line Where the block is asked
 * \returns A pointer to the reallocated block on success, NULL on failure
 * \sa SCE_Mem_Alloc SCE_Mem_Free
 * 
 * You will generally want to call SCE_realloc() that wraps this function.
 * 
 * \see SCE_realloc()
 */
void* SCE_Mem_Realloc (const char *file, unsigned int line, void *p, size_t s)
{
    SCE_SMemAlloc *mem = NULL;

    if (!p)
        return SCE_Mem_Alloc (file, line, s); /* nouvelle allocation */
    else
    {
        mem = SCE_Mem_LocateAllocFromPointer (p);
        if (!mem)
        {
            Logger_Log (SCE_INVALID_POINTER);
            return NULL;
        }
        mem = realloc (mem, sizeof *mem + s);
        if (!mem)
        {
            /* en cas d'echec realloc conserve la memoire deja alloue,
               donc on ne libere aucune memoire */
            Logger_Log (SCE_OUT_OF_MEMORY);
            return NULL;
        }
    }

    return SCE_Mem_GetAllocAddress (mem);
}

/**
 * \brief Frees a pointer allocated by SCE_Mem_Alloc
 * 
 * You will generally want to call SCE_free() that wraps this function.
 * 
 * \see SCE_free
 */
void SCE_Mem_Free (void *p)
{
    if (p)
    {
        SCE_SMemAlloc *m = SCE_Mem_LocateAllocFromPointer (p);
        if (m && m != &allocs)
            SCE_Mem_EraseAlloc (m);
    }
}


void* SCE_Mem_Dup (const void *p, size_t size)
{
    void *new = SCE_malloc (size);
    if (new)
        memcpy (new, p, size);
    else
        Logger_LogSrc ();
    return new;
}

void SCE_Mem_Convert(int tdest, void *dest, int tsrc, const void *src, size_t n)
{
    union SCE_UMemType
    {
        GLubyte *ub;
        GLbyte *b;
        GLushort *us;
        GLshort *s;
        GLuint *ui;
        GLint *i;
        GLfloat *f;
        GLdouble *d;
    };

    size_t i;
    union SCE_UMemType tin, tout;

#define SCE_MEM_FOR(type, namedest, namesrc)\
    case type:\
        tout.namedest = dest;\
        for (i=0; i<n; i++)\
            tout.namedest[i] = tin.namesrc[i];\
        break;

#define SCE_MEM_SWITCH(type, namesrc)\
    case type:\
        tin.namesrc = (void*)src;\
        switch (tdest)\
        {\
        SCE_MEM_FOR (SCE_UNSIGNED_BYTE,  ub, namesrc)\
        SCE_MEM_FOR (SCE_BYTE,           b,  namesrc)\
        SCE_MEM_FOR (SCE_UNSIGNED_SHORT, us, namesrc)\
        SCE_MEM_FOR (SCE_SHORT,          s,  namesrc)\
        SCE_MEM_FOR (SCE_UNSIGNED_INT,   ui, namesrc)\
        SCE_MEM_FOR (SCE_INT,            i,  namesrc)\
        SCE_MEM_FOR (SCE_FLOAT,          f,  namesrc)\
        SCE_MEM_FOR (SCE_DOUBLE,         d,  namesrc)\
        }\
        break;

    switch (tsrc)
    {
    SCE_MEM_SWITCH (SCE_UNSIGNED_BYTE,  ub)
    SCE_MEM_SWITCH (SCE_BYTE,           b)
    SCE_MEM_SWITCH (SCE_UNSIGNED_SHORT, us)
    SCE_MEM_SWITCH (SCE_SHORT,          s)
    SCE_MEM_SWITCH (SCE_UNSIGNED_INT,   ui)
    SCE_MEM_SWITCH (SCE_INT,            i)
    SCE_MEM_SWITCH (SCE_FLOAT,          f)
    SCE_MEM_SWITCH (SCE_DOUBLE,         d)
    }
}

void* SCE_Mem_ConvertDup (int tdest, int tsrc, const void *src, size_t n)
{
    size_t size;
    void *dest = NULL;

    /* TODO: mosh */
    switch (tdest)
    {
    case SCE_DOUBLE:
        size = sizeof (GLdouble);
        break;

    case SCE_FLOAT:
        size = sizeof (GLfloat);
        break;

    case SCE_INT:
    case SCE_UNSIGNED_INT:
        size = sizeof (GLint);
        break;

    case SCE_SHORT:
    case SCE_UNSIGNED_SHORT:
        size = sizeof (GLshort);
        break;

    case SCE_BYTE:
    case SCE_UNSIGNED_BYTE:
        size = sizeof (GLbyte);

    default:
#ifdef SCE_DEBUG
        Logger_Log (SCE_INVALID_ARG);
        Logger_LogMsg ("invalid type for the destination");
#endif
        return NULL;
    }

    if (tdest == tsrc)
        return SCE_Mem_Dup (src, size * n);

    dest = SCE_malloc (size * n);
    if (!dest)
    {
        Logger_LogSrc ();
        return NULL;
    }

    SCE_Mem_Convert (tdest, dest, tsrc, src, n);
    return dest;
}

#ifdef SCE_DEBUG
void SCE_Mem_List (void)
{
    unsigned int n = 0;
    SCE_SMemAlloc *a = NULL;
    SCE_Mem_For (a)
    {
        Logger_PrintMsg ("- allocation in %s (%u): %u bytes.\n",
                         a->file, a->line, a->size);
        n++;
    }
    Logger_PrintMsg ("you have %u non-freeds allocations.\n", n);
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
#endif

/** @} */
