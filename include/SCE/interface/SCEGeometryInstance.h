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
 
/* created: 25/10/2008
   updated: 01/12/2008 */

#ifndef SCEGEOMETRYINSTANCE_H
#define SCEGEOMETRYINSTANCE_H

#include <SCE/utils/SCEList.h>
#include <SCE/interface/SCENode.h>
#include <SCE/interface/SCEMesh.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SCE_SIMPLE_INSTANCING 0
#define SCE_PSEUDO_INSTANCING 1
#define SCE_HARDWARE_INSTANCING 2

/** \copydoc sce_sgeometryinstance */
typedef struct sce_sgeometryinstance SCE_SGeometryInstance;
/** \copydoc sce_sgeometryinstancegroup */
typedef struct sce_sgeometryinstancegroup SCE_SGeometryInstanceGroup;

typedef void (*SCE_FGeometryInstanceRenderFunc)(SCE_SGeometryInstance*);

/**
 * \brief Geometry instance
 *
 * This structure stores informations about one instance of a group of
 * instances.
 * \sa SCE_SGeometryGroup
 */
struct sce_sgeometryinstance
{
    float *m;                          /**< Instance's matrix */
    SCE_FGeometryInstanceRenderFunc renderfunc; /**< Render function */
    SCE_SGeometryInstanceGroup *group; /**< Group of the instance */
    int removed;                       /**< Is it removed from its group? */
#if SCE_LIST_ITERATOR_NO_MALLOC
    SCE_SListIterator iterator;
#endif
    SCE_SListIterator *it;             /**< Own iterator, used by the groups */
    void *data;                        /**< Used defined data */
};

/**
 * \brief A group of geometry instances
 *
 * This structure defines a group of multiple instances of one geometry
 * \sa SCE_SGeometryInstance
 */
struct sce_sgeometryinstancegroup
{
    SCE_SMesh *mesh;            /**< Mesh of this group (common data) */
    SCE_SList *instances;       /**< Instances of this group */
    int type;                   /**< Instancing method */
    /* attrib indices for pseudo instancing */
    int attrib1, attrib2, attrib3;
};

void SCE_Instance_Init (SCE_SGeometryInstance*);
void SCE_Instance_InitGroup (SCE_SGeometryInstanceGroup*);

SCE_SGeometryInstance* SCE_Instance_Create (void);
void SCE_Instance_Delete (SCE_SGeometryInstance*);

SCE_SGeometryInstanceGroup* SCE_Instance_CreateGroup (void);
void SCE_Instance_DeleteGroup (SCE_SGeometryInstanceGroup*);

void SCE_Instance_SetInstancingType (SCE_SGeometryInstanceGroup*, int);
void SCE_Instance_SetAttribIndices (SCE_SGeometryInstanceGroup*, int, int, int);

void SCE_Instance_AddInstance (SCE_SGeometryInstanceGroup*,
                               SCE_SGeometryInstance*);
void SCE_Instance_RemoveInstance (SCE_SGeometryInstance*);

SCE_SList* SCE_Instance_GetGroupInstancesList (SCE_SGeometryInstanceGroup*);
int SCE_Instance_HaveGroupInstance (SCE_SGeometryInstanceGroup*);

void SCE_Instance_SetGroupMesh (SCE_SGeometryInstanceGroup*, SCE_SMesh*);
SCE_SMesh* SCE_Instance_GetGroupMesh (SCE_SGeometryInstanceGroup*);

void SCE_Instance_SetMatrix (SCE_SGeometryInstance*, SCE_TMatrix4);
float* SCE_Instance_GetMatrix (SCE_SGeometryInstance*);

SCE_SGeometryInstanceGroup* SCE_Instance_GetGroup (SCE_SGeometryInstance*);

void SCE_Instance_SetRenderCallback (SCE_SGeometryInstance*,
                                     SCE_FGeometryInstanceRenderFunc);

void SCE_Instance_SetData (SCE_SGeometryInstance*, void*);
void* SCE_Instance_GetData (SCE_SGeometryInstance*);

void SCE_Instance_RenderGroup (SCE_SGeometryInstanceGroup*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
