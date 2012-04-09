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

/* created: 17/04/2010
   updated: 08/04/2012 */

#ifndef SCETYPE_H
#define SCETYPE_H

/* external dependencies */
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

enum sce_etype {
    SCE_NONE_TYPE = 0,
    SCE_BYTE,
    SCE_UNSIGNED_BYTE,
    SCE_SHORT,
    SCE_UNSIGNED_SHORT,
    SCE_INT,
    SCE_UNSIGNED_INT,
    SCE_FLOAT,
    SCE_DOUBLE,
    SCE_SIZE_T,
    SCE_NUM_NORMAL_TYPES,
    /* extra types for GL compatibility */
    SCE_HALF_FLOAT,
    SCE_UNSIGNED_BYTE_3_3_2,
    SCE_UNSIGNED_BYTE_2_3_3_REV,
    SCE_UNSIGNED_SHORT_5_6_5,
    SCE_UNSIGNED_SHORT_5_6_5_REV,
    SCE_UNSIGNED_SHORT_4_4_4_4,
    SCE_UNSIGNED_SHORT_4_4_4_4_REV,
    SCE_UNSIGNED_SHORT_5_5_5_1,
    SCE_UNSIGNED_SHORT_1_5_5_5_REV,
    SCE_UNSIGNED_INT_8_8_8_8,
    SCE_UNSIGNED_INT_8_8_8_8_REV,
    SCE_UNSIGNED_INT_10_10_10_2,
    SCE_UNSIGNED_INT_2_10_10_10_REV,
    SCE_UNSIGNED_INT_24_8,
    SCE_UNSIGNED_INT_10F_11F_11F_REV,
    SCE_UNSIGNED_INT_5_9_9_9_REV,
    SCE_FLOAT_32_UNSIGNED_INT_24_8_REV,
    SCE_NUM_TYPES
};
typedef enum sce_etype SCE_EType;

typedef unsigned int    SCEenum;
typedef unsigned int    SCEbitfield;
typedef int             SCEsizei;         /* 4 bytes signed */
typedef int             SCEint;           /* 4 bytes signed */
typedef unsigned int    SCEuint;          /* 4 bytes unsigned */
typedef short           SCEshort;         /* 2 bytes signed */
typedef unsigned short  SCEushort;        /* 2 bytes unsigned */
typedef signed char     SCEbyte;          /* 1 byte signed */
typedef unsigned char   SCEubyte;         /* 1 byte unsigned */
typedef char            SCEchar;          /* wat */
typedef float           SCEfloat;         /* single precision float */
typedef double          SCEdouble;        /* double precision float */

size_t SCE_Type_Sizeof (SCE_EType);

void SCE_Type_Convert (int, void*, int, const void*, size_t);
void* SCE_Type_ConvertDup (int, int, const void*, size_t);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
