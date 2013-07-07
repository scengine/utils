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
 
/* created: 21/12/2006
   updated: 10/03/2013 */

#ifndef SCEVECTOR_H
#define SCEVECTOR_H

#include <string.h> /* for memcpy */
#include "SCE/utils/SCEMath.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef float SCE_TVector4[4];
typedef float SCE_TVector3[3];
typedef float SCE_TVector2[2];

#define SCE_Vec4(v) (v)[0], (v)[1], (v)[2], (v)[3]
#define SCE_Vec3(v) (v)[0], (v)[1], (v)[2]
#define SCE_Vec2(v) (v)[0], (v)[1]

#define SCE_Vector3_Distance(a, b)\
    SCE_Math_Sqrt ((((a)[0]-(b)[0])*((a)[0]-(b)[0]))+\
                   (((a)[1]-(b)[1])*((a)[1]-(b)[1]))+\
                   (((a)[2]-(b)[2])*((a)[2]-(b)[2])))
#define SCE_Vector2_Distance(a, b)\
    SCE_Math_Sqrt ((((a)[0]-(b)[0])*((a)[0]-(b)[0]))+\
                   (((a)[1]-(b)[1])*((a)[1]-(b)[1])))

#define SCE_Vector3_Length(a)\
    SCE_Math_Sqrt(((a)[0]*(a)[0])+((a)[1]*(a)[1])+((a)[2]*(a)[2]))
#define SCE_Vector2_Length(a)\
    SCE_Math_Sqrt(((a)[0]*(a)[0])+((a)[1]*(a)[1]))

/* TODO: use '=' instead... ? */
#define SCE_Vector4_Copy(v1, v2) memcpy (v1, v2, 4 * sizeof (float))
#define SCE_Vector3_Copy(v1, v2) memcpy (v1, v2, 3 * sizeof (float))
#define SCE_Vector2_Copy(v1, v2) memcpy (v1, v2, 2 * sizeof (float))

#define SCE_Vector2_Set(v, x, y) do {\
        (v)[0] = (x), (v)[1] = (y);} while (0)
#define SCE_Vector3_Set(v, x, y, z) do {\
        (v)[0] = (x), (v)[1] = (y), (v)[2] = (z);} while (0)
#define SCE_Vector4_Set(v, x, y, z, w) do {\
        (v)[0] = (x), (v)[1] = (y), (v)[2] = (z), (v)[3] = (w);} while (0)

#define SCE_Vector3_Dot(v1, v2)\
    (((v1)[0]*(v2)[0])+((v1)[1]*(v2)[1])+((v1)[2]*(v2)[2]))
#define SCE_Vector2_Dot(v1, v2)\
    (((v1)[0]*(v2)[0])+((v1)[1]*(v2)[1]))
#define SCE_Vector4_Dot(v1, v2)\
    (((v1)[0]*(v2)[0])+((v1)[1]*(v2)[1])+((v1)[2]*(v2)[2])+((v1)[3]*(v2)[3]))

#define SCE_Vector3_Cross(v1, v2, v3) do {\
    (v1)[0] = (v2)[1]*(v3)[2] - (v2)[2]*(v3)[1];\
    (v1)[1] = (v2)[2]*(v3)[0] - (v2)[0]*(v3)[2];\
    (v1)[2] = (v2)[0]*(v3)[1] - (v2)[1]*(v3)[0];} while (0)

#define SCE_Vector3_IsNull(v)                                   \
    (SCE_Math_Fabsf ((v)[0]) + SCE_Math_Fabsf ((v)[1]) +        \
     SCE_Math_Fabsf ((v)[2]) < SCE_EPSILONF)

#define SCE_Vector2_Operator1(v, o, n) do {\
        (v)[0] o (n), (v)[1] o (n);} while (0)
#define SCE_Vector2_Operator1v(v1, o, v2) do {\
    (v1)[0] o (v2)[0];\
    (v1)[1] o (v2)[1];} while (0)

#define SCE_Vector2_Operator2(v, o1, v2, o2, n) do {\
    (v)[0] o1 (v2)[0] o2 (n);\
    (v)[1] o1 (v2)[1] o2 (n);} while (0)
#define SCE_Vector2_Operator2v(v1, o1, v2, o2, v3) do {\
    (v1)[0] o1 (v2)[0] o2 (v3)[0];\
    (v1)[1] o1 (v2)[1] o2 (v3)[1];} while (0)

#define SCE_Vector2_Operator3v(v1, o1, v2, o2, v3, o3, v4) do {\
    (v1)[0] o1 (v2)[0] o2 (v3)[0] o3 (v4)[0];\
    (v1)[1] o1 (v2)[1] o2 (v3)[1] o3 (v4)[1];} while (0)


#define SCE_Vector3_Operator1(v, o, n) do {\
        (v)[0] o (n); (v)[1] o (n); (v)[2] o (n);} while (0)
#define SCE_Vector3_Operator1v(v1, o, v2) do {\
    (v1)[0] o (v2)[0];\
    (v1)[1] o (v2)[1];\
    (v1)[2] o (v2)[2];} while (0)

#define SCE_Vector3_Operator2(v, o1, v2, o2, n) do {\
    (v)[0] o1 (v2)[0] o2 (n);\
    (v)[1] o1 (v2)[1] o2 (n);\
    (v)[2] o1 (v2)[2] o2 (n);} while (0)
#define SCE_Vector3_Operator2v(v1, o1, v2, o2, v3) do { \
    (v1)[0] o1 (v2)[0] o2 (v3)[0];\
    (v1)[1] o1 (v2)[1] o2 (v3)[1];\
    (v1)[2] o1 (v2)[2] o2 (v3)[2];} while (0)

#define SCE_Vector3_Operator3(v1, o1, v2, o2, v3, o3, n) do {\
    (v1)[0] o1 (v2)[0] o2 (v3)[0] o3 (n);\
    (v1)[1] o1 (v2)[1] o2 (v3)[1] o3 (n);\
    (v1)[2] o1 (v2)[2] o2 (v3)[2] o3 (n);} while (0)
#define SCE_Vector3_Operator3v(v1, o1, v2, o2, v3, o3, v4) do {\
    (v1)[0] o1 (v2)[0] o2 (v3)[0] o3 (v4)[0];\
    (v1)[1] o1 (v2)[1] o2 (v3)[1] o3 (v4)[1];\
    (v1)[2] o1 (v2)[2] o2 (v3)[2] o3 (v4)[2];} while (0)


#define SCE_Vector4_Operator1(v, o, n) do {\
        (v)[0] o (n); (v)[1] o (n); (v)[2] o (n); (v)[3] o (n);} while (0)
#define SCE_Vector4_Operator1v(v1, o, v2) do {\
    (v1)[0] o (v2)[0];\
    (v1)[1] o (v2)[1];\
    (v1)[2] o (v2)[2];\
    (v1)[3] o (v2)[3];} while (0)

#define SCE_Vector4_Operator2(v, o1, v2, o2, n) do {\
    (v)[0] o1 (v2)[0] o2 (n);\
    (v)[1] o1 (v2)[1] o2 (n);\
    (v)[2] o1 (v2)[2] o2 (n);\
    (v)[3] o1 (v2)[3] o2 (n);} while (0)
#define SCE_Vector4_Operator2v(v1, o1, v2, o2, v3) do { \
    (v1)[0] o1 (v2)[0] o2 (v3)[0];\
    (v1)[1] o1 (v2)[1] o2 (v3)[1];\
    (v1)[2] o1 (v2)[2] o2 (v3)[2];\
    (v1)[3] o1 (v2)[3] o2 (v3)[3];} while (0)

#define SCE_Vector4_Operator3(v1, o1, v2, o2, v3, o3, n) do {\
    (v1)[0] o1 (v2)[0] o2 (v3)[0] o3 (n);\
    (v1)[1] o1 (v2)[1] o2 (v3)[1] o3 (n);\
    (v1)[2] o1 (v2)[2] o2 (v3)[2] o3 (n);\
    (v1)[3] o1 (v2)[3] o2 (v3)[3] o3 (n);} while (0)
#define SCE_Vector4_Operator3v(v1, o1, v2, o2, v3, o3, v4) do {\
    (v1)[0] o1 (v2)[0] o2 (v3)[0] o3 (v4)[0];\
    (v1)[1] o1 (v2)[1] o2 (v3)[1] o3 (v4)[1];\
    (v1)[2] o1 (v2)[2] o2 (v3)[2] o3 (v4)[2];\
    (v1)[3] o1 (v2)[3] o2 (v3)[3] o3 (v4)[3];} while (0)


int SCE_Vector3_IsZero (const SCE_TVector3);
int SCE_Vector3_Collinear (const SCE_TVector3, const SCE_TVector3);

void SCE_Vector3_Normalize (SCE_TVector3);
void SCE_Vector2_Normalize (SCE_TVector2);

void SCE_Vector3_RotateX (SCE_TVector3, float, float);
void SCE_Vector3_RotateY (SCE_TVector3, float, float);
void SCE_Vector3_RotateZ (SCE_TVector3, float, float);

void SCE_Vector3_GetMin (SCE_TVector3, const SCE_TVector3, const SCE_TVector3);
void SCE_Vector3_GetMax (SCE_TVector3, const SCE_TVector3, const SCE_TVector3);
void SCE_Vector3_AbsMin (SCE_TVector3, const SCE_TVector3, const SCE_TVector3);
void SCE_Vector3_AbsMax (SCE_TVector3, const SCE_TVector3, const SCE_TVector3);

void SCE_Vector3_Interpolate (SCE_TVector3, SCE_TVector3, float, SCE_TVector3);

float SCE_Vector3_Trilinear1 (SCE_TVector3, float[8]);

void SCE_Vector3_Perpendicular (const SCE_TVector3, SCE_TVector3);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
