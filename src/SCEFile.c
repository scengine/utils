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
   updated: 10/08/2012 */

#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEFile.h"

/* standard C functions */
SCE_SFileSystem sce_cfs;

static void* my_fopen (const char *fname, const char *mode)
{
    FILE *fp = NULL;
    if (!(fp = fopen (fname, mode))) {
        SCEE_LogErrno (fname);
        return NULL;
    }
    return fp;
}

int SCE_Init_File (void)
{
    sce_cfs.udata = NULL;
    sce_cfs.xopen = my_fopen;
    sce_cfs.xclose = (SCE_FCloseFunc)fclose;
    sce_cfs.xread = (SCE_FReadFunc)fread;
    sce_cfs.xwrite = (SCE_FWriteFunc)fwrite;
    sce_cfs.xseek = (SCE_FSeekFunc)fseek;
    sce_cfs.xtell = (SCE_FTellFunc)ftell;
    sce_cfs.xrewind = (SCE_FRewindFunc)rewind;
    sce_cfs.xflush = (SCE_FFlushFunc)fflush;
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

int SCE_File_Open (SCE_SFile *fp, SCE_SFileSystem *fs, const char *fname,
                   const char *mode)
{
    if (!fs)
        fs = &sce_cfs;
    fp->fs = fs;
    fp->file = fs->xopen (fname, mode);
    if (fp->file)
        return SCE_OK;
    else {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
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
size_t SCE_File_Write (void *ptr, size_t size, size_t nmemb, SCE_SFile *fp)
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
