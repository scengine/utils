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
 
/* created: 28/02/2008
   updated: 07/01/2009 */

#include "SCE/utils/SCEVector.h"

#include "SCE/utils/SCEPlane.h"


/**
 * \file SCEPlane.c
 * \copydoc plane
 * 
 * \file SCEPlane.h
 * \copydoc plane
 */

/**
 * \defgroup plane Planes managment
 * \ingroup utils
 * \brief Planes managment functions
 */

/** @{ */

/**
 * \brief Initialize a SCE_SPlane
 * \param p a SCE_SPlane
 */
void SCE_Plane_Init (SCE_SPlane *p)
{
    SCE_Vector3_Set (p->n, 0., 0., 0. /* 1. ? */);
    p->d = 0.;
}

/**
 * \brief Defines a plane as \f$a.x + b.y + c.z + d = 0\f$.
 * \param p the plane to define
 */
void SCE_Plane_Set (SCE_SPlane *p, float x, float y, float z, float d)
{
    SCE_Vector3_Set (p->n, x, y, z);
    p->d = d;
}
/**
 * \brief Vectorial version of SCE_Plane_Set()
 * \param p the plane to define
 */
void SCE_Plane_Setv (SCE_SPlane *p, SCE_TVector3 n, float d)
{
    SCE_Vector3_Copy (p->n, n);
    p->d = d;
}

/**
 * \brief Makes a plane from a normal vector and a point included in the plane
 * \sa SCE_Plane_SetFromPointv()
 */
void SCE_Plane_SetFromPoint (SCE_SPlane *p, SCE_TVector3 n,
                             float x, float y, float z)
{
    SCE_Vector3_Copy (p->n, n);
    p->d = - (n[0]*x + n[1]*y + n[2]*z);
}
/**
 * \brief Makes a plane from a normal vector and a point included in the plane
 * \sa SCE_Plane_SetFromPoint()
 */
void SCE_Plane_SetFromPointv (SCE_SPlane *p, SCE_TVector3 n, SCE_TVector3 v)
{
    SCE_Vector3_Copy (p->n, n);
    p->d = - SCE_Vector3_Dot (n, v);
}

/**
 * \brief Normalize a plane
 * \param p the plane to normalize
 * \param normalize_distance 
 */
void SCE_Plane_Normalize (SCE_SPlane *p, int normalize_distance)
{
    if (!normalize_distance)
        SCE_Vector3_Normalize (p->n);
    else
    {
        float a = SCE_Vector3_Length (p->n);
        SCE_Vector3_Operator1 (p->n, /=, a);
        p->d /= a;
    }
}

/**
 * \brief Gets the shorter distance from a plane to a point
 * \param p a SCE_SPlane
 * \param x,y,z point's coordinate
 * \returns the shorter distance from \p p to the given point
 */
float SCE_Plane_DistanceToPoint (SCE_SPlane *p, float x, float y, float z)
{
    return p->n[0]*x + p->n[1]*y + p->n[2]*z + p->d;
}
/**
 * \brief Vectorial version of SCE_Plane_DistanceToPoint()
 * \param p a SCE_SPlane
 * \param v a SCE_TVector3 representing the point
 * \returns the shorter distance from \p p to the given point \p v
 * 
 * \see SCE_Plane_DistanceToPoint()
 */
float SCE_Plane_DistanceToPointv (SCE_SPlane *p, SCE_TVector3 v)
{
    return SCE_Vector3_Dot (p->n, v) + p->d;
}

/** @} */
