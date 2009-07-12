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

/* created: 17/02/2009
   updated: 18/02/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCETexture.h>
#include <SCE/interface/SCESkybox.h>

/**
 * \defgroup skybox Skybox
 * \ingroup interface
 * \internal
 * \brief 
 */

/** @{ */

static void SCE_Skybox_Init (SCE_SSkybox *skybox)
{
    skybox->mesh = NULL;
    skybox->tex = NULL;
    skybox->mode = SCE_FALSE;
    skybox->textype = 0;        /* NOTE: what if SCE_TEX_* is 0 ? */
    skybox->shader = NULL;
#if 0
    skybox->group = NULL;
#endif
    skybox->entity = NULL;
    skybox->instance = NULL;
}

/**
 * \brief Creates a skybox
 */
SCE_SSkybox* SCE_Skybox_Create (void)
{
    SCE_SSceneEntityProperties *props = NULL;
    SCE_SSkybox *skybox = NULL;
    SCE_btstart ();
    if (!(skybox = SCE_malloc (sizeof *skybox)))
        goto failure;
    SCE_Skybox_Init (skybox);
    if (!(skybox->mesh = SCE_Mesh_Create ()))
        goto failure;
#if 0
    if (!(skybox->group = SCE_SceneEntity_CreateGroup ()))
        goto failure;
#endif
    if (!(skybox->entity = SCE_SceneEntity_Create ()))
        goto failure;
    if (!(skybox->instance = SCE_SceneEntity_CreateInstance ()))
        goto failure;
#if 0
    SCE_SceneEntity_AddEntity (skybox->group, skybox->entity);
    SCE_SceneEntity_AddInstance (skybox->group, skybox->instance);
#else
    SCE_SceneEntity_AddInstanceToEntity (skybox->entity, skybox->instance);
#endif
    props = SCE_SceneEntity_GetProperties (skybox->entity);
    props->cullface = SCE_FALSE;
    props->depthtest = SCE_FALSE;
    props->alphatest = SCE_FALSE;
    goto success;
failure:
    SCE_Skybox_Delete (skybox), skybox = NULL;
    SCEE_LogSrc ();
success:
    SCE_btend ();
    return skybox;
}
/**
 * \brief Deletes a skybox
 */
void SCE_Skybox_Delete (SCE_SSkybox *skybox)
{
    if (skybox)
    {
        SCE_SceneEntity_DeleteInstance (skybox->instance);
        SCE_SceneEntity_Delete (skybox->entity);
        SCE_Mesh_Delete (skybox->mesh);
        SCE_free (skybox);
    }
}


/**
 * \brief Defines the size of a bounding box
 * \param skybox a skybox
 * \param size the new size of the bounding box
 */
void SCE_Skybox_SetSize (SCE_SSkybox *skybox, float size)
{
    SCE_SNode *node = SCE_SceneEntity_GetInstanceNode (skybox->instance);
    SCE_Matrix4_Scale (SCE_Node_GetMatrix (node), size, size, size);
    SCE_Node_HasMoved (node);   /* never know... */
}
/**
 * \brief Defines the texture of a skybox
 * \param skybox a skybox
 * \param tex the texture to set
 * \param mode the mode for using the 2D texture
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * This function modify the texture coordinates of the mesh of \p skybox
 * according to the type of \p tex. If the texture \p tex is a cubemap
 * (SCE_TEX_CUBE) then indices are set and vertices not duplicated, if the type
 * is a 2D texture (SCE_TEX_2D) then the same texture is used on each face of
 * the skybox, unless you are using the parameter \p mode to specify that
 * the 2D texture contains all the six images of the faces. \p mode can be
 * SCE_TRUE or SCE_FALSE. If it is SCE_TRUE the texture \p tex has all the
 * images ordered in an array W = 3 and H = 2. The order is POSX, NEGX, POSY,
 * etc.
 * \sa SCE_Skybox_SetShader(), SCE_SceneEntity_AddTexture()
 * \warning \p mode not yet supported
 * \todo badly documented
 */
int SCE_Skybox_SetTexture (SCE_SSkybox *skybox, SCE_STexture *tex, int mode)
{
    int code = SCE_OK, type;
    SCEvertices v[72];
    SCE_TVector3 origin = {-0.5, -0.5, -0.5};

    SCE_btstart ();
    /* remove the previous one */
    if (skybox->tex)
        SCE_SceneEntity_RemoveTexture (skybox->entity, skybox->tex);
    SCE_SceneEntity_AddTexture (skybox->entity, tex);
    skybox->tex = tex;

    type = SCE_Texture_GetType (tex);
    if (type == SCE_TEX_CUBE || type == SCE_RENDER_COLOR_CUBE ||
        type == SCE_RENDER_DEPTH_CUBE)
    {
        unsigned int i;
        SCEindices *indices;
        if (type == skybox->textype)
            goto success;
        SCE_Mesh_GenerateIndexedCubeVertices (v, origin, 1.0, 1.0, 1.0);
        indices = (SCEindices*)SCE_Mesh_GetIndexedCubeIndices ();
        if (SCE_Mesh_AddVerticesDup (skybox->mesh, 0, SCE_POSITION,
                                     SCE_VERTICES_TYPE, 3, 8, v) < 0)
            goto failure;
        /* normalize the vectors */
        for (i = 0; i < 8; i++)
            SCE_Vector3_Normalize (&v[i*3]);
        if (SCE_Mesh_AddVerticesDup (skybox->mesh, 0, SCE_TEXCOORD0,
                                     SCE_VERTICES_TYPE, 3, 8, v) < 0)
        if (SCE_Mesh_SetIndicesDup (skybox->mesh, 0, SCE_INDICES_TYPE,
                                    24, indices) < 0)
            goto failure;
        SCE_Mesh_ActivateIB (skybox->mesh, SCE_TRUE);
    }
    else if (mode)
    {
        SCEvertices texcoord[] =
        {
            /* lol */
            0., 0.,  1., 0.,  1., 1.,  0., 1.,
            0., 0.,  1., 0.,  1., 1.,  0., 1.,
            0., 0.,  1., 0.,  1., 1.,  0., 1.,
            0., 0.,  1., 0.,  1., 1.,  0., 1.,

            0., 0.,  1., 0.,  1., 1.,  0., 1.,
            0., 0.,  1., 0.,  1., 1.,  0., 1.
        };
        SCE_Mesh_GenerateCubeVertices (v, origin, 1.0, 1.0, 1.0);
        if (SCE_Mesh_AddVerticesDup (skybox->mesh, 0, SCE_POSITION,
                                     SCE_VERTICES_TYPE, 3, 24, v) < 0)
            goto failure;
        if (SCE_Mesh_AddVerticesDup (skybox->mesh, 0, SCE_TEXCOORD0,
                                     SCE_VERTICES_TYPE, 2, 24, texcoord) < 0)
            goto failure;
        SCE_Mesh_ActivateIB (skybox->mesh, SCE_FALSE);
    }
    else
    {
        SCEvertices texcoord[] =
        {
            0., 0.,  1., 0.,  1., 1.,  0., 1.,
            0., 0.,  1., 0.,  1., 1.,  0., 1.,
            0., 0.,  1., 0.,  1., 1.,  0., 1.,
            0., 0.,  1., 0.,  1., 1.,  0., 1.,
            0., 0.,  1., 0.,  1., 1.,  0., 1.,
            0., 0.,  1., 0.,  1., 1.,  0., 1.
        };
        SCE_Mesh_GenerateCubeVertices (v, origin, 1.0, 1.0, 1.0);
        if (SCE_Mesh_AddVerticesDup (skybox->mesh, 0, SCE_POSITION,
                                     SCE_VERTICES_TYPE, 3, 24, v) < 0)
            goto failure;
        if (SCE_Mesh_AddVerticesDup (skybox->mesh, 0, SCE_TEXCOORD0,
                                     SCE_VERTICES_TYPE, 2, 24, texcoord) < 0)
            goto failure;
        SCE_Mesh_ActivateIB (skybox->mesh, SCE_FALSE);
    }
    SCE_Mesh_SetRenderMode (skybox->mesh, SCE_QUADS);
    if (SCE_Mesh_Build (skybox->mesh) < 0)
        goto failure;
    SCE_SceneEntity_SetMesh (skybox->entity, skybox->mesh);
    skybox->mode = mode;
    skybox->textype = type;
    goto success;
failure:
    SCEE_LogSrc ();
    code = SCE_ERROR;
success:
    SCE_btend ();
    return code;
}
/**
 * \brief Defines the shader of a skybox
 * \param skybox a skybox
 * \param shader the shader to set
 */
void SCE_Skybox_SetShader (SCE_SSkybox *skybox, SCE_SShader *shader)
{
    SCE_SceneEntity_SetShader (skybox->entity, shader);
    skybox->shader = shader;
}

#if 0
/**
 * \brief Gets the scene entity group of a skybox
 * \sa SCE_Skybox_GetEntity(), SCE_SSceneEntityGroup
 */
SCE_SSceneEntityGroup* SCE_Skybox_GetEntityGroup (SCE_SSkybox *skybox)
{
    return skybox->group;
}
#endif
/**
 * \brief Gets the scene entity of a skybox
 * \sa SCE_Skybox_GetEntityGroup(), SCE_SSceneEntity
 */
SCE_SSceneEntity* SCE_Skybox_GetEntity (SCE_SSkybox *skybox)
{
    return skybox->entity;
}
/**
 * \brief Gets the entity instance of a skybox
 * \sa SCE_SSkybox, SCE_SSceneEntityInstance
 */
SCE_SSceneEntityInstance* SCE_Skybox_GetInstance (SCE_SSkybox *skybox)
{
    return skybox->instance;
}

/** @} */
