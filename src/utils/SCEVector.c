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
 
/* Cree le : 21 decembre 2006
   derniere modification le 22/09/2007 */

#include <SCE/utils/SCEMath.h>
#include <SCE/utils/SCEVector.h>


/**
 * \file SCEVector.c
 * \copydoc vector
 * 
 * \file SCEVector.h
 * \copydoc vector
 */

/**
 * \defgroup vector Vectors transformations
 * \ingroup utils
 * \brief Vector transformations functions
 */

/** @{ */


/* revise le 22/09/2007 */
/**
 * \brief Normalize a vector
 * \param v a SCE_TVector3 to normalize
 * 
 * Normalize a SCE_TVector3
 */
void SCE_Vector3_Normalize (SCE_TVector3 v)
{
    float r = SCE_Math_InvSqrt ((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2]));
    SCE_Vector3_Operator1 (v, *=, r);
}
/* revise le 22/09/2007 */
/**
 * \brief Normalize a vector
 * \param v a SCE_TVector2 to normalize
 * 
 * Normalize a SCE_TVector2
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


/** @} */
