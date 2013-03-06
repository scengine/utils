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

#ifndef SCESHA1_H
#define SCESHA1_H

#include <stdlib.h>
#include <stdio.h>

#define SCE_SHA1_STRING_SIZE 41
#define SCE_SHA1_SIZE 20

typedef unsigned char SCE_TSha1[SCE_SHA1_SIZE];

void SCE_Sha1_Init (SCE_TSha1 sum);

void SCE_Sha1_Sum (SCE_TSha1 sum, const unsigned char *data, size_t len);
int SCE_Sha1_StreamSum (SCE_TSha1 sum, SCE_SFile *fp);
int SCE_Sha1_FileSum (SCE_TSha1 sum, const char *filename);

int SCE_Sha1_FromString (SCE_TSha1 sum, const char *str);
void SCE_Sha1_ToString (char *str, SCE_TSha1 sum);

int SCE_Sha1_Equal (SCE_TSha1 s1, SCE_TSha1 s2);

#endif /* guard */
