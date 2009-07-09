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
 
/* created: 21/10/2007
   updated: 05/04/2009 */

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

/**
 * \brief Normalizes a quaternion
 * \param q the quaternion to normalize
 */
void SCE_Quaternion_Normalize (SCE_TQuaternion q)
{
    float norm = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
    if (norm != 0.0)
    {
        q[0] /= norm; q[1] /= norm; q[2] /= norm; q[3] /= norm;
    }
}

/**
 * \brief Computes the 4th component of a normalized quaternion
 */
void SCE_Quaternion_ComputeW (SCE_TQuaternion q)
{
    float t = 1.0f - q[0]*q[0] - q[1]*q[1] - q[2]*q[2];
    if (t < 0.0f)
        q[3] = 0.0f;
    else
        q[3] = -SCE_Math_Sqrt (t);
}

/**
 * \brief Conjugate a quaternion
 * \param q the quaternion to conjugate
 * \param r the result
 */
void SCE_Quaternion_Conjugate (const SCE_TQuaternion q, SCE_TQuaternion r)
{
    SCE_Quaternion_Set (r, -q[0], -q[1], -q[2], q[3]);
}
/**
 * \brief Conjugate a quaternion
 * \param q the quaternion to conjugate
 */
void SCE_Quaternion_ConjugateCopy (SCE_TQuaternion q)
{
    SCE_Quaternion_Set (q, -q[0], -q[1], -q[2], q[3]);
}

/**
 * \brief Dot product between two quaternions
 */
float SCE_Quaternion_Dot (SCE_TQuaternion a, SCE_TQuaternion b)
{
    return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3]);
}


/**
 * \brief Multiplies two quaternions
 * \param a,b the quaternions to multiply
 * \param r where write the result of the multiplication.
 */
void SCE_Quaternion_Mul (SCE_TQuaternion a, SCE_TQuaternion b, SCE_TQuaternion r)
{
    SCE_Quaternion_Set (r,
                        a[3]*b[0] + a[0]*b[3] + a[1]*b[2] - a[2]*b[1],
                        a[3]*b[1] + a[1]*b[3] + a[2]*b[0] - a[0]*b[2],
                        a[3]*b[2] + a[2]*b[3] + a[0]*b[1] - a[1]*b[0],
                        a[3]*b[3] - a[0]*b[0] - a[1]*b[1] - a[2]*b[2]);
}
/**
 * \brief Multiplies two quaternions
 * \param a,b the quaternions to multiply, the result is stored in \p a
 * \todo *CopyInv() functions, writes into b instead of a
 */
void SCE_Quaternion_MulCopy (SCE_TQuaternion a, SCE_TQuaternion b)
{
    float t[3];
    t[0] = a[3]*b[0] + a[0]*b[3] + a[1]*b[2] - a[2]*b[1];
    t[1] = a[3]*b[1] + a[1]*b[3] + a[2]*b[0] - a[0]*b[2];
    t[2] = a[3]*b[2] + a[2]*b[3] + a[0]*b[1] - a[1]*b[0];
    a[3] = a[3]*b[3] - a[0]*b[0] - a[1]*b[1] - a[2]*b[2];
    a[2] = t[2];
    a[1] = t[1];
    a[0] = t[0];
}
/**
 * \brief Multiplies two quaternions
 * \param a,b the quaternions to multiply, the result is stored in \p b
 */
void SCE_Quaternion_MulCopyInv (SCE_TQuaternion a, SCE_TQuaternion b)
{
    float t[3];
    t[0] = a[3]*b[0] + a[0]*b[3] + a[1]*b[2] - a[2]*b[1];
    t[1] = a[3]*b[1] + a[1]*b[3] + a[2]*b[0] - a[0]*b[2];
    t[2] = a[3]*b[2] + a[2]*b[3] + a[0]*b[1] - a[1]*b[0];
    b[3] = a[3]*b[3] - a[0]*b[0] - a[1]*b[1] - a[2]*b[2];
    b[2] = t[2];
    b[1] = t[1];
    b[0] = t[0];
}

/**
 * \brief wtf?
 */
void SCE_Quaternion_MulV3 (SCE_TQuaternion q, SCE_TVector3 v, SCE_TQuaternion r)
{
    r[3] = - (q[0]*v[0]) - (q[1]*v[1]) - (q[2]*v[2]);
    r[0] =   (q[3]*v[0]) + (q[1]*v[2]) - (q[2]*v[1]);
    r[1] =   (q[3]*v[1]) + (q[2]*v[0]) - (q[0]*v[2]);
    r[2] =   (q[3]*v[2]) + (q[0]*v[1]) - (q[1]*v[0]);
}

/**
 * \brief Applies a rotation to a 3D vector
 */
void SCE_Quaternion_RotateV3 (SCE_TQuaternion q, SCE_TVector3 in,
                              SCE_TVector3 out)
{
    SCE_TQuaternion tmp, inv, final;

    SCE_Quaternion_Conjugate (q, inv);
    SCE_Quaternion_Normalize (inv);

    SCE_Quaternion_MulV3 (q, in, tmp);
    SCE_Quaternion_Mul (tmp, inv, final);

    out[0] = final[0];
    out[1] = final[1];
    out[2] = final[2];
}

/**
 * \brief Sets the rotation of a quaternion
 * \param q the quaternion for which set rotation
 * \param a the rotation angle in radians
 * \param x X part of the rotation vector
 * \param y Y part of the rotation vector
 * \param z Z part of the rotation vector
 * 
 * \warning the rotation vector will NOT be normalized.
 * \see SCE_Quaternion_Rotatev()
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

/**
 * \brief Linear interpolation of quaternions
 * \param a,b interpolate between these quaternions
 * \param w interpolation factor, must be 0 <= \p w <= 1
 * \param r where store the resulting quaternion
 * \sa SCE_Quaternion_SLERP()
 * \todo noob-maths's function, "pomped" from SLERP()
 */
void SCE_Quaternion_Linear (SCE_TQuaternion a, SCE_TQuaternion b, float w,
                            SCE_TQuaternion r)
{
    float w_;
    float q1w = b[3];
    float q1x = b[0];
    float q1y = b[1];
    float q1z = b[2];
    /* compute "cosine of angle between quaternions" using dot product */
    float cosOmega = SCE_Quaternion_Dot (a, b);

    if (cosOmega < 0.0f)
    {
        q1w = -q1w;
        q1x = -q1x;
        q1y = -q1y;
        q1z = -q1z;
        cosOmega = -cosOmega;
    }

    w_ = 1.0f - w;

    /* interpolate and return new quaternion */
    r[3] = (w_ * a[3]) + (w * q1w);
    r[0] = (w_ * a[0]) + (w * q1x);
    r[1] = (w_ * a[1]) + (w * q1y);
    r[2] = (w_ * a[2]) + (w * q1z);
}

/**
 * \brief Spherical interpolation of quaternions
 * \param a,b interpolate between these quaternions
 * \param w interpolation factor, must be 0 <= \p w <= 1
 * \param r where store the resulting quaternion
 * \sa SCE_Quaternion_Linear()
 */
void SCE_Quaternion_SLERP (SCE_TQuaternion a, SCE_TQuaternion b, float w,
                           SCE_TQuaternion r)
{
    float k0, k1;
    /* if negative dot, use -q1.  two quaternions q and -q
       represent the same rotation, but may produce
       different slerp.  we chose q or -q to rotate using
       the acute angle. */
    float q1w = b[3];
    float q1x = b[0];
    float q1y = b[1];
    float q1z = b[2];
    /* compute "cosine of angle between quaternions" using dot product */
    float cosOmega = SCE_Quaternion_Dot (a, b);

    if (cosOmega < 0.0f)
    {
        q1w = -q1w;
        q1x = -q1x;
        q1y = -q1y;
        q1z = -q1z;
        cosOmega = -cosOmega;
    }

#if 0
    /* we should have two unit quaternions, so dot should be <= 1.0 */
    assert (cosOmega < 1.1f);
#endif

    /* compute interpolation fraction, checking for quaternions
       almost exactly the same */

    if (cosOmega > 0.9999f)
    {
        /* very close - just use linear interpolation,
           which will protect againt a divide by zero */

        k0 = 1.0f - w;
        k1 = w;
    }
    else
    {
        /* compute the sin of the angle using the
           trig identity sin^2(omega) + cos^2(omega) = 1 */
        float sinOmega = sqrt (1.0f - (cosOmega * cosOmega));

        /* compute the angle from its sin and cosine */
        float omega = atan2 (sinOmega, cosOmega);

        /* compute inverse of denominator, so we only have
           to divide once */
        float oneOverSinOmega = 1.0f / sinOmega;

        /* Compute interpolation parameters */
        k0 = sin ((1.0f - w) * omega) * oneOverSinOmega;
        k1 = sin (w * omega) * oneOverSinOmega;
    }

    /* interpolate and return new quaternion */
    r[3] = k0 * a[3] + k1 * q1w;
    r[0] = k0 * a[0] + k1 * q1x;
    r[1] = k0 * a[1] + k1 * q1y;
    r[2] = k0 * a[2] + k1 * q1z;
}

/** @} */
