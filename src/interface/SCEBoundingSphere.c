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
 
/* created: 06/01/2009
   updated: 14/01/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCEBoundingSphere.h>

/**
 * \file SCEBoundingSphere.c
 * \copydoc boundingsphere
 * 
 * \file SCEBoundingSphere.h
 * \copydoc boundingsphere
 */

/**
 * \defgroup boundingsphere Bounding sphere managment
 * \ingroup interface
 * \brief Boundig sphere managment and computing functions
 */

/** @{ */

/**
 * \brief Initialize a bounding sphere
 * \param box the bounding sphere to initialize
 */
void SCE_BoundingSphere_Init (SCE_SBoundingSphere *sphere)
{
    SCE_Vector3_Set (sphere->center, 0.0, 0.0, 0.0);
    SCE_Vector3_Set (sphere->ocenter, 0.0, 0.0, 0.0);

    sphere->radius = sphere->oradius = 1.0;
#if 0
    SCE_Vector3_Set (sphere->x, sphere->radius, 0.0, 0.0);
    SCE_Vector3_Set (sphere->ox, sphere->radius, 0.0, 0.0);
    SCE_Vector3_Set (sphere->y, 0.0, sphere->radius, 0.0);
    SCE_Vector3_Set (sphere->oy, 0.0, sphere->radius, 0.0);
    SCE_Vector3_Set (sphere->z, 0.0, 0.0, sphere->radius);
    SCE_Vector3_Set (sphere->oz, 0.0, 0.0, sphere->radius);
#endif
    sphere->pushed = SCE_FALSE;
}

void SCE_BoundingSphere_Set (SCE_SBoundingSphere *sphere,
                             float x, float y, float z, float radius)
{
    SCE_Vector3_Set (sphere->center, x, y, z);
    sphere->radius = radius;
}
void SCE_BoundingSphere_Setv (SCE_SBoundingSphere *sphere,
                              SCE_TVector3 center, float radius)
{
    SCE_Vector3_Copy (sphere->center, center);
    sphere->radius = radius;
}

void SCE_BoundingSphere_SetCenter (SCE_SBoundingSphere *sphere,
                                   float x, float y, float z)
{
    SCE_Vector3_Set (sphere->center, x, y, z);
}
void SCE_BoundingSphere_SetCenterv (SCE_SBoundingSphere *sphere,
                                    SCE_TVector3 center)
{
    SCE_Vector3_Copy (sphere->center, center);
}
void SCE_BoundingSphere_SetRadius (SCE_SBoundingSphere *sphere, float radius)
{
    sphere->radius = radius;
}

float* SCE_BoundingSphere_GetCenter (SCE_SBoundingSphere *sphere)
{
    return sphere->center;
}
void SCE_BoundingSphere_GetCenterv (SCE_SBoundingSphere *sphere,
                                    SCE_TVector3 center)
{
    SCE_Vector3_Copy (center, sphere->center);
}
float SCE_BoundingSphere_GetRadius (SCE_SBoundingSphere *sphere)
{
    return sphere->radius;
}

void SCE_BoundingSphere_Push (SCE_SBoundingSphere *sphere, SCE_TMatrix4 m)
{
    if (!sphere->pushed)
    {
        float highest;
        /* 1st: saves the current data */
        SCE_Vector3_Copy (sphere->ocenter, sphere->center);
        sphere->oradius = sphere->radius;
        /* 2nd: apply the matrix */
        /* TODO: wrong */
#if 0
        /* find the highest scaling component */
        highest = MAX (m[0], m[1]);
        highest = MAX (highest, m[2]);
        highest = MAX (highest, m[4]);
        highest = MAX (highest, m[5]);
        highest = MAX (highest, m[6]);
        highest = MAX (highest, m[8]);
        highest = MAX (highest, m[9]);
        highest = MAX (highest, m[10]);
        /* use it */
        sphere->radius *= highest;
#endif
        /* apply the matrix to the center vector */
        SCE_Matrix4_MulV3Copy (m, sphere->center);
        sphere->pushed = SCE_TRUE;
    }
}
void SCE_BoundingSphere_Pop (SCE_SBoundingSphere *sphere)
{
    if (sphere->pushed)
    {
        SCE_Vector3_Copy (sphere->center, sphere->ocenter);
        sphere->radius = sphere->oradius;
        sphere->pushed = SCE_FALSE;
    }
}

/**
 * \brief Indicates if a bounding sphere is in "pushed" state
 * \sa SCE_BoundingSphere_Push(), SCE_BoundingSphere_Pop()
 */
int SCE_BoundingSphere_IsPushed (SCE_SBoundingSphere *sphere)
{
    return sphere->pushed;
}

/** @} */
