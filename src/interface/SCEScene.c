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
 
/* created: 19/01/2008
   updated: 31/01/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEListFastForeach.h>
#include <SCE/utils/SCEVector.h>
#include <SCE/core/SCECore.h>

#include <SCE/interface/SCEScene.h>
#include <SCE/interface/SCEBoundingBox.h>
#include <SCE/interface/SCEBatch.h>


/**
 * \file SCEScene.c
 * \copydoc scene
 * 
 * \file SCEScene.h
 * \copydoc scene
 */

/**
 * \defgroup scene Scene managment
 * \ingroup interface
 * \internal
 */

/** @{ */

static SCE_SCamera *default_camera = NULL;

/** \internal */
int SCE_Init_Scene (void)
{
    if (!(default_camera = SCE_Camera_Create ()))
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    return SCE_OK;
}
/** \internal */
void SCE_Quit_Scene (void)
{
    SCE_Camera_Delete (default_camera), default_camera = NULL;
}


static void SCE_Scene_InitStates (SCE_SSceneStates *states)
{
    states->clearcolor = states->cleardepth = SCE_TRUE;
    states->frustum_culling = SCE_FALSE;
    states->lighting = SCE_TRUE;
    states->lod = SCE_FALSE;
}

static void SCE_Scene_Init (SCE_SScene *scene)
{
    unsigned int i;
    scene->rootnode = NULL;
    scene->node_updated = SCE_FALSE;
    scene->n_nodes = 0;
    scene->octree = NULL;
    for (i = 0; i < SCE_SCENE_NUM_RESOURCE_GROUP; i++)
        scene->rgroups[i] = NULL;
    scene->egroups = NULL;
    scene->entities = NULL;
    scene->lights = NULL;
    scene->skybox = NULL;
    scene->rclear = scene->gclear = scene->bclear = scene->aclear = 0.5;
    scene->dclear = 1.0;
    scene->rendertarget = NULL;
    scene->cubeface = 0;
    scene->camera = NULL;
    SCE_Scene_InitStates (&scene->states);
}

/**
 * \brief Creates a new scene
 * \returns a newly allocated scene or NULL on error
 */
SCE_SScene* SCE_Scene_Create (void)
{
    unsigned int i;
    SCE_SScene *scene = NULL;

    SCE_btstart ();
    if (!(scene = SCE_malloc (sizeof *scene)))
        goto failure;
    SCE_Scene_Init (scene);
    if (!(scene->rootnode = SCE_Node_Create ()))
        goto failure;
    if (!(scene->octree = SCE_Octree_Create ()))
        goto failure;
    for (i = 0; i < SCE_SCENE_NUM_RESOURCE_GROUP; i++)
    {
        if (!(scene->rgroups[i] = SCE_SceneResource_CreateGroup ()))
            goto failure;
        SCE_SceneResource_SetGroupType (scene->rgroups[i], i);
    }
    if (!(scene->egroups = SCE_List_Create (NULL)))
        goto failure;
    if (!(scene->entities = SCE_List_Create (NULL)))
        goto failure;
    if (!(scene->lights = SCE_List_Create (NULL)))
        goto failure;
    SCE_Octree_SetSize (scene->octree, SCE_SCENE_OCTREE_SIZE,
                        SCE_SCENE_OCTREE_SIZE, SCE_SCENE_OCTREE_SIZE);
    goto success;

failure:
    SCE_Scene_Delete (scene), scene = NULL;
    Logger_LogSrc ();
success:
    SCE_btend ();
    return scene;
}

/**
 * \brief Deletes a scene
 * \param scene the scene you want to delete
 */
void SCE_Scene_Delete (SCE_SScene *scene)
{
    if (scene)
    {
        unsigned int i;
        SCE_List_Delete (scene->lights);
        SCE_List_Delete (scene->entities);
        SCE_List_Delete (scene->egroups);
        for (i = 0; i < SCE_SCENE_NUM_RESOURCE_GROUP; i++)
            SCE_SceneResource_DeleteGroup (scene->rgroups[i]);
        SCE_Octree_DeleteRecursive (scene->octree);
        SCE_Node_Delete (scene->rootnode);
        SCE_free (scene);
    }
}


/**
 * \brief Gets the root node of a scene
 * \param scene a scene
 * \returns the scene's root node
 */
SCE_SNode* SCE_Scene_GetRootNode (SCE_SScene *scene)
{
    return scene->rootnode;
}


void SCE_Scene_OnNodeMoved (SCE_SNode *node, void *param)
{
    /* TODO: tmp */
    SCE_SBoundingSphere *bs = SCE_Octree_GetElementBoundingSphere (
        ((SCE_SSceneEntityInstance*)SCE_Node_GetData (node))->element);
    SCE_BoundingSphere_Push (bs, SCE_Node_GetFinalMatrix (node));
    /* the data of a node is its instance */
    SCE_Octree_ReinsertElement (((SCE_SSceneEntityInstance*)
                                 SCE_Node_GetData (node))->element);
    /*SCE_Octree_InsertElement (((SCE_SScene*)param)->octree, ((SCE_SSceneEntityInstance*)
      SCE_Node_GetData (node))->element, 0);*/
    SCE_BoundingSphere_Pop (bs);
}


/**
 * \brief Adds a node to a scene
 * \param scene a scene
 * \param node the nod you want to attach to \p scene
 * 
 * This function attach the given note to the root node of the given scene. It
 * is equivalent to SCE_Node_Attach(SCE_Scene_GetRootNode(\p scene), \p node)
 * \see SCE_Node_Attach()
 */
void SCE_Scene_AddNode (SCE_SScene *scene, SCE_SNode *node)
{
    SCE_Node_Attach (scene->rootnode, node);
    scene->n_nodes++;
}
/**
 * \brief Removes a node from a scene
 * \param node the node to remove
 * \warning the node \p node HAVE to be previously added to the scene \p scene,
 * an undefined comportement can happen otherwise.
 */
void SCE_Scene_RemoveNode (SCE_SScene *scene, SCE_SNode *node)
{
    scene->n_nodes--;
    SCE_Node_Detach (node);
}

/**
 * \brief Adds an instance to a scene
 * \param einst the instance to add
 * \returns SCE_ERROR if the insertion of the octree element has failed,
 * SCE_OK otherwise
 *
 * Adds an instance to a scene, adds its node by calling SCE_Scene_AddNode()
 * and inserts its octree element by calling SCE_Octree_InsertElement().
 * \sa SCE_Scene_RemoveInstance(), SCE_Scene_AddNode(), SCE_Scene_MakeOctree()
 */
int SCE_Scene_AddInstance (SCE_SScene *scene, SCE_SSceneEntityInstance *einst)
{
    SCE_SNode *node = SCE_SceneEntity_GetInstanceNode (einst);
    SCE_SOctreeElement *element=SCE_SceneEntity_GetInstanceOctreeElement(einst);
    SCE_SBoundingSphere *bs = SCE_Octree_GetElementBoundingSphere (element);
    /* NOTE: not relative to the (future) parent */
    SCE_BoundingSphere_Push (bs, SCE_Node_GetFinalMatrix (node));
    if (SCE_Octree_InsertElement (scene->octree, element, SCE_FALSE) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    SCE_BoundingSphere_Pop (bs);
    SCE_Node_SetOnMovedCallback (node, SCE_Scene_OnNodeMoved, scene);
    SCE_Scene_AddNode (scene, node);
    return SCE_OK;
}
/**
 * \brief Removes an instance from a scene
 * \param einst the instance to remove
 * \sa SCE_Scene_AddInstance()
 */
void SCE_Scene_RemoveInstance (SCE_SScene *scene,
                               SCE_SSceneEntityInstance *einst)
{
    SCE_SNode *node = SCE_SceneEntity_GetInstanceNode (einst);
    SCE_SOctreeElement *element=SCE_SceneEntity_GetInstanceOctreeElement(einst);
    SCE_Scene_RemoveNode (scene, node);
    SCE_Octree_RemoveElement (element);
}

/**
 * \brief Adds an entity to a scene
 * \param scene a scene
 * \param entity the entity to add
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_Scene_AddEntity (SCE_SScene *scene, SCE_SSceneEntity *entity)
{
    if (SCE_List_PrependNewl (scene->entities, entity) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    return SCE_OK;
}
/**
 * \brief Removes an entity from a scene
 * \param entity the entity to remove
 */
void SCE_Scene_RemoveEntity (SCE_SScene *scene, SCE_SSceneEntity *entity)
{
    SCE_List_EraseFromData (scene->entities, entity);
}

/**
 * \brief Adds an entity group to a scene
 * \param scene a scene
 * \param group the entity group to add
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_Scene_AddEntityGroup (SCE_SScene *scene, SCE_SSceneEntityGroup *group)
{
    SCE_SListIterator *it = NULL;
    SCE_SList *l = SCE_SceneEntity_GetGroupEntitiesList (group);

    if (SCE_List_PrependNewl (scene->egroups, group) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    SCE_List_ForEach (it, l)
    {
        if (SCE_Scene_AddEntity (scene, SCE_List_GetData (it)) < 0)
        {
            Logger_LogSrc ();
            return SCE_ERROR;
        }
    }

    return SCE_OK;
}

/**
 * \brief Adds a light to a scene
 * \param scene a scene
 * \param light a light to add to the given scene
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_Scene_AddLight (SCE_SScene *scene, SCE_SLight *light)
{
    if (SCE_List_PrependNewl (scene->lights, light) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    return SCE_OK;
}

/**
 * \brief Adds a resource to a scene
 */
void SCE_Scene_AddResource (SCE_SScene *scene, int id, SCE_SSceneResource *res)
{
    SCE_SceneResource_AddResource (scene->rgroups[id], res);
}

/**
 * \brief Defines the skybox of a scene
 */
void SCE_Scene_SetSkybox (SCE_SScene *scene, SCE_SSkybox *skybox)
{
    if (scene->skybox)
        SCE_Scene_RemoveEntity (scene, SCE_Skybox_GetEntity (scene->skybox));
    scene->skybox = skybox;
    if (skybox)
        SCE_Scene_AddEntity (scene, SCE_Skybox_GetEntity (skybox));
}


/**
 * \deprecated
 * \brief Calls a function for each entity group of a scene
 * \param scene a scene
 * \param f the function to call for each group
 * \param param user data to pass as second argument of \p f
 * \see SCE_FSceneForEachEntityGroupFunc
 */
void SCE_Scene_ForEachEntityGroup (SCE_SScene *scene,
                                   SCE_FSceneForEachEntityGroupFunc f,
                                   void *param)
{
    SCE_SListIterator *it, *pro;
    SCE_List_ForEachProtected (pro, it, scene->egroups)
    {
        if (!f (SCE_List_GetData (it), param))
            break;
    }
}


/**
 * \brief Defines the size of the octree of a scene
 * \param w,h,d the new dimensions of the octree
 * \sa SCE_Octree_SetSize()
 */
void SCE_Scene_SetOctreeSize (SCE_SScene *scene, float w, float h, float d)
{
    SCE_Octree_SetSize (scene->octree, w, h, d);
}
/**
 * \brief Makes an octree for the given scene
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * Makes an octree for the given scene and clears the previous one.
 * \sa SCE_Octree_RecursiveMake(), SCE_Octree_Clear()
 */
int SCE_Scene_MakeOctree (SCE_SScene *scene, unsigned int rec,
                          int loose, float margin)
{
    SCE_Octree_Clear (scene->octree);
    if (SCE_Octree_RecursiveMake (scene->octree, rec, NULL,
                                  NULL, loose, margin) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    return SCE_OK;
}

int SCE_Scene_SetupBatching (SCE_SScene *scene, unsigned int n, int *order)
{
    if (SCE_Batch_SortEntities (scene->entities, SCE_SCENE_NUM_RESOURCE_GROUP,
                                scene->rgroups, 2, order) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    return SCE_OK;
}

int SCE_Scene_SetupDefaultBatching (SCE_SScene *scene)
{
    int order[] = {SCE_SCENE_SHADERS_GROUP, SCE_SCENE_TEXTURES0_GROUP};
    if (SCE_Batch_SortEntities (scene->entities, SCE_SCENE_NUM_RESOURCE_GROUP,
                                scene->rgroups, 2, order) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    return SCE_OK;
}


/**
 * \brief Defines the beginning of a frame
 * \deprecated
 */
void SCE_Scene_BeginFrame (SCE_SScene *scene)
{
    scene->node_updated = SCE_FALSE;
}

/**
 * \brief Defines the end of a frame
 * \deprecated
 */
void SCE_Scene_EndFrame (SCE_SScene *scene)
{
    /* what to do? maybe call glFlush()? */
    (void) scene;
}


static void SCE_Scene_DoFrustumCulling (SCE_SScene *scene)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, scene->egroups)
    {
        SCE_SceneEntity_SelectInFrustumInstances (SCE_List_GetData (it),
                                                  scene->camera);
    }
}

static void SCE_Scene_ComputeLODs (SCE_SScene *scene)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, scene->egroups)
        SCE_SceneEntity_DetermineLODs (SCE_List_GetData (it), scene->camera);
}

/**
 * \brief Prepares a scene for rendering
 * \param scene the scene to prepare
 * \param camera the camera used for the render, can be NULL then a default
 *        camera (the same one that SCE_Camera_Create() returns) is used
 * \param rendertarget the render target
 * \param cubeface if the render target is a cubemap, it is used to determine
 *        which face is the render target. Otherwise, if the render target is
 *        not a cubmap, this parameter is useless.
 *        This can be one of SCE_TEX_POSX, SCE_TEX_NEGX, SCE_TEX_POSY,
 *        SCE_TEX_NEGY, SCE_TEX_POSZ or SCE_TEX_NEGZ.
 * 
 * This function prepares the given scene's active renderer for render. It
 * assignes a camera and a render target to the active scene's renderer.
 * 
 * \note cubeface is saved even if the render target is not a cubemap.
 * \sa SCE_Scene_Render(), SCE_Texture_RenderTo()
 */
void SCE_Scene_Update (SCE_SScene *scene, SCE_SCamera *camera,
                       SCE_STexture *rendertarget, SCEuint cubeface)
{
    /* assign */
    scene->rendertarget = rendertarget;
    scene->cubeface = cubeface;
    scene->camera = (camera ? camera : default_camera);

    /* updating scene nodes */
    if (!scene->node_updated)
        SCE_Node_FastUpdateRootRecursive (scene->rootnode, scene->n_nodes);
    scene->node_updated = SCE_TRUE;

    SCE_Camera_Update (scene->camera);

    SCE_Octree_MarkVisibles (scene->octree, scene->camera);

    /* frustum culling computation */
    if (scene->states.frustum_culling)
        SCE_Scene_DoFrustumCulling (scene);

    /* determinating LODs */
    if (scene->states.lod)
        SCE_Scene_ComputeLODs (scene);
}


/**
 * \internal
 * \brief Clears the buffers before the rendering
 */
void SCE_Scene_ClearBuffers (SCE_SScene *scene)
{
    SCEbitfield depthbuffer = 0;

    if (scene->states.cleardepth)
        depthbuffer = GL_DEPTH_BUFFER_BIT;
    if (scene->states.clearcolor)
        depthbuffer |= GL_COLOR_BUFFER_BIT;

    SCE_CClearColor (scene->rclear, scene->gclear, scene->bclear,scene->aclear);
    SCE_CClearDepth (scene->dclear);
    /* what does glClear(0)? */
    if (depthbuffer)
        SCE_CClear (depthbuffer);
}


static void SCE_Scene_RenderEntities (SCE_SScene *scene)
{
    SCE_SSceneEntity *entity = NULL;
    SCE_SListIterator *it;
    SCE_List_ForEach (it, scene->entities)
    {
        entity = SCE_List_GetData (it);
        if (SCE_SceneEntity_HaveInstance (entity))
        {
            SCE_SceneEntity_UseResources (entity);
            SCE_SceneEntity_Render (entity);
        }
    }
    SCE_Texture_Flush ();
    SCE_Material_Use (NULL);
    SCE_Shader_Use (NULL);
}


static void SCE_Scene_RenderSkybox (SCE_SScene *scene, SCE_SCamera *cam)
{
    SCE_TVector3 pos;
    float *matcam = NULL;
    SCE_SSceneEntityInstance *einst = SCE_Skybox_GetInstance (scene->skybox);
    SCE_SNode *node = SCE_SceneEntity_GetInstanceNode (einst);

#if 1
    matcam = SCE_Node_GetFinalMatrix (SCE_Camera_GetNode (cam));
    SCE_Matrix4_GetTranslation (matcam, pos);
    matcam = SCE_Node_GetMatrix (node);
    matcam[3]  = pos[0];
    matcam[7]  = pos[1];
    matcam[11] = pos[2];
    SCE_Node_HasMoved (node);
    SCE_Node_UpdateRootRecursive (node);
#else
    SCE_Node_Attach (SCE_Camera_GetNode (cam), node);
    SCE_Node_HasMoved (node);
    SCE_Node_UpdateRootRecursive (SCE_Camera_GetNode (cam));
    SCE_Node_Detach (node);
#endif
    SCE_CSetState2 (GL_DEPTH_TEST, GL_CULL_FACE, SCE_FALSE);/*TODO: kick that */
    glDisable (GL_LIGHTING);
    SCE_SceneEntity_UseResources (SCE_Skybox_GetEntity (scene->skybox));
    /*glEnable (GL_LIGHTING);
    glDisable (GL_LIGHT0);
    glDisable (GL_LIGHT1);
    glDisable (GL_LIGHT2);*/
    glDisable (GL_LIGHTING);
    glFlush ();
    glDisable (GL_LIGHTING);
    SCE_SceneEntity_Render (SCE_Skybox_GetEntity (scene->skybox));
    SCE_CSetState2 (GL_DEPTH_TEST, GL_CULL_FACE, SCE_TRUE);
    SCE_Texture_Flush ();
    SCE_Shader_Use (NULL);
}

/**
 * \brief Renders a scene into a render target
 * \param scene a scene
 * \param cam the camera used for the render or NULL to keep the current one.
 * \param rendertarget the render target or NULL to keep the current one. If
 *        both this parameter and the current render targed are NULL, the
 *        default OpenGL's render buffer will be used as the render target.
 * \param cubeface see SCE_Scene_Update()'s cubeface parameter.
 * \see SCE_Scene_Update()
 */
void SCE_Scene_Render (SCE_SScene *scene, SCE_SCamera *cam,
                       SCE_STexture *rendertarget, int cubeface)
{
    SCE_SListIterator *it = NULL;

    if (!cam)
        cam = scene->camera;
    if (!rendertarget)
        rendertarget = scene->rendertarget;
    if (cubeface < 0)
        cubeface = scene->cubeface;

    /* mise en place du render target */
    SCE_Texture_RenderTo (rendertarget, cubeface);

    /* preparation des tampons */
    if (scene->skybox)
        scene->states.clearcolor = SCE_FALSE;
    SCE_Scene_ClearBuffers (scene);

    /* activation de la camera et mise en place des matrices */
    SCE_CSetActiveMatrix (SCE_MAT_MODELVIEW);
    SCE_CPushMatrix ();
    SCE_Camera_Use (cam);

    /* render skybox (if any) */
    if (scene->skybox)
    {
        SCE_Light_ActivateLighting (SCE_FALSE);
        SCE_Scene_RenderSkybox (scene, cam);
    }

    if (!scene->states.lighting)
        SCE_Light_ActivateLighting (SCE_FALSE);
    else
    {
        SCE_Light_ActivateLighting (SCE_TRUE);
        /* assignation de la matrice camera au gestionnaire de lumieres */
        SCE_Light_SetCameraMatrix (SCE_Camera_GetFinalView (cam));

        /* TODO: activation de toutes les lumières (bourrin & temporaire) */
        SCE_List_ForEach (it, scene->lights)
            SCE_Light_Use (SCE_List_GetData (it));
    }

    SCE_Scene_RenderEntities (scene);

    /* restauration des parametres par defaut */
    SCE_Light_Use (NULL);
    SCE_Light_ActivateLighting (SCE_FALSE);

    SCE_CSetActiveMatrix (SCE_MAT_MODELVIEW);
    SCE_CPopMatrix ();

    if (rendertarget)
        SCE_Texture_RenderTo (NULL, 0);

    SCE_CFlush ();
}

/** @} */
