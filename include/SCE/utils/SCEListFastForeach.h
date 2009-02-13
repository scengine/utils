/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2009  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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

/* created: 26/01/2009
   updated: 27/01/2009 */

#ifndef SCELISTFASTFOREACH_H
#define SCELISTFASTFOREACH_H

#include <SCE/utils/SCEList.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*SCE_FListFastForeach)(SCE_SListIterator*, void*);
typedef void (*SCE_FListFastForeach2)(void*, void*);
typedef void (*SCE_FListFastForeach3)(SCE_SListIterator*);
typedef void (*SCE_FListFastForeach4)(void*);

int SCE_Init_FastList (void);
void SCE_Quit_FastList (void);

void SCE_List_FastForEach (SCE_SList*, unsigned int,
                           SCE_FListFastForeach, void*);
void SCE_List_FastForEach2 (SCE_SList*, unsigned int,
                           SCE_FListFastForeach2, void*);
void SCE_List_FastForEach3 (SCE_SList*, unsigned int,
                           SCE_FListFastForeach3);
void SCE_List_FastForEach4 (SCE_SList*, unsigned int,
                           SCE_FListFastForeach4);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
