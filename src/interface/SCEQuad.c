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
 
/* created: 06/04/2008
   updated: 08/07/2008 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEResource.h>

#include <SCE/core/SCECMatrix.h>

#include <SCE/interface/SCEMesh.h>
#include <SCE/interface/SCEQuad.h>

/**
 * \file SCEQuad.c
 * \copydoc quad
 * \file SCEQuad.h
 * \copydoc quad
 */

/**
 * \defgroup quad Offers quad mesh
 * \ingroup interface
 * \brief
 */

/** @{ */

/**
 * \brief created quad
 * \internal
 */
static SCE_SMesh *mesh = NULL;
/**
 * \internal
 */
static SCE_SMesh *ptr[2];

int SCE_Init_Quad (void)
{
    /*SCEvertices vertices[] = {-1., -1.,  1., -1.,  1., 1.,  -1., 1.};*/
    SCEvertices vertices[] = {0., 0.,  1., 0.,  1., 1.,  0., 1.};
    /*SCEvertices texcoord[] = {0., 0.,  1., 0.,  1., 1.,  0., 1.};*/
    SCEvertices normals[] =  {0.,0.,1.,  0.,0.,1., 0.,0.,1., 0.,0.,1.};
    SCEvertices colors[] = {1., 1., 1., 1.,  1., 1., 1., 1.,
                            1., 1., 1., 1.,  1., 1., 1., 1.};
    SCE_btstart ();
    if (!(mesh = SCE_Mesh_Create ()))
        goto fail;
    if (SCE_Resource_Add (SCE_Mesh_GetResourceType (),
                          SCE_QUAD_MESH_NAME, mesh) < 0)
        goto fail;
    if (SCE_Mesh_AddVertices (mesh, 0, SCE_POSITION, SCE_VERTICES_TYPE, 2, 4,
                              vertices, SCE_FALSE) < 0)
        goto fail;
    if (SCE_Mesh_AddVertices (mesh, 0, SCE_TEXCOORD0, SCE_VERTICES_TYPE, 2, 4,
                              vertices, SCE_FALSE) < 0)
        goto fail;
    if (SCE_Mesh_AddVertices (mesh, 0, SCE_NORMAL, SCE_VERTICES_TYPE, 3, 4,
                              normals, SCE_FALSE) < 0)
        goto fail;
    if (SCE_Mesh_AddVertices (mesh, 0, SCE_COLOR, SCE_VERTICES_TYPE, 4, 4,
                              colors, SCE_FALSE) < 0)
        goto fail;
    SCE_Mesh_ActivateVB (mesh, 0, SCE_TRUE);
    SCE_Mesh_SetRenderMode (mesh, SCE_QUADS);
    if (SCE_Mesh_Build (mesh) < 0)
        goto fail;

    ptr[0] = mesh;
    ptr[1] = NULL;

    /* log resource */
    if (SCE_Resource_Add (SCE_Mesh_GetResourceType (),
                          SCE_QUAD_MESHS_NAME, ptr) < 0)
        goto fail;
    SCE_btend ();
    return SCE_OK;
fail:
    SCE_Mesh_Delete (mesh);
    SCEE_LogSrc ();
    SCE_btend ();
    return SCE_ERROR;
}
void SCE_Quit_Quad (void)
{
    SCE_Resource_Free (ptr);
    SCE_Mesh_Delete (mesh), mesh = NULL;
}

void SCE_Quad_DrawDefault (void)
{
    SCE_Mesh_Render (mesh);
}

void SCE_Quad_MakeMatrix (SCE_TMatrix4 mat, float x, float y, float w, float h)
{
    SCE_Matrix4_Scale (mat, w, h, 1.);
    SCE_Matrix4_MulTranslate (mat, x/w, y/w, 0.);
}
void SCE_Quad_MakeMatrixFromRectangle (SCE_TMatrix4 mat, SCE_SIntRect *r)
{
    SCE_Quad_MakeMatrix (mat, r->p1[0], r->p1[1], SCE_Rectangle_GetWidth (r),
                         SCE_Rectangle_GetHeight (r));
}
void SCE_Quad_MakeMatrixFromRectanglef (SCE_TMatrix4 mat, SCE_SFloatRect *r)
{
    SCE_Quad_MakeMatrix (mat, r->p1[0], r->p1[1], SCE_Rectangle_GetWidthf (r),
                         SCE_Rectangle_GetHeightf (r));
}

void SCE_Quad_Draw (float x, float y, float w, float h)
{
    SCE_TMatrix4 mat;
    SCE_CPushMatrix ();
    SCE_Quad_MakeMatrix (mat, x, y, w, h);
    SCE_CMultMatrix (mat);
    SCE_Mesh_Render (mesh);
    SCE_CPopMatrix ();
}

void SCE_Quad_DrawFromRectangle (SCE_SIntRect *r)
{
    SCE_Quad_Draw (r->p1[0], r->p1[1], SCE_Rectangle_GetWidth (r),
                   SCE_Rectangle_GetHeight (r));
}
void SCE_Quad_DrawFromRectanglef (SCE_SFloatRect *r)
{
    SCE_Quad_Draw (r->p1[0], r->p1[1], SCE_Rectangle_GetWidthf (r),
                   SCE_Rectangle_GetHeightf (r));
}

SCE_SMesh* SCE_Quad_GetMesh (void)
{
    return mesh;
}

/** @} */
