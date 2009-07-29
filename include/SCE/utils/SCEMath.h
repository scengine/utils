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
   updated: 21/06/2009 */

#ifndef SCEMATH_H
#define SCEMATH_H

#include <math.h>
#include <SCE/SCECommon.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SCE_USE_SQRT_HACK 0

/* PI */
#ifndef M_PI
 #define M_PI 3.14159265358979323846
#endif

#ifndef RAD
 #define RAD 0.0174532925
#endif


#ifndef MAX
 #define MAX(a, b) (a > b ? a : b)
#endif

#ifndef MIN
 #define MIN(a, b) (a < b ? a : b)
#endif

float SCE_Math_Sqrt (float) SCE_GNUC_PURE;
float SCE_Math_InvSqrt (float) SCE_GNUC_PURE;

int SCE_Math_Powi (int, unsigned int) SCE_GNUC_PURE;

int SCE_Math_PowerOfTwo (int) SCE_GNUC_PURE;

int SCE_Math_Clamp (int, int, int) SCE_GNUC_PURE;
float SCE_Math_Clampf (float, float, float) SCE_GNUC_PURE;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
