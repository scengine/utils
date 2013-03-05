/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2013  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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

/* created: 22/02/2013
   updated: 22/02/2013 */

#include "SCE/utils/SCEUtils.h"
#include "SCE/utils/SCENullFileSystem.h"

/* does nothing, used in combination with FileCache for in-memory files */
SCE_SFileSystem sce_nullfs;

static void* xfopen (SCE_SFileSystem *fs, const char *fname, int flags)
{
    return (void*)((int)NULL + 1);   /* lol. */
}
static int xflush (void *fd)
{
    return 0;
}
static int xclose (void *fd)
{
    return 0;
}
static size_t xread (void *data, size_t size, size_t nmemb, void *fd)
{
    return 0;
}
static size_t xwrite (const void *data, size_t size, size_t nmemb, void *fd)
{
    return size * nmemb;
}
static int xseek (void *fd, long offset, int whence)
{
    return 0;
}
static long xtell (void *fd)
{
    return 0;
}
static void xrewind (void *fd)
{
}
static int xtruncate (SCE_SFile *fp, size_t length)
{
    return SCE_OK;
}
static size_t xlength (const void *f)
{
    return 0;
}

int SCE_Init_NullFS (void)
{
    sce_nullfs.udata = NULL;
    sce_nullfs.subfs = NULL;
    sce_nullfs.xinit = NULL;
    sce_nullfs.xopen = xfopen;
    sce_nullfs.xclose = xclose;
    sce_nullfs.xread = xread;
    sce_nullfs.xwrite = xwrite;
    sce_nullfs.xseek = xseek;
    sce_nullfs.xtell = xtell;
    sce_nullfs.xrewind = xrewind;
    sce_nullfs.xflush = xflush;
    sce_nullfs.xtruncate = xtruncate;
    sce_nullfs.xlength = xlength;
    return SCE_OK;
}
void SCE_Quit_NullFS (void)
{
}
