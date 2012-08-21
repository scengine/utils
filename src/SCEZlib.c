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

/* created: 13/08/2012
   updated: 13/08/2012 */

#include "zlib.h"
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEArray.h"
#include "SCE/utils/SCEZlib.h"

#define CHUNK_SIZE 16384

/* TODO: duplicated from SCEGZFile.c */
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


int SCE_Zlib_Compress (void *data, size_t size, int level, SCE_SArray *out)
{
    z_stream strm;
    int ret;
    unsigned char buf[CHUNK_SIZE];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit (&strm, level);
    if (ret != Z_OK) {
        SCEE_Log (ret);
        SCEE_LogMsg ("zlib deflateInit() error: %s", xstrerr (ret));
        goto fail;
    }

    strm.avail_in = size;
    strm.next_in = data;

    do {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = buf;
        ret = deflate (&strm, Z_FINISH);
        if (ret == Z_STREAM_ERROR || ret == Z_MEM_ERROR ||
            ret == Z_DATA_ERROR || ret == Z_NEED_DICT) {
            deflateEnd (&strm);
            SCEE_Log (ret);
            SCEE_LogMsg ("zlib deflate() error: %s", xstrerr (ret));
            goto fail;
        }
        if (SCE_Array_Append (out, buf, CHUNK_SIZE - strm.avail_out) < 0) {
            deflateEnd (&strm);
            goto fail;
        }
    } while (strm.avail_out == 0);

    deflateEnd (&strm);

    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}

int SCE_Zlib_Decompress (void *data, size_t size, SCE_SArray *out)
{
    z_stream strm;
    int ret;
    unsigned char buf[CHUNK_SIZE];

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

    strm.avail_in = size;
    strm.next_in = data;

    do {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = buf;
        ret = inflate (&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_MEM_ERROR ||
            ret == Z_DATA_ERROR || ret == Z_NEED_DICT) {
            inflateEnd (&strm);
            SCEE_Log (ret);
            SCEE_LogMsg ("zlib inflate() error: %s", xstrerr (ret));
            goto fail;
        }
        if (SCE_Array_Append (out, buf, CHUNK_SIZE - strm.avail_out) < 0) {
            inflateEnd (&strm);
            goto fail;
        }
    } while (strm.avail_out == 0);

    inflateEnd (&strm);

    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}
