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

/* created: 14/06/2013
   updated: 14/06/2013 */

#include <stdlib.h>
#include <string.h>
#include "SCE/utils/SCEMath.h"
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEMemory.h"
#include "SCE/utils/SCEArray2D.h"

void SCE_Array2D_Init (SCE_SArray2D *a)
{
    a->ptr = NULL;
    a->empty_pattern = NULL;
    a->size = 1;
    a->w = a->h = 0;
    a->x = a->y = 0;
}
void SCE_Array2D_Clear (SCE_SArray2D *a)
{
    SCE_free (a->ptr);
    SCE_free (a->empty_pattern);
}

void SCE_Array2D_SetElementSize (SCE_SArray2D *a, size_t size)
{
    a->size = size;
}

int SCE_Array2D_SetEmptyPattern (SCE_SArray2D *a, const void *pattern)
{
    SCE_free (a->empty_pattern);
    if (!(a->empty_pattern = SCE_malloc (a->size))) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    memcpy (a->empty_pattern, pattern, a->size);
    return SCE_OK;
}

static int SCE_Array2D_IsPointAllocated (SCE_SArray2D *a, long x, long y)
{
    long u, v;

    u = x + a->x;
    v = y + a->y;

    return u < a->w && v < a->h && u >= 0 && v >= 0;
}

static size_t xoffset (SCE_SArray2D *a, long x, long y)
{
    x += a->x;
    y += a->y;
    return (y * a->w + x) * a->size;
}

static int SCE_Array2D_Realloc (SCE_SArray2D *a, long x, long y,
                                size_t w, size_t h)
{
    SCE_SArray2D tmp;
    char *new = NULL;
    size_t i, j;

    if (!(new = SCE_malloc (w * h * a->size))) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }

    /* fillup with empty pattern */
    for (i = 0; i < w * h; i++)
        memcpy (&new[i * a->size], a->empty_pattern, a->size);

    tmp = *a;

    a->ptr = new;
    a->x = x;
    a->y = y;
    a->w = w;
    a->h = h;

    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            x = i - a->x;
            y = j - a->y;

            if (SCE_Array2D_IsPointAllocated (&tmp, x, y))
                memcpy ( &a->ptr[xoffset (   a, x, y)],
                        &tmp.ptr[xoffset (&tmp, x, y)], a->size);
        }
    }

    SCE_free (tmp.ptr);

    return SCE_OK;
}

static int SCE_Array2D_Expand (SCE_SArray2D *a, long x, long y)
{
    size_t new_x, new_y;
    size_t new_w, new_h;

    x += a->x;
    y += a->y;

    new_x = a->x;
    new_y = a->y;
    new_w = a->w;
    new_h = a->h;

    if (x >= (long)a->w) {
        new_w = MAX ((size_t)x + 1, 2 * a->w);
    } else if (x < 0) {
        new_w = MAX ((size_t)(-x) + a->w, 2 * a->w);
        new_x = a->x + (size_t)(new_w - (long)a->w);
    }

    if (y >= (long)a->h) {
        new_h = MAX ((size_t)y + 1, 2 * a->h);
    } else if (y < 0) {
        new_h = MAX ((size_t)(-y) + a->h, 2 * a->h);
        new_y = a->y + (size_t)(new_h - (long)a->h);
    }

    return SCE_Array2D_Realloc (a, new_x, new_y, new_w, new_h);
}

int SCE_Array2D_Set (SCE_SArray2D *a, long x, long y, void *data)
{
    if (!SCE_Array2D_IsPointAllocated (a, x, y)) {
        if (SCE_Array2D_Expand (a, x, y) < 0) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
    }

    memcpy (&a->ptr[xoffset (a, x, y)], data, a->size);
    return SCE_OK;
}

/**
 * \brief 
 * \return SCE_TRUE if the operation succeeded, SCE_FALSE if the given
 * coordinates are outside of the allocated area 
 */
int SCE_Array2D_Get (SCE_SArray2D *a, long x, long y, void *data)
{
    if (!SCE_Array2D_IsPointAllocated (a, x, y))
        return SCE_FALSE;

    memcpy (data, &a->ptr[xoffset (a, x, y)], a->size);
    return SCE_TRUE;
}
