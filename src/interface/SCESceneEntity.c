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
 
/* created: 03/11/2008
   updated: 29/06/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECore.h>
#include <SCE/interface/SCESceneEntity.h>

#include <SCE/interface/SCETexture.h>
#include <SCE/interface/SCEMaterial.h>
#include <SCE/interface/SCEShaders.h>

/**
 * \file SCESceneEntity.c
 * \copydoc sceneentity
 * 
 * \file SCESceneEntity.h
 * \copydoc sceneentity
 */

/**
 * \defgroup sceneentity Scene Entity
 * \ingroup interface
 * \internal
 * \brief Scene entities managment
 */

/** @{ */

void SCE_SceneEntity_InitInstance (SCE_SSceneEntityInstance *einst)
{
    einst->node = NULL;
    einst->instance = NULL;
    einst->lod = NULL;
    einst->entity = NULL;
    einst->group = NULL;
    einst->it = &einst->iterator;
    SCE_List_InitIt (einst->it);
    einst->it2 = &einst->iterator2;
    SCE_List_InitIt (einst->it2);
}

/**
 * \brief Creates an entity instance an initializes all its substructures
 */
SCE_SSceneEntityInstance* SCE_SceneEntity_CreateInstance (void)
{
    SCE_SSceneEntityInstance *einst = NULL;

    SCE_btstart ();
    if (!(einst = SCE_malloc (sizeof *einst)))
        goto failure;
    SCE_SceneEntity_InitInstance (einst);
    if (!(einst->truenode = einst->node = SCE_Node_Create ()))
        goto failure;
    if (!(einst->instance = SCE_Instance_Create ()))
        goto failure;
    if (!(einst->lod = SCE_Lod_Create ()))
        goto failure;
#if !SCE_LIST_ITERATOR_NO_MALLOC
    if (!(einst->it = SCE_List_CreateIt ()))
        goto failure;
    if (!(einst->it2 = SCE_List_CreateIt ()))
        goto failure;
#endif
    /* for compatibility with the entities manager */
    SCE_List_SetData (einst->it, einst);
    SCE_List_SetData (einst->it2, einst);
    /* see SCE_Scene_OnNodeMoved() */
    SCE_Node_SetData (einst->node, einst);
    /* see SCE_SceneEntity_ForEachInstanceInGroup() */
    SCE_Instance_SetData (einst->instance, einst);
    /* set the matrix pointer for the instance */
    SCE_Instance_SetMatrix (einst->instance,
                            SCE_Node_GetFinalMatrix (einst->node));
    goto success;

failure:
    SCE_SceneEntity_DeleteInstance (einst), einst = NULL;
    Logger_LogSrc ();
success:
    SCE_btend ();
    return einst;
}

void SCE_SceneEntity_DeleteInstance (SCE_SSceneEntityInstance *einst)
{
    if (einst)
    {
#if !SCE_LIST_ITERATOR_NO_MALLOC
        SCE_List_DeleteIt (einst->it);
        SCE_List_DeleteIt (einst->it2);
#endif
        SCE_Lod_Delete (einst->lod);
        SCE_Instance_Delete (einst->instance);
        SCE_Node_Delete (einst->truenode);
        SCE_free (einst);
    }
}

/**
 * \brief Duplicates an instance, the copy is in the same entity group of
 * \p einst
 */
SCE_SSceneEntityInstance*
SCE_SceneEntity_DupInstance (SCE_SSceneEntityInstance *einst)
{
    SCE_SSceneEntityInstance *new = NULL;
    if (!(new = SCE_SceneEntity_CreateInstance ()))
    {
        Logger_LogSrc ();
        return NULL;
    }
    SCE_SceneEntity_AddInstance (einst->group, new);
    return new;
}


static int SCE_SceneEntity_IsBSInFrustum (SCE_SSceneEntityInstance*,
                                          SCE_SCamera*);

void SCE_SceneEntity_InitProperties (SCE_SSceneEntityProperties *props)
{
    props->cullface = SCE_TRUE;
    props->cullmode = SCE_BACK;
    props->depthtest = SCE_TRUE;
    props->depthmode = SCE_LESS;
    props->alphatest = SCE_FALSE;
}

void SCE_SceneEntity_Init (SCE_SSceneEntity *entity)
{
    entity->igroup = NULL;

    entity->mesh = NULL;
    SCE_BoundingBox_Init (&entity->box);
    SCE_BoundingSphere_Init (&entity->sphere);
    entity->textures = NULL;
    entity->shader = NULL;
    entity->material = NULL;
    SCE_SceneEntity_InitProperties (&entity->props);

    entity->group = NULL;
    entity->isinfrustumfunc = SCE_SceneEntity_IsBSInFrustum;
#if SCE_LIST_ITERATOR_NO_MALLOC
    entity->it = &entity->iterator;
    SCE_List_InitIt (entity->it);
#else
    entity->it = NULL;
#endif
}

SCE_SSceneEntity* SCE_SceneEntity_Create (void)
{
    SCE_SSceneEntity *entity = NULL;

    SCE_btstart ();
    if (!(entity = SCE_malloc (sizeof *entity)))
        goto failure;
    SCE_SceneEntity_Init (entity);
    if (!(entity->igroup = SCE_Instance_CreateGroup ()))
        goto failure;
    if (!(entity->textures = SCE_List_Create (NULL)))
        goto failure;
#if !SCE_LIST_ITERATOR_NO_MALLOC
    if (!(entity->it = SCE_List_CreateIt ()))
        goto failure;
#endif
    /* for compatibility with groups */
    SCE_List_SetData (entity->it, entity);
    /* resources don't manages their own iterator (not yet) */
    SCE_List_CanDeleteIterators (entity->textures, SCE_TRUE);
    goto success;

failure:
    SCE_SceneEntity_Delete (entity), entity = NULL;
    Logger_LogSrc ();
success:
    SCE_btend ();
    return entity;
}

void SCE_SceneEntity_Delete (SCE_SSceneEntity *entity)
{
    if (entity)
    {
#if !SCE_LIST_ITERATOR_NO_MALLOC
        SCE_List_DeleteIt (entity->it);
#endif
        SCE_List_Delete (entity->textures);
        SCE_Instance_DeleteGroup (entity->igroup);
        SCE_free (entity);
    }
}


void SCE_SceneEntity_InitGroup (SCE_SSceneEntityGroup *group)
{
    group->entities = NULL;
    group->n_entities = 0;
}

void SCE_SceneEntity_YouDontHaveGroup (void *entity)
{
    ((SCE_SSceneEntity*)entity)->group = NULL;
}

SCE_SSceneEntityGroup* SCE_SceneEntity_CreateGroup (void)
{
    SCE_SSceneEntityGroup *group = NULL;

    SCE_btstart ();
    if (!(group = SCE_malloc (sizeof *group)))
        goto failure;
    SCE_SceneEntity_InitGroup (group);
    if (!(group->entities = SCE_List_Create (SCE_SceneEntity_YouDontHaveGroup)))
        goto failure;
    SCE_List_CanDeleteIterators (group->entities, SCE_TRUE);
    goto success;
failure:
    SCE_SceneEntity_DeleteGroup (group), group = NULL;
    Logger_LogSrc ();
success:
    SCE_btend ();
    return group;
}

void SCE_SceneEntity_DeleteGroup (SCE_SSceneEntityGroup *group)
{
    if (group)
    {
        SCE_List_Delete (group->entities);
        SCE_free (group);
    }
}


/**
 * \brief Adds an entity to a group
 */
void SCE_SceneEntity_AddEntity (SCE_SSceneEntityGroup *group,
                                SCE_SSceneEntity *entity)
{
    SCE_SceneEntity_RemoveEntity (entity);
    SCE_List_Appendl (group->entities, entity->it); /* order defined */
    entity->group = group;
    group->n_entities++;
}
/**
 * \brief Removes an entity from its group
 */
void SCE_SceneEntity_RemoveEntity (SCE_SSceneEntity *entity)
{
    if (entity->group)
    {
        SCE_List_Removel (entity->it);
        entity->group->n_entities--;
        entity->group = NULL;
    }
}

#if 0
/**
 * \brief Sorts the list of the entities that the given LOD group contains
 *
 * This function sorts the list of the entities that \p lgroup contains then
 * the first element is the first level of detail based on the number of
 * vertices of each entity's mesh.
 */
void SCE_SceneEntity_SortGroup (SCE_SSceneEntityLODGroup *group)
{
    int changefunc (SCE_SListIterator *it, SCE_SListIterator *it2)
    {
#define gnf SCE_Mesh_GetNumFaces
        return (gnf (((SCE_SSceneEntity*)SCE_List_GetData (it))->mesh)
                > gnf (((SCE_SSceneEntity*)SCE_List_GetData (it2))->mesh));
#undef gnf
    }
    SCE_List_Sortl (group->entities, changefunc);
}
#endif

unsigned int SCE_SceneEntity_GetGroupNumEntities (SCE_SSceneEntityGroup *g)
{
    return g->n_entities;
}
SCE_SList* SCE_SceneEntity_GetGroupEntitiesList (SCE_SSceneEntityGroup *g)
{
    return g->entities;
}


/**
 * \brief Setup instance informations, like bounding box, from an entity
 *
 * Copies informations from an entity to an instance:
 * - bounding box;
 * - bounding sphere.
 * This function is useful when you want to add an instance to an entity
 * directly, ie. not using an entity group.
 */
void SCE_SceneEntity_SetInstanceDataFromEntity (SCE_SSceneEntityInstance *einst,
                                                SCE_SSceneEntity *entity)
{
    SCE_Node_GetElement (einst->node)->sphere = &entity->sphere;
    SCE_Lod_SetBoundingBox (einst->lod, &entity->box);
}

/**
 * \brief Adds an instance to an entity group
 * \param group the group where add the instance (can be NULL)
 * \param einst the instance to add
 *
 * If \p group is NULL, \p einst is added to the previous group where it was
 * and \p einst->instance is added to the previous geometry group where it was,
 * if \p einst was never added to a group, calling this function with
 * \p group NULL generates a segmentation fault (in the better case).
 * Specific data of the first \p group 's entity are assigned to \p einst by
 * calling SCE_SceneEntity_SetInstanceDataFromEntity().
 * \sa SCE_SceneEntity_RemoveInstance(), SCE_Instance_AddInstance(),
 * SCE_SceneEntity_SelectInstance(), SCE_SceneEntity_SetInstanceDataFromEntity()
 */
void SCE_SceneEntity_AddInstance (SCE_SSceneEntityGroup *group,
                                  SCE_SSceneEntityInstance *einst)
{
    SCE_SSceneEntity *entity =
        SCE_List_GetData (SCE_List_GetFirst (group->entities));

    einst->group = group;
    SCE_SceneEntity_AddInstanceToEntity (entity, einst);
    SCE_SceneEntity_SetInstanceDataFromEntity (einst, entity);
}

/**
 * \brief Defines the entity of the given instance and adds its geometry
 * instance to the geometry group of \p entity
 * \sa SCE_Instance_AddInstance(), SCE_SceneEntity_ReplaceInstanceToEntity(),
 * SCE_SceneEntity_RemoveInstanceFromEntity()
 */
void SCE_SceneEntity_AddInstanceToEntity (SCE_SSceneEntity *entity,
                                          SCE_SSceneEntityInstance *einst)
{
    SCE_Instance_AddInstance (entity->igroup, einst->instance);
    einst->entity = entity;
}
/**
 * \brief Replaces an instance into its previous entity
 * \sa SCE_SceneEntity_AddInstanceToEntity(),
 * SCE_SceneEntity_RemoveInstanceFromEntity()
 */
void SCE_SceneEntity_ReplaceInstanceToEntity (SCE_SSceneEntityInstance *einst)
{
    SCE_Instance_AddInstance (einst->entity->igroup, einst->instance);
}
/**
 * \brief Removes the geometry instance of the given entity instance from its
 * geometry group
 * \sa SCE_SceneEntity_AddInstanceToEntity(),
 * SCE_SceneEntity_ReplaceInstanceToEntity()
 */
void SCE_SceneEntity_RemoveInstanceFromEntity (SCE_SSceneEntityInstance *einst)
{
    SCE_Instance_RemoveInstance (einst->instance);
}

/**
 * \brief Flushs the instances list of the group of \p entity
 * \sa SCE_SSceneEntity::igroup
 */
void SCE_SceneEntity_Flush (SCE_SSceneEntity *entity)
{
    /* muhahaha */
    SCE_List_Flush (SCE_Instance_GetInstancesList (entity->igroup));
}

/**
 * \brief Gets the node of the given instance
 */
SCE_SNode* SCE_SceneEntity_GetInstanceNode (SCE_SSceneEntityInstance *einst)
{
    return einst->node;
}
/**
 * \brief Gets the geometry instance of the given instance
 */
SCE_SGeometryInstance*
SCE_SceneEntity_GetInstanceInstance (SCE_SSceneEntityInstance *einst)
{
    return einst->instance;
}
SCE_SOctreeElement*
SCE_SceneEntity_GetInstanceElement (SCE_SSceneEntityInstance *einst)
{
    return SCE_Node_GetElement (einst->node);
}
/**
 * \brief Gets the "level of detail" structure of the given instance
 */
SCE_SLevelOfDetail*
SCE_SceneEntity_GetInstanceLOD (SCE_SSceneEntityInstance *einst)
{
    return einst->lod;
}
/**
 * \brief Gets the first iterator of an instance (for scene manager)
 */
SCE_SListIterator*
SCE_SceneEntity_GetInstanceIterator1 (SCE_SSceneEntityInstance *einst)
{
    return einst->it;
}
/**
 * \brief Gets the second iterator of an instance (for models)
 */
SCE_SListIterator*
SCE_SceneEntity_GetInstanceIterator2 (SCE_SSceneEntityInstance *einst)
{
    return einst->it2;
}


/**
 * \brief Gets the rendering properties of an entity
 * \sa SCE_SSceneEntityProperties
 */
SCE_SSceneEntityProperties*
SCE_SceneEntity_GetProperties (SCE_SSceneEntity *entity)
{
    return &entity->props;
}
/**
 * \brief Defines the mesh of a scene entity
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * Assigns (without duplicate) \p mesh to \p entity.
 * This function creates a bounding box for \p mesh and store it into
 * \p entity.
 * \sa SCE_SSceneEntity
 */
void SCE_SceneEntity_SetMesh (SCE_SSceneEntity *entity, SCE_SMesh *mesh)
{
    SCE_btstart ();
    entity->mesh = mesh;
    SCE_Instance_SetGroupMesh (entity->igroup, mesh);
    if (!mesh)
    {
        SCE_BoundingBox_Init (&entity->box);
        SCE_BoundingSphere_Init (&entity->sphere);
    }
    else
    {
        SCE_Mesh_GenerateBoundingBox (mesh, &entity->box);
        SCE_Mesh_GenerateBoundingSphere (mesh, &entity->sphere);
    }
    SCE_btend ();
}

/**
 * \brief Gets the mesh assigned to the given entity
 *
 * This function returns the meshs assigned to \p entity by a previous call of
 * SCE_SceneEntity_SetMesh().
 */
SCE_SMesh* SCE_SceneEntity_GetMesh (SCE_SSceneEntity *entity)
{
    return entity->mesh;
}


/**
 * \brief Adds a texture to an entity
 * \param entity the entity where add the texture
 * \param r the resource that represent the texture
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * When this function is called, the entity \p entity becomes a new owner of
 * \p r and \p r is stored into entity->textures. If \p r is NULL, the
 * comportement of this function is undefined.
 * \sa SCE_SceneEntity_RemoveTexture(), SCE_SceneEntity_SetShader(),
 * SCE_SceneEntity_SetMaterial(), SCE_SceneResource_AddOwner()
 */
int SCE_SceneEntity_AddTexture (SCE_SSceneEntity *entity, SCE_SSceneResource *r)
{
    if (SCE_List_PrependNewl (entity->textures, r) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    if (SCE_SceneResource_AddOwner (r, entity) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    return SCE_OK;
}
/**
 * \brief Removes a texture from an entity
 * \param entity the entity from where remove the texture
 * \param r the resource that represent the texture to remove
 *
 * Removes a texture already added to \p entity by calling
 * SCE_SceneEntity_AddTexture().
 * \sa SCE_SceneEntity_AddTexture(), SCE_SceneResource_RemoveOwner()
 */
void SCE_SceneEntity_RemoveTexture (SCE_SSceneEntity *entity,
                                    SCE_SSceneResource *r)
{
    SCE_SListIterator *it = SCE_List_LocateIterator (entity->textures, r, NULL);
    if (it)
    {
        SCE_List_Removel (it);
        SCE_List_DeleteIt (it);
    }
    SCE_SceneResource_RemoveOwner (r, entity);
}
/**
 * \brief Gets the list of the textures used by the given entity
 * \sa SCE_SceneEntity_AddTexture()
 */
SCE_SList* SCE_SceneEntity_GetTexturesList (SCE_SSceneEntity *entity)
{
    return entity->textures;
}

/**
 * \brief Defines the shader used by an entity
 * \param entity the entity that will use the shader
 * \param r the resource that represent the shader
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * If \p r is NULL, \p entity don't use any shader and this function returns
 * SCE_OK
 * \sa SCE_SceneEntity_SetMaterial(), SCE_SceneEntity_AddTexture(),
 * SCE_SceneResource_AddOwner(), SCE_SceneResource_RemoveOwner(),
 * SCE_SceneEntity_GetShader()
 */
int SCE_SceneEntity_SetShader (SCE_SSceneEntity *entity, SCE_SSceneResource *r)
{
    if (entity->shader)
        SCE_SceneResource_RemoveOwner (entity->shader, entity);
    entity->shader = r;
    if (r)
    {
        if (SCE_SceneResource_AddOwner (r, entity) < 0)
        {
            Logger_LogSrc ();
            return SCE_ERROR;
        }
    }
    return SCE_OK;
}
/**
 * \brief Gets the shader used by the given entity
 * \sa SCE_SceneEntity_SetShader()
 */
SCE_SSceneResource* SCE_SceneEntity_GetShader (SCE_SSceneEntity *entity)
{
    return entity->shader;
}

/**
 * \brief Defines the material used by an entity
 * \param entity the entity that will use the material
 * \param r the resource that represent the material
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * If \p r is NULL, \p entity don't use any material and this function returns
 * SCE_OK
 * \sa SCE_SceneEntity_SetShader(), SCE_SceneEntity_AddTexture(),
 * SCE_SceneResource_AddOwner(), SCE_SceneResource_RemoveOwner(),
 * SCE_SceneEntity_GetMaterial()
 */
int SCE_SceneEntity_SetMaterial (SCE_SSceneEntity *entity, SCE_SSceneResource*r)
{
    if (entity->material)
        SCE_SceneResource_RemoveOwner (entity->material, entity);
    entity->material = r;
    if (r)
    {
        if (SCE_SceneResource_AddOwner (r, entity) < 0)
        {
            Logger_LogSrc ();
            return SCE_ERROR;
        }
    }
    return SCE_OK;
}
/**
 * \brief Gets the material used by the given entity
 * \sa SCE_SceneEntity_SetMaterial()
 */
SCE_SSceneResource* SCE_SceneEntity_GetMaterial (SCE_SSceneEntity *entity)
{
    return entity->material;
}

/**
 * \brief Indicates if an entity have a resource of the given type \p type
 * \sa SCE_SceneResource_GetType()
 */
int SCE_SceneEntity_HasResourceOfType (SCE_SSceneEntity *entity, int type)
{
    SCE_SListIterator *it;
    SCE_List_ForEach (it, entity->textures)
    {
        if (SCE_SceneResource_GetType (SCE_List_GetData (it)) == type)
            return SCE_TRUE;
    }
    if ((entity->shader && SCE_SceneResource_GetType(entity->shader) == type) ||
      (entity->material && SCE_SceneResource_GetType(entity->material) == type))
        return SCE_TRUE;
    return SCE_FALSE;
}

/**
 * \brief Indicates if an entity have any instance
 */
int SCE_SceneEntity_HasInstance (SCE_SSceneEntity *entity)
{
    return SCE_Instance_HasGroupInstance (entity->igroup);
}

/**
 * \brief Gets the geometry instances group of an entity
 */
SCE_SGeometryInstanceGroup*
SCE_SceneEntity_GetInstancesGroup (SCE_SSceneEntity *entity)
{
    return entity->igroup;
}

static int SCE_SceneEntity_IsBBInFrustum (SCE_SSceneEntityInstance *einst,
                                          SCE_SCamera *cam)
{
    int result;
    SCE_SBoundingBox *box = &einst->entity->box;

    SCE_BoundingBox_Push (box, SCE_Node_GetFinalMatrix (einst->node));
    result = SCE_Frustum_BoundingBoxInBool (SCE_Camera_GetFrustum (cam), box);
    SCE_BoundingBox_Pop (box);

    return result;
}
static int SCE_SceneEntity_IsBSInFrustum (SCE_SSceneEntityInstance *einst,
                                          SCE_SCamera *cam)
{
    int result;
    SCE_SBoundingSphere *sphere = &einst->entity->sphere;

    SCE_BoundingSphere_Push (sphere, SCE_Node_GetFinalMatrix (einst->node));
    result = SCE_Frustum_BoundingSphereInBool (SCE_Camera_GetFrustum (cam),
                                               sphere);
    SCE_BoundingSphere_Pop (sphere);

    return result;
}
/**
 * \brief Defines the bounding volume to use for frustum culling
 * (frustum culling only for now)
 * \param volume the bounding volume to use, can be SCE_BOUNDINGBOX or
 * SCE_BOUNDINGSPHERE
 * \todo maybe rename it to SetupFrustumCullingBoundingVolume
 */
void SCE_SceneEntity_SetupBoundingVolume (SCE_SSceneEntity *entity, int volume)
{
    switch (volume)
    {
    case SCE_BOUNDINGBOX:
        entity->isinfrustumfunc = SCE_SceneEntity_IsBBInFrustum; break;
    case SCE_BOUNDINGSPHERE:
        entity->isinfrustumfunc = SCE_SceneEntity_IsBSInFrustum;
    }
}


/**
 * \deprecated
 * \brief Combines the nodes of the given entity instances
 *
 * The node of \p toattach becomes the same node of \p einst, so the instances
 * are attached but no new matrix is generated.
 * \sa SCE_SceneEntity_DetachInstance()
 */
void SCE_SceneEntity_AttachInstance (SCE_SSceneEntityInstance *einst,
                                     SCE_SSceneEntityInstance *toattach)
{
    /* TODO: warning, one node for two instances is dangerous.
       node->data is a pointer to the first instance.
       see SCE_Scene_OnNodeMoved() in SCEScene.c */
    toattach->node = einst->node;
    SCE_Instance_SetMatrix (toattach->instance,
                            SCE_Node_GetFinalMatrix (toattach->node));
}
/**
 * \deprecated
 * \brief Detachs the given instance from the instance where it was attached to
 *
 * Calling this function when \p einst wasn't attached has no effect
 * \sa SCE_SceneEntity_AttachInstance()
 */
void SCE_SceneEntity_DetachInstance (SCE_SSceneEntityInstance *einst)
{
    einst->node = einst->truenode;
    SCE_Instance_SetMatrix (einst->instance,
                            SCE_Node_GetFinalMatrix (einst->node));
}


/**
 * \brief Computes the LOD for an entity instance and add its geometry instance
 * to the corresponding group
 * 
 * Computes the LOD for an entity instance and add it to the corresponding
 * entity in the \p einst's entity group. \p einst->entity must be in a
 * group structure (SCE_SSceneEntityGroup).
 */
void SCE_SceneEntity_DetermineInstanceLOD (SCE_SSceneEntityInstance *einst,
                                           SCE_SCamera *cam)
{
    int entityid, lod;
    SCE_SSceneEntity *entity = NULL;
    SCE_SSceneEntityGroup *group = einst->group;

    SCE_Lod_Compute (einst->lod, SCE_Node_GetFinalMatrix (einst->node), cam);
    /* get max LOD */
    lod = SCE_Lod_GetLOD (einst->lod);
    entityid = (lod >= group->n_entities ? group->n_entities - 1 : lod);
    /* add instance to the 'groupid' group */
    entity = SCE_List_GetData (SCE_List_GetIterator (group->entities,entityid));
    SCE_SceneEntity_AddInstanceToEntity (entity, einst);
}
/**
 * \brief Determines if an instance is in the given frustum (returns a boolean)
 * \sa SCE_Frustum_BoundingBoxInBool(), SCE_Frustum_BoundingSphereInBool(),
 * SCE_SceneEntity_SetupBoundingVolume()
 */
int SCE_SceneEntity_IsInstanceInFrustum (SCE_SSceneEntityInstance *einst,
                                         SCE_SCamera *cam)
{
    return einst->entity->isinfrustumfunc (einst, cam);
}


/**
 * \brief Applies the properties of an entity by calling SCE_CSetState()
 */
void SCE_SceneEntity_ApplyProperties (SCE_SSceneEntity *entity)
{
    SCE_CSetState (GL_CULL_FACE, entity->props.cullface);
    SCE_CSetCulledFaces (entity->props.cullmode);
    SCE_CSetState (GL_DEPTH_TEST, entity->props.depthtest);
    SCE_CSetValidPixels (entity->props.depthmode);
    SCE_CSetState (GL_ALPHA_TEST, entity->props.alphatest);
}

/**
 * \brief Enables the resources that the given entity is using
 *
 * This function calls SCE_Shader_Use(), SCE_Texture_Use() and
 * SCE_Material_Use(), respectively, for the shader, the textures and the
 * material that \p entity have.
 */
void SCE_SceneEntity_UseResources (SCE_SSceneEntity *entity)
{
    SCE_SListIterator *it = NULL;

    SCE_SceneEntity_ApplyProperties (entity);

    if (entity->material)
        SCE_Material_Use (SCE_SceneResource_GetResource (entity->material));
    else
        SCE_Material_Use (NULL);

    if (entity->shader)
        SCE_Shader_Use (SCE_SceneResource_GetResource (entity->shader));
    else
        SCE_Shader_Use (NULL);

    SCE_Texture_BeginLot ();
    SCE_List_ForEach (it, entity->textures)
        SCE_Texture_Use (SCE_SceneResource_GetResource (SCE_List_GetData (it)));
    SCE_Texture_EndLot ();
}


/**
 * \brief Render all the instances of the given entity
 */
void SCE_SceneEntity_Render (SCE_SSceneEntity *entity)
{
    SCE_Instance_RenderGroup (entity->igroup);
}

/** @} */
