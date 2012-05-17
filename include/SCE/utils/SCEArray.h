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
   updated: 17/05/2012 */

#ifndef SCEARRAY_H
#define SCEARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sce_sarray SCE_SArray;
struct sce_sarray {
    unsigned char *ptr;
    size_t size;
    size_t allocated;
};

void SCE_Array_Init (SCE_SArray*);
void SCE_Array_Clear (SCE_SArray*);

int SCE_Array_Append (SCE_SArray*, void*, size_t);
void* SCE_Array_Get (const SCE_SArray*);
size_t SCE_Array_GetSize (const SCE_SArray*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
