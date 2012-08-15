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

/* created: 10/08/2012
   updated: 13/08/2012 */

#include <stdio.h>
#include "zlib.h"
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEMemory.h"
#include "SCE/utils/SCEMath.h"  /* MIN() */
#include "SCE/utils/SCEString.h"
#include "SCE/utils/SCEArray.h"
#include "SCE/utils/SCEList.h"
#include "SCE/utils/SCEFile.h"
#include "SCE/utils/SCEGZFile.h"

SCE_SFileSystem sce_gzfs;

typedef struct xfile xfile;
struct xfile {
    char *fname;
    SCE_SArray data;
    size_t size;
    size_t pos;
    int is_sync;
    int readable;
    int writable;
    int cached;       /* whether \c data needs to be reloaded from the HDD */
    SCE_SGZFileCache *cache;
    SCE_SListIterator it;
};

static void xfile_init (xfile *file)
{
    file->fname = NULL;
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


static const char* xstrerr (int code)
{
    switch (code) {
    case Z_NEED_DICT: return "dictionary needed";
    case Z_STREAM_ERROR: return "stream error";
    case Z_DATA_ERROR: return "data error";
    case Z_MEM_ERROR: return "memory error";
    case Z_STREAM_END: return "stream end"; /* not really an error.. */
    case Z_VERSION_ERROR: return "version error";
    default: return "unknown error code";
    }
}

static int xdecomp (xfile *file, FILE *fd)
{
#define CHUNK_SIZE 32768

    z_stream strm;
    int ret;
    unsigned char in[CHUNK_SIZE];
    unsigned char out[CHUNK_SIZE];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit (&strm);
    if (ret != Z_OK) {
        SCEE_Log (ret);
        SCEE_LogMsg ("zlib inflateInit() error: %s", xstrerr (ret));
        goto fail;
    }

    do {
        strm.avail_in = fread (in, 1, CHUNK_SIZE, fd);
        if (ferror (fd)) {
            inflateEnd (&strm);
            SCEE_LogErrno (file->fname);
            goto fail;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        do {
            size_t size;

            strm.avail_out = CHUNK_SIZE;
            strm.next_out = out;
            ret = inflate (&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR || ret == Z_MEM_ERROR ||
                ret == Z_DATA_ERROR || ret == Z_NEED_DICT) {
                inflateEnd (&strm);
                SCEE_Log (ret);
                SCEE_LogMsg ("zlib inflate() error: %s", xstrerr (ret));
                goto fail;
            }
            size = CHUNK_SIZE - strm.avail_out;
            if (SCE_Array_Append (&file->data, out, size) < 0) {
                inflateEnd (&strm);
                goto fail;
            }
        } while (strm.avail_out == 0);

    } while (ret != Z_STREAM_END);

    inflateEnd (&strm);
    if (ret != Z_STREAM_END) {
        SCEE_Log (876);
        SCEE_LogMsg ("%s: compressed file seem incomplete", file->fname);
        goto fail;
    }

    file->size = SCE_Array_GetSize (&file->data);
    file->cached = SCE_TRUE;

    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}


static void* xopen (const char *fname, int flags)
{
    xfile *file = NULL;
    FILE *fd = NULL;

    if (!(file = xfile_create (fname)))
        goto fail;

    fd = fopen (fname, "rb");
    if (flags & SCE_FILE_CREATE) {
        if (!fd) {
            if (!(fd = fopen (fname, "wb"))) {
                SCEE_LogErrno (fname);
                goto fail;
            }
            fclose (fd);
            fd = NULL;
        }
    } else if (!fd) {
        SCEE_LogErrno (fname);
        goto fail;
    }

    if (flags & SCE_FILE_WRITE)
        file->writable = SCE_TRUE;
    if (flags & SCE_FILE_READ)
        file->readable = SCE_TRUE;

    if (flags & SCE_FILE_READ && fd) {
        if (xdecomp (file, fd) < 0)
            goto fail;
    }

    if (fd)
        fclose (fd);

    return file;
fail:
    if (fd)
        fclose (fd);
    xfile_delete (file);
    SCEE_LogSrc ();
    return NULL;
}

static int xflush (void *f)
{
    FILE *fd = NULL;
    z_stream strm;
    int ret;
    unsigned char out[CHUNK_SIZE];
    size_t size;
    xfile *file = f;
        
    if (file->is_sync || !file->writable)
        return 0;
    /* flushing an input stream should actually fail, but since we use xflush()
       in xclose(), we'll be kind. */

    if (!(fd = fopen (file->fname, "wb"))) {
        SCEE_LogErrno (file->fname);
        return EOF;
    }

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    /* maximum compression, hahahaha */
    ret = deflateInit (&strm, 9);
    if (ret != Z_OK) {
        SCEE_Log (ret);
        SCEE_LogMsg ("zlib deflateInit() error: %s", xstrerr (ret));
        goto fail;
    }

    strm.avail_in = SCE_Array_GetSize (&file->data);
    strm.next_in = SCE_Array_Get (&file->data);

    do {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = out;
        ret = deflate (&strm, Z_FINISH);
        if (ret == Z_STREAM_ERROR || ret == Z_MEM_ERROR ||
            ret == Z_DATA_ERROR || ret == Z_NEED_DICT) {
            deflateEnd (&strm);
            SCEE_Log (ret);
            SCEE_LogMsg ("zlib deflate() error: %s", xstrerr (ret));
            goto fail;
        }
        size = CHUNK_SIZE - strm.avail_out;
        size = fwrite (out, 1, size, fd);
    } while (strm.avail_out == 0);

    deflateEnd (&strm);
    fclose (fd);

    file->is_sync = SCE_TRUE;

    return 0;
fail:
    fclose (fd);
    SCEE_LogSrc ();
    return EOF;
}

static void SCE_GZFile_UncacheGZFile (xfile*);
static int xclose (void *fd)
{
    xfile *file = fd;
    if (xflush (file) != 0) {
        SCEE_LogSrc ();
        return EOF;
    }
    if (file->cache)
        SCE_GZFile_UncacheGZFile (file);
    xfile_delete (file);
    return 0;
}

static void SCE_GZFile_Cache (SCE_SGZFileCache*, xfile*);

static int xreload (xfile *file)
{
    FILE *fp = NULL;

    if (!(fp = fopen (file->fname, "rb"))) {
        SCEE_LogErrno (file->fname);
        goto fail;
    }
    if (xdecomp (file, fp) < 0)
        goto fail;

    fclose (fp);

    /* put it on top of the cache */
    if (file->cache)
        SCE_GZFile_Cache (file->cache, file);

    return SCE_OK;
fail:
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("reloading of cached GZFile failed");
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

static size_t xwrite (void *data, size_t size, size_t nmemb, void *fd)
{
    size_t remaining, s;
    unsigned char *ptr = NULL;
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
        ptr = data;
        if (SCE_Array_Append (&file->data, &ptr[remaining],
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


int SCE_Init_GZFile (void)
{
    sce_gzfs.udata = NULL;
    sce_gzfs.xopen = xopen;
    sce_gzfs.xclose = xclose;
    sce_gzfs.xread = xread;
    sce_gzfs.xwrite = xwrite;
    sce_gzfs.xseek = xseek;
    sce_gzfs.xtell = xtell;
    sce_gzfs.xrewind = xrewind;
    sce_gzfs.xflush = xflush;
    return SCE_OK;
}
void SCE_Quit_GZFile (void)
{
}


void SCE_GZFile_InitCache (SCE_SGZFileCache *fc)
{
    fc->max_cached = 1;
    fc->n_cached = 0;
    SCE_List_Init (&fc->cached);
    pthread_mutex_init (&fc->cached_mutex, NULL);
}
void SCE_GZFile_ClearCache (SCE_SGZFileCache *fc)
{
    SCE_List_Clear (&fc->cached);
    pthread_mutex_destroy (&fc->cached_mutex);
}

void SCE_GZFile_SetMaxCachedFiles (SCE_SGZFileCache *fc, unsigned int m)
{
    fc->max_cached = m;
}
unsigned int SCE_GZFile_GetNumCachedFiles (SCE_SGZFileCache *fc)
{
    return fc->n_cached;
}

static void SCE_GZFile_Cache (SCE_SGZFileCache *fc, xfile *file)
{
    pthread_mutex_lock (&fc->cached_mutex);
    SCE_List_Remove (&file->it);
    SCE_List_Appendl (&fc->cached, &file->it);
    fc->n_cached++;
    pthread_mutex_unlock (&fc->cached_mutex);
}

static void SCE_GZFile_Uncache (SCE_SGZFileCache *fc, xfile *file)
{
    pthread_mutex_lock (&fc->cached_mutex);
    SCE_List_Remove (&file->it);
    fc->n_cached--;
    pthread_mutex_unlock (&fc->cached_mutex);
    xflush (file);
    SCE_Array_Clear (&file->data);
    SCE_Array_Init (&file->data);
    file->cached = SCE_FALSE;
}


void SCE_GZFile_CacheFile (SCE_SGZFileCache *fc, SCE_SFile *fd)
{
    xfile *file = SCE_File_Get (fd);
    SCE_GZFile_Cache (fc, file);
    file->cache = fc;
}
static void SCE_GZFile_UncacheGZFile (xfile *file)
{
    pthread_mutex_lock (&file->cache->cached_mutex);
    SCE_List_Remove (&file->it);
    file->cache->n_cached--;
    pthread_mutex_unlock (&file->cache->cached_mutex);
    file->cache = NULL;
}
void SCE_GZFile_UncacheFile (SCE_SFile *fd)
{
    SCE_GZFile_UncacheGZFile (SCE_File_Get (fd));
}


void SCE_GZFile_UpdateCache (SCE_SGZFileCache *fc)
{
    while (fc->n_cached > fc->max_cached) {
        xfile *file = SCE_List_GetData (SCE_List_GetFirst (&fc->cached));
        SCE_GZFile_Uncache (fc, file);
    }
}
