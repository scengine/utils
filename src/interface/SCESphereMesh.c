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

/* created: 02/06/2009
   updated: 03/06/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCESphereMesh.h>

/**
 * \defgroup spheremesh Sphere Mesh
 * \ingroup interface
 * \internal
 * \brief 
 */

/** @{ */

/**
 * \brief Generates vertex positions and indices of a UV sphere
 * \param center,radius,segments,rings parameters that defines the sphere
 * to create. \p center can be NULL
 * \param pout vertices positions output (will allocate memory)
 * \param n_pos number of positions in \p pout
 * \param iout indices output (will allocate memory)
 * \sa SCE_SphereMesh_CreateUV(), SCE_SphereMesh_CreateDefaultUV()
 */
int
SCE_SphereMesh_GenerateUV (SCE_TVector3 center, float radius, SCEuint segments,
                           SCEuint rings, SCEvertices **pout, SCEuint *n_pos,
                           SCEindices **iout)
{
    unsigned int i, j;
    SCE_TVector3 c = {0.0, 0.0, 0.0};
    int n_indices;
    SCEvertices *p = NULL;
    SCEindices *indices = NULL, *index = NULL;
    float seg_offset = 2.0 * M_PI / (float)segments;
    float rin_offset = M_PI / (float)rings;

    *n_pos = (segments * rings + 2) * 3;
    if (!(p = SCE_malloc (*n_pos * sizeof *p)))
        goto failure;
    n_indices = segments * rings * 6;
    if (!(indices = SCE_malloc (n_indices * sizeof *indices)))
        goto failure;

    if (center)
        SCE_Vector3_Copy (c, center);

    SCE_Vector3_Set (&p[segments * rings * 3], 0.0, 0.0, radius);
    SCE_Vector3_Set (&p[segments * rings * 3 + 3], 0.0, 0.0, -radius);

    for (i = 0; i < rings; i++)
    {
        SCE_TVector3 v1;
        float angle1 = rin_offset * i;
        v1[2] = cos (angle1) * radius;
        v1[0] = v1[1] = sin (angle1) * radius;
        for (j = 0; j < segments; j++)
        {
            SCE_TVector3 v2;
            float angle2 = seg_offset * j;
            v2[0] = cos (angle2);
            v2[1] = sin (angle2);
            v2[2] = 1.0;
            SCE_Vector3_Operator1v (v2, *=, v1);
            SCE_Vector3_Copy (&p[(i * segments + j) * 3], v2);
        }
    }

    index = indices;
    /* middle */
    for (i = 1; i < rings; i++)
    {
        for (j = 0; j < segments; j++)
        {
            unsigned int j1 = (j == 0 ? segments - 1 : j - 1);
            /* make two triangles */
            index[0] = i * segments + j;
            index[1] = (i - 1) * segments + j1;
            index[2] = i * segments + j1;

            index[3] = i * segments + j;
            index[4] = (i - 1) * segments + j;
            index[5] = (i - 1) * segments + j1;
            index = &index[6];
        }
    }
    /* top/middle */
    for (i = 0; i < segments; i++)
    {
        unsigned int i1 = (i == 0 ? segments - 1 : i - 1);
        /* upper triangle */
        index[0] = segments * rings;
        index[2] = i;
        index[1] = i1;
        /* lower triangle */
        index[3] = segments * rings + 1;
        index[4] = segments * (rings - 1) + i;
        index[5] = segments * (rings - 1) + i1;
        index = &index[6];
    }

    *pout = p;
    *iout = indices;    
    SCE_btend ();
    return n_indices;
failure:
    SCE_free (p);
    SCE_free (indices);
    Logger_LogSrc ();
    SCE_btend ();
    return SCE_ERROR;
}

/**
 * \brief Creates an UV sphere mesh
 * \param center,radius,segments,rings parameters that defines the sphere
 * to create. \p center can be NULL
 * \sa SCE_SphereMesh_CreateDefaultUV(), SCE_SphereMesh_GenerateUV()
 */
SCE_SMesh* SCE_SphereMesh_CreateUV (SCE_TVector3 center, float radius,
                                    SCEuint segments, SCEuint rings)
{
    SCE_SMesh *mesh = NULL;
    SCEvertices *pos = NULL;
    SCEindices *indices = NULL;
    int n_indices;
    SCEuint n_pos;

    SCE_btstart ();
    n_indices = SCE_SphereMesh_GenerateUV (center, radius, segments, rings,
                                            &pos, &n_pos, &indices);
    if (n_indices < 0)
        goto failure;
    if (!(mesh = SCE_Mesh_Create ()))
        goto failure;
    if (SCE_Mesh_AddVertices (mesh, 0, SCE_POSITION, SCE_VERTICES_TYPE, 3,
                              n_pos, pos, SCE_TRUE) < 0)
        goto failure;
    if (SCE_Mesh_SetIndices (mesh, 0, SCE_INDICES_TYPE, n_indices, indices,
                             SCE_TRUE) < 0)
        goto failure;
    SCE_Mesh_SetRenderMode (mesh, SCE_TRIANGLES);
    SCE_Mesh_Build (mesh);
    SCE_btend ();
    return mesh;
failure:
    SCE_Mesh_Delete (mesh);
    Logger_LogSrc ();
    SCE_btend ();
    return NULL;
}

/**
 * \brief Creates an UV sphere
 * \param center,radius parameters that defines the sphere to create.
 * \p center can be NULL
 *
 * The segments and the rings of the created sphere are SCE_SPHEREMESH_SEGMENTS
 * and SCE_SPHEREMESH_RINGS
 * \sa SCE_SphereMesh_CreateUV(), SCE_SphereMesh_GenerateUV()
 */
SCE_SMesh* SCE_SphereMesh_CreateDefaultUV (SCE_TVector3 center, float radius)
{
    return SCE_SphereMesh_CreateUV (center, radius, SCE_SPHEREMESH_SEGMENTS,
                                    SCE_SPHEREMESH_RINGS);
}

/** @} */
