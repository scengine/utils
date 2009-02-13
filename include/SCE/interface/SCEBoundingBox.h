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
 
/* created: 27/02/2008
   updated: 06/05/2008 */

#ifndef SCEBOUNDINGBOX_H
#define SCEBOUNDINGBOX_H

#include <SCE/utils/SCEMatrix.h>
#include <SCE/utils/SCEVector.h>
#include <SCE/utils/SCEPlane.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sce_sboundingbox SCE_SBoundingBox;
struct sce_sboundingbox
{
    SCE_TVector3 p[8];
    SCE_TVector3 olds[8];
    SCE_SPlane planes[6];
    int pushed;
};


/* initialise une structure de bounding box */
void SCE_BoundingBox_Init (SCE_SBoundingBox*);

/* defini une bounding box */
void SCE_BoundingBox_Set (SCE_SBoundingBox*, SCE_TVector3, float, float, float);
void SCE_BoundingBox_Setv (SCE_SBoundingBox*, SCE_TVector3, SCE_TVector3);
void SCE_BoundingBox_SetFromCenter (SCE_SBoundingBox*, SCE_TVector3,
                                    float, float, float);

/* defini les dimensions d'une bounding box */
void SCE_BoundingBox_SetSize (SCE_SBoundingBox*, float, float, float);

/* positionne le centre d'une bounding box */
void SCE_BoundingBox_SetCenter (SCE_SBoundingBox*, float, float, float);
void SCE_BoundingBox_SetCenterv (SCE_SBoundingBox*, SCE_TVector3);
/* retourne le centre de la bounding box */
void SCE_BoundingBox_GetCenterv (SCE_SBoundingBox*, SCE_TVector3);
float* SCE_BoundingBox_GetOrigin (SCE_SBoundingBox*);
void SCE_BoundingBox_GetOriginv (SCE_SBoundingBox*, SCE_TVector3);

/* retourne les 8 points d'une bounding box */
float* SCE_BoundingBox_GetPoints (SCE_SBoundingBox*);
void SCE_BoundingBox_GetPointsv (SCE_SBoundingBox*, SCE_TVector3*);

void SCE_BoundingBox_MakePlanes (SCE_SBoundingBox*);
SCE_SPlane* SCE_BoundingBox_GetPlanes (SCE_SBoundingBox*);
void SCE_BoundingBox_GetPlanesv (SCE_SBoundingBox*, SCE_SPlane*);

/* retourne les dimensions d'une bounding box */
float SCE_BoundingBox_GetWidth (SCE_SBoundingBox*);
float SCE_BoundingBox_GetHeight (SCE_SBoundingBox*);
float SCE_BoundingBox_GetDepth (SCE_SBoundingBox*);

/* assigne une matrice aux points d'une bounding box */
void SCE_BoundingBox_Push (SCE_SBoundingBox*, SCE_TMatrix4);
/* restaure les points sauvegardes avant l'application d'une matrice */
void SCE_BoundingBox_Pop (SCE_SBoundingBox*);

int SCE_BoundingBox_IsPushed (SCE_SBoundingBox*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
