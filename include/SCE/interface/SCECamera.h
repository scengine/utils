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
 
/* created: 21/12/2006
   updated: 13/03/2009 */

#ifndef SCECAMERA_H
#define SCECAMERA_H

#include <SCE/utils/SCEVector.h>
#include <SCE/interface/SCEFrustum.h>
#include <SCE/interface/SCENode.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup camera
 * @{
 */

/** \copydoc sce_sviewport */
typedef struct sce_sviewport SCE_SViewport;
/**
 * \brief An OpenGL's viewport that defines and rectangular region where render
 */
struct sce_sviewport
{
    int x, /**< X coordinate of the origin of the viewport */
        y; /**< Y coordinate of the origin of the viewport */
    int w, /**< width of the viewport */
        h; /**< height of the viewport */
}; 

/** \copydoc sce_scamera */
typedef struct sce_scamera SCE_SCamera;
/**
 * \brief A camera structure
 *
 * A camera structure stores information about the view point to render the
 * scene, the vision's angle, the orientation of the view, etc.
 */
struct sce_scamera
{
    SCE_TMatrix4 finalview;    /* matrice de vue combinee avec le noeud */
    SCE_TMatrix4 finalviewinv; /* finalview inversee */
    SCE_TMatrix4 view;         /* matrice de vue */
    SCE_TMatrix4 viewinv;      /* 'view' inversee */
    SCE_TMatrix4 proj;         /* matrice de projection */
    SCE_TMatrix4 projinv;      /* 'proj' inversee */
    SCE_TMatrix4 finalviewproj;/* final view projection matrix */
    SCE_TMatrix4 finalviewprojinv; /* final inverse view projection matrix */
    SCE_SViewport viewport;    /* viewport de la camera */
    SCE_SFrustum frustum;      /* frustum de vue de la camera */
    SCE_SNode *node;           /* noeud de la camera */
};

/** @} */

void SCE_Camera_Init (SCE_SCamera*);

SCE_SCamera* SCE_Camera_Create (void);
void SCE_Camera_Delete (SCE_SCamera*);

void SCE_Camera_SetViewport (SCE_SCamera*, int, int, int, int);

float* SCE_Camera_GetView (SCE_SCamera*);
float* SCE_Camera_GetViewInverse (SCE_SCamera*);
float* SCE_Camera_GetProj (SCE_SCamera*);
float* SCE_Camera_GetProjInverse (SCE_SCamera*);
float* SCE_Camera_GetFinalViewProj (SCE_SCamera*);
float* SCE_Camera_GetFinalViewProjInverse (SCE_SCamera*);

void SCE_Camera_GetPositionv (SCE_SCamera*, SCE_TVector3);

SCE_SNode* SCE_Camera_GetNode (SCE_SCamera*);

SCE_SFrustum* SCE_Camera_GetFrustum (SCE_SCamera*);

float* SCE_Camera_GetFinalView (SCE_SCamera*);
float* SCE_Camera_GetFinalViewInverse (SCE_SCamera*);

void SCE_Camera_Update (SCE_SCamera*);

void SCE_Camera_Use (SCE_SCamera*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
