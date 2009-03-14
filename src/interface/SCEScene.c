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
   updated: 10/03/2009 */

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

/* dat iz omg coef u no??? */
static float omg_coeffs[2] = {0.1, 0.01};

typedef struct sce_ssceneoctree SCE_SSceneOctree;
struct sce_ssceneoctree
{
    SCE_SList *instances[3];       /* pkeu 3 laiveuls ser coul */
    SCE_SList *lights;
    SCE_SList *cameras;
};

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


static void SCE_Scene_InitOctree (SCE_SSceneOctree *tree)
{
    unsigned int i;
    for (i = 0; i < 3; i++)
        tree->instances[i] = NULL;
    tree->lights = NULL;
    tree->cameras = NULL;
}

static void SCE_Scene_DeleteOctree (SCE_SSceneOctree*);

static SCE_SSceneOctree* SCE_Scene_CreateOctree (void)
{
    unsigned int i;
    SCE_SSceneOctree *tree = NULL;
    tree = SCE_malloc (sizeof *tree);
    if (!tree)
        goto failure;
    SCE_Scene_InitOctree (tree);
    for (i = 0; i < 3; i++)
    {
        if (!(tree->instances[i] = SCE_List_Create (NULL)))
            goto failure;
    }
    if (!(tree->lights = SCE_List_Create (NULL)))
        goto failure;
    if (!(tree->cameras = SCE_List_Create (NULL)))
        goto failure;
    goto success;
failure:
    SCE_Scene_DeleteOctree (tree), tree = NULL;
    Logger_LogSrc ();
success:
    return tree;
}

static void SCE_Scene_DeleteOctree (SCE_SSceneOctree *tree)
{
    if (tree)
    {
        unsigned int i;
        for (i = 0; i < 3; i++)
            SCE_List_Delete (tree->instances[i]);
        SCE_List_Delete (tree->lights);
        SCE_List_Delete (tree->cameras);
    }
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
    scene->instances = NULL;
    scene->selected = NULL;
    scene->selected_join = NULL;
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
    if (!(scene->instances = SCE_List_Create (NULL)))
        goto failure;
    if (!(scene->selected = SCE_List_Create (NULL)))
        goto failure;
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


/* called when a node has moved */
void SCE_Scene_OnNodeMoved (SCE_SNode *node, void *param)
{
    /* TODO: tmp */
    SCE_SOctreeElement *el = NULL;
    SCE_SSceneEntityInstance *einst = NULL;
    SCE_SBoundingSphere *bs = NULL;

    einst = SCE_Node_GetData (node);
    el = SCE_Node_GetElement (node);
    bs = el->sphere;

    SCE_BoundingSphere_Push (bs, SCE_Node_GetFinalMatrix (node));
    SCE_Octree_ReinsertElement (el);
    SCE_BoundingSphere_Pop (bs);
}


static unsigned int SCE_Scene_DetermineElementList (SCE_SOctreeElement *el,
                                                    SCE_SOctree *tree)
{
    float d = SCE_BoundingSphere_GetRadius (el->sphere) * 2.0;
    float c = d / SCE_BoundingBox_GetWidth (SCE_Octree_GetBoundingBox (tree));
    if (c < omg_coeffs[1])
        return 2;
    else if (c < omg_coeffs[0])
        return 1;
    return 0;
}
/* inserts an instance into an octree */
static void SCE_Scene_InsertInstance (SCE_SOctree *tree, SCE_SOctreeElement *el)
{
    unsigned int id;
    SCE_SSceneOctree *stree = NULL;
    stree = SCE_Octree_GetData (tree);
    id = SCE_Scene_DetermineElementList (el, tree);
    SCE_List_Prependl (stree->instances[id], &el->it);
}
/* inserts a light into an octree */
static void SCE_Scene_InsertLight (SCE_SOctree *tree, SCE_SOctreeElement *el)
{
    SCE_SSceneOctree *stree = NULL;
    stree = SCE_Octree_GetData (tree);
    SCE_List_Prependl (stree->lights, &el->it);
}
/* inserts a camera into an octree */
static void SCE_Scene_InsertCamera (SCE_SOctree *tree, SCE_SOctreeElement *el)
{
    SCE_SSceneOctree *stree = NULL;
    stree = SCE_Octree_GetData (tree);
    SCE_List_Prependl (stree->cameras, &el->it);
}

/**
 * \brief Adds a node to a scene
 * \param scene a scene
 * \param node the node you want to attach to \p scene
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
 *
 * Adds an instance to a scene, adds its node by calling SCE_Scene_AddNode()
 * and inserts its octree element by calling SCE_Octree_InsertElement().
 * \sa SCE_Scene_RemoveInstance(), SCE_Scene_AddNode(), SCE_Scene_MakeOctree()
 */
void SCE_Scene_AddInstance (SCE_SScene *scene, SCE_SSceneEntityInstance *einst)
{
    SCE_SOctreeElement *el = NULL;
    SCE_SNode *node = NULL;
    SCE_SBoundingSphere *bs = NULL;

    node = SCE_SceneEntity_GetInstanceNode (einst);
    el = SCE_SceneEntity_GetInstanceElement (einst);
    bs = el->sphere;

    el->insert = SCE_Scene_InsertInstance;
    SCE_Octree_AddElement (scene->octree, el);

    /* NOTE: not relative to the (future) parent (gne?) */
    SCE_Node_SetOnMovedCallback (node, SCE_Scene_OnNodeMoved, scene);
    SCE_Scene_AddNode (scene, node);
    SCE_List_Prependl (scene->instances,
                       SCE_SceneEntity_GetInstanceIterator1 (einst));

    SCE_BoundingSphere_Push (bs, SCE_Node_GetFinalMatrix (node));
    SCE_Octree_InsertElement (scene->octree, el);
    SCE_BoundingSphere_Pop (bs);
}
/**
 * \brief Removes an instance from a scene
 * \param einst the instance to remove
 * \sa SCE_Scene_AddInstance()
 */
void SCE_Scene_RemoveInstance (SCE_SScene *scene,
                               SCE_SSceneEntityInstance *einst)
{
    SCE_SOctreeElement *el = NULL;
    SCE_SNode *node = SCE_SceneEntity_GetInstanceNode (einst);
    el = SCE_SceneEntity_GetInstanceElement (einst);

    SCE_List_Removel (SCE_SceneEntity_GetInstanceIterator1 (einst));
    SCE_Scene_RemoveNode (scene, node);
    SCE_Octree_RemoveElement (el);
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
 * \deprecated
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
 * \todo lol
 */
int SCE_Scene_AddLight (SCE_SScene *scene, SCE_SLight *light)
{
    SCE_SNode *node = NULL;
    if (SCE_List_PrependNewl (scene->lights, light) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    node = SCE_Light_GetNode (light);
    SCE_Node_GetElement (node)->insert = SCE_Scene_InsertLight;
    SCE_Node_HasMoved (node);
    SCE_Octree_AddElement (scene->octree, SCE_Node_GetElement (node));
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

/* builds internal structure for an octree */
static int SCE_Scene_MakeOctreeInternal (SCE_SOctree *tree)
{
    SCE_SSceneOctree *stree = SCE_Scene_CreateOctree ();
    if (!tree)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    SCE_Octree_SetData (tree, stree);
    if (SCE_Octree_HasChildren (tree))
    {
        unsigned int i;
        SCE_SOctree **children = SCE_Octree_GetChildren (tree);
        for (i = 0; i < 8; i++)
        {
            if (SCE_Scene_MakeOctreeInternal (children[i]) < 0)
            {
                Logger_LogSrc ();
                return SCE_ERROR;
            }
        }
    }
    return SCE_OK;
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
    /* creates the internal structures */
    if (SCE_Scene_MakeOctreeInternal (scene->octree) < 0)
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
 * \deprecated
 * \brief Defines the beginning of a frame
 */
void SCE_Scene_BeginFrame (SCE_SScene *scene)
{
    scene->node_updated = SCE_FALSE;
}

/**
 * \deprecated
 * \brief Defines the end of a frame
 */
void SCE_Scene_EndFrame (SCE_SScene *scene)
{
    /* what to do? maybe call glFlush()? */
    (void) scene;
}


static float SCE_Scene_GetOctreeSize (SCE_SOctree *tree, SCE_SCamera *cam)
{
    return SCE_Lod_ComputeBoundingBoxSurface (SCE_Octree_GetBoundingBox (tree),
                                              cam);
}

static void SCE_Scene_AddOctreeInstances (SCE_SScene *scene, SCE_SOctree *tree,
                                          unsigned int id)
{
    SCE_SSceneOctree *stree = SCE_Octree_GetData (tree);
    SCE_List_Join (scene->selected_join, stree->instances[id]);
    scene->selected_join = stree->instances[id];
}

static void SCE_Scene_SelectOctreeInstances (SCE_SScene *scene, SCE_SOctree *tree)
{
    unsigned int i;
    float size;
    size = SCE_Scene_GetOctreeSize (tree, scene->camera);
    SCE_Scene_AddOctreeInstances (scene, tree, 0);
    for (i = 0; i < 2; i++)
    {
        if (omg_coeffs[i] * size < /*scene->contribution_culling_size*/0.01)
            break;              /* the next are smaller, so stop */
        else
            SCE_Scene_AddOctreeInstances (scene, tree, i+1);
    }
}

static void SCE_Scene_SelectOctreeInstancesRec (SCE_SScene *scene,
                                                SCE_SOctree *tree)
{
    SCE_Scene_SelectOctreeInstances (scene, tree);
    if (SCE_Octree_HasChildren (tree))
    {
        unsigned int i;
        SCE_SOctree **children = SCE_Octree_GetChildren (tree);
        for (i = 0; i < 8; i++)
            SCE_Scene_SelectOctreeInstancesRec (scene, children[i]);
    }
}

static void SCE_Scene_SelectVisibleInstances (SCE_SScene *scene,
                                              SCE_SSceneOctree *stree,
                                              unsigned int id)
{
    SCE_SListIterator *it = NULL;
    SCE_SSceneEntityInstance *einst = NULL;
    SCE_List_ForEach (it, stree->instances[id])
    {
        einst = SCE_List_GetData (it);
        if (SCE_SceneEntity_IsInstanceInFrustum (einst, scene->camera))
        {
            SCE_List_Prependl (scene->selected,
                               SCE_SceneEntity_GetInstanceIterator1 (einst));
        }
    }
}

static void SCE_Scene_SelectVisibleOctrees (SCE_SScene *scene,
                                            SCE_SOctree *tree)
{
    if (!SCE_Octree_IsVisible (tree))
        return;
    if (!SCE_Octree_IsPartiallyVisible (tree))
        SCE_Scene_SelectOctreeInstancesRec (scene, tree);
    else
    {
#if 0
        SCE_Scene_SelectOctreeInstances (scene, tree);
#else
        unsigned int i;
        float size;
        SCE_SSceneOctree *stree = NULL;

        stree = SCE_Octree_GetData (tree);
        size = SCE_Scene_GetOctreeSize (tree, scene->camera);

        /* do visibility test for each instance */
        SCE_Scene_SelectVisibleInstances (scene, stree, 0);
        for (i = 0; i < 2; i++)
        {
            if (omg_coeffs[i] * size < /*scene->contribution_culling_size*/0.01)
                break;              /* the next are smaller, so stop */
            else
                SCE_Scene_SelectVisibleInstances (scene, stree, i+1);
        }
#endif

        if (SCE_Octree_HasChildren (tree))
        {
            unsigned int i;
            SCE_SOctree **children = SCE_Octree_GetChildren (tree);
            for (i = 0; i < 8; i++)
                SCE_Scene_SelectVisibleOctrees (scene, children[i]);
        }
    }
}

static void SCE_Scene_SelectVisibles (SCE_SScene *scene)
{
    SCE_Scene_SelectVisibleOctrees (scene, scene->octree);
}


static void SCE_Scene_DetermineEntitiesUsingLOD (SCE_SScene *scene)
{
    SCE_SListIterator *it = NULL;
    SCE_SList *instances = scene->selected;
    SCE_List_ForEach (it, instances)
    {
        SCE_SceneEntity_DetermineInstanceLOD (SCE_List_GetData (it),
                                              scene->camera);
    }
}

static void SCE_Scene_DetermineEntities (SCE_SScene *scene)
{
    SCE_SListIterator *it = NULL;
    SCE_SList *instances = scene->selected;
    SCE_List_ForEach (it, instances)
        SCE_SceneEntity_ReplaceInstanceToEntity (SCE_List_GetData (it));
}


static void SCE_Scene_FlushEntities (SCE_SScene *scene)
{
    SCE_SListIterator *it = NULL;
    SCE_SList *entities = scene->entities;
    SCE_List_ForEach (it, entities)
        SCE_SceneEntity_Flush (SCE_List_GetData (it));
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
    SCE_SBoundingSphere sphere;
    SCE_SNode *node = NULL;
    SCE_SOctreeElement *el = NULL;

    /* assign */
    scene->rendertarget = rendertarget;
    scene->cubeface = cubeface;
    scene->camera = (camera ? camera : default_camera);

    /* init sphere */
    SCE_BoundingSphere_Init (&sphere);
    SCE_BoundingSphere_SetRadius (&sphere, 0.00001); /* oe */
    /* TODO: what if scene has many cameras? */
    node = SCE_Camera_GetNode (scene->camera);
    el = SCE_Node_GetElement (node);
    el->insert = SCE_Scene_InsertCamera;
    el->sphere = &sphere;
    SCE_Octree_AddElement (scene->octree, el);
    SCE_Octree_InsertElement (scene->octree, el); /* wesh */

    if (scene->states.lod || scene->states.frustum_culling)
        SCE_Scene_FlushEntities (scene);

    if (scene->states.frustum_culling)
    {
        /* do it before FastUpdateRootRecursive(), otherwise the calls of
           List_Removel() can fail */
        SCE_List_BreakAll (scene->selected);
        SCE_List_Flush (scene->selected);
        scene->selected_join = scene->selected;
    }

    /* updating scene nodes */
    if (!scene->node_updated)
        SCE_Node_FastUpdateRootRecursive (scene->rootnode, scene->n_nodes);
    /* NOTE: node_updated or do_update_nodes can be a state because its value
       may change during scene update */
    scene->node_updated = SCE_TRUE;

    SCE_Camera_Update (scene->camera);

    if (scene->states.frustum_culling)
    {
        SCE_Octree_MarkVisibles (scene->octree,
                                 SCE_Camera_GetFrustum (scene->camera));
        SCE_Scene_SelectVisibles (scene);
    }

    /* determinating entities */
    if (scene->states.lod)
        SCE_Scene_DetermineEntitiesUsingLOD (scene);
    else if (scene->states.frustum_culling)
        SCE_Scene_DetermineEntities (scene);
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
        if (SCE_SceneEntity_HasInstance (entity))
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
    SCE_SSceneEntity *entity = SCE_Skybox_GetEntity (scene->skybox);
    SCE_SSceneEntityInstance *einst = SCE_Skybox_GetInstance (scene->skybox);
    SCE_SNode *node = SCE_SceneEntity_GetInstanceNode (einst);

    matcam = SCE_Node_GetFinalMatrix (SCE_Camera_GetNode (cam));
    SCE_Matrix4_GetTranslation (matcam, pos);
    matcam = SCE_Node_GetMatrix (node);
    matcam[3]  = pos[0];
    matcam[7]  = pos[1];
    matcam[11] = pos[2];
    SCE_Node_HasMoved (node);
    SCE_Node_UpdateRootRecursive (node);
    /* TODO: WHAT THE FUUUUCK */
    /*SCE_CSetState (GL_DEPTH_TEST, SCE_FALSE);
    SCE_CActivateDepthBuffer (GL_FALSE);*/
    SCE_CSetState (GL_CULL_FACE, SCE_FALSE);
    SCE_SceneEntity_UseResources (entity);
    SCE_SceneEntity_Render (entity);
    /*SCE_CSetState (GL_DEPTH_TEST, SCE_TRUE);
    SCE_CActivateDepthBuffer (GL_TRUE);*/
    SCE_CSetState (GL_CULL_FACE, SCE_TRUE);
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
    {
        scene->states.clearcolor = SCE_FALSE;
        scene->states.cleardepth = SCE_TRUE;
    }
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
