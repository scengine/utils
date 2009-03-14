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
   updated: 10/03/2009 */

#ifndef SCESCENEENTITY_H
#define SCESCENEENTITY_H

#include <SCE/utils/SCEList.h>
#include <SCE/interface/SCENode.h>
#include <SCE/interface/SCEGeometryInstance.h>
#include <SCE/interface/SCELevelOfDetail.h>
#include <SCE/interface/SCESceneResource.h>
#include <SCE/interface/SCEBoundingBox.h>
#include <SCE/interface/SCEBoundingSphere.h>
#include <SCE/interface/SCECamera.h>
#include <SCE/interface/SCEOctree.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** Types of volumes that can be used for frustum culling/other operations */
#define SCE_BOUNDINGBOX 1
#define SCE_BOUNDINGSPHERE 2

/** \copydoc sce_ssceneentityproperties */
typedef struct sce_ssceneentityproperties SCE_SSceneEntityProperties;
struct sce_ssceneentityproperties
{
    int cullface:1;
    int cullmode;
    int depthtest:1;
    int depthmode;
    int alphatest:1;
};

/** \copydoc sce_ssceneenStityinstance */
typedef struct sce_ssceneentityinstance SCE_SSceneEntityInstance;
/** \copydoc sce_ssceneentity */
typedef struct sce_ssceneentity SCE_SSceneEntity;
/** \copydoc sce_ssceneentitygroup */
typedef struct sce_ssceneentitygroup SCE_SSceneEntityGroup;

/**
 * \brief An instance of a scene entity
 * \sa SCE_SSceneEntity
 */
struct sce_ssceneentityinstance
{
    SCE_SNode *node;                 /**< Node */
    SCE_SNode *truenode;             /**< The node allocated by the instance */
    SCE_SGeometryInstance *instance; /**< Geometry instance */
    SCE_SLevelOfDetail *lod;         /**< LOD managment structure */
    SCE_SSceneEntity *entity;        /**< Entity used by the instance */
    SCE_SSceneEntityGroup *group;    /**< Group that contains the instance */
    SCE_SListIterator iterator, iterator2;
    SCE_SListIterator *it, *it2;     /**< Own iterators for fast add/remove */
};

/**
 * \brief A scene entity that defines a model that can be rendered via an
 * instance
 * \sa SCE_SSceneEntityInstance
 */
struct sce_ssceneentity
{
    SCE_SGeometryInstanceGroup *igroup; /**< Geometry group */

    SCE_SMesh *mesh;              /**< Mesh of the entity */
    SCE_SBoundingBox box;         /**< \c mesh's bounding box */
    SCE_SBoundingSphere sphere;   /**< \c mesh's bounding sphere */
    SCE_SList *textures;          /**< Textures used by the entity */
    SCE_SSceneResource *shader;   /**< Shader used by the entity */
    SCE_SSceneResource *material; /**< Material used by the entity */
    SCE_SSceneEntityProperties props;

    SCE_SSceneEntityGroup *group; /**< Group of the entity */
    /** Used to determine if the instance is in the given frustum */
    int (*isinfrustumfunc)(SCE_SSceneEntityInstance*, SCE_SCamera*);
#if SCE_LIST_ITERATOR_NO_MALLOC
    SCE_SListIterator iterator;
#endif
    SCE_SListIterator *it;        /**< Own iterator for group */
};

/**
 * \brief Defines a group of entities they represent the same geometry but in
 * a different LOD
 */
struct sce_ssceneentitygroup
{
    SCE_SList *entities;
    unsigned int n_entities;
};

/* callback prototype for 'ForEach' function */
typedef int (*SCE_FForEachSceneEntityInstanceFunc)(SCE_SSceneEntityGroup*,
                                                   SCE_SSceneEntityInstance*,
                                                   void*);

void SCE_SceneEntity_InitInstance (SCE_SSceneEntityInstance*);
SCE_SSceneEntityInstance* SCE_SceneEntity_CreateInstance (void);
void SCE_SceneEntity_DeleteInstance (SCE_SSceneEntityInstance*);

void SCE_SceneEntity_InitProperties (SCE_SSceneEntityProperties*);
void SCE_SceneEntity_Init (SCE_SSceneEntity*);
SCE_SSceneEntity* SCE_SceneEntity_Create (void);
void SCE_SceneEntity_Delete (SCE_SSceneEntity*);

void SCE_SceneEntity_InitGroup (SCE_SSceneEntityGroup*);
SCE_SSceneEntityGroup* SCE_SceneEntity_CreateGroup (void);
void SCE_SceneEntity_DeleteGroup (SCE_SSceneEntityGroup*);

void SCE_SceneEntity_AddEntity (SCE_SSceneEntityGroup*, SCE_SSceneEntity*);
void SCE_SceneEntity_RemoveEntity (SCE_SSceneEntity*);
#if 0
void SCE_SceneEntity_SortGroup (SCE_SSceneEntityGroup*);
#endif
unsigned int SCE_SceneEntity_GetGroupNumEntities (SCE_SSceneEntityGroup*);
SCE_SList* SCE_SceneEntity_GetGroupEntitiesList (SCE_SSceneEntityGroup*);

void SCE_SceneEntity_SetInstanceDataFromEntity (SCE_SSceneEntityInstance*,
                                                SCE_SSceneEntity*);
void SCE_SceneEntity_AddInstance (SCE_SSceneEntityGroup*,
                                  SCE_SSceneEntityInstance*);
void SCE_SceneEntity_AddInstanceToEntity (SCE_SSceneEntity*,
                                          SCE_SSceneEntityInstance*);
void SCE_SceneEntity_ReplaceInstanceToEntity (SCE_SSceneEntityInstance*);
void SCE_SceneEntity_RemoveInstanceFromEntity (SCE_SSceneEntityInstance*);

void SCE_SceneEntity_Flush (SCE_SSceneEntity*);


SCE_SNode* SCE_SceneEntity_GetInstanceNode (SCE_SSceneEntityInstance*);
SCE_SGeometryInstance*
SCE_SceneEntity_GetInstanceInstance (SCE_SSceneEntityInstance*);
SCE_SOctreeElement*
SCE_SceneEntity_GetInstanceElement (SCE_SSceneEntityInstance*);
SCE_SLevelOfDetail*
SCE_SceneEntity_GetInstanceLOD (SCE_SSceneEntityInstance*);
SCE_SListIterator*
SCE_SceneEntity_GetInstanceIterator1 (SCE_SSceneEntityInstance*);
SCE_SListIterator*
SCE_SceneEntity_GetInstanceIterator2 (SCE_SSceneEntityInstance*);


void SCE_SceneEntity_SetMesh (SCE_SSceneEntity*, SCE_SMesh*);
SCE_SMesh* SCE_SceneEntity_GetMesh (SCE_SSceneEntity*);

int SCE_SceneEntity_AddTexture (SCE_SSceneEntity*, SCE_SSceneResource*);
void SCE_SceneEntity_RemoveTexture (SCE_SSceneEntity*, SCE_SSceneResource*);
SCE_SList* SCE_SceneEntity_GetTexturesList (SCE_SSceneEntity*);

int SCE_SceneEntity_SetShader (SCE_SSceneEntity*, SCE_SSceneResource*);
SCE_SSceneResource* SCE_SceneEntity_GetShader (SCE_SSceneEntity*);

int SCE_SceneEntity_SetMaterial (SCE_SSceneEntity*, SCE_SSceneResource*);
SCE_SSceneResource* SCE_SceneEntity_GetMaterial (SCE_SSceneEntity*);

int SCE_SceneEntity_HasResourceOfType (SCE_SSceneEntity*, int);
int SCE_SceneEntity_HasInstance (SCE_SSceneEntity*);

SCE_SGeometryInstanceGroup*
SCE_SceneEntity_GetInstancesGroup (SCE_SSceneEntity*);

void SCE_SceneEntity_SetupBoundingVolume (SCE_SSceneEntity*, int);


void SCE_SceneEntity_AttachInstance (SCE_SSceneEntityInstance*,
                                     SCE_SSceneEntityInstance*);
void SCE_SceneEntity_DetachInstance (SCE_SSceneEntityInstance*);


void SCE_SceneEntity_DetermineInstanceLOD (SCE_SSceneEntityInstance*,
                                         SCE_SCamera*);
int SCE_SceneEntity_IsInstanceInFrustum (SCE_SSceneEntityInstance*,
                                         SCE_SCamera*);

void SCE_SceneEntity_ApplyProperties (SCE_SSceneEntity*);

void SCE_SceneEntity_UseResources (SCE_SSceneEntity*);

void SCE_SceneEntity_Render (SCE_SSceneEntity*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
