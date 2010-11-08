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
 
/* created: 21/10/2007
   updated: 08/11/2010 */

#ifndef SCEQUATERNION_H
#define SCEQUATERNION_H

#include "SCE/utils/SCEVector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef float SCE_TQuaternion[4];

#define SCE_Quaternion_Set(q, x, y, z, w)do {\
        (q)[0] = x; (q)[1] = y; (q)[2] = z; (q)[3] = w;} while (0);
#define SCE_Quaternion_Identity(q) SCE_Quaternion_Set (q, 0., 0., 0., 1.)

#define SCE_Quaternion_Copy(r, q) memcpy (r, q, sizeof (SCE_TQuaternion))

void SCE_Quaternion_Normalize (SCE_TQuaternion);

void SCE_Quaternion_ComputeW (SCE_TQuaternion);
void SCE_Quaternion_Conjugate (const SCE_TQuaternion, SCE_TQuaternion);
void SCE_Quaternion_ConjugateCopy (SCE_TQuaternion);

float SCE_Quaternion_Dot (SCE_TQuaternion, SCE_TQuaternion);

void SCE_Quaternion_Mul (SCE_TQuaternion, SCE_TQuaternion, SCE_TQuaternion);
void SCE_Quaternion_MulCopy (SCE_TQuaternion, SCE_TQuaternion);
void SCE_Quaternion_MulCopyInv (SCE_TQuaternion, SCE_TQuaternion);

void SCE_Quaternion_MulV3 (SCE_TQuaternion, SCE_TVector3, SCE_TQuaternion);
void SCE_Quaternion_RotateV3 (SCE_TQuaternion, SCE_TVector3, SCE_TVector3);

void SCE_Quaternion_Rotate (SCE_TQuaternion, float, float, float, float);
void SCE_Quaternion_Rotatev (SCE_TQuaternion, float, SCE_TVector3);

void SCE_Quaternion_Linear (SCE_TQuaternion, SCE_TQuaternion, float,
                            SCE_TQuaternion);
void SCE_Quaternion_SLERP (SCE_TQuaternion, SCE_TQuaternion, float,
                           SCE_TQuaternion);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
