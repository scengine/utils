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
   updated: 24/08/2009 */

#include <SCE/utils/SCEMath.h>
#include <SCE/utils/SCEVector.h>


/**
 * \file SCEVector.c
 * \copydoc vector
 * \file SCEVector.h
 * \copydoc vector
 */

/**
 * \defgroup vector Vectors
 * \ingroup utils
 * \brief Vector transformations functions
 */

/** @{ */

/**
 * \brief Normalizes a vector
 */
void SCE_Vector3_Normalize (SCE_TVector3 v)
{
    float r = SCE_Math_InvSqrt ((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2]));
    SCE_Vector3_Operator1 (v, *=, r);
}
/**
 * \brief Normalizes a vector
 */
void SCE_Vector2_Normalize (SCE_TVector2 v)
{
    float r = SCE_Math_InvSqrt ((v[0]*v[0])+(v[1]*v[1]));
    SCE_Vector2_Operator1 (v, *=, r);
}

/**
 * \brief Rotate a 3D vector through the X axis
 * \param v a SCE_TVector3 to rotate
 * \param cos_ the cosinus of the angle
 * \param sin_ the sinus of the angle
 * 
 * Rotate a 3D vector through the X axis
 */
void SCE_Vector3_RotateX (SCE_TVector3 v, float cos_, float sin_)
{
    SCE_TVector3 vec;

    vec[1] = v[1];
    vec[2] = v[2];

    v[1] = (vec[1] * cos_) - (vec[2] * sin_);
    v[2] = (vec[1] * sin_) + (vec[2] * cos_);
}
/**
 * \brief Rotate a 3D vector through the Y axis
 * \param v a SCE_TVector3 to rotate
 * \param cos_ the cosinus of the angle
 * \param sin_ the sinus of the angle
 * 
 * Rotate a 3D vector through the Y axis
 */
void SCE_Vector3_RotateY (SCE_TVector3 v, float cos_, float sin_)
{
    SCE_TVector3 vec;

    vec[0] = v[0];
    vec[2] = v[2];

    v[0] = (vec[0] * cos_) + (vec[2] * sin_);
    v[2] = (vec[2] * cos_) - (vec[0] * sin_);
}
/**
 * \brief Rotate a 3D vector through the Z axis
 * \param v a SCE_TVector3 to rotate
 * \param cos_ the cosinus of the angle
 * \param sin_ the sinus of the angle
 * 
 * Rotate a 3D vector through the Z axis
 */
void SCE_Vector3_RotateZ (SCE_TVector3 v, float cos_, float sin_)
{
    SCE_TVector3 vec;

    vec[0] = v[0];
    vec[1] = v[1];

    v[0] = (vec[0] * cos_) - (vec[1] * sin_);
    v[1] = (vec[0] * sin_) + (vec[1] * cos_);
}

/**
 * \brief Constructs a vector with the minimal values of each other's ones
 * \param newv the new vector (can be \p v1 or \p v2)
 */
void SCE_Vector3_GetMin (SCE_TVector3 newv, const SCE_TVector3 v1,
                         const SCE_TVector3 v2)
{
    newv[0] = MIN (v1[0], v2[0]);
    newv[1] = MIN (v1[1], v2[1]);
    newv[2] = MIN (v1[2], v2[2]);
}
/**
 * \brief Constructs a vector with the maximum values of each other's ones
 * \param newv the new vector (can be \p v1 or \p v2)
 */
void SCE_Vector3_GetMax (SCE_TVector3 newv, const SCE_TVector3 v1,
                         const SCE_TVector3 v2)
{
    newv[0] = MAX (v1[0], v2[0]);
    newv[1] = MAX (v1[1], v2[1]);
    newv[2] = MAX (v1[2], v2[2]);
}

/**
 * \brief Linear interpolation between two vectors
 */
void SCE_Vector3_Interpolate (SCE_TVector3 a, SCE_TVector3 b, float w,
                              SCE_TVector3 r)
{
    float w1 = 1.0f - w;
    SCE_Vector3_Operator2v (r, = w1 *, a, + w *, b);
}

/** @} */
