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

/* created: 17/05/2012
   updated: 18/05/2012 */

#include <stdlib.h>
#include <string.h>
#include "SCE/utils/SCEMath.h"
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEMemory.h"
#include "SCE/utils/SCEArray.h"

void SCE_Array_Init (SCE_SArray *a)
{
    a->ptr = NULL;
    a->removed = 0;
    a->size = 0;
    a->allocated = 0;
}
void SCE_Array_Clear (SCE_SArray *a)
{
    SCE_free (a->ptr);
}

static int SCE_Array_Realloc (SCE_SArray *a, size_t size)
{
    unsigned char *ptr = NULL;
    size_t pot = SCE_Math_NextPowerOfTwo (SCE_Array_GetSize (a));

    if (a->allocated >= pot && a->allocated < 2 * pot) {
        memmove (a->ptr, SCE_Array_Get (a), size);
    } else {
        a->allocated = pot;
        if (!(ptr = SCE_malloc (a->allocated))) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
        if (a->ptr) {
            memcpy (ptr, SCE_Array_Get (a), size);
            SCE_free (a->ptr);
        }
        a->ptr = ptr;
    }

    a->size -= a->removed;
    a->removed = 0;
    return SCE_OK;
}

int SCE_Array_Append (SCE_SArray *a, void *data, size_t size)
{
    size_t offset;
    size_t old_size;

    old_size = SCE_Array_GetSize (a);
    offset = a->size;
    a->size += size;
    if (a->size > a->allocated) {
        offset -= a->removed;
        if (SCE_Array_Realloc (a, old_size) < 0) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
    }
    memcpy (&a->ptr[offset], data, size);
    return SCE_OK;
}

int SCE_Array_Pop (SCE_SArray *a, size_t size)
{
    size_t old_size;

    a->removed += size;
    old_size = SCE_Array_GetSize (a);
    if (old_size * 2 < a->allocated) {
        if (SCE_Array_Realloc (a, old_size) < 0) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
    }
    return SCE_OK;
}

void* SCE_Array_Get (const SCE_SArray *a)
{
    return &a->ptr[a->removed];
}

size_t SCE_Array_GetSize (const SCE_SArray *a)
{
    return a->size - a->removed;
}
