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
 
/* created: 21/12/2006
   updated: 07/04/2009 */

#include <string.h>

#include <SCE/utils/SCEMath.h>

#include <SCE/utils/SCEMatrix.h>


int SCE_Init_Matrix (void)
{
    return 0; /* fuck SCE_OK... */
}
void SCE_Quit_Matrix (void)
{
}


void SCE_Matrix4_Identity (SCE_TMatrix4 m)
{
    m[0] = m[5] = m[10] = m[15] = 1.0f;
    m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] =
    m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
}
void SCE_Matrix3_Identity (SCE_TMatrix3 m)
{
    m[0] = m[4] = m[8] = 1.0f;
    m[1] = m[2] = m[3] = m[5] = m[6] = m[7] = 0.0f;
}
void SCE_Matrix4x3_Identity (SCE_TMatrix4x3 m)
{
    m[0] = m[5] = m[10] = 1.0f;
    m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] =
    m[9] = m[11] = 0.0f;
}

void SCE_Matrix4_CopyM3 (SCE_TMatrix4 m, SCE_TMatrix3 n)
{
    m[0] = n[0]; m[1] = n[1]; m[2] = n[2];
    m[4] = n[3]; m[5] = n[4]; m[6] = n[5];
    m[8] = n[6]; m[9] = n[7]; m[10] = n[8];
}

void SCE_Matrix3_CopyM4 (SCE_TMatrix3 m, SCE_TMatrix4 n)
{
    m[0] = n[0]; m[1] = n[1]; m[2] = n[2];
    m[3] = n[4]; m[4] = n[5]; m[5] = n[6];
    m[6] = n[8]; m[7] = n[9]; m[8] = n[10];
}
void SCE_Matrix3_CopyM4x3 (SCE_TMatrix3 m, SCE_TMatrix4x3 n)
{
    m[0] = n[0]; m[1] = n[1]; m[2] = n[2];
    m[3] = n[4]; m[4] = n[5]; m[5] = n[6];
    m[6] = n[8]; m[7] = n[9]; m[8] = n[10];
}

void SCE_Matrix4x3_CopyM3 (SCE_TMatrix4 m, SCE_TMatrix3 n)
{
    m[0] = n[0]; m[1] = n[1]; m[2] = n[2];
    m[4] = n[3]; m[5] = n[4]; m[6] = n[5];
    m[8] = n[6]; m[9] = n[7]; m[10] = n[8];
}


float* SCE_Matrix4_Mul (SCE_TMatrix4 m, SCE_TMatrix4 n, SCE_TMatrix4 r)
{
    r[0]  = (m[0] *n[0]) + (m[1] *n[4]) + (m[2] *n[8]) + (m[3] *n[12]);
    r[1]  = (m[0] *n[1]) + (m[1] *n[5]) + (m[2] *n[9]) + (m[3] *n[13]);
    r[2]  = (m[0] *n[2]) + (m[1] *n[6]) + (m[2] *n[10]) + (m[3] *n[14]);
    r[3]  = (m[0] *n[3]) + (m[1] *n[7]) + (m[2] *n[11]) + (m[3] *n[15]);

    r[4]  = (m[4] *n[0]) + (m[5] *n[4]) + (m[6] *n[8]) + (m[7] *n[12]);
    r[5]  = (m[4] *n[1]) + (m[5] *n[5]) + (m[6] *n[9]) + (m[7] *n[13]);
    r[6]  = (m[4] *n[2]) + (m[5] *n[6]) + (m[6] *n[10]) + (m[7] *n[14]);
    r[7]  = (m[4] *n[3]) + (m[5] *n[7]) + (m[6] *n[11]) + (m[7] *n[15]);

    r[8]  = (m[8] *n[0]) + (m[9] *n[4]) + (m[10]*n[8]) + (m[11]*n[12]);
    r[9]  = (m[8] *n[1]) + (m[9] *n[5]) + (m[10]*n[9]) + (m[11]*n[13]);
    r[10] = (m[8] *n[2]) + (m[9] *n[6]) + (m[10]*n[10]) + (m[11]*n[14]);
    r[11] = (m[8] *n[3]) + (m[9] *n[7]) + (m[10]*n[11]) + (m[11]*n[15]);

    r[12] = (m[12]*n[0]) + (m[13]*n[4]) + (m[14]*n[8]) + (m[15]*n[12]);
    r[13] = (m[12]*n[1]) + (m[13]*n[5]) + (m[14]*n[9]) + (m[15]*n[13]);
    r[14] = (m[12]*n[2]) + (m[13]*n[6]) + (m[14]*n[10]) + (m[15]*n[14]);
    r[15] = (m[12]*n[3]) + (m[13]*n[7]) + (m[14]*n[11]) + (m[15]*n[15]);

    return r;
}
float* SCE_Matrix4_MulCopy (SCE_TMatrix4 m, SCE_TMatrix4 n)
{
    SCE_TMatrix4 tm;
    SCE_Matrix4_Mul (m, n, tm);
    SCE_Matrix4_Copy (m, tm);
    return m;
}
float* SCE_Matrix3_Mul (SCE_TMatrix3 m, SCE_TMatrix3 n, SCE_TMatrix3 r)
{
    r[0] = m[0]*n[0] + m[1]*n[3] + m[2]*n[6];
    r[1] = m[0]*n[1] + m[1]*n[4] + m[2]*n[7];
    r[2] = m[0]*n[2] + m[1]*n[5] + m[2]*n[8];

    r[3] = m[3]*n[0] + m[4]*n[3] + m[5]*n[6];
    r[4] = m[3]*n[1] + m[4]*n[4] + m[5]*n[7];
    r[5] = m[3]*n[2] + m[4]*n[5] + m[5]*n[8];

    r[6] = m[6]*n[0] + m[7]*n[3] + m[8]*n[6];
    r[7] = m[6]*n[1] + m[7]*n[4] + m[8]*n[7];
    r[8] = m[6]*n[2] + m[7]*n[5] + m[8]*n[8];

    return r;
}
float* SCE_Matrix3_MulCopy (SCE_TMatrix3 m, SCE_TMatrix3 n)
{
    SCE_TMatrix3 tm;
    SCE_Matrix3_Mul (m, n, tm);
    SCE_Matrix3_Copy (m, tm);
    return m;
}

float* SCE_Matrix4x3_Mul (SCE_TMatrix4x3 m, SCE_TMatrix4x3 n, SCE_TMatrix4x3 r)
{
    r[0]  = (m[0] *n[0]) + (m[1] *n[4]) + (m[2] *n[8]);
    r[1]  = (m[0] *n[1]) + (m[1] *n[5]) + (m[2] *n[9]);
    r[2]  = (m[0] *n[2]) + (m[1] *n[6]) + (m[2] *n[10]);
    r[3]  = (m[0] *n[3]) + (m[1] *n[7]) + (m[2] *n[11]) + m[3];

    r[4]  = (m[4] *n[0]) + (m[5] *n[4]) + (m[6] *n[8]);
    r[5]  = (m[4] *n[1]) + (m[5] *n[5]) + (m[6] *n[9]);
    r[6]  = (m[4] *n[2]) + (m[5] *n[6]) + (m[6] *n[10]);
    r[7]  = (m[4] *n[3]) + (m[5] *n[7]) + (m[6] *n[11]) + m[7];

    r[8]  = (m[8] *n[0]) + (m[9] *n[4]) + (m[10]*n[8]);
    r[9]  = (m[8] *n[1]) + (m[9] *n[5]) + (m[10]*n[9]);
    r[10] = (m[8] *n[2]) + (m[9] *n[6]) + (m[10]*n[10]);
    r[11] = (m[8] *n[3]) + (m[9] *n[7]) + (m[10]*n[11]) + m[11];
    return r;
}
float* SCE_Matrix4x3_MulCopy (SCE_TMatrix4x3 m, SCE_TMatrix4x3 n)
{
    SCE_TMatrix4x3 tm;
    SCE_Matrix4x3_Mul (m, n, tm);
    SCE_Matrix4x3_Copy (m, tm);
    return m;
}


void SCE_Matrix4_Add (SCE_TMatrix4 a, SCE_TMatrix4 b)
{
    a[0] += b[0]; a[1] += b[1]; a[2] += b[2]; a[3] += b[3];
    a[4] += b[4]; a[5] += b[5]; a[6] += b[6]; a[7] += b[7];
    a[8] += b[8]; a[9] += b[9]; a[10] += b[10]; a[11] += b[11];
    a[12] += b[12]; a[13] += b[13]; a[14] += b[14]; a[15] += b[15];
}

void SCE_Matrix4_Sub (SCE_TMatrix4 a, SCE_TMatrix4 b)
{
    a[0] -= b[0]; a[1] -= b[1]; a[2] -= b[2]; a[3] -= b[3];
    a[4] -= b[4]; a[5] -= b[5]; a[6] -= b[6]; a[7] -= b[7];
    a[8] -= b[8]; a[9] -= b[9]; a[10] -= b[10]; a[11] -= b[11];
    a[12] -= b[12]; a[13] -= b[13]; a[14] -= b[14]; a[15] -= b[15];
}


void SCE_Matrix4_Transpose (SCE_TMatrix4 m, SCE_TMatrix4 n)
{
    n[1] = m[4];
    n[2] = m[8];
    n[3] = m[12];
    n[6] = m[9];
    n[7] = m[13];
    n[11] = m[14];
}
void SCE_Matrix4_TransposeCopy (SCE_TMatrix4 m)
{
    float t;
    t = m[1]; m[1] = m[4]; m[4] = t;
    t = m[2]; m[2] = m[8]; m[8] = t;
    t = m[3]; m[3] = m[12]; m[12] = t;
    t = m[6]; m[6] = m[9]; m[9] = t;
    t = m[7]; m[7] = m[13]; m[13] = t;
    t = m[11]; m[11] = m[14]; m[14] = t;
}
void SCE_Matrix3_Transpose (SCE_TMatrix3 m, SCE_TMatrix3 n)
{
    n[1] = m[3];
    n[2] = m[6];
    n[5] = m[7];
}
void SCE_Matrix3_TransposeCopy (SCE_TMatrix3 m)
{
    float t;
    t = m[1]; m[1] = m[3]; m[3] = t;
    t = m[2]; m[2] = m[6]; m[6] = t;
    t = m[5]; m[5] = m[7]; m[7] = t;
}

void SCE_Matrix4_Inverse (SCE_TMatrix4 m, SCE_TMatrix4 inv)
{
    /* this code comes from the mesa implementation of GLU */
    float det;

    inv[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
        + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
    inv[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
        - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
    inv[8] =   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
        + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
    inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
        - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];

    det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
    if (det == 0.0)
        return;

    inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
        - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
    inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
        + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
    inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
        - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
    inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
        + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
    inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15]
        + m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
    inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15]
        - m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
    inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15]
        + m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
    inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14]
        - m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
    inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11]
        - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
    inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11]
        + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
    inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11]
        - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
    inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10]
        + m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

    det = 1.0 / det;

    inv[0] *= det; inv[1] *= det; inv[2] *= det; inv[3] *= det;
    inv[4] *= det; inv[5] *= det; inv[6] *= det; inv[7] *= det;
    inv[8] *= det; inv[9] *= det; inv[10] *= det; inv[11] *= det;
    inv[12] *= det; inv[13] *= det; inv[14] *= det; inv[15] *= det;
}
void SCE_Matrix4_InverseCopy (SCE_TMatrix4 m)
{
    SCE_TMatrix4 tm;
    SCE_Matrix4_Inverse (m, tm);
    SCE_Matrix4_Copy (m, tm);
}

void SCE_Matrix3_Inverse (SCE_TMatrix3 m, SCE_TMatrix3 inv)
{
    float det;

    inv[0] =  m[4]*m[8] - m[4]*0*0 - m[7]*m[5];
    inv[3] = -m[3]*m[8] + m[3]*0*0 + m[6]*m[5];
    inv[6] =  m[3]*m[7] - m[6]*m[4];

    det = m[0]*inv[0] + m[1]*inv[3] + m[2]*inv[6];
    if (det == 0.0)
        return;

    inv[1] = -m[1]*m[8] + m[7]*m[2];
    inv[4] =  m[0]*m[8] - m[6]*m[2];
    inv[7] = -m[0]*m[7] + m[6]*m[1];
    inv[2] =  m[1]*m[5] - m[4]*m[2];
    inv[5] = -m[0]*m[5] + m[3]*m[2];
    inv[8] =  m[0]*m[4] - m[3]*m[1];

    det = 1.0 / det;

    inv[0] *= det; inv[1] *= det; inv[2] *= det;
    inv[3] *= det; inv[4] *= det; inv[5] *= det;
    inv[6] *= det; inv[7] *= det; inv[8] *= det;
}
void SCE_Matrix3_InverseCopy (SCE_TMatrix3 m)
{
    SCE_TMatrix3 tm;
    SCE_Matrix3_Inverse (m, tm);
    SCE_Matrix3_Copy (m, tm);
}

void SCE_Matrix4x3_Inverse (SCE_TMatrix4x3 m, SCE_TMatrix4x3 inv)
{
    float det;

    inv[0] =   m[5]*m[10] - m[5]*m[11]*0 - m[9]*m[6]
        + m[9]*m[7]*0 + 0*m[6]*m[11] - 0*m[7]*m[10];
    inv[4] =  -m[4]*m[10] + m[4]*m[11]*0 + m[8]*m[6]
        - m[8]*m[7]*0 - 0*m[6]*m[11] + 0*m[7]*m[10];
    inv[8] =   m[4]*m[9] - m[4]*m[11]*0 - m[8]*m[5]
        + m[8]*m[7]*0 + 0*m[5]*m[11] - 0*m[7]*m[9];
    inv[1] = -m[4]*m[9]*0 + m[4]*m[10]*0 + m[8]*m[5]*0
        - m[8]*m[6]*0 - 0*m[5]*m[10] + 0*m[6]*m[9];

    det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[1];
    if (det == 0.0)
        return;

    inv[1] =  -m[1]*m[10] + m[9]*m[2];
    inv[5] =   m[0]*m[10] - m[8]*m[2];
    inv[9] =  -m[0]*m[9] + m[8]*m[1];
    inv[2] =   m[1]*m[6] - m[5]*m[2];
    inv[6] =  -m[0]*m[6] + m[4]*m[2];
    inv[10] =  m[0]*m[5] - m[4]*m[1];
    inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11]
        - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
    inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11]
        + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
    inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11]
        - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];

    det = 1.0 / det;

    inv[0] *= det; inv[1] *= det; inv[2] *= det; inv[3] *= det;
    inv[4] *= det; inv[5] *= det; inv[6] *= det; inv[7] *= det;
    inv[8] *= det; inv[9] *= det; inv[10] *= det; inv[11] *= det;
}
void SCE_Matrix4x3_InverseCopy (SCE_TMatrix4x3 m)
{
    SCE_TMatrix4x3 tm;
    SCE_Matrix4x3_Inverse (m, tm);
    SCE_Matrix4x3_Copy (m, tm);
}


void SCE_Matrix4_Interpolate (SCE_TMatrix4 m, SCE_TMatrix4 n, float w,
                              SCE_TMatrix4 r)
{
    float x = 1.0 - w;
    SCE_Matrix4_Set
        (r,
         m[0]*x+n[0]*w,   m[1]*x+n[1]*w,   m[2]*x+n[2]*w,   m[3]*x+n[3]*w,
         m[4]*x+n[4]*w,   m[5]*x+n[5]*w,   m[6]*x+n[6]*w,   m[7]*x+n[7]*w,
         m[8]*x+n[8]*w,   m[9]*x+n[9]*w,   m[10]*x+n[10]*w, m[11]*x+n[11]*w,
         m[12]*x+n[12]*w, m[13]*x+n[13]*w, m[14]*x+n[14]*w, m[15]*x+n[15]*x);
}
void SCE_Matrix3_Interpolate (SCE_TMatrix3 m, SCE_TMatrix3 n, float w,
                              SCE_TMatrix3 r)
{
    float x = 1.0 - w;
    SCE_Matrix3_Set(r,
                    m[0]*x+n[0]*w,m[1]*x+n[1]*w,m[2]*x+n[2]*w,
                    m[3]*x+n[3]*w,m[4]*x+n[4]*w,m[5]*x+n[5]*w,
                    m[6]*x+n[6]*w,m[7]*x+n[7]*w,m[8]*x+n[8]*x);
}
void SCE_Matrix4x3_Interpolate (SCE_TMatrix4x3 m, SCE_TMatrix4x3 n, float w,
                                SCE_TMatrix4x3 r)
{
    float x = 1.0 - w;
    SCE_Matrix4x3_Set
        (r,
         m[0]*x+n[0]*w, m[1]*x+n[1]*w, m[2]*x+n[2]*w,   m[3]*x+n[3]*w,
         m[4]*x+n[4]*w, m[5]*x+n[5]*w, m[6]*x+n[6]*w,   m[7]*x+n[7]*w,
         m[8]*x+n[8]*w, m[9]*x+n[9]*w, m[10]*x+n[10]*w, m[11]*x+n[11]*w);
}


void SCE_Matrix4_Translate (SCE_TMatrix4 m, float x, float y, float z)
{
    m[3] = x; m[7] = y; m[11] = z;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
    m[1] = m[2] = m[4]  = m[6] = m[14] =
    m[8] = m[9] = m[12] = m[13] = 0.0f;
}
void SCE_Matrix4_Translatev (SCE_TMatrix4 m, SCE_TVector3 v)
{
    m[3] = v[0]; m[7] = v[1]; m[11] = v[2];
    m[0] = m[5] = m[10] = m[15] = 1.0f;
    m[1] = m[2] = m[4]  = m[6] = m[14] =
    m[8] = m[9] = m[12] = m[13] = 0.0f;
}

void SCE_Matrix4_MulTranslate (SCE_TMatrix4 m, float x, float y, float z)
{
    SCE_TMatrix4 tm, tm2;
    SCE_Matrix4_Copy (tm2, m);
    SCE_Matrix4_Translate (tm, x, y, z);
    SCE_Matrix4_Mul (tm2, tm, m);
}

void SCE_Matrix4_MulTranslatev (SCE_TMatrix4 m, SCE_TVector3 v)
{
    SCE_TMatrix4 tm, tm2;
    SCE_Matrix4_Copy (tm2, m);
    SCE_Matrix4_Translatev (tm, v);
    SCE_Matrix4_Mul (tm2, tm, m);
}


void SCE_Matrix4_RotX (SCE_TMatrix4 m, float a)
{
    float cosinus = cos (a);
    float sinus = sin (a);

    SCE_Matrix4_Set (m,
                     1., 0.,      0.,       0.,
                     0., cosinus, -sinus,   0.,
                     0., sinus,   cosinus,  0.,
                     0., 0.,      0.,       1.);
}
void SCE_Matrix3_RotX (SCE_TMatrix3 m, float a)
{
    float cosinus = cos (a);
    float sinus = sin (a);

    SCE_Matrix3_Set (m,
                     1., 0.,      0.,
                     0., cosinus, -sinus,
                     0., sinus,   cosinus);
}
void SCE_Matrix4_RotY (SCE_TMatrix4 m, float a)
{
    float cosinus = cos (a);
    float sinus = sin (a);

    SCE_Matrix4_Set (m,
                     cosinus, 0., -sinus,  0.,
                     0.,      1., 0.,      0.,
                     sinus,   0., cosinus, 0.,
                     0.,      0., 0.,      1.);
}
void SCE_Matrix3_RotY (SCE_TMatrix3 m, float a)
{
    float cosinus = cos (a);
    float sinus = sin (a);

    SCE_Matrix3_Set (m,
                     cosinus, 0., -sinus,
                     0.,      1., 0.,
                     sinus,   0., cosinus);
}
void SCE_Matrix4_RotZ (SCE_TMatrix4 m, float a)
{
    float cosinus = cos (a);
    float sinus = sin (a);

    SCE_Matrix4_Set (m,
                     cosinus, -sinus,  0., 0.,
                     sinus,   cosinus, 0., 0.,
                     0.,      0.,      1., 0.,
                     0.,      0.,      0., 1.);
}
void SCE_Matrix3_RotZ (SCE_TMatrix3 m, float a)
{
    float cosinus = cos (a);
    float sinus = sin (a);

    SCE_Matrix3_Set (m,
                     cosinus, -sinus,  0.,
                     sinus,   cosinus, 0.,
                     0.,      0.,      1.);
}

void SCE_Matrix4_MulRotX (SCE_TMatrix4 m, float a)
{
    SCE_TMatrix4 tm, tm2;
    SCE_Matrix4_Copy (tm2, m);
    SCE_Matrix4_RotX (tm, a);
    SCE_Matrix4_Mul (tm2, tm, m);
}
void SCE_Matrix3_MulRotX (SCE_TMatrix3 m, float a)
{
    SCE_TMatrix4 tm, tm2;
    SCE_Matrix3_Copy (tm2, m);
    SCE_Matrix3_RotX (tm, a);
    SCE_Matrix3_Mul (tm2, tm, m);
}

void SCE_Matrix4_MulRotY (SCE_TMatrix4 m, float a)
{
    SCE_TMatrix4 tm, tm2;
    SCE_Matrix4_Copy (tm2, m);
    SCE_Matrix4_RotY (tm, a);
    SCE_Matrix4_Mul (tm2, tm, m);
}
void SCE_Matrix3_MulRotY (SCE_TMatrix3 m, float a)
{
    SCE_TMatrix4 tm, tm2;
    SCE_Matrix3_Copy (tm2, m);
    SCE_Matrix3_RotX (tm, a);
    SCE_Matrix3_Mul (tm2, tm, m);
}

void SCE_Matrix4_MulRotZ (SCE_TMatrix4 m, float a)
{
    SCE_TMatrix4 tm, tm2;
    SCE_Matrix4_Copy (tm2, m);
    SCE_Matrix4_RotZ (tm, a);
    SCE_Matrix4_Mul (tm2, tm, m);
}
void SCE_Matrix3_MulRotZ (SCE_TMatrix3 m, float a)
{
    SCE_TMatrix4 tm, tm2;
    SCE_Matrix3_Copy (tm2, m);
    SCE_Matrix3_RotX (tm, a);
    SCE_Matrix3_Mul (tm2, tm, m);
}


void SCE_Matrix4_Rotate (SCE_TMatrix4 m, float a, float x, float y, float z)
{
#define UN_COS 0
#define XSIN 1
#define YSIN 2
#define ZSIN 3
    float val[4];
    float xy, xz, yz;
    float cosinus = cos (a);
    float sinus = sin (a);

    val[UN_COS] = 1.0f - cosinus;

    val[XSIN] = x * sinus;
    val[YSIN] = y * sinus;
    val[ZSIN] = z * sinus;

    xy = x * y;
    xz = x * z;
    yz = y * z;

    m[0] = (x*x*val[UN_COS]) + cosinus;
    m[1] = (xy*val[UN_COS]) - val[ZSIN];
    m[2] = (xz*val[UN_COS]) + val[YSIN];
    m[4] = (xy*val[UN_COS]) + val[ZSIN];
    m[5] = (y*y*val[UN_COS]) + cosinus;
    m[6] = (yz*val[UN_COS]) - val[XSIN];
    m[8] = (xz*val[UN_COS]) - val[YSIN];
    m[9] = (yz*val[UN_COS]) + val[XSIN];
    m[10]= (z*z*val[UN_COS]) + cosinus;

    m[3] = m[7] = m[11] = m[12] = m[13] = m[14] = 0.0f;
    m[15] = 1.0f;
}
void SCE_Matrix4_Rotatev (SCE_TMatrix4 m, float a, SCE_TVector3 axe)
{
    SCE_Matrix4_Rotate (m, a, axe[0], axe[1], axe[2]);
}

void SCE_Matrix3_Rotate (SCE_TMatrix3 m, float a, float x, float y, float z)
{
    float val[4];
    float xy, xz, yz;
    float cosinus = cos (a);
    float sinus = sin (a);

    val[UN_COS] = 1.0f - cosinus;

    val[XSIN] = x * sinus;
    val[YSIN] = y * sinus;
    val[ZSIN] = z * sinus;

    xy = x * y;
    xz = x * z;
    yz = y * z;

    m[0] = (x*x*val[UN_COS]) + cosinus;
    m[1] = (xy*val[UN_COS]) - val[ZSIN];
    m[2] = (xz*val[UN_COS]) + val[YSIN];
    m[3] = (xy*val[UN_COS]) + val[ZSIN];
    m[4] = (y*y*val[UN_COS]) + cosinus;
    m[5] = (yz*val[UN_COS]) - val[XSIN];
    m[6] = (xz*val[UN_COS]) - val[YSIN];
    m[7] = (yz*val[UN_COS]) + val[XSIN];
    m[8] = (z*z*val[UN_COS]) + cosinus;
#undef UN_COS
#undef XSIN
#undef YSIN
#undef ZSIN
}
void SCE_Matrix3_Rotatev (SCE_TMatrix3 m, float a, SCE_TVector3 axe)
{
    SCE_Matrix3_Rotate (m, a, axe[0], axe[1], axe[2]);
}

void SCE_Matrix4_MulRotate (SCE_TMatrix4 m, float a, float x, float y, float z)
{
    SCE_TMatrix4 tm;
    SCE_Matrix4_Rotate (tm, a, x, y, z);
    SCE_Matrix4_MulCopy (m, tm);
}
void SCE_Matrix4_MulRotatev (SCE_TMatrix4 m, float a, SCE_TVector3 v)
{
    SCE_TMatrix4 tm;
    SCE_Matrix4_Rotatev (tm, a, v);
    SCE_Matrix4_MulCopy (m, tm);
}

void SCE_Matrix3_MulRotate (SCE_TMatrix3 m, float a, float x, float y, float z)
{
    SCE_TMatrix4 tm;
    SCE_Matrix3_Rotate (tm, a, x, y, z);
    SCE_Matrix3_MulCopy (m, tm);
}
void SCE_Matrix3_MulRotatev (SCE_TMatrix3 m, float a, SCE_TVector3 v)
{
    SCE_TMatrix4 tm;
    SCE_Matrix3_Rotatev (tm, a, v);
    SCE_Matrix3_MulCopy (m, tm);
}


void SCE_Matrix4_Scale (SCE_TMatrix4 m, float x, float y, float z)
{
    SCE_Matrix4_Set (m,
                     x,  0., 0., 0.,
                     0., y,  0., 0.,
                     0., 0., z,  0.,
                     0., 0., 0., 1.);
}
void SCE_Matrix4_Scalev (SCE_TMatrix4 m, SCE_TVector3 v)
{
    SCE_Matrix4_Set (m,
                     v[0], 0.,   0.,   0.,
                     0.,   v[1], 0.,   0.,
                     0.,   0.,   v[2], 0.,
                     0.,   0.,   0.,   1.);
}

void SCE_Matrix3_Scale (SCE_TMatrix3 m, float x, float y, float z)
{
    SCE_Matrix3_Set (m,
                     x,  0., 0.,
                     0., y,  0.,
                     0., 0., z);
}
void SCE_Matrix3_Scalev (SCE_TMatrix3 m, SCE_TVector3 v)
{
    SCE_Matrix3_Set (m,
                     v[0], 0.,   0.,
                     0.,   v[1], 0.,
                     0.,   0.,   v[2]);
}

void SCE_Matrix4_MulScale (SCE_TMatrix4 m, float x, float y, float z)
{
    SCE_TMatrix4 tm;
    SCE_Matrix4_Scale (tm, x, y, z);
    SCE_Matrix4_MulCopy (m, tm);
}
void SCE_Matrix4_MulScalev (SCE_TMatrix4 m, SCE_TVector3 v)
{
    SCE_TMatrix4 tm;
    SCE_Matrix4_Scalev (tm, v);
    SCE_Matrix4_MulCopy (m, tm);
}

void SCE_Matrix3_MulScale (SCE_TMatrix3 m, float x, float y, float z)
{
    SCE_TMatrix3 tm;
    SCE_Matrix3_Scale (tm, x, y, z);
    SCE_Matrix3_MulCopy (m, tm);
}
void SCE_Matrix3_MulScalev (SCE_TMatrix3 m, SCE_TVector3 v)
{
    SCE_TMatrix3 tm;
    SCE_Matrix3_Scalev (tm, v);
    SCE_Matrix3_MulCopy (m, tm);
}


void SCE_Matrix4_FromQuaternion (SCE_TMatrix4 m, SCE_TQuaternion q)
{
    float xx, xy, xz, xw, yy, yz, yw, zz, zw;
    xx = q[0] * q[0]; xy = q[0] * q[1]; xz = q[0] * q[2];
    xw = q[0] * q[3]; yy = q[1] * q[1]; yz = q[1] * q[2];
    yw = q[1] * q[3]; zz = q[2] * q[2]; zw = q[2] * q[3];
    SCE_Matrix4_Set (m,
                     1.-2.*(yy+zz), 2.*(xy-zw),    2.*(xz+yw),    0.,
                     2.*(xy+zw),    1.-2.*(xx+zz), 2.*(yz-xw),    0.,
                     2.*(xz-yw),    2.*(yz + xw),  1.-2.*(xx+yy), 0.,
                     0.,            0.,            0.,            1.);
}
void SCE_Matrix3_FromQuaternion (SCE_TMatrix3 m, SCE_TQuaternion q)
{
    float xx, xy, xz, xw, yy, yz, yw, zz, zw;
    xx = q[0] * q[0]; xy = q[0] * q[1]; xz = q[0] * q[2];
    xw = q[0] * q[3]; yy = q[1] * q[1]; yz = q[1] * q[2];
    yw = q[1] * q[3]; zz = q[2] * q[2]; zw = q[2] * q[3];
    SCE_Matrix3_Set (m,
                     1.-2.*(yy+zz), 2.*(xy-zw),    2.*(xz+yw),
                     2.*(xy+zw),    1.-2.*(xx+zz), 2.*(yz-xw),
                     2.*(xz-yw),    2.*(yz + xw),  1.-2.*(xx+yy));
}
void SCE_Matrix4x3_FromQuaternion (SCE_TMatrix4x3 m, SCE_TQuaternion q)
{
    float xx, xy, xz, xw, yy, yz, yw, zz, zw;
    xx = q[0] * q[0]; xy = q[0] * q[1]; xz = q[0] * q[2];
    xw = q[0] * q[3]; yy = q[1] * q[1]; yz = q[1] * q[2];
    yw = q[1] * q[3]; zz = q[2] * q[2]; zw = q[2] * q[3];
    SCE_Matrix4x3_Set (m,
                       1.-2.*(yy+zz), 2.*(xy-zw),    2.*(xz+yw),    0.,
                       2.*(xy+zw),    1.-2.*(xx+zz), 2.*(yz-xw),    0.,
                       2.*(xz-yw),    2.*(yz + xw),  1.-2.*(xx+yy), 0.);
}

void SCE_Matrix4_ToQuaternion (SCE_TMatrix4 m, SCE_TQuaternion q)
{
    float t = m[0] + m[5] + m[10] + 1.0f;

    if (t > 0.)
    {
        t = 0.5f / SCE_Math_Sqrt (t);
        q[0] = (m[6] - m[9]) * t;
        q[1] = (m[8] - m[2]) * t;
        q[2] = (m[1] - m[4]) * t;
        q[3] = 0.25f / t;
    }
    else
    {
        if ((m[0] > m[5]) && (m[0] > m[10]))
        {
            t = SCE_Math_InvSqrt (1.0f + m[0] - m[5] - m[10]) * 0.5f;
            q[0] = 0.5f * t;
            q[1] = (m[4] + m[1]) * t;
            q[2] = (m[8] + m[2]) * t;
            q[3] = (m[9] + m[6]) * t;
        }
        else if (m[5] > m[10])
        {
            t = SCE_Math_InvSqrt (1.0f - m[0] + m[5] - m[10]) * 0.5f;
            q[0] = (m[4] + m[1]) * t;
            q[1] = 0.5f * t;
            q[2] = (m[9] + m[6]) * t;
            q[3] = (m[8] + m[2]) * t;
        }
        else
        {
            t = SCE_Math_InvSqrt (1.0f - m[0] - m[5] + m[10]) * 0.5f;
            q[0] = (m[8] + m[2]) * t;
            q[1] = (m[9] + m[6]) * t;
            q[2] = 0.5f * t;
            q[3] = (m[4] + m[1]) * t;
        }
    }
}
void SCE_Matrix3_ToQuaternion (SCE_TMatrix3 m, SCE_TQuaternion q)
{
    float t = m[0] + m[4] + m[8] + 1.0f;

    if (t > 0.0f)
    {
        t = 0.5f / SCE_Math_Sqrt (t);
        q[0] = (m[5] - m[7]) * t;
        q[1] = (m[6] - m[2]) * t;
        q[2] = (m[1] - m[3]) * t;
        q[3] = 0.25f / t;
    }
    else
    {
        if ((m[0] > m[4]) && (m[0] > m[8]))
        {
            t = SCE_Math_InvSqrt (1.0f + m[0] - m[4] - m[8]) * 0.5f;
            q[0] = 0.5f * t;
            q[1] = (m[3] + m[1]) * t;
            q[2] = (m[6] + m[2]) * t;
            q[3] = (m[7] + m[5]) * t;
        }
        else if (m[4] > m[8])
        {
            t = SCE_Math_InvSqrt (1.0f - m[0] + m[4] - m[8]) * 0.5f;
            q[0] = (m[3] + m[1]) * t;
            q[1] = 0.5f * t;
            q[2] = (m[7] + m[5]) * t;
            q[3] = (m[6] + m[2]) * t;
        }
        else
        {
            t = SCE_Math_InvSqrt (1.0f - m[0] - m[4] + m[8]) * 0.5f;
            q[0] = (m[6] + m[2]) * t;
            q[1] = (m[7] + m[5]) * t;
            q[2] = 0.5f * t;
            q[3] = (m[3] + m[1]) * t;
        }
    }
}
void SCE_Matrix4x3_ToQuaternion (SCE_TMatrix4x3 m, SCE_TQuaternion q)
{
    float t = m[0] + m[5] + m[10] + 1.0f;

    if (t > 0.0f)
    {
        t = 0.5f * SCE_Math_Sqrt (t);
        q[0] = (m[6] - m[9]) * t;
        q[1] = (m[8] - m[2]) * t;
        q[2] = (m[1] - m[4]) * t;
        q[3] = 0.25f / t;
    }
    else
    {
        if ((m[0] > m[5]) && (m[0] > m[10]))
        {
            t = SCE_Math_InvSqrt (1.0f + m[0] - m[5] - m[10]) * 0.5f;
            q[0] = 0.5f * t;
            q[1] = (m[4] + m[1]) * t;
            q[2] = (m[8] + m[2]) * t;
            q[3] = (m[9] + m[6]) * t;
        }
        else if (m[5] > m[10])
        {
            t = SCE_Math_InvSqrt (1.0f - m[0] + m[5] - m[10]) * 0.5f;
            q[0] = (m[4] + m[1]) * t;
            q[1] = 0.5f * t;
            q[2] = (m[9] + m[6]) * t;
            q[3] = (m[8] + m[2]) * t;
        }
        else
        {
            t = SCE_Math_InvSqrt (1.0f - m[0] - m[5] + m[10]) * 0.5f;
            q[0] = (m[8] + m[2]) * t;
            q[1] = (m[9] + m[6]) * t;
            q[2] = 0.5f * t;
            q[3] = (m[4] + m[1]) / t;
        }
    }
}


void SCE_Matrix4_MulV3 (SCE_TMatrix4 m, SCE_TVector3 v, SCE_TVector3 v2)
{
    v2[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2]  + m[3];
    v2[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2]  + m[7];
    v2[2] = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11];
}
void SCE_Matrix4_MulV3Copy (SCE_TMatrix4 m, SCE_TVector3 v)
{
    float tv[2];
    tv[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2]  + m[3];
    tv[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2]  + m[7];
    v[2]  = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11];
    v[0]  = tv[0];
    v[1]  = tv[1];
}
void SCE_Matrix4_MulV4 (SCE_TMatrix4 m, SCE_TVector4 v, SCE_TVector4 v2)
{
    v2[0] = m[0]*v[0]  + m[1]*v[1]  + m[2]*v[2]  + m[3]*v[3];
    v2[1] = m[4]*v[0]  + m[5]*v[1]  + m[6]*v[2]  + m[7]*v[3];
    v2[2] = m[8]*v[0]  + m[9]*v[1]  + m[10]*v[2] + m[11]*v[3];
    v2[3] = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3];
}
void SCE_Matrix4_MulV4Copy (SCE_TMatrix4 m, SCE_TVector4 v)
{
    float tv[3];
    tv[0] = m[0]*v[0]  + m[1]*v[1]  + m[2]*v[2]  + m[3]*v[3];
    tv[1] = m[4]*v[0]  + m[5]*v[1]  + m[6]*v[2]  + m[7]*v[3];
    tv[2] = m[8]*v[0]  + m[9]*v[1]  + m[10]*v[2] + m[11]*v[3];
    v[3]  = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3];
    v[0]  = tv[0];
    v[1]  = tv[1];
    v[2]  = tv[2];
}
void SCE_Matrix3_MulV3 (SCE_TMatrix4 m, SCE_TVector3 v, SCE_TVector3 v2)
{
    v2[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2];
    v2[1] = m[3]*v[0] + m[4]*v[1] + m[5]*v[2];
    v2[2] = m[6]*v[0] + m[7]*v[1] + m[8]*v[2];
}
void SCE_Matrix3_MulV3Copy (SCE_TMatrix4 m, SCE_TVector3 v)
{
    float tv[2];
    tv[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2];
    tv[1] = m[3]*v[0] + m[4]*v[1] + m[5]*v[2];
    v[2]  = m[6]*v[0] + m[7]*v[1] + m[8]*v[2];
    v[0]  = tv[0];
    v[1]  = tv[1];
}
void SCE_Matrix4x3_MulV3 (SCE_TMatrix4x3 m, SCE_TVector3 v, SCE_TVector3 v2)
{
    v2[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2]  + m[3];
    v2[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2]  + m[7];
    v2[2] = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11];
}
void SCE_Matrix4x3_MulV3Copy (SCE_TMatrix4x3 m, SCE_TVector3 v)
{
    float tv[2];
    tv[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2] + m[3];
    tv[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2] + m[7];
    v[2]  = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11];
    v[0]  = tv[0];
    v[1]  = tv[1];
}
void SCE_Matrix4x3_MulV4 (SCE_TMatrix4x3 m, SCE_TVector4 v, SCE_TVector3 v2)
{
    v2[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2]  + m[3]*v[3];
    v2[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2]  + m[7]*v[3];
    v2[2] = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11]*v[3];
}
void SCE_Matrix4x3_MulV4Add (SCE_TMatrix4x3 m, SCE_TVector4 v, SCE_TVector3 v2)
{
    v2[0] += m[0]*v[0] + m[1]*v[1] + m[2]*v[2]  + m[3]*v[3];
    v2[1] += m[4]*v[0] + m[5]*v[1] + m[6]*v[2]  + m[7]*v[3];
    v2[2] += m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11]*v[3];
}
void SCE_Matrix4x3_MulV4Copy (SCE_TMatrix4x3 m, SCE_TVector4 v)
{
    float tv[2];
    tv[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2]  + m[3]*v[3];
    tv[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2]  + m[7]*v[3];
    v[2]  = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11]*v[3];
    v[0]  = tv[0];
    v[1]  = tv[1];
}

void SCE_Matrix4_Projection (SCE_TMatrix4 m, float a, float r, float n, float f)
{
    m[5] = 1.0f / tan (a * 0.5f);
    m[0] = m[5] / r;
    m[10] = -f / (f - n * 2.0f);
    m[11] = -2.0f * n * (f / (f - n));
    m[14] = -1.0f;

    m[1] = m[2] = m[3] = m[4] = m[6] = m[7] =
        m[8] = m[9] = m[12] = m[13] = m[15] = 0.0f;
}


void SCE_Matrix4_LookAt (SCE_TMatrix4 m, SCE_TVector3 pos,
                         SCE_TVector3 view, SCE_TVector3 up1)
{
    /* this code comes from the mesa implementation of GLU */
    SCE_TVector3 forward, side, up;

    SCE_Vector3_Operator2v (forward, =, view, -, pos);
    SCE_Vector3_Normalize (forward);

    /* Side = forward x up */
    SCE_Vector3_Cross (side, forward, up1);
    SCE_Vector3_Normalize (side);

    /* Recompute up as: up = side x forward */
    SCE_Vector3_Cross (up, side, forward);

    SCE_Matrix4_Set (m,
                     side[0], up[0], -forward[0], -pos[0],
                     side[1], up[1], -forward[1], -pos[1],
                     side[2], up[2], -forward[2], -pos[2],
                     0.0,     0.0,   0.0,         1.0);
}

void SCE_Matrix4_GetTranslation (SCE_TMatrix4 m, SCE_TVector3 v)
{
    v[0] = m[3]; v[1] = m[7]; v[2] = m[11];
}
