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
   updated: 10/08/2012 */

#ifndef SCEGZFILE_H
#define SCEGZFILE_H

#include "SCE/utils/SCEFile.h"

#ifdef __cplusplus
extern "C" {
#endif

extern SCE_SFileSystem sce_gzfs;

int SCE_Init_GZFile (void);
void SCE_Quit_GZFile (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
