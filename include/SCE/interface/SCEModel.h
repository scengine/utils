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

/* created: 27/06/2009
   updated: 10/07/2009 */

#ifndef SCEMODEL_H
#define SCEMODEL_H

#include <stdarg.h>
#include <SCE/utils/SCEMatrix.h>
#include <SCE/utils/SCEList.h>
#include <SCE/interface/SCETexture.h>
#include <SCE/interface/SCEShaders.h>
#include <SCE/interface/SCESceneEntity.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SCE_MAX_MODEL_ENTITIES 8
#define SCE_MAX_MODEL_LOD_LEVELS SCE_MAX_MODEL_ENTITIES

typedef struct sce_smodelentity SCE_SModelEntity;
struct sce_smodelentity
{
    SCE_SSceneEntity *entity;
    int is_instance;
    SCE_SListIterator it;
};

typedef struct sce_smodelentitygroup SCE_SModelEntityGroup;
struct sce_smodelentitygroup
{
    SCE_SSceneEntityGroup *group;
    int is_instance;
    SCE_SListIterator it;
};

/* instance types */
/** \brief The model isn't an instance */
#define SCE_MODEL_NOT_INSTANCE 0
/** \brief Makes a new model using the scene entities of the root one */
#define SCE_MODEL_SOFT_INSTANCE 1
/** \brief Copy the main structure element on the fly */
#define SCE_MODEL_HARD_INSTANCE 2

typedef struct sce_smodel SCE_SModel;
struct sce_smodel
{
    SCE_SList *entities[SCE_MAX_MODEL_ENTITIES];
    SCE_SList *groups;
    SCE_SList *instances;
    SCE_SNode *root;            /* Root node */
    int root_instance;          /* Is root node an instance node? */
    int instance_type;          /* Is an instance? */
};

SCE_SModel* SCE_Model_Create (void);
void SCE_Model_Delete (SCE_SModel*);

int SCE_Model_AddEntityv (SCE_SModel*, int, SCE_SMesh*, SCE_SShader*,
                          SCE_STexture**);
int SCE_Model_AddEntity (SCE_SModel*, int, SCE_SMesh*, SCE_SShader*, ...);

void SCE_Model_SetRootNode (SCE_SModel*, SCE_SNode*);
SCE_SNode* SCE_Model_GetRootNode (SCE_SModel*);
int SCE_Model_RootNodeIsInstance (SCE_SModel*);

int SCE_Model_AddInstance (SCE_SModel*, unsigned int, SCE_SSceneEntityInstance*,
                           int);
int SCE_Model_AddNewInstance (SCE_SModel*, unsigned int, int, float*);

unsigned int SCE_Model_GetNumLOD (SCE_SModel*);
SCE_SSceneEntity* SCE_Model_GetEntity (SCE_SModel*, int, unsigned int);
SCE_SList* SCE_Model_GetEntitiesList (SCE_SModel*, int);
SCE_SSceneEntity* SCE_Model_GetEntityEntity (SCE_SModelEntity*);

int SCE_Model_Instanciate (SCE_SModel*, SCE_SModel*, int, int);
SCE_SModel* SCE_Model_CreateInstanciate (SCE_SModel*, int, int);

int SCE_Model_GetInstanceType (SCE_SModel*);

SCE_SList* SCE_Model_GetInstancesList (SCE_SModel*);
SCE_SSceneEntityGroup* SCE_Model_GetSceneEntityGroup(SCE_SModel*, unsigned int);

#if 0
int SCE_Model_IsRoot();
void SCE_Model_GiveRoot();
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
