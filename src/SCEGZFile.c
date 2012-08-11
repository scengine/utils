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
   updated: 11/08/2012 */

#include <stdio.h>
#include "zlib.h"
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEMemory.h"
#include "SCE/utils/SCEMath.h"  /* MIN() */
#include "SCE/utils/SCEString.h"
#include "SCE/utils/SCEArray.h"
#include "SCE/utils/SCEFile.h"
#include "SCE/utils/SCEGZFile.h"

SCE_SFileSystem sce_gzfs;

typedef struct xfile xfile;
struct xfile {
    char *fname;
    SCE_SArray data;
    size_t pos;
    int is_sync;
    int readable;
    int writable;
};

static void xfile_init (xfile *fp)
{
    fp->fname = NULL;
    SCE_Array_Init (&fp->data);
    fp->pos = 0;
    fp->is_sync = SCE_TRUE;
    fp->readable = SCE_FALSE;
    fp->writable = SCE_FALSE;
}
static void xfile_clear (xfile *fp)
{
    SCE_free (fp->fname);
    SCE_Array_Clear (&fp->data);
}

static xfile* xfile_create (const char *fname)
{
    xfile *fp = NULL;
    if (!(fp = SCE_malloc (sizeof *fp)))
        SCEE_LogSrc ();
    else {
        xfile_init (fp);
        if (!(fp->fname = SCE_String_Dup (fname))) {
            SCEE_LogSrc ();
            SCE_free (fp);
            return NULL;
        }
    }
    return fp;
}
static void xfile_delete (xfile *fp)
{
    if (fp) {
        xfile_clear (fp);
        SCE_free (fp);
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
                SCEE_LogErrno (fname);
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

static int xclose (void *fd)
{
    if (xflush (fd) != 0) {
        SCEE_LogSrc ();
        return EOF;
    }
    xfile_delete (fd);
    return 0;
}

static size_t xread (void *data, size_t size, size_t nmemb, void *fd)
{
    size_t s;
    unsigned char *ptr = NULL;
    xfile *file = fd;

    if (!file->readable)
        return 0;

    s = MIN (size * nmemb, SCE_Array_GetSize (&file->data) - file->pos);
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

    remaining = SCE_Array_GetSize (&file->data) - file->pos;
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

    return size * nmemb;
}

static int xseek (void *fd, long offset, int whence)
{
    size_t size;
    long new;
    xfile *file = fd;

    new = file->pos;
    size = SCE_Array_GetSize (&file->data);

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
