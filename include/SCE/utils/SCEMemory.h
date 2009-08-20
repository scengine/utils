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
 
/* created: 22/12/2006
   updated: 23/10/2008 */

#ifndef SCEMEMORY_H
#define SCEMEMORY_H

#include <stdlib.h>
#include <SCE/SCECommon.h>
#include <SCE/core/SCECTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup memory
 * @{
 */

/**
 * \brief Main malloc wrapper
 * \see SCE_Mem_Alloc()
 */
#ifdef SCE_DEBUG
#define SCE_malloc(size) SCE_Mem_Alloc(__FILE__, __LINE__, size)
#else
#define SCE_malloc(size) malloc(size)
#endif
/**
 * \brief Main calloc wrapper
 * \see SCE_Mem_Calloc()
 */
#ifdef SCE_DEBUG
#define SCE_calloc(size, nb) SCE_Mem_Calloc(__FILE__, __LINE__, size, nb)
#else
#define SCE_calloc(size, nb) calloc(size, nb)
#endif
/**
 * \brief Main realloc wrapper
 * \see SCE_Mem_Realloc()
 */
#ifdef SCE_DEBUG
#define SCE_realloc(ptr, size) SCE_Mem_Realloc(__FILE__, __LINE__, ptr, size)
#else
#define SCE_realloc(ptr, size) realloc(ptr, size)
#endif
/**
 * \brief Main free wrapper
 * \see SCE_Mem_Free()
 * \note This macro is guaranteed to expand to a function. It means that it is
 *       possible to use it exactly as a function, including passing it as
 *       argument of another function taking a function pointer and so on.
 */
#ifdef SCE_DEBUG
#define SCE_free(p) SCE_Mem_Free (__FILE__, __LINE__, p)
#else
#define SCE_free free
#endif

/** @} */

int SCE_Init_Mem (void);

void* SCE_Mem_Alloc (const char*, unsigned int, size_t)
    SCE_GNUC_MALLOC
    SCE_GNUC_ALLOC_SIZE (3);
void* SCE_Mem_Calloc (const char*, unsigned int, size_t, size_t)
    SCE_GNUC_MALLOC
    SCE_GNUC_ALLOC_SIZE2 (3, 4);
void* SCE_Mem_Realloc (const char*, unsigned int, void*, size_t)
    SCE_GNUC_ALLOC_SIZE (4);
void SCE_Mem_Free (const char*, int, void*);

void* SCE_Mem_Dup (const void*, size_t)
    SCE_GNUC_MALLOC
    SCE_GNUC_ALLOC_SIZE (2);

void SCE_Mem_Convert (int, void*, int, const void*, size_t);
void* SCE_Mem_ConvertDup (int, int, const void*, size_t);

#ifdef SCE_DEBUG
int SCE_Mem_IsValid (void*);
void SCE_Mem_List (void);
size_t SCE_Mem_GetSize (void*);
const char* SCE_Mem_GetFile (void*);
unsigned int SCE_Mem_GetLine (void*);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
