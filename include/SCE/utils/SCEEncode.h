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

/* created: 06/05/2012
   updated: 22/08/2012 */

#ifndef SCEENCODE_H
#define SCEENCODE_H

#include <stdlib.h>
#include "SCE/utils/SCEFile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_ENCODE_LONG_SIZE 4

int SCE_Encode_Float (float, int, unsigned char, unsigned char, int*,
                      unsigned char**);
float SCE_Decode_Float (const unsigned char**, int, unsigned char,
                        unsigned char, int*);

size_t SCE_Encode_Floats (const float*, size_t, int, unsigned char,
                          unsigned char, unsigned char*);
void SCE_Decode_Floats (float*, size_t, int, unsigned char,
                        unsigned char, const unsigned char*);

size_t SCE_Encode_StreamFloat (float, int, unsigned char, unsigned char,
                               SCE_SFile*);
float SCE_Decode_StreamFloat (int, unsigned char, unsigned char, SCE_SFile*);

size_t SCE_Encode_StreamFloats (const float*, size_t, int, unsigned char,
                                unsigned char, SCE_SFile*);
void SCE_Decode_StreamFloats (float*, size_t, int, unsigned char,
                              unsigned char, SCE_SFile*);

void SCE_Encode_Long (long, unsigned char*);
long SCE_Decode_Long (const unsigned char*);

void SCE_Encode_StreamLong (long, SCE_SFile*);
long SCE_Decode_StreamLong (SCE_SFile*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
