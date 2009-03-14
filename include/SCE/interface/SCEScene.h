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
 
/* created: 19/01/2007
   updated: 10/03/2009 */

#ifndef SCESCENE_H
#define SCESCENE_H

#include <SCE/utils/SCEList.h>
/*#include <SCE/interface/SCEState.h>*/
#include <SCE/interface/SCENode.h>
#include <SCE/interface/SCECamera.h>
#include <SCE/interface/SCETexture.h>
#include <SCE/interface/SCEMaterial.h>
/*#include <SCE/interface/SCEShaders.h>*/
#include <SCE/interface/SCESceneEntity.h>
#include <SCE/interface/SCEOctree.h>
#include <SCE/interface/SCESkybox.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup scene
 * @{
 */

#define SCE_SCENE_SHADERS_GROUP 0
#define SCE_SCENE_MATERIALS_GROUP 1
#define SCE_SCENE_TEXTURES0_GROUP 2
#define SCE_SCENE_NUM_RESOURCE_GROUP 3

/* default octree size */
#define SCE_SCENE_OCTREE_SIZE (16384.0)

typedef int (*SCE_FSceneForEachEntityGroupFunc)(SCE_SSceneEntityGroup *g,
                                                void *p);

/** \copydoc sce_sscenestates */
typedef struct sce_sscenestates SCE_SSceneStates;
/**
 * \brief States of a scene
 */
struct sce_sscenestates
{
    int clearcolor, cleardepth; /**< Clear the buffers? */
    int frustum_culling; /**< Use frustum culling? */
    int lighting;        /**< Use lighting? */
    int lod;             /**< Use LOD? */
};

/* scene */
/** \copydoc sce_sscene */
typedef struct sce_sscene SCE_SScene;
/**
 * \brief Scene description structure
 */
struct sce_sscene
{
    SCE_SNode *rootnode;        /**< Root node */
    int node_updated;           /**< Defines the root node's status */
    unsigned int n_nodes;       /**< Number of nodes attached to \c rootnode */

    SCE_SOctree *octree;        /**< Scene's octree */

    /** All the resources of the scene */
    SCE_SSceneResourceGroup *rgroups[SCE_SCENE_NUM_RESOURCE_GROUP];

    SCE_SList *instances;       /**< Instances */
    SCE_SList *selected;        /**< Selected instances */
    SCE_SList *selected_join;   /**< Where to join lists */
    SCE_SList *egroups;         /**< Scene's entities groups list */
    SCE_SList *entities;        /**< Scene's entities */
    SCE_SList *lights;          /**< Scene's lights list */

    SCE_SSkybox *skybox;        /**< Scene skybox */

    /** Color buffer clear values */
    float rclear, gclear, bclear, aclear;
    float dclear;               /**< Depth buffer clear value */
    SCE_STexture *rendertarget; /**< Scene's render target */
    SCEuint cubeface;           /**< Face of the cubemap (used if
                                 * sce_sscenerenderinfos::rendertarget is
                                 * a cubemap) */
    SCE_SCamera *camera;        /**< The camera for the current render */
    SCE_SSceneStates states;    /**< Scene's state */
};

/** @} */

int SCE_Init_Scene (void);
void SCE_Quit_Scene (void);

SCE_SScene* SCE_Scene_Create (void);
void SCE_Scene_Delete (SCE_SScene*);

SCE_SNode* SCE_Scene_GetRootNode (SCE_SScene*);

void SCE_Scene_OnNodeMoved (SCE_SNode*, void*);

void SCE_Scene_AddNode (SCE_SScene*, SCE_SNode*);
void SCE_Scene_RemoveNode (SCE_SScene*, SCE_SNode*);
void SCE_Scene_AddInstance (SCE_SScene*, SCE_SSceneEntityInstance*);
void SCE_Scene_RemoveInstance (SCE_SScene*, SCE_SSceneEntityInstance*);
int SCE_Scene_AddEntity (SCE_SScene*, SCE_SSceneEntity*);
void SCE_Scene_RemoveEntity (SCE_SScene*, SCE_SSceneEntity*);
int SCE_Scene_AddEntityGroup (SCE_SScene*, SCE_SSceneEntityGroup*);
int SCE_Scene_AddLight (SCE_SScene*, SCE_SLight*);
void SCE_Scene_AddResource (SCE_SScene*, int, SCE_SSceneResource*);

void SCE_Scene_SetSkybox (SCE_SScene*, SCE_SSkybox*);

void SCE_Scene_ForEachEntity (SCE_SScene*, SCE_FSceneForEachEntityGroupFunc,
                              void*);

void SCE_Scene_SetOctreeSize (SCE_SScene*, float, float, float);
int SCE_Scene_MakeOctree (SCE_SScene*, unsigned int, int, float);

int SCE_Scene_SetupBatching (SCE_SScene*, unsigned int, int*);
int SCE_Scene_SetupDefaultBatching (SCE_SScene*);

void SCE_Scene_BeginFrame (SCE_SScene*);
void SCE_Scene_EndFrame (SCE_SScene*);

void SCE_Scene_ClearBuffers (SCE_SScene*);

void SCE_Scene_Update (SCE_SScene*, SCE_SCamera*, SCE_STexture*, SCEuint);
void SCE_Scene_Render (SCE_SScene*, SCE_SCamera*, SCE_STexture*, int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
