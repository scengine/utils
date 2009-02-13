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

#ifndef SCEBOUNDINGSPHERE_H
#define SCEBOUNDINGSPHERE_H

#include <SCE/utils/SCEMatrix.h>
#include <SCE/utils/SCEVector.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sce_sboundingsphere SCE_SBoundingSphere;
struct sce_sboundingsphere
{
    /* 'o' says 'old' */
    SCE_TVector3 center, ocenter;
    /*SCE_TVector3 x, y, z, ox, oy, oz;*/
    float radius, oradius;
    int pushed;
};

void SCE_BoundingSphere_Init (SCE_SBoundingSphere*);

void SCE_BoundingSphere_Set (SCE_SBoundingSphere*, float, float, float, float);
void SCE_BoundingSphere_Setv (SCE_SBoundingSphere*, SCE_TVector3, float);

void SCE_BoundingSphere_SetCenter (SCE_SBoundingSphere*, float, float, float);
void SCE_BoundingSphere_SetCenterv (SCE_SBoundingSphere*, SCE_TVector3);
void SCE_BoundingSphere_SetRadius (SCE_SBoundingSphere*, float);

float* SCE_BoundingSphere_GetCenter (SCE_SBoundingSphere*);
void SCE_BoundingSphere_GetCenterv (SCE_SBoundingSphere*, SCE_TVector3);
float SCE_BoundingSphere_GetRadius (SCE_SBoundingSphere*);

void SCE_BoundingSphere_Push (SCE_SBoundingSphere*, SCE_TMatrix4);
void SCE_BoundingSphere_Pop (SCE_SBoundingSphere*);

int SCE_BoundingSphere_IsPushed (SCE_SBoundingSphere*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
