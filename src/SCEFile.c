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

/* created: 09/08/2012
   updated: 16/08/2012 */

#include <string.h>
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEFile.h"

/* standard C functions */
SCE_SFileSystem sce_cfs;

static void* my_fopen (SCE_SFileSystem *fs, const char *fname, int flags)
{
    FILE *fp = NULL;
    char mode[4] = {0};

    if (flags & SCE_FILE_CREATE) {
        if (!(fp = fopen (fname, "r"))) {
            if (!(fp = fopen (fname, "w"))) {
                SCEE_LogErrno (fname);
                return NULL;
            }
        }
        fclose (fp);
        fp = NULL;
    }

    if (flags & (SCE_FILE_READ | SCE_FILE_WRITE)) {
        if (flags & SCE_FILE_TRUNCATE)
            strcpy (mode, "w+");
        else
            strcpy (mode, "r+");
    } else if (flags & SCE_FILE_READ)
        strcpy (mode, "r");
    else if (flags & SCE_FILE_WRITE)
        strcpy (mode, "w");
    else {
        /* wtf? */
        SCEE_Log (42);
        SCEE_LogMsg ("invalid flags parameter");
        return NULL;
    }

    if (!(fp = fopen (fname, mode))) {
        SCEE_LogErrno (fname);
        return NULL;
    }

    return fp;
}

static int my_truncate (SCE_SFile *fp, size_t length)
{
    SCEE_Log (42);
    SCEE_LogMsg ("truncate to arbitrary length not supported");
    return SCE_ERROR;
}

static size_t my_length (const void *f)
{
    long pos, size;
    FILE *fp = f;

    pos = ftell (fp);
    fseek (fp, 0, SEEK_END);
    size = ftell (fp);
    if (size < 0) {
        SCEE_LogErrno ("ftell()");
        return 0;               /* hihi. */
    }
    fseek (fp, pos, SEEK_SET);
    return (size_t)size;                /* ugly cast */
}

int SCE_Init_File (void)
{
    sce_cfs.udata = NULL;
    sce_cfs.subfs = NULL;
    sce_cfs.xinit = NULL;
    sce_cfs.xopen = my_fopen;
    sce_cfs.xclose = (SCE_FCloseFunc)fclose;
    sce_cfs.xread = (SCE_FReadFunc)fread;
    sce_cfs.xwrite = (SCE_FWriteFunc)fwrite;
    sce_cfs.xseek = (SCE_FSeekFunc)fseek;
    sce_cfs.xtell = (SCE_FTellFunc)ftell;
    sce_cfs.xrewind = (SCE_FRewindFunc)rewind;
    sce_cfs.xflush = (SCE_FFlushFunc)fflush;
    sce_cfs.xtruncate = my_truncate;
    sce_cfs.xlength = my_length;
    return SCE_OK;
}
void SCE_Quit_File (void)
{
}


void SCE_File_Init (SCE_SFile *fp)
{
    fp->file = NULL;
    fp->fs = NULL;
}
void* SCE_File_Get (SCE_SFile *fp)
{
    return fp->file;
}

int SCE_File_Open (SCE_SFile *fp, SCE_SFileSystem *fs, const char *fname,
                   int flags)
{
    if (!fs)
        fs = &sce_cfs;
    fp->fs = fs;
    if (!(fp->file = fs->xopen (fs->subfs, fname, flags)))
        goto fail;
    /* call the user-defined initializing function, if any */
    if (fs->xinit && fs->xinit (fs, fp) < 0)
        goto fail;
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}
int SCE_File_Close (SCE_SFile *fp)
{
    int r = fp->fs->xclose (fp->file);
    fp->fs = NULL;
    fp->file = NULL;
    return r;
}

size_t SCE_File_Read (void *ptr, size_t size, size_t nmemb, SCE_SFile *fp)
{
    return fp->fs->xread (ptr, size, nmemb, fp->file);
}
size_t SCE_File_Write (const void *ptr, size_t size, size_t nmemb,
                       SCE_SFile *fp)
{
    return fp->fs->xwrite (ptr, size, nmemb, fp->file);
}

int SCE_File_Seek (SCE_SFile *fp, long offset, int whence)
{
    return fp->fs->xseek (fp->file, offset, whence);
}
long SCE_File_Tell (SCE_SFile *fp)
{
    return fp->fs->xtell (fp->file);
}
void SCE_File_Rewind (SCE_SFile *fp)
{
    fp->fs->xrewind (fp->file);
}

int SCE_File_Flush (SCE_SFile *fp)
{
    return fp->fs->xflush (fp->file);
}

int SCE_File_Truncate (SCE_SFile *fp, size_t length)
{
    return fp->fs->xtruncate (fp, length);
}

size_t SCE_File_Length (const SCE_SFile *fp)
{
    return fp->fs->xlength (fp->file);
}
