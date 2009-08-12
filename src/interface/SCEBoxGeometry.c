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

/* created: 07/08/2009
   updated: 07/08/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCEBoxGeometry.h>

static int is_init = SCE_FALSE;

#define p0 -1.0f, -1.0f, -1.0f
#define p1  1.0f, -1.0f, -1.0f
#define p2  1.0f,  1.0f, -1.0f
#define p3 -1.0f,  1.0f, -1.0f
#define p4 -1.0f,  1.0f,  1.0f
#define p5  1.0f,  1.0f,  1.0f
#define p6  1.0f, -1.0f,  1.0f
#define p7 -1.0f, -1.0f,  1.0f

static SCEvertices pos_indiv_triangle[] = {
    /* front Z */
    p0, p1, p2, p2, p3, p0,
    /* back Z */
    p4, p5, p6, p6, p7, p4,
    /* front X */
    p1, p2, p5, p5, p6, p1,
    /* back X */
    p0, p3, p4, p4, p7, p0,
    /* front Y */
    p0, p1, p6, p6, p7, p0,
    /* back Y */
    p2, p3, p4, p4, p5, p2
};

static SCEvertices texcoord_interior_triangle[] = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
};

static SCEvertices texcoord_exterior_triangle[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
};

static SCEvertices texcoord_cubemap[] = {
    -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f
};

static SCEindices indices_lines[] = {
    0, 1,  1, 2,  2, 3,  3, 0,
    3, 4,  4, 7,  7, 6,  6, 5,
    5, 4,  5, 2,  6, 1,  0, 7
};

static SCEindices indices_triangles[] = {
    1, 0, 2,  2, 0, 3,
    3, 0, 4,  4, 0, 7,
    7, 0, 6,  6, 0, 1,
    1, 6, 2,  2, 6, 5,
    5, 2, 3,  3, 5, 4,
    4, 5, 7,  7, 5, 6
};


int SCE_Init_BoxGeom (void)
{
    size_t i;
    if (is_init)
        return SCE_OK;
    for (i = 0; i < 8; i++)
        SCE_Vector3_Normalize (&texcoord_cubemap[i * 3]);
    is_init = SCE_TRUE;
    return SCE_OK;
}
void SCE_Quit_BoxGeom (void)
{
    is_init = SCE_FALSE;
}


static void SCE_BoxGeom_MulIndiv (SCEvertices *v, SCE_SBox *box)
{
    SCE_TVector3 coeff;
    size_t i;
    coeff[0] = SCE_Box_GetWidth (box) * 0.5f;
    coeff[1] = SCE_Box_GetHeight (box) * 0.5f;
    coeff[2] = SCE_Box_GetDepth (box) * 0.5f;
    for (i = 0; i < 8; i++)
        SCE_Vector3_Operator1v (&v[i * 3], *=, coeff);
}

static int SCE_BoxGeom_GenPoints (SCE_SBox *box, SCE_EBoxGeomTexCoordMode mode,
                                  SCE_SGeometry *geom)
{
    SCE_SGeometryArray array;

    SCE_Geometry_SetNumVertices (geom, 8);
    /* no texturing, except in case of cubemap */
    if (mode == SCE_BOX_CUBEMAP_TEXCOORD) {
        SCE_Geometry_InitArray (&array);
        SCE_Geometry_SetArrayData (&array, SCE_TEXCOORD0, SCE_VERTICES_TYPE,
                                   0, 3, texcoord_cubemap, SCE_FALSE);
        if (!SCE_Geometry_AddArrayDupDup (geom, &array, SCE_FALSE))
            goto fail;
    }
    SCE_Geometry_InitArray (&array);
    SCE_Geometry_SetArrayData (&array, SCE_POSITION, SCE_VERTICES_TYPE, 0, 3,
                               SCE_Box_GetPoints (box), SCE_FALSE);
    if (!SCE_Geometry_AddArrayDupDup (geom, &array, SCE_FALSE))
        goto fail;
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}

static int SCE_BoxGeom_GenLines (SCE_SBox *box, SCE_EBoxGeomTexCoordMode mode,
                                 SCE_SGeometry *geom)
{
    SCE_SGeometryArray array;
    /* TODO: texturing not supported yet */
    SCE_Geometry_SetNumVertices (geom, 8);
    SCE_Geometry_SetNumIndices (geom, 24);
#if 0
    switch (mode) {
    case 
        }
#endif
    /* ... just cubemap */
    if (mode == SCE_BOX_CUBEMAP_TEXCOORD) {
        SCE_Geometry_InitArray (&array);
        SCE_Geometry_SetArrayData (&array, SCE_TEXCOORD0, SCE_VERTICES_TYPE, 0,
                                   3, texcoord_cubemap, SCE_FALSE);
        if (!SCE_Geometry_AddArrayDupDup (geom, &array, SCE_FALSE))
            goto fail;
    }
    SCE_Geometry_InitArray (&array);
    SCE_Geometry_SetArrayData (&array, SCE_POSITION, SCE_VERTICES_TYPE, 0,
                               3, SCE_Box_GetPoints (box), SCE_FALSE);
    if (!SCE_Geometry_AddArrayDupDup (geom, &array, SCE_FALSE))
        goto fail;
    SCE_Geometry_InitArray (&array);
    SCE_Geometry_SetArrayIndices (&array, indices_lines, SCE_FALSE);
    if (!SCE_Geometry_SetIndexArrayDupDup (geom, &array, SCE_FALSE))
        goto fail;

    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}

static int SCE_BoxGeom_GenTriangles (SCE_SBox *box,
                                     SCE_EBoxGeomTexCoordMode mode,
                                     SCE_SGeometry *geom)
{
    SCEvertices *v = NULL, *t = NULL;
    SCEindices *i = NULL;
    int indiv = SCE_FALSE;

    switch (mode) {
    case SCE_BOX_EXTERIOR_TEXCOORD:
        SCE_Geometry_SetNumVertices (geom, 24);
        v = pos_indiv_triangle;
        t = texcoord_exterior_triangle;
        indiv = SCE_TRUE;
        break;
    case SCE_BOX_INTERIOR_TEXCOORD:
        SCE_Geometry_SetNumVertices (geom, 24);
        v = pos_indiv_triangle;
        t = texcoord_interior_triangle;
        indiv = SCE_TRUE;
        break;
    case SCE_BOX_CUBEMAP_TEXCOORD:
        t = texcoord_cubemap;
    case SCE_BOX_NONE_TEXCOORD:
        SCE_Geometry_SetNumVertices (geom, 8);
        SCE_Geometry_SetNumIndices (geom, 36);
        v = SCE_Box_GetPoints (box);
        i = indices_triangles;
    }
    {
        SCE_SGeometryArray array, *ap = NULL;
        SCE_Geometry_InitArray (&array);
        SCE_Geometry_SetArrayData (&array, SCE_POSITION, SCE_VERTICES_TYPE,
                                   0, 3, v, SCE_FALSE);
        ap = SCE_Geometry_AddArrayDupDup (geom, &array, SCE_FALSE);
        if (!ap)
            goto fail;
        if (indiv)
            SCE_BoxGeom_MulIndiv (SCE_Geometry_GetData (ap), box);
        if (t) {
            SCE_Geometry_InitArray (&array);
            SCE_Geometry_SetArrayData (&array, SCE_TEXCOORD0, SCE_VERTICES_TYPE,
                                       0, 3, t, SCE_FALSE);
            if (!SCE_Geometry_AddArrayDupDup (geom, &array, SCE_FALSE))
                goto fail;
        }
        if (i) {
            SCE_Geometry_InitArray (&array);
            SCE_Geometry_SetArrayIndices (&array, i, SCE_FALSE);
            if (!SCE_Geometry_SetIndexArrayDupDup (geom, &array, SCE_FALSE))
                goto fail;
        }
    }
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}

/**
 * \brief Generate a box into the given geometry
 * \param box the box model
 * \param prim the primitive type of the generated box
 * \param mode the generation mode (texture coordinate usage)
 * \param geom where store the geometry
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * \sa SCE_BoxGeom_Create()
 */
int SCE_BoxGeom_Generate (SCE_SBox *box, SCE_CPrimitiveType prim,
                          SCE_EBoxGeomTexCoordMode mode, SCE_SGeometry *geom)
{
    switch (prim) {
    case SCE_POINTS:
        SCE_Geometry_SetPrimitiveType (geom, SCE_POINTS);
        if (SCE_BoxGeom_GenPoints (box, mode, geom) < 0)
            goto fail;
        break;
    case SCE_LINES:
        SCE_Geometry_SetPrimitiveType (geom, SCE_LINES);
        if (SCE_BoxGeom_GenLines (box, mode, geom) < 0)
            goto fail;
        break;
    case SCE_TRIANGLES:
        SCE_Geometry_SetPrimitiveType (geom, SCE_TRIANGLES);
        if (SCE_BoxGeom_GenTriangles (box, mode, geom) < 0)
            goto fail;
    }
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}

/**
 * \brief Creates a geometry containing a box
 * \param box the box model
 * \param prim the primitive type of the generated box
 * \param mode the generation mode (texture coordinate usage)
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * \sa SCE_BoxGeom_Generate()
 */
SCE_SGeometry* SCE_BoxGeom_Create (SCE_SBox *box, SCE_CPrimitiveType prim,
                                   SCE_EBoxGeomTexCoordMode mode)
{
    SCE_SGeometry * geom = NULL;
    if (!(geom = SCE_Geometry_Create ()))
        SCEE_LogSrc ();
    else {
        if (SCE_BoxGeom_Generate (box, prim, mode, geom) < 0) {
            SCE_Geometry_Delete (geom), geom = NULL;
            SCEE_LogSrc ();
        }
    }
    return geom;
}
