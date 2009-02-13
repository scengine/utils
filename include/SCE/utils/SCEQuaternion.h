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
 
/* Cree le : 21/10/2007
   derniere modification : 21/10/2007 */

#ifndef SCEQUATERNION_H
#define SCEQUATERNION_H

#include <SCE/utils/SCEVector.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef float SCE_TQuaternion[4];

#define SCE_Quaternion_Set(q, x, y, z, w){\
    (q)[0] = x; (q)[1] = y; (q)[2] = z; (q)[3] = w;}
#define SCE_Quaternion_Identity(q) SCE_Quaterion_Set (q, 0., 0., 0., 1.)

#define SCE_Quaternion_Copy(r, q) memcpy (r, q, 4 * sizeof (*(r)))

/* normalize un quaternion */
void SCE_Quaterion_Normalize (SCE_TQuaternion);

/* calcul le conjuge d'un quaternion */
void SCE_Quaternion_Conjugate (SCE_TQuaternion);

/* multiplie deux quaternions */
void SCE_Quaterion_Mul (SCE_TQuaternion, SCE_TQuaternion, SCE_TQuaternion);

/* construit un quaternion a partir d'un axe de rotation et d'un angle */
void SCE_Quaternion_Rotate (SCE_TQuaternion, float, float, float, float);
void SCE_Quaternion_Rotatev (SCE_TQuaternion, float, SCE_TVector3);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
