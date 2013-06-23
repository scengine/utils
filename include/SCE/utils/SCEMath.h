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
 
/* created: 22/12/2006
   updated: 15/04/2013 */

#ifndef SCEMATH_H
#define SCEMATH_H

#include <math.h>
#include "SCE/utils/SCEMacros.h"

#ifdef __cplusplus
extern "C" {
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
 #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
 #define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define SCE_EPSILONF (0.000001f)
#define SCE_EPSILOND (0.000000001)

#ifdef HAVE_FABSF
# define SCE_Math_Fabsf fabsf
#else
# define SCE_Math_Fabsf(x) ((float)fabs ((x)))
#endif
#ifdef HAVE_COSF
# define SCE_Math_Cosf cosf
#else
# define SCE_Math_Cosf(x) ((float)cos ((x)))
#endif
#ifdef HAVE_SINF
# define SCE_Math_Sinf sinf
#else
# define SCE_Math_Sinf(x) ((float)sin ((x)))
#endif
#ifdef HAVE_TANF
# define SCE_Math_Tanf tanf
#else
# define SCE_Math_Tanf(x) ((float)tan ((x)))
#endif
#ifdef HAVE_POWF
# define SCE_Math_Powf powf
#else
# define SCE_Math_Powf(x, y) ((float)pow ((x), (y)))
#endif
#ifdef HAVE_ATANF
# define SCE_Math_Atanf atanf
#else
# define SCE_Math_Atanf(x) ((float)atan ((x)))
#endif
#ifdef HAVE_ATAN2F
# define SCE_Math_Atan2f atan2f
#else
# define SCE_Math_Atan2f(x, y) ((float)atan2 ((x), (y)))
#endif
#ifdef HAVE_FLOORF
# define SCE_Math_Floorf floorf
#else
# define SCE_Math_Floorf(x) ((float)floorf (x))
#endif

int SCE_Math_IsZero (float) SCE_GNUC_PURE;

float SCE_Math_Sqrt (float) SCE_GNUC_PURE;
float SCE_Math_InvSqrt (float) SCE_GNUC_PURE;

int SCE_Math_Powi (int, unsigned int) SCE_GNUC_PURE;

int SCE_Math_PowerOfTwo (unsigned int) SCE_GNUC_PURE;
unsigned int SCE_Math_NextPowerOfTwo (unsigned int) SCE_GNUC_PURE;

int SCE_Math_Clamp (int, int, int) SCE_GNUC_PURE;
float SCE_Math_Clampf (float, float, float) SCE_GNUC_PURE;

float SCE_Math_Fractf (float);

int SCE_Math_Ring (int, int);

int SCE_Math_RandRange (int, int);
float SCE_Math_RandRangef (float, float);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
