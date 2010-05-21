/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2010  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
   updated: 21/05/2010 */

#ifndef SCEMATRIX_H
#define SCEMATRIX_H

#include <SCE/utils/SCEVector.h>
#include <SCE/utils/SCEQuaternion.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef float SCE_TMatrix4[16];
typedef float SCE_TMatrix3[9];
typedef float SCE_TMatrix4x3[12];

#define SCE_Matrix4_Set(ma, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p){\
    (ma)[0]  = a; (ma)[1]  = b; (ma)[2]  = c; (ma)[3]  = d; \
    (ma)[4]  = e; (ma)[5]  = f; (ma)[6]  = g; (ma)[7]  = h; \
    (ma)[8]  = i; (ma)[9]  = j; (ma)[10] = k; (ma)[11] = l; \
    (ma)[12] = m; (ma)[13] = n; (ma)[14] = o; (ma)[15] = p;}

#define SCE_Matrix3_Set(ma, a,b,c,d,e,f,g,h,i){\
    (ma)[0] = a; (ma)[1] = b; (ma)[2] = c;\
    (ma)[3] = d; (ma)[4] = e; (ma)[5] = f;\
    (ma)[6] = g; (ma)[7] = h; (ma)[8] = i;}

#define SCE_Matrix4x3_Set(ma, a,b,c,d,e,f,g,h,i,j,k,l){\
    (ma)[0]  = a; (ma)[1]  = b; (ma)[2]  = c; (ma)[3]  = d; \
    (ma)[4]  = e; (ma)[5]  = f; (ma)[6]  = g; (ma)[7]  = h; \
    (ma)[8]  = i; (ma)[9]  = j; (ma)[10] = k; (ma)[11] = l;}

/* copies n into m */
#if 0
#define SCE_Matrix4_Copy(m, n) memcpy ((m), (n), 16 * sizeof *(m))
#else
#define SCE_Matrix4_Copy(m, n) memcpy ((m), (n), 16 * sizeof (float))
#endif
#define SCE_Matrix4_CopyM4x3(m, n) memcpy ((m), (n), 12 * sizeof *(m))
#define SCE_Matrix3_Copy(m, n) memcpy ((m), (n), 9 * sizeof *(m))
#define SCE_Matrix4x3_Copy(m, n) memcpy ((m), (n), 12 * sizeof *(m))
#define SCE_Matrix4x3_CopyM4(m, n) memcpy ((m), (n), 12 * sizeof *(m))


int SCE_Init_Matrix (void);
void SCE_Quit_Matrix (void);

void SCE_Matrix4_Identity (SCE_TMatrix4);
void SCE_Matrix3_Identity (SCE_TMatrix3);
void SCE_Matrix4x3_Identity (SCE_TMatrix4x3);

void SCE_Matrix4_CopyM3 (SCE_TMatrix4, SCE_TMatrix3);
void SCE_Matrix3_CopyM4 (SCE_TMatrix3, SCE_TMatrix4);
void SCE_Matrix3_CopyM4x3 (SCE_TMatrix3, SCE_TMatrix4x3);
void SCE_Matrix4x3_CopyM3 (SCE_TMatrix4x3, SCE_TMatrix3);

float* SCE_Matrix4_Mul (SCE_TMatrix4, SCE_TMatrix4, SCE_TMatrix4);
float* SCE_Matrix4_MulCopy (SCE_TMatrix4, SCE_TMatrix4);
float* SCE_Matrix3_Mul (SCE_TMatrix3, SCE_TMatrix3, SCE_TMatrix3);
float* SCE_Matrix3_MulCopy (SCE_TMatrix3, SCE_TMatrix3);
float* SCE_Matrix4x3_Mul (SCE_TMatrix4x3, SCE_TMatrix4x3, SCE_TMatrix4x3);
float* SCE_Matrix4x3_MulCopy (SCE_TMatrix4x3, SCE_TMatrix4x3);

void SCE_Matrix4_Add (SCE_TMatrix4, SCE_TMatrix4);
void SCE_Matrix4_Sub (SCE_TMatrix4, SCE_TMatrix4);

void SCE_Matrix4_Transpose (const SCE_TMatrix4, SCE_TMatrix4);
void SCE_Matrix4_TransposeCopy (SCE_TMatrix4);
void SCE_Matrix3_Transpose (const SCE_TMatrix3, SCE_TMatrix3);
void SCE_Matrix3_TransposeCopy (SCE_TMatrix3);

void SCE_Matrix4_Inverse (const SCE_TMatrix4, SCE_TMatrix4);
void SCE_Matrix4_InverseCopy (SCE_TMatrix4);
void SCE_Matrix3_Inverse (const SCE_TMatrix3, SCE_TMatrix3);
void SCE_Matrix3_InverseCopy (SCE_TMatrix3);
void SCE_Matrix4x3_Inverse (const SCE_TMatrix4x3, SCE_TMatrix4x3);
void SCE_Matrix4x3_InverseCopy (SCE_TMatrix4x3);

void SCE_Matrix4_Interpolate (SCE_TMatrix4, SCE_TMatrix4, float, SCE_TMatrix4);
void SCE_Matrix3_Interpolate (SCE_TMatrix3, SCE_TMatrix3, float, SCE_TMatrix3);
void SCE_Matrix4x3_Interpolate (SCE_TMatrix4x3, SCE_TMatrix4x3, float,
                                SCE_TMatrix4x3);

void SCE_Matrix4_Translate (SCE_TMatrix4, float, float, float);
void SCE_Matrix4_Translatev (SCE_TMatrix4, SCE_TVector3);

void SCE_Matrix4_MulTranslate (SCE_TMatrix4, float, float, float);
void SCE_Matrix4_MulTranslatev (SCE_TMatrix4, SCE_TVector3);

float SCE_Matrix4_Trace (const SCE_TMatrix4);
float SCE_Matrix3_Trace (const SCE_TMatrix3);
float SCE_Matrix4x3_Trace (const SCE_TMatrix4x3);

void SCE_Matrix4_RotX (SCE_TMatrix4, float);
void SCE_Matrix3_RotX (SCE_TMatrix3, float);
void SCE_Matrix4_RotY (SCE_TMatrix4, float);
void SCE_Matrix3_RotY (SCE_TMatrix3, float);
void SCE_Matrix4_RotZ (SCE_TMatrix4, float);
void SCE_Matrix3_RotZ (SCE_TMatrix3, float);

void SCE_Matrix4_MulRotX (SCE_TMatrix4, float);
void SCE_Matrix3_MulRotX (SCE_TMatrix3, float);
void SCE_Matrix4_MulRotY (SCE_TMatrix4, float);
void SCE_Matrix3_MulRotY (SCE_TMatrix3, float);
void SCE_Matrix4_MulRotZ (SCE_TMatrix4, float);
void SCE_Matrix3_MulRotZ (SCE_TMatrix3, float);

void SCE_Matrix4_Rotate (SCE_TMatrix4, float, float, float, float);
void SCE_Matrix4_Rotatev (SCE_TMatrix4, float, SCE_TVector3);
void SCE_Matrix3_Rotate (SCE_TMatrix3, float, float, float, float);
void SCE_Matrix3_Rotatev (SCE_TMatrix3, float, SCE_TVector3);

void SCE_Matrix4_MulRotate (SCE_TMatrix4, float, float, float, float);
void SCE_Matrix4_MulRotatev(SCE_TMatrix4, float, SCE_TVector3);
void SCE_Matrix3_MulRotate (SCE_TMatrix3, float, float, float, float);
void SCE_Matrix3_MulRotatev(SCE_TMatrix3, float, SCE_TVector3);

void SCE_Matrix4_Scale (SCE_TMatrix4, float, float, float);
void SCE_Matrix4_Scalev (SCE_TMatrix4, SCE_TVector3);
void SCE_Matrix3_Scale (SCE_TMatrix3, float, float, float);
void SCE_Matrix3_Scalev (SCE_TMatrix3, SCE_TVector3);

void SCE_Matrix4_MulScale (SCE_TMatrix4, float, float, float);
void SCE_Matrix4_MulScalev (SCE_TMatrix4, SCE_TVector3);
void SCE_Matrix3_MulScale (SCE_TMatrix3, float, float, float);
void SCE_Matrix3_MulScalev (SCE_TMatrix3, SCE_TVector3);

void SCE_Matrix4_FromQuaternion (SCE_TMatrix4, SCE_TQuaternion);
void SCE_Matrix3_FromQuaternion (SCE_TMatrix3, SCE_TQuaternion);
void SCE_Matrix4x3_FromQuaternion (SCE_TMatrix4x3, SCE_TQuaternion);

void SCE_Matrix4_ToQuaternion (SCE_TMatrix4, SCE_TQuaternion);
void SCE_Matrix3_ToQuaternion (SCE_TMatrix3, SCE_TQuaternion);
void SCE_Matrix4x3_ToQuaternion (SCE_TMatrix4x3, SCE_TQuaternion);

void SCE_Matrix4_MulV3 (SCE_TMatrix4, SCE_TVector3, SCE_TVector3);
void SCE_Matrix4_MulV3w (SCE_TMatrix4, SCE_TVector3, float, SCE_TVector3);
void SCE_Matrix4_MulV3Copy (SCE_TMatrix4, SCE_TVector3);
void SCE_Matrix4_MulV3Copyw (SCE_TMatrix4, SCE_TVector3, float);
void SCE_Matrix4_MulV4 (SCE_TMatrix4, SCE_TVector4, SCE_TVector4);
void SCE_Matrix4_MulV4Copy (SCE_TMatrix4, SCE_TVector4);

void SCE_Matrix3_MulV3 (SCE_TMatrix4, SCE_TVector3, SCE_TVector3);
void SCE_Matrix3_MulV3Copy (SCE_TMatrix4, SCE_TVector3);

void SCE_Matrix4x3_MulV3 (SCE_TMatrix4x3, SCE_TVector3, SCE_TVector3);
void SCE_Matrix4x3_MulV3w (SCE_TMatrix4x3, SCE_TVector3, float, SCE_TVector3);
void SCE_Matrix4x3_MulV3Add (SCE_TMatrix4x3, SCE_TVector3, SCE_TVector3);
void SCE_Matrix4x3_MulV3Addw (SCE_TMatrix4x3, SCE_TVector3, float,SCE_TVector3);
void SCE_Matrix4x3_MulV3Copy (SCE_TMatrix4x3, SCE_TVector3);
void SCE_Matrix4x3_MulV3Copyw (SCE_TMatrix4x3, SCE_TVector3, float);
void SCE_Matrix4x3_MulV4 (SCE_TMatrix4x3, SCE_TVector4, SCE_TVector3);
void SCE_Matrix4x3_MulV4Add (SCE_TMatrix4x3, SCE_TVector4, SCE_TVector3);
void SCE_Matrix4x3_MulV4Copy (SCE_TMatrix4x3, SCE_TVector4);

void SCE_Matrix4_Projection (SCE_TMatrix4, float, float, float, float);

void SCE_Matrix4_LookAt (SCE_TMatrix4, SCE_TVector3,
                         SCE_TVector3, SCE_TVector3);

/* NOTE: should be named GetTranslationv... */
void SCE_Matrix4_GetTranslation (SCE_TMatrix4, SCE_TVector3);
void SCE_Matrix4x3_GetTranslation (SCE_TMatrix4, SCE_TVector3);
/*void SCE_Matrix4_GetRotation (SCE_TMatrix4, float*, SCE_TVector3);*/

void SCE_Matrix4_SetTranslation (SCE_TMatrix4, SCE_TVector3);
void SCE_Matrix4x3_SetTranslation (SCE_TMatrix4x3, SCE_TVector3);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
