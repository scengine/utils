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
   updated: 29/10/2010 */

#ifndef SCEPLANE_H
#define SCEPLANE_H

#include "SCE/utils/SCEVector.h"
#include "SCE/utils/SCELine.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sce_splane SCE_SPlane;
struct sce_splane {
    SCE_TVector3 n;
    float d;
};

void SCE_Plane_Init (SCE_SPlane*);

void SCE_Plane_Set (SCE_SPlane*, float, float, float, float);
void SCE_Plane_Setv (SCE_SPlane*, SCE_TVector3, float);

void SCE_Plane_SetFromPoint (SCE_SPlane*, SCE_TVector3, float, float, float);
void SCE_Plane_SetFromPointv (SCE_SPlane*, SCE_TVector3, SCE_TVector3);

void SCE_Plane_SetFromTriangle (SCE_SPlane*, SCE_TVector3, SCE_TVector3,
                                SCE_TVector3);

void SCE_Plane_Normalize (SCE_SPlane*, int);

float SCE_Plane_DistanceToPoint (SCE_SPlane*, float, float, float);
float SCE_Plane_DistanceToPointv (SCE_SPlane*, SCE_TVector3);

int SCE_Plane_LineIntersection (SCE_SPlane*, SCE_SLine3*, SCE_TVector3);

int SCE_Plane_TriangleLineIntersection (SCE_TVector3, SCE_TVector3,
                                        SCE_TVector3, SCE_SLine3*,
                                        SCE_TVector3);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
