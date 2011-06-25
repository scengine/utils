/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2011  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
 
/* created: 05/01/2007
   updated: 25/06/2011 */

#ifndef SCEMEDIA_H
#define SCEMEDIA_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* obsolete, void type is 0 */
/*#define SCE_UNKNOWN_TYPE -1*/

typedef void* (*SCE_FMediaLoadFunc) (FILE*, const char*, void*);
typedef int (*SCE_FMediaSaveFunc) (void*, const char*);

/**
 * \brief Parse a path to a file to load
 * \param data user data
 * \param type type of the data of \p path
 * \param path path to parse
 * \return the final path that will be used for loading, must return a
 * freeable pointer (ie. SCE_free() will be called on it)
 */
typedef char* (*SCE_FMediaParsePathFunc) (void *data, int type,
                                          const char *path);

int SCE_Init_Media (void);
void SCE_Quit_Media (void);

void SCE_Media_SetParsePathFunc (SCE_FMediaParsePathFunc, void*);

int SCE_Media_Register (int, const char*, SCE_FMediaLoadFunc,
                        SCE_FMediaSaveFunc);

void* SCE_Media_Load (int, const char*, void*);
int SCE_Media_Save (int, void*, const char*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
