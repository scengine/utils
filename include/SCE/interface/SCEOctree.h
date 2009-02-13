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
 
/* created: 06/05/2008
   updated: 31/01/2009 */

#ifndef SCEOCTREE_H
#define SCEOCTREE_H

#include <SCE/utils/SCEList.h>
#include <SCE/utils/SCEVector.h>
#include <SCE/interface/SCEBoundingBox.h>
#include <SCE/interface/SCEBoundingSphere.h>
#include <SCE/interface/SCECamera.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup octree
 * @{
 */

/** \copydoc sce_soctreeelement */
typedef struct sce_soctreeelement SCE_SOctreeElement;
/** \copydoc sce_soctree */
typedef struct sce_soctree SCE_SOctree;

struct sce_soctreeelement
{
#if SCE_LIST_ITERATOR_NO_MALLOC
    SCE_SListIterator iterator;
#endif
    SCE_SListIterator *it;       /**< Own iterator */
    SCE_SOctree *octree;         /**< Octree where the element is contained */
    void *owner;                 /**< Object that is the owner of the element */
    SCE_SBoundingSphere *sphere; /**< Sphere used for collision computations */
};


/**
 * \brief Type for depth-control function of SCE_Octree_RecursiveMake()
 * \param tree the current octree
 * \param param user-defined data
 * \returns SCE_TRUE to stop generation of octree's chlidren or SCE_FALSE to
 *          continue recursive generation
 * 
 * This type is used to control when stop octree's children generation when
 * doing a recursive children generation.
 * \see SCE_Octree_RecursiveMake()
 */
typedef int (*SCE_FOctreeLimitFunc)(SCE_SOctree *tree, void *param);

/**
 * \brief Octree definition structure
 * \warning You shouldn't directly access to any of the members of this
 *          structure.
 */
struct sce_soctree
{
    SCE_SOctree *child[8];  /**< Array of octree's children */
    int marks;              /**< 1 - Does octree having children?
                             * 2 - Is the octree visible? */
    SCE_SOctree* parent;    /**< Octree's parent */
    SCE_SBoundingBox box;   /**< Octree's bounding box */
    SCE_SList *elements;    /**< Elements contained in the octree */
};

/** @} */

SCE_SOctree* SCE_Octree_Create (void);
void SCE_Octree_Clear (SCE_SOctree*);
void SCE_Octree_Delete (SCE_SOctree*);
void SCE_Octree_DeleteRecursive (SCE_SOctree*);

SCE_SOctreeElement* SCE_Octree_CreateElement (void);
void SCE_Octree_DeleteElement (SCE_SOctreeElement*);

void SCE_Octree_SetCenter (SCE_SOctree*, float, float, float);
void SCE_Octree_SetCenterv (SCE_SOctree*, SCE_TVector3);
void SCE_Octree_GetCenterv (SCE_SOctree*, SCE_TVector3);
void SCE_Octree_SetSize (SCE_SOctree*, float, float, float);
float SCE_Octree_GetWidth (SCE_SOctree*);
float SCE_Octree_GetHeight (SCE_SOctree*);
float SCE_Octree_GetDepth (SCE_SOctree*);

int SCE_Octree_HaveChildren (SCE_SOctree*);
int SCE_Octree_IsVisible (SCE_SOctree*);
int SCE_Octree_IsPartiallyVisible (SCE_SOctree*);

SCE_SOctree** SCE_Octree_GetChildren (SCE_SOctree*);

int SCE_Octree_MakeChildren (SCE_SOctree*, int, float);
int SCE_Octree_RecursiveMake (SCE_SOctree*, unsigned int,
                              SCE_FOctreeLimitFunc, void*, int, float);

void SCE_Octree_SetElementOwner (SCE_SOctreeElement*, void*);
void* SCE_Octree_GetElementOwner (SCE_SOctreeElement*);
int SCE_Octree_IsElementVisible (SCE_SOctreeElement*);
int SCE_Octree_IsElementPartiallyVisible (SCE_SOctreeElement*);

void SCE_Octree_SetElementBoundingSphere (SCE_SOctreeElement*,
                                          SCE_SBoundingSphere*);
SCE_SBoundingSphere* SCE_Octree_GetElementBoundingSphere (SCE_SOctreeElement*);

int SCE_Octree_InsertElement (SCE_SOctree*, SCE_SOctreeElement*, int);
int SCE_Octree_ReinsertElement (SCE_SOctreeElement*);

void SCE_Octree_MarkVisibles (SCE_SOctree*, SCE_SCamera*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
