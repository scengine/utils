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

#ifndef SCEARRAY2D_H
#define SCEARRAY2D_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sce_sarray2d SCE_SArray2D;
struct sce_sarray2d {
    char *ptr;
    void *empty_pattern;
    size_t size;                /* size of each element (default is 1) */
    size_t w, h;                /* allocated size */
    size_t x, y;                /* coordinates (offset) of the origin (0,0) */
};

void SCE_Array2D_Init (SCE_SArray2D*);
void SCE_Array2D_Clear (SCE_SArray2D*);

void SCE_Array2D_SetElementSize (SCE_SArray2D*, size_t);
int SCE_Array2D_SetEmptyPattern (SCE_SArray2D*, const void*);

int SCE_Array2D_Set (SCE_SArray2D*, long, long, void*);
int SCE_Array2D_Get (SCE_SArray2D*, long, long, void*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
