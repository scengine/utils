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
 
/* created: 02/11/2008
   last change: 18/11/2008 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEVector.h>
#include <SCE/interface/SCELevelOfDetail.h>

static int SCE_Lod_DefaultGetLodFunc (float);

void SCE_Lod_Init (SCE_SLevelOfDetail *lod)
{
    lod->lod = 0;
    lod->box = NULL;
    lod->size = 0.0f;
    lod->dist = 0.0f;
    lod->getlod = SCE_Lod_DefaultGetLodFunc;
}

SCE_SLevelOfDetail* SCE_Lod_Create (void)
{
    SCE_SLevelOfDetail *lod = NULL;
    SCE_btstart ();
    lod = SCE_malloc (sizeof *lod);
    if (!lod)
        Logger_LogSrc();
    else
        SCE_Lod_Init (lod);
    return lod;
}

void SCE_Lod_Delete (SCE_SLevelOfDetail *lod)
{
    SCE_free (lod);
}


void SCE_Lod_SetBoundingBox (SCE_SLevelOfDetail *lod, SCE_SBoundingBox *box)
{
    lod->box = box;
}

void SCE_Lod_SetGetLODFunc (SCE_SLevelOfDetail *lod, SCE_FGetLODFunc f)
{
    if (f)
        lod->getlod = f;
    else
        lod->getlod = SCE_Lod_DefaultGetLodFunc;
}


SCE_SBoundingBox* SCE_Lod_GetBoundingBox (SCE_SLevelOfDetail *lod)
{
    return lod->box;
}


static float SCE_Lod_ComputeDistance (SCE_TMatrix4 m, SCE_SCamera *cam)
{
    SCE_TVector4 v1, v2;

    /* get camera's position */
    SCE_Matrix4_GetTranslation (SCE_Camera_GetFinalViewInverse (cam), v1);
    /* get matrix position */
    SCE_Matrix4_GetTranslation (m, v2);
    /* compute distance */
    return SCE_Vector3_Distance (v1, v2);
}

static float SCE_Lod_ComputeBoundingBoxSize (SCE_SBoundingBox *box,
                                             float dist, SCE_SCamera *cam)
{
    float *points;
    SCE_TVector4 v1, v2;

    points = SCE_BoundingBox_GetPoints (box);
    /* see SCEBoundingBox.c for infos about the points 0 and 2 */
    SCE_Vector3_Copy (v1, &points[0]);
    SCE_Vector3_Copy (v2, &points[2]);
    /* adjust distance */
    dist += SCE_BoundingBox_GetDepth (box)/2;
    v1[2] -= dist;
    v2[2] -= dist;
    v1[3] = v2[3] = 1.0;
    /* project points */
    SCE_Matrix4_MulV4 (SCE_Camera_GetProj (cam), v1);
    SCE_Matrix4_MulV4 (SCE_Camera_GetProj (cam), v2);
    /* depth useless */
    SCE_Vector2_Operator1 (v1, /=, v1[3]);
    SCE_Vector2_Operator1 (v2, /=, v2[3]);
    /* distance between the two projected points */
    return SCE_Vector2_Distance (v1, v2);
}

static int SCE_Lod_DefaultGetLodFunc (float size)
{
    return (unsigned int)(/*2.8284*/0.4/size - 1.0);
}

int SCE_Lod_Compute (SCE_SLevelOfDetail *lod, SCE_TMatrix4 m, SCE_SCamera *cam)
{
    lod->dist = SCE_Lod_ComputeDistance (m, cam);
    lod->size = SCE_Lod_ComputeBoundingBoxSize (lod->box, lod->dist, cam);
    lod->lod = lod->getlod (lod->size);
    return lod->lod;
}

int SCE_Lod_GetLOD (SCE_SLevelOfDetail *lod)
{
    return lod->lod;
}

float SCE_Lod_GetSize (SCE_SLevelOfDetail *lod)
{
    return lod->size;
}

float SCE_Lod_GetDistance (SCE_SLevelOfDetail *lod)
{
    return lod->dist;
}
