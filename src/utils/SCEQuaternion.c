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

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEQuaternion.h>

/**
 * \file SCEQuaternion.c
 * \copydoc quaternion
 * 
 * \file SCEQuaternion.h
 * \copydoc quaternion
 */

/**
 * \defgroup quaternion Quaternion managment
 * \ingroup utils
 * \brief Quaternions managment functions
 */

/** @{ */

/* ajoute le 22/04/2008 */
/**
 * \brief Normalize a quaternion
 * \param q the quaterion to normalize
 */
void SCE_Quaternion_Normalize (SCE_TQuaternion q)
{
    float norm = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
    if (norm != 0.0)
    {
        q[0] /= norm; q[1] /= norm; q[2] /= norm; q[3] /= norm;
    }
}

/* ajoute le 23/04/2008 */
/**
 * \brief Conjugate a quaternion
 * \param q the quaterion to conjugate
 */
void SCE_Quaternion_Conjugate (SCE_TQuaternion q)
{
    SCE_Quaternion_Set (q, -q[0], -q[1], -q[2], q[3]);
}

/* ajoute le 23/04/2008 */
/**
 * \brief Multiply two quaternions
 * \param a a quaterninon to mutpily with \p b
 * \param b another quaternion to multiply with \p a
 * \param r a queternion where write the result of the multiplication.
 * 
 * This function multiplies the two quaternions \p a and \p b.
 */
void SCE_Quaternion_Mul (SCE_TQuaternion a, SCE_TQuaternion b, SCE_TQuaternion r)
{
    SCE_Quaternion_Set(r,
                       a[3]*b[0] + a[0]*b[3] + a[1]*b[2] - a[2]*b[1],
                       a[3]*b[1] + a[1]*b[3] + a[2]*b[0] - a[0]*b[2],
                       a[3]*b[2] + a[2]*b[3] + a[0]*b[1] - a[1]*b[0],
                       a[3]*b[3] - a[0]*b[0] - a[1]*b[1] - a[2]*b[2]);
}

/* ajoute le 23/04/2008 */
/**
 * \brief Sets the rotation of a quaternion
 * \param q the quaternion for which set rotation
 * \param a the rotation angle in radians
 * \param x X part of the rotation vector
 * \param y Y part of the rotation vector
 * \param z Z part of the rotation vector
 * 
 * \warning the rotation vector will NOT be normalized.
 */
void SCE_Quaternion_Rotate (SCE_TQuaternion q, float a,
                            float x, float y, float z)
{
    float cosinus = cos (a / 2.);
    float sinus = sin (a / 2.);
    q[0] = x * sinus;
    q[1] = y * sinus;
    q[2] = z * sinus;
    q[3] = cosinus;
}
/**
 * \brief Vectorial version of SCE_Quaternion_Rotate()
 * \param q the quaternion for which set rotation
 * \param a the rotation angle in radians
 * \param axis the rotation axis vector
 * 
 * \warning the rotation vector will NOT be normalized.
 * \see SCE_Quaternion_Rotate()
 */
void SCE_Quaternion_Rotatev (SCE_TQuaternion q, float a, SCE_TVector3 axis)
{
    SCE_Quaternion_Rotate (q, a, axis[0], axis[1], axis[2]);
}

/** @} */
