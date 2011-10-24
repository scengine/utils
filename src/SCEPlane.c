/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2011  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
   updated: 17/10/2011 */

#include "SCE/utils/SCEVector.h"
#include "SCE/utils/SCELine.h"
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
void SCE_Plane_Setv (SCE_SPlane *p, const SCE_TVector3 n, float d)
{
    SCE_Vector3_Copy (p->n, n);
    p->d = d;
}

/**
 * \brief Makes a plane from a normal vector and a point included in the plane
 * \sa SCE_Plane_SetFromPointv()
 */
void SCE_Plane_SetFromPoint (SCE_SPlane *p, const SCE_TVector3 n,
                             float x, float y, float z)
{
    SCE_Vector3_Copy (p->n, n);
    p->d = - (n[0]*x + n[1]*y + n[2]*z);
}
/**
 * \brief Makes a plane from a normal vector and a point included in the plane
 * \sa SCE_Plane_SetFromPoint()
 */
void SCE_Plane_SetFromPointv (SCE_SPlane *p, const SCE_TVector3 n,
                              const SCE_TVector3 v)
{
    SCE_Vector3_Copy (p->n, n);
    p->d = - SCE_Vector3_Dot (n, v);
}

/**
 * \brief Makes a plane from a triangle
 * \param p the plane to make
 * \param a,b,c points of the triangle
 */
void SCE_Plane_SetFromTriangle (SCE_SPlane *p, const SCE_TVector3 a,
                                const SCE_TVector3 b, const SCE_TVector3 c)
{
    SCE_TVector3 n, ab, ac;
    SCE_Vector3_Operator2v (ab, =, b, -, a);
    SCE_Vector3_Operator2v (ac, =, c, -, a);
    SCE_Vector3_Cross (n, ab, ac);
    SCE_Plane_SetFromPointv (p, n, a);
}

/**
 * \brief Copies a plane
 * \param dst destination
 * \param src source
 */
void SCE_Plane_Copy (SCE_SPlane *dst, const SCE_SPlane *src)
{
    SCE_Vector3_Copy (dst->n, src->n);
    dst->d = src->d;
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
    else {
        float a = SCE_Vector3_Length (p->n);
        SCE_Vector3_Operator1 (p->n, /=, a);
        p->d /= a;
    }
}


/**
 * \brief Gets the shorter distance from a plane to a point
 * \param p a SCE_SPlane
 * \param x,y,z point's coordinate
 * \returns the shorter distance from \p p to the given point. If the plane
 * is facing the point, the returned value is positive, otherwise it is
 * negative.
 * \note The plane \p p has to be normalized
 * \sa SCE_Plane_DistanceToPointv()
 */
float SCE_Plane_DistanceToPoint (const SCE_SPlane *p, float x, float y, float z)
{
    return p->n[0]*x + p->n[1]*y + p->n[2]*z + p->d;
}
/**
 * \brief Vectorial version of SCE_Plane_DistanceToPoint()
 * \param p a plane
 * \param v a point
 * \returns the shorter distance from \p p to the given point \p v
 * 
 * \see SCE_Plane_DistanceToPoint()
 */
float SCE_Plane_DistanceToPointv (const SCE_SPlane *p, const SCE_TVector3 v)
{
    return SCE_Vector3_Dot (p->n, v) + p->d;
}

/**
 * \brief Checks for intersection with a line and return the intersection point
 * \param p a plane
 * \param l a line
 * \param v here will be written the intersection point
 * \returns SCE_TRUE if the intersection point exists
 */
int SCE_Plane_LineIntersection (const SCE_SPlane *p, const SCE_SLine3 *l,
                                SCE_TVector3 v)
{
    float a, k;
    float div = SCE_Vector3_Dot (p->n, l->n);
    /* TODO: Math_IsNull() */
    if (SCE_Math_Fabsf (div) < 0.000001f)
        return SCE_FALSE;
    a = -SCE_Vector3_Dot (p->n, l->o) - p->d;
    k = a / div;
    SCE_Vector3_Operator3 (v, =, l->o, +, l->n, *, k);
    return SCE_TRUE;
}

static int SCE_Plane_SameSign (float a, float b)
{
    return (a < 0.0f && b < 0.0f || a > 0.0f && b > 0.0f);
}

/**
 * \brief Checks for intersection between a triangle and a line
 * \param a,b,c a triangle
 * \param l a line
 * \param d will store the intersection point here, even if it is outside
 * the triangle
 * \returns SCE_TRUE if the line intersects the triangle, SCE_FALSE otherwise
 */
int SCE_Plane_TriangleLineIntersection (const SCE_TVector3 a,
                                        const SCE_TVector3 b,
                                        const SCE_TVector3 c,
                                        const SCE_SLine3 *l,
                                        SCE_TVector3 d)
{
    SCE_SPlane p;
    SCE_TVector3 n, ab, ac, bc, v;
    float dot1, dot2;

    /* much like SCE_Plane_SetFromTriangle(), but we need those vectors
       so we compute them only once */
    SCE_Vector3_Operator2v (ab, =, b, -, a);
    SCE_Vector3_Operator2v (ac, =, c, -, a);
    SCE_Vector3_Operator2v (bc, =, c, -, b);
    SCE_Vector3_Cross (n, ab, ac);
    SCE_Plane_SetFromPointv (&p, n, a);

    if (!SCE_Plane_LineIntersection (&p, l, d))
        return SCE_FALSE;

    /* split with ab */
    SCE_Vector3_Cross (n, p.n, ab);
    SCE_Vector3_Operator2v (v, =, d, -, a); /* AD */
    dot1 = SCE_Vector3_Dot (v, n);
    dot2 = SCE_Vector3_Dot (ac, n);
    if (!SCE_Plane_SameSign (dot1, dot2))
        return SCE_FALSE;

    /* split with ac */
    SCE_Vector3_Cross (n, p.n, ac);
    /* no need to recompute it */
    /* SCE_Vector3_Operator2v (v, =, d, -, a);*/ /* AD */
    dot1 = SCE_Vector3_Dot (v, n);
    dot2 = SCE_Vector3_Dot (ab, n);
    if (!SCE_Plane_SameSign (dot1, dot2))
        return SCE_FALSE;

    /* split with bc */
    SCE_Vector3_Cross (n, p.n, bc);
    SCE_Vector3_Operator2v (v, =, d, -, b); /* BD */
    dot1 = SCE_Vector3_Dot (v, n);
    dot2 = - SCE_Vector3_Dot (ab, n); /* no need to compute the BA vector */
    if (!SCE_Plane_SameSign (dot1, dot2))
        return SCE_FALSE;

    return SCE_TRUE;
}

/** @} */
