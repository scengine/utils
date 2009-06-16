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
 
/* created: 08/01/2009
   updated: 16/06/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCECollide.h>

/**
 * \file SCECollide.c
 * \copydoc collide
 * 
 * \file SCECollide.h
 * \copydoc collide
 */

/**
 * \defgroup collide Collider
 * \ingroup interface
 * \brief Collider module that manages collisions between some simple shapesg
 */

/** @{ */

int SCE_Collide_PlanesWithPoint (SCE_SPlane *planes, unsigned int n,
                                 float x, float y, float z)
{
    unsigned int i;
    for (i = 0; i < n; i++)
    {
        if (SCE_Plane_DistanceToPoint (&planes[i], x, y, z) <= 0.0f)
            return SCE_FALSE;
    }
    return SCE_TRUE;
}
int SCE_Collide_PlanesWithPointv (SCE_SPlane *planes, unsigned int n,
                                  SCE_TVector3 p)
{
    unsigned int i;
    for (i = 0; i < n; i++)
    {
        if (SCE_Plane_DistanceToPointv (&planes[i], p) <= 0.0f)
            return SCE_FALSE;
    }
    return SCE_TRUE;
}

int SCE_Collide_PlanesWithBB (SCE_SPlane *planes, unsigned int n,
                              SCE_SBoundingBox *box)
{
    unsigned int i, j, k, total = 0;
    float *points = SCE_BoundingBox_GetPoints (box);
    for (i = 0; i < n; i++)
    {
        k = 0;
        for (j = 0; j < 8; j++)
        {
            if (SCE_Plane_DistanceToPointv (&planes[i], &points[j*3]) > 0.0f)
                k++;
        }
        if (k == 8)
            total++;
        else if (k == 0)
            return SCE_COLLIDE_OUT;
    }
    return (total == n ? SCE_COLLIDE_IN : SCE_COLLIDE_PARTIALLY);
}
int SCE_Collide_PlanesWithBBBool (SCE_SPlane *planes, unsigned int n,
                                  SCE_SBoundingBox *box)
{
    unsigned int i, j;
    float *points = SCE_BoundingBox_GetPoints (box);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (SCE_Plane_DistanceToPointv (&planes[i], &points[j*3]) > 0.0f)
                break;
        }
        if (j == 8)
            return SCE_FALSE;
    }
    return SCE_TRUE;
}
int SCE_Collide_PlanesWithBS (SCE_SPlane *planes, unsigned int n,
                              SCE_SBoundingSphere *sphere)
{
    float d;
    unsigned int i, passed = 0;
    float *c, r;

    c = SCE_BoundingSphere_GetCenter (sphere);
    r = SCE_BoundingSphere_GetRadius (sphere);

    /* works only with convex meshs */
    for (i = 0; i < n; i++)
    {
        d = SCE_Plane_DistanceToPointv (&planes[i], c);
        if (d >= r)
            passed++;
        else if (d < -r)
            return SCE_COLLIDE_OUT;
        else
            passed--;     /* the sphere can't be totally in (maybe partially) */
    }
    return (passed == n ? SCE_COLLIDE_IN : SCE_COLLIDE_PARTIALLY);
}
int SCE_Collide_PlanesWithBSBool (SCE_SPlane *planes, unsigned int n,
                                  SCE_SBoundingSphere *sphere)
{
    unsigned int i;
    float *c, r;

    c = SCE_BoundingSphere_GetCenter (sphere);
    r = SCE_BoundingSphere_GetRadius (sphere);

    /* works only with convex meshs */
    for (i = 0; i < n; i++)
    {
        if (SCE_Plane_DistanceToPointv (&planes[i], c) < -r)
            return SCE_FALSE;
    }
    return SCE_TRUE;
}

#if 0
int SCE_Collide_RectWithBS (SCE_SFloatRect *rect, SCE_SBoundingSphere *sphere)
{
    SCE_SFloatRect rect2;
    float r = SCE_BoundingSphere_GetRadius (sphere);
    rect2 = *rect;
    /* intrusion! */
    rect2.p1[0] -= r;
    rect2.p1[1] -= r;
    rect2.p2[0] += r;
    rect2.p2[1] += r;
    if (SCE_Rectangle_IsInfv (&rect2, SCE_BoundingSphere_GetCenter (sphere)))
    {
        if (SCE_Rectangle_IsInfv (&rect, SCE_BoundingSphere_GetCenter (sphere)))
            return SCE_COLLIDE_IN;
        else
            return SCE_COLLIDE_PARTIALLY;
    }
    return SCE_COLLIDE_OUT;
}
#endif

int SCE_Collide_AABBWithPoint (SCE_SBoundingBox *box, float x, float y, float z)
{
    float *p = SCE_BoundingBox_GetPoints (box);
    if (x >= p[0] && x <= p[3] &&
        y >= p[1] && y <= p[10] &&
        z >= p[2] && z <= p[23])
        return SCE_COLLIDE_IN;
    else
        return SCE_COLLIDE_OUT;
}
int SCE_Collide_AABBWithPointv (SCE_SBoundingBox *box, SCE_TVector3 p)
{
    return SCE_Collide_AABBWithPoint (box, p[0], p[1], p[2]);
}
int SCE_Collide_AABBWithBS (SCE_SBoundingBox *b, SCE_SBoundingSphere *s)
{
    float r = 0.0;
    float *c = NULL;
    float *p = NULL;
    r = SCE_BoundingSphere_GetRadius (s);
    c = SCE_BoundingSphere_GetCenter (s);
    p = SCE_BoundingBox_GetPoints (b);

    if (c[0] >= p[0]-r && c[0] <= p[3]+r &&
        c[1] >= p[1]-r && c[1] <= p[10]+r &&
        c[2] >= p[2]-r && c[2] <= p[23]+r)
    {
        if (c[0] >= p[0]+r && c[0] <= p[3]-r &&
            c[1] >= p[1]+r && c[1] <= p[10]-r &&
            c[2] >= p[2]+r && c[2] <= p[23]-r)
            return SCE_COLLIDE_IN;
        else
            return SCE_COLLIDE_PARTIALLY;
    }
    else
        return SCE_COLLIDE_OUT;
}
int SCE_Collide_AABBWithBSBool (SCE_SBoundingBox *b, SCE_SBoundingSphere *s)
{
    float r = 0.0;
    float *c = NULL;
    float *p = NULL;
    r = SCE_BoundingSphere_GetRadius (s);
    c = SCE_BoundingSphere_GetCenter (s);
    p = SCE_BoundingBox_GetPoints (b);

    if (c[0] >= p[0]-r && c[0] <= p[3]+r &&
        c[1] >= p[1]-r && c[1] <= p[10]+r &&
        c[2] >= p[2]-r && c[2] <= p[23]+r)
        return SCE_TRUE;
    else
        return SCE_FALSE;
}

int SCE_Collide_BBWithPoint (SCE_SBoundingBox *box, float x, float y, float z)
{
    unsigned int i;
    SCE_SPlane *planes = SCE_BoundingBox_GetPlanes (box);
    SCE_BoundingBox_MakePlanes (box);
    for (i = 0; i < 6; i++)
    {
        if (SCE_Plane_DistanceToPoint (&planes[i], x, y, z) <= 0.0f)
            return SCE_FALSE;
    }
    return SCE_TRUE;
}
int SCE_Collide_BBWithPointv (SCE_SBoundingBox *box, SCE_TVector3 p)
{
    unsigned int i;
    SCE_SPlane *planes = SCE_BoundingBox_GetPlanes (box);
    SCE_BoundingBox_MakePlanes (box);
    for (i = 0; i < 6; i++)
    {
        if (SCE_Plane_DistanceToPointv (&planes[i], p) <= 0.0f)
            return SCE_FALSE;
    }
    return SCE_TRUE;
}
int SCE_Collide_BBWithBS (SCE_SBoundingBox *box, SCE_SBoundingSphere *sphere)
{
    SCE_BoundingBox_MakePlanes (box);
    return SCE_Collide_PlanesWithBS (SCE_BoundingBox_GetPlanes(box), 6, sphere);
}
int SCE_Collide_BBWithBB (SCE_SBoundingBox *box, SCE_SBoundingBox *box2)
{
    SCE_BoundingBox_MakePlanes (box);
    return SCE_Collide_PlanesWithBB (SCE_BoundingBox_GetPlanes(box), 6, box2);
}

int SCE_Collide_BSWithPoint (SCE_SBoundingSphere *sphere,
                             float x, float y, float z)
{
    SCE_TVector3 p;
    float *c = SCE_BoundingSphere_GetCenter (sphere);
    float r = SCE_BoundingSphere_GetRadius (sphere);
    SCE_Vector3_Set (p, x, y, z);
    return (fabs (SCE_Vector3_Dot (c, p)) < r*r);
}
int SCE_Collide_BSWithPointv (SCE_SBoundingSphere *sphere, SCE_TVector3 p)
{
    float *c = SCE_BoundingSphere_GetCenter (sphere);
    float r = SCE_BoundingSphere_GetRadius (sphere);
    return (fabs (SCE_Vector3_Dot (c, p)) < r*r);
}
int SCE_Collide_BSWithBB (SCE_SBoundingSphere *sphere, SCE_SBoundingBox *box)
{
    unsigned int i, n = 0;
    float *points = SCE_BoundingBox_GetPoints (box);
    for (i = 0; i < 8; i++)
    {
        if (SCE_Collide_BSWithPointv (sphere, &points[i]))
            n++;
    }
    if (n == 0)
        return (SCE_Collide_BBWithBS (box, sphere) ?
                SCE_COLLIDE_PARTIALLY : SCE_COLLIDE_OUT);
    else
        return (n == 8 ? SCE_COLLIDE_IN : SCE_COLLIDE_PARTIALLY);
}
int SCE_Collide_BSWithBS (SCE_SBoundingSphere *sphere,
                          SCE_SBoundingSphere *sphere2)
{
    float *c = SCE_BoundingSphere_GetCenter (sphere);
    float *c2 = SCE_BoundingSphere_GetCenter (sphere2);
    float r = SCE_BoundingSphere_GetRadius (sphere);
    float r2 = SCE_BoundingSphere_GetRadius (sphere2);
    float d = fabs (SCE_Vector3_Dot (c, c2));
    r  = r - r2; r  *= r;
    r2 = r + r2; r2 *= r2;
    if (d <= r - r2)
        return SCE_COLLIDE_IN;
    else if (d < r + r2)
        return SCE_COLLIDE_PARTIALLY;
    else
        return SCE_COLLIDE_OUT;
}

/** @} */
