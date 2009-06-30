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
   updated: 27/06/2009 */

#ifndef SCEMODEL_H
#define SCEMODEL_H

#include <stdarg.h>
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
    SCE_SMesh *mesh;
    SCE_SList *textures;
    SCE_SShader *shader;
    SCE_SList *locales;         /* Local space instances position */
    int is_instance;

    SCE_SListIterator iterator;
    SCE_SListIterator *it;
};

typedef struct sce_smodelentitygroup SCE_SModelEntityGroup;
struct sce_smodelentitygroup
{
    SCE_SSceneEntityGroup *group;
    int is_instance;
    SCE_SListIterator iterator;
    SCE_SListIterator *it;
};

typedef struct sce_smodel SCE_SModel;
struct sce_smodel
{
    SCE_SList *entities[SCE_MAX_MODEL_ENTITIES];
    SCE_SList *groups;
    SCE_SList *instances;
    SCE_TMatrix4 matrix;
    int is_instance;            /* Is an instance? */
};

SCE_SModel* SCE_Model_Create (void);
void SCE_Model_Delete (SCE_SModel*);

int SCE_Model_AddEntityArg (SCE_SModel*, int, SCE_SMesh*, SCE_SShader*, va_list);
int SCE_Model_AddEntity (SCE_SModel*, int, SCE_SMesh*, SCE_SShader*, ...);

int SCE_Model_AddInstance (SCE_SModel*, unsigned int, SCE_TMatrix4);
int SCE_Model_AddInstanceDup (SCE_SModel*, unsigned int, SCE_TMatrix4);

unsigned int SCE_Model_GetNumLOD (SCE_SModel*);
float* SCE_Model_GetMatrix (SCE_SModel*);
float* SCE_Model_GetLocalMatrix (SCE_SModel*, unsigned int);

int SCE_Model_Build (SCE_SModel*);

int SCE_Model_Instanciate (SCE_SModel*, SCE_SModel*);
SCE_SModel* SCE_Model_CreateInstanciate (SCE_SModel*);

SCE_SList* SCE_Model_GetInstances (SCE_SModel*);
SCE_SSceneEntityGroup* SCE_Model_GetSceneEntityGroup(SCE_SModel*, unsigned int);

#if 0
int SCE_Model_IsRoot();
void SCE_Model_GiveRoot();
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
