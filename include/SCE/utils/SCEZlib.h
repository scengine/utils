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

#ifndef SCEZLIB_H
#define SCEZLIB_H

#include "SCE/utils/SCEArray.h"

#ifdef __cplusplus
extern "C" {
#endif

int SCE_Zlib_Compress (void*, size_t, int, SCE_SArray*);
int SCE_Zlib_Decompress (void*, size_t, SCE_SArray*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
