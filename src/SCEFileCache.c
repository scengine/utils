/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2012  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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

/* created: 15/08/2012
   updated: 21/08/2012 */

#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEMemory.h"
#include "SCE/utils/SCEMath.h"  /* MIN() */
#include "SCE/utils/SCEString.h"
#include "SCE/utils/SCEArray.h"
#include "SCE/utils/SCEList.h"
#include "SCE/utils/SCEFile.h"
#include "SCE/utils/SCEFileCache.h"

SCE_SFileSystem sce_cachefs;

typedef struct xfile xfile;
struct xfile {
    char *fname;
    SCE_SFileSystem *subfs;
    SCE_SArray data;
    size_t size;
    size_t pos;
    int is_sync;
    int readable;
    int writable;
    int cached;       /* whether \c data is allocated or need reloading */
    SCE_SFileCache *cache;
    SCE_SListIterator it;
};

static void xfile_init (xfile *file)
{
    file->fname = NULL;
    file->subfs = NULL;
    SCE_Array_Init (&file->data);
    file->size = 0;
    file->pos = 0;
    file->is_sync = SCE_TRUE;
    file->readable = SCE_FALSE;
    file->writable = SCE_FALSE;
    file->cached = SCE_TRUE;
    file->cache = NULL;
    SCE_List_InitIt (&file->it);
    SCE_List_SetData (&file->it, file);
}
static void xfile_clear (xfile *file)
{
    SCE_free (file->fname);
    SCE_Array_Clear (&file->data);
    SCE_List_Remove (&file->it);
}

static xfile* xfile_create (const char *fname)
{
    xfile *file = NULL;
    if (!(file = SCE_malloc (sizeof *file)))
        SCEE_LogSrc ();
    else {
        xfile_init (file);
        if (!(file->fname = SCE_String_Dup (fname))) {
            SCEE_LogSrc ();
            SCE_free (file);
            return NULL;
        }
    }
    return file;
}
static void xfile_delete (xfile *file)
{
    if (file) {
        xfile_clear (file);
        SCE_free (file);
    }
}


static int xload (xfile *file, SCE_SFile *f)
{
    long size;
    void *data = NULL;

    size = SCE_File_Length (f);
    if (!(data = SCE_malloc (size)))
        goto fail;
    SCE_File_Rewind (f);
    if (SCE_File_Read (data, 1, size, f) != size)
        goto fail;
    if (SCE_Array_Append (&file->data, data, size) < 0)
        goto fail;
    SCE_free (data);

    file->size = SCE_Array_GetSize (&file->data);
    file->cached = SCE_TRUE;

    return SCE_OK;
fail:
    SCE_free (data);
    SCEE_LogSrc ();
    return SCE_ERROR;
}

static int xsave (xfile *file, SCE_SFile *f)
{
    size_t size;
    void *data;

    data = SCE_Array_Get (&file->data);
    size = SCE_Array_GetSize (&file->data);

    SCE_File_Rewind (f);
    if (SCE_File_Write (data, 1, size, f) != size)
        goto fail;

    file->is_sync = SCE_TRUE;

    return SCE_OK;
fail:
    SCE_free (data);
    SCEE_LogSrc ();
    return SCE_ERROR;
}


static int xinit (SCE_SFileSystem *fs, SCE_SFile *file)
{
    if (fs->udata)
        SCE_FileCache_CacheFile (fs->udata, file);
}

static void* xopen (SCE_SFileSystem *fs, const char *fname, int flags)
{
    xfile *file = NULL;
    SCE_SFile f;

    if (!(file = xfile_create (fname)))
        goto fail;

    file->subfs = fs;
    SCE_File_Init (&f);

    if (SCE_File_Open (&f, fs, fname, flags) < 0)
        goto fail;

    if (flags & SCE_FILE_WRITE)
        file->writable = SCE_TRUE;
    if (flags & SCE_FILE_READ)
        file->readable = SCE_TRUE;

    if (flags & SCE_FILE_READ) {
        if (xload (file, &f) < 0)
            goto fail;
    }

    SCE_File_Close (&f);

    return file;
fail:
    SCE_File_Close (&f);
    xfile_delete (file);
    SCEE_LogSrc ();
    return NULL;
}

static int xflush (void *fd)
{
    xfile *file = fd;
    SCE_SFile f;

    /* there is nothing to flush */
    if (!file->cached)
        return 0;

    SCE_File_Init (&f);
    if (SCE_File_Open (&f, file->subfs, file->fname, SCE_FILE_WRITE |
                       SCE_FILE_CREATE | SCE_FILE_TRUNCATE) < 0)
        goto fail;
    if (xsave (file, &f) < 0)
        goto fail;

    SCE_File_Close (&f);

    return 0;
fail:
    SCE_File_Close (&f);
    SCEE_LogSrc ();
    return EOF;
}

static void SCE_FileCache_UncacheXFile (xfile*);
static int xclose (void *fd)
{
    xfile *file = fd;
    if (xflush (file) != 0) {
        SCEE_LogSrc ();
        return EOF;
    }
    if (file->cache)
        SCE_FileCache_UncacheXFile (file);
    xfile_delete (file);
    return 0;
}


static void SCE_FileCache_Cache (SCE_SFileCache*, xfile*);
static int xreload (xfile *file)
{
    SCE_SFile f;

    SCE_File_Init (&f);
    if (SCE_File_Open (&f, file->subfs, file->fname, SCE_FILE_READ) < 0)
        goto fail;
    if (xload (file, &f) < 0)
        goto fail;

    SCE_File_Close (&f);

    /* put it on top of the cache */
    if (file->cache)
        SCE_FileCache_Cache (file->cache, file);

    return SCE_OK;
fail:
    SCE_File_Close (&f);
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("reloading of cached file failed");
    return SCE_ERROR;
}

static size_t xread (void *data, size_t size, size_t nmemb, void *fd)
{
    size_t s;
    unsigned char *ptr = NULL;
    xfile *file = fd;

    if (!file->readable)
        return 0;
    if (!file->cached) {
        if (xreload (file) < 0)
            return 0;
    }

    s = MIN (size * nmemb, file->size - file->pos);
    ptr = SCE_Array_Get (&file->data);
    memcpy (data, &ptr[file->pos], s);
    file->pos += s;

    return s;
}

static size_t xwrite (const void *data, size_t size, size_t nmemb, void *fd)
{
    size_t remaining, s;
    unsigned char *ptr = NULL;
    const unsigned char *cptr = NULL;
    xfile *file = fd;

    if (!file->writable)
        return 0;
    if (!file->cached) {
        if (xreload (file) < 0)
            return 0;
    }

    remaining = file->size - file->pos;
    s = MIN (remaining, size * nmemb);
    ptr = SCE_Array_Get (&file->data);
    memcpy (&ptr[file->pos], data, s);

    file->is_sync = SCE_FALSE;

    if (remaining < size * nmemb) {
        cptr = data;
        if (SCE_Array_Append (&file->data, (void*)&cptr[remaining],
                              size * nmemb - remaining) < 0) {
            SCEE_LogSrc ();
            return 0;
        }
    }

    file->pos += size * nmemb;
    file->size = SCE_Array_GetSize (&file->data);

    return size * nmemb;
}

static int xseek (void *fd, long offset, int whence)
{
    size_t size;
    long new;
    xfile *file = fd;

    new = file->pos;
    size = file->size;

    switch (whence) {
    case SEEK_SET: new = offset; break;
    case SEEK_CUR: new += offset; break;
    case SEEK_END: new = size + offset; break;
    }

    if (new < 0)
        new = 0;
    else if (new > size)
        new = size;

    file->pos = (size_t)new;

    return 0;
}

static long xtell (void *fd)
{
    xfile *file = fd;
    return file->pos;
}

static void xrewind (void *fd)
{
    xfile *file = fd;
    file->pos = 0;
}

static int xtruncate (SCE_SFile *fd, size_t size)
{
    xfile *file = SCE_File_Get (fd);
    long d;

    if (!file->cached) {
        if (xreload (file) < 0)
            goto fail;
    }

    d = SCE_Array_GetSize (&file->data) - size;
    if (d < 0) {
        if (SCE_Array_Append (&file->data, NULL, -d) < 0)
            goto fail;
    } else if (d > 0) {
        if (SCE_Array_PopBack (&file->data, d) < 0)
            goto fail;
    }

    file->size = SCE_Array_GetSize (&file->data);
    if (file->pos > file->size)
        file->pos = file->size;

    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}

static size_t xlength (const void *f)
{
    const xfile *file = f;
    return file->size;
}

int SCE_Init_FileCache (void)
{
    sce_cachefs.udata = NULL;
    sce_cachefs.subfs = NULL;
    sce_cachefs.xinit = xinit;
    sce_cachefs.xopen = xopen;
    sce_cachefs.xclose = xclose;
    sce_cachefs.xread = xread;
    sce_cachefs.xwrite = xwrite;
    sce_cachefs.xseek = xseek;
    sce_cachefs.xtell = xtell;
    sce_cachefs.xrewind = xrewind;
    sce_cachefs.xflush = xflush;
    sce_cachefs.xtruncate = xtruncate;
    sce_cachefs.xlength = xlength;
    return SCE_OK;
}
void SCE_Quit_FileCache (void)
{
}


void* SCE_FileCache_GetRaw (SCE_SFile *f)
{
    xfile *file = SCE_File_Get (f);
    if (!file->cached) {
        if (xreload (file) < 0) {
            SCEE_LogSrc ();
            return NULL;
        }
    }
    if (!SCE_Array_Get (&file->data)) {
        SCEE_Log (42);
        SCEE_LogMsg ("%s file is empty", file->fname);
    }
    return SCE_Array_Get (&file->data);
}

void SCE_FileCache_InitCache (SCE_SFileCache *fc)
{
    fc->max_cached = 1;
    fc->n_cached = 0;
    SCE_List_Init (&fc->cached);
    pthread_mutex_init (&fc->cached_mutex, NULL);
}
void SCE_FileCache_ClearCache (SCE_SFileCache *fc)
{
    SCE_List_Clear (&fc->cached);
    pthread_mutex_destroy (&fc->cached_mutex);
}

void SCE_FileCache_SetMaxCachedFiles (SCE_SFileCache *fc, unsigned int m)
{
    fc->max_cached = m;
}
unsigned int SCE_FileCache_GetNumCachedFiles (SCE_SFileCache *fc)
{
    return fc->n_cached;
}

static void SCE_FileCache_Cache (SCE_SFileCache *fc, xfile *file)
{
    pthread_mutex_lock (&fc->cached_mutex);
    SCE_List_Remove (&file->it);
    SCE_List_Appendl (&fc->cached, &file->it);
    fc->n_cached++;
    pthread_mutex_unlock (&fc->cached_mutex);
}

static void SCE_FileCache_Uncache (SCE_SFileCache *fc, xfile *file)
{
    pthread_mutex_lock (&fc->cached_mutex);
    SCE_List_Remove (&file->it);
    fc->n_cached--;
    pthread_mutex_unlock (&fc->cached_mutex);
    xflush (file);              /* TODO: what if xflush() fails? */
    SCE_Array_Clear (&file->data);
    SCE_Array_Init (&file->data);
    file->cached = SCE_FALSE;
}


void SCE_FileCache_CacheFile (SCE_SFileCache *fc, SCE_SFile *fd)
{
    xfile *file = SCE_File_Get (fd);
    SCE_FileCache_Cache (fc, file);
    file->cache = fc;
}
static void SCE_FileCache_UncacheXFile (xfile *file)
{
    pthread_mutex_lock (&file->cache->cached_mutex);
    SCE_List_Remove (&file->it);
    file->cache->n_cached--;
    pthread_mutex_unlock (&file->cache->cached_mutex);
    file->cache = NULL;
}
void SCE_FileCache_UncacheFile (SCE_SFile *fd)
{
    SCE_FileCache_UncacheXFile (SCE_File_Get (fd));
}


void SCE_FileCache_Update (SCE_SFileCache *fc)
{
    while (fc->n_cached > fc->max_cached) {
        xfile *file = SCE_List_GetData (SCE_List_GetFirst (&fc->cached));
        SCE_FileCache_Uncache (fc, file);
    }
}

int SCE_FileCache_Sync (SCE_SFileCache *fc)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, &fc->cached) {
        xfile *file = SCE_List_GetData (SCE_List_GetFirst (&fc->cached));
        if (xflush (file) == EOF) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
    }
    return SCE_OK;
}
