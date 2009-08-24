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
 
/* created: 02/01/2007
   updated: 07/07/2009 */

#ifndef SCERESOURCE_H
#define SCERESOURCE_H

#ifdef __cplusplus
extern "C" {
#endif

#if 0
/* examples */
void* loadf (const char *fname, int forced, void *udata);
int savef (void *object, const char *fname, int recursive, void *udata);
#endif

typedef void* (*SCE_FLoadResourceFunc)(const char*, int, void*);
typedef int (*SCE_FSaveResourceFunc)(void*, const char*, int, void*);

int SCE_Init_Resource (void);
void SCE_Quit_Resource (void);

int SCE_Resource_RegisterType (int, SCE_FLoadResourceFunc,
                               SCE_FSaveResourceFunc);

int SCE_Resource_Add (int, const char*, void*);
void* SCE_Resource_Load (int, const char*, int, void*);
int SCE_Resource_Free (void*);

unsigned int SCE_Resource_NumUsed (const char*, void*);
unsigned int SCE_Resource_NumLoaded (void);

char* SCE_Resource_GetName (void*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
