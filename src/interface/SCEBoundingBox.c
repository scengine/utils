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
   updated: 21/06/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCEBoundingBox.h>

/**
 * \file SCEBoundingBox.c
 * \copydoc boundingbox
 * 
 * \file SCEBoundingBox.h
 * \copydoc boundingbox
 */

/**
 * \defgroup boundingbox Bounding box managment
 * \ingroup interface
 * \brief Boundig box managment and computing functions
 */

/** @{ */

/**
 * \brief Initialize a bounding box
 * \param box the bounding box to initialize
 */
void SCE_BoundingBox_Init (SCE_SBoundingBox *box)
{
    int i;
    for (i=0; i<8; i++)
        SCE_Vector3_Operator1 (box->p[i], =, 0.);
    for (i=0; i<8; i++)
        SCE_Vector3_Operator1 (box->olds[i], =, 0.);
    box->pushed = SCE_FALSE;
}

/**
 * \brief Sets size and origin of a bounding box
 * \param box a bounding box
 * \param o origin vector of the bounding box
 * \param w,h,d new bounding box's width, height and depth
 * 
 * \see SCE_BoundingBox_SetFromCenter(), SCE_BoundingBox_Setv()
 */
void SCE_BoundingBox_Set (SCE_SBoundingBox *box, SCE_TVector3 o,
                          float w, float h, float d)
{
    SCE_Vector3_Set (box->p[0], o[0],   o[1],   o[2]);
    SCE_Vector3_Set (box->p[1], o[0]+w, o[1],   o[2]);
    SCE_Vector3_Set (box->p[2], o[0]+w, o[1]+h, o[2]);
    SCE_Vector3_Set (box->p[3], o[0],   o[1]+h, o[2]);
    SCE_Vector3_Set (box->p[4], o[0],   o[1]+h, o[2]+d);
    SCE_Vector3_Set (box->p[5], o[0]+w, o[1]+h, o[2]+d);
    SCE_Vector3_Set (box->p[6], o[0]+w, o[1],   o[2]+d);
    SCE_Vector3_Set (box->p[7], o[0],   o[1],   o[2]+d);
}
/**
 * \brief Vectorial version of SCE_BoundingBox_Set()
 * \param bx a bounding box
 * \param o origin vector of the bounding box
 * \param d dimension vector of the bouding box
 * \see SCE_BoundingBox_Set()
 */
void SCE_BoundingBox_Setv (SCE_SBoundingBox *bx, SCE_TVector3 o, SCE_TVector3 d)
{
    SCE_BoundingBox_Set (bx, o, d[0], d[1], d[2]);
}
/*
 *  4_____________5
 *  |\           |\
 *  | \          | \
 *  |  \         |  \
 *  |   \        |   \
 *  |    \3______|____\2
 *  |    |       |    |         z   y
 *  |7___|_______|6   |          \ |
 *   \   |        \   |           \|__ x
 *    \  |         \  |
 *     \ |          \ |
 *      \|___________\|
 *       0            1
 */

/**
 * \brief Sets position and size of a bounding box
 * \param box a bounding box
 * \param c the coordinates of the new bounding box's center
 * \param w,h,d new bounding box's width, height and depth
 */
void SCE_BoundingBox_SetFromCenter (SCE_SBoundingBox *box, SCE_TVector3 c,
                                    float w, float h, float d)
{
    SCE_TVector3 origin;
    origin[0] = c[0] - w/2.;
    origin[1] = c[1] - h/2.;
    origin[2] = c[2] - d/2.;
    SCE_BoundingBox_Set (box, origin, w, h, d);
}

/**
 * \brief Sets the size of a bouding box
 * \param box a bounding box
 * \param w,h,d new bounding box's width, height and depth
 */
void SCE_BoundingBox_SetSize (SCE_SBoundingBox *box, float w, float h, float d)
{
    SCE_TVector3 center;
    SCE_BoundingBox_GetCenterv (box, center);
    SCE_BoundingBox_SetFromCenter (box, center, w, h, d);
}

/**
 * \brief Sets the center of a bounding box
 * \param box a bounding box
 * \param x,y,z new center coordinates
 * 
 * This function updates center coordinates of a bounding box (move it).
 * \see SCE_BoundingBox_SetCenterv()
 */
void SCE_BoundingBox_SetCenter (SCE_SBoundingBox *box,
                                float x, float y, float z)
{
    SCE_TVector3 center;
    center[0] = x;
    center[1] = y;
    center[2] = z;
    SCE_BoundingBox_SetCenterv (box, center);
}
/**
 * \brief Vectorial version of SCE_BoundingBox_SetCenter()
 * \see SCE_BoundingBox_SetCenter()
 */
void SCE_BoundingBox_SetCenterv (SCE_SBoundingBox *box, SCE_TVector3 c)
{
    int i;
    SCE_TVector3 dir, center;
    SCE_BoundingBox_GetCenterv (box, center);
    SCE_Vector3_Operator2v (dir, =, c, -, center);
    /* translation de tous les points */
    for (i=0; i<8; i++)
        SCE_Vector3_Operator1v (box->p[i], +=, dir);
}
/**
 * \brief Gets center of a bounding box
 * \param box a bounding box
 * \param center a vector where store bounding box's center coordinates
 */
void SCE_BoundingBox_GetCenterv (SCE_SBoundingBox *box, SCE_TVector3 center)
{
    center[0] = (box->p[1][0] - box->p[0][0]) / 2.0 + box->p[0][0];
    center[1] = (box->p[2][1] - box->p[1][1]) / 2.0 + box->p[1][1];
    center[2] = (box->p[7][2] - box->p[0][2]) / 2.0 + box->p[0][2];
}

float* SCE_BoundingBox_GetOrigin (SCE_SBoundingBox *box)
{
    return box->p;
}

void SCE_BoundingBox_GetOriginv (SCE_SBoundingBox *box, SCE_TVector3 o)
{
    SCE_Vector3_Copy (o, box->p);
}


/**
 * \brief Gets the eigth corners of a bounding box
 * \param box a bounding box
 * \returns an array containing the eight vector coordinates of the eight
 *          bounding box's corners
 * \see SCE_BoundingBox_GetPointsv()
 */
float* SCE_BoundingBox_GetPoints (SCE_SBoundingBox *box)
{
    return box->p;
}
/**
 * \brief Gets the eigth corners of a bounding box
 * \param box a bounding box
 * \param p an array of eight SCE_TVector3 to fill with the eight corner
 *          coordinates of the bounding box
 * 
 * This function is the vectorial version of SCE_BoundingBox_GetPoints().
 * \see SCE_BoundingBox_GetPoints()
 */
void SCE_BoundingBox_GetPointsv (SCE_SBoundingBox *box, SCE_TVector3 *p)
{
    int i;
    for (i=0; i<8; i++)
        SCE_Vector3_Copy (p[i], box->p[i]);
}


void SCE_BoundingBox_MakePlanes (SCE_SBoundingBox *box)
{
    SCE_TVector3 n;

    /* near */
    SCE_Vector3_Operator2v (n, =, box->p[1], -, box->p[6]);
    SCE_Plane_SetFromPointv (&box->planes[0], n, box->p[1]);
    /* far */
    SCE_Vector3_Operator1 (n, *=, -1.0);
    SCE_Plane_SetFromPointv (&box->planes[1], n, box->p[6]);

    /* left */
    SCE_Vector3_Operator2v (n, =, box->p[1], -, box->p[0]);
    SCE_Plane_SetFromPointv (&box->planes[2], n, box->p[1]);
    /* right */
    SCE_Vector3_Operator1 (n, *=, -1.0);
    SCE_Plane_SetFromPointv (&box->planes[3], n, box->p[0]);

    /* up */
    SCE_Vector3_Operator2v (n, =, box->p[1], -, box->p[2]);
    SCE_Plane_SetFromPointv (&box->planes[4], n, box->p[1]);
    /* down */
    SCE_Vector3_Operator1 (n, *=, -1.0);
    SCE_Plane_SetFromPointv (&box->planes[5], n, box->p[2]);
}
SCE_SPlane* SCE_BoundingBox_GetPlanes (SCE_SBoundingBox *box)
{
    return box->planes;
}
void SCE_BoundingBox_GetPlanesv (SCE_SBoundingBox *box, SCE_SPlane *planes)
{
    unsigned int i;
    for (i = 0; i < 6; i++)
        planes[i] = box->planes[i];
}


/**
 * \brief Get the width of a bounding box
 * \param box a bouding box
 * \returns width of the given bounding box
 */
float SCE_BoundingBox_GetWidth (SCE_SBoundingBox *box)
{
    return box->p[1][0] - box->p[0][0];
}
/**
 * \brief Get the height of a bounding box
 * \param box a bouding box
 * \returns height of the given bounding box
 */
float SCE_BoundingBox_GetHeight (SCE_SBoundingBox *box)
{
    return box->p[2][1] - box->p[1][1];
}
/**
 * \brief Get the depth of a bounding box
 * \param box a bouding box
 * \returns depth of the given bounding box
 */
float SCE_BoundingBox_GetDepth (SCE_SBoundingBox *box)
{
    return box->p[7][2] - box->p[0][2];
}
/**
 * \brief Writes the dimensions of a bounding box into the given pointers
 * \param w,h,d will writes the dimensions here, some pointers can be NULL
 * \sa SCE_BoundingBox_GetWidth(), SCE_BoundingBox_GetHeight(),
 * SCE_BoundingBox_GetDepth()
 */
void SCE_BoundingBox_GetDimensionsv (SCE_SBoundingBox *box,
                                     float *w, float *h, float *d)
{
    if (w)
        *w = SCE_BoundingBox_GetWidth (box);
    if (h)
        *h = SCE_BoundingBox_GetHeight (box);
    if (d)
        *d = SCE_BoundingBox_GetDepth (box);
}

/**
 * \brief Applies a matrix to a bounding box and saves the old data
 * \param box a bounding box
 * \param m the matrix to apply
 * \sa SCE_BoundingBox_Pop()
 */
void SCE_BoundingBox_Push (SCE_SBoundingBox *box, SCE_TMatrix4 m)
{
    if (!box->pushed)
    {
        int i;
        for (i=0; i<8; i++)
            SCE_Vector3_Copy (box->olds[i], box->p[i]);
        for (i=0; i<8; i++)
            SCE_Matrix4_MulV3Copy (m, box->p[i]);
        box->pushed = SCE_TRUE;
    }
}
/**
 * \brief Restores the data of a bounding box saved by SCE_BoundingBox_Push()
 * \param box a bounding box
 * \sa SCE_BoundingBox_Push()
 */
void SCE_BoundingBox_Pop (SCE_SBoundingBox *box)
{
    if (box->pushed)
    {
        int i;
        for (i=0; i<8; i++)
            SCE_Vector3_Copy (box->p[i], box->olds[i]);
        box->pushed = SCE_FALSE;
    }
}

/**
 * \brief Indicates if a bounding box is in "pushed" state
 * \sa SCE_BoundingBox_Push(), SCE_BoundingBox_Pop()
 */
int SCE_BoundingBox_IsPushed (SCE_SBoundingBox *box)
{
    return box->pushed;
}

/** @} */
