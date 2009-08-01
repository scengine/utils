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
 
/* created: 10/07/2007
   updated: 01/07/2009 */

#ifndef SCENODE_H
#define SCENODE_H

#include <SCE/utils/SCEVector.h>
#include <SCE/utils/SCEMatrix.h>
#include <SCE/utils/SCEList.h>
#include <SCE/interface/SCEOctree.h>
#include <SCE/SCECommon.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup node
 * @{
 */

/** \copydoc sce_snode */
typedef struct sce_snode SCE_SNode;
typedef void (*SCE_FNodeCallback)(SCE_SNode*, void*);
typedef void (*SCE_FNodeCallback2)(SCE_SNode*);

/**
 * \brief Node definition structure
 * \warning You shouldn't directly access to any of the members of this
 *          structure.
 */
struct sce_snode
{
    SCE_SNode *parent;         /**< Parent node */
    SCE_SList *child;          /**< Children */
    SCE_SOctreeElement *element; /**< Element */
#if SCE_LIST_ITERATOR_NO_MALLOC
    SCE_SListIterator iterator;
#endif
    SCE_SListIterator *it;     /**< Own iterator */
    SCE_TMatrix4 matrix;       /**< Node matrix */
    SCE_TMatrix4 fmatrix;      /**< Real node's transformation matrix, with all
                                *   parent nodes' matrices applyed */
    void *data;                /**< User-defined data */
    int marks;                 /**< Has the node moved since the last update? */
    /* \deprecated */ /* TODO: kick that */
    SCE_FNodeCallback moved;   /**< Called when the node has moved */
    void *movedparam;          /**< \c moved parameter */
};

/** @} */

SCE_SNode* SCE_Node_Create (void);
void SCE_Node_Delete (SCE_SNode*);
void SCE_Node_DeleteRecursive (SCE_SNode*);

void SCE_Node_Attach (SCE_SNode*, SCE_SNode*);
SCE_SNode* SCE_Node_AttachNew (SCE_SNode*);
void SCE_Node_Detach (SCE_SNode*);

void SCE_Node_Insert (SCE_SNode*, SCE_SNode*);

void SCE_Node_MultMatrix (SCE_SNode*);
void SCE_Node_LoadMatrix (SCE_SNode*);

float* SCE_Node_GetMatrix (SCE_SNode*);
void SCE_Node_CopyMatrix (SCE_SNode*, SCE_TMatrix4);

float* SCE_Node_GetFinalMatrix (SCE_SNode*);
void SCE_Node_CopyFinalMatrix (SCE_SNode*, SCE_TMatrix4);

void SCE_Node_SetOnMovedCallback (SCE_SNode*, SCE_FNodeCallback, void*);

void SCE_Node_HasMoved (SCE_SNode*);
void SCE_Node_HasNotMoved (SCE_SNode*);
int SCE_Node_IsMoved (SCE_SNode*);

void SCE_Node_Force (SCE_SNode*);
void SCE_Node_DontForce (SCE_SNode*);
int SCE_Node_IsForced (SCE_SNode*);

void SCE_Node_UpdateRecursive (SCE_SNode*);
void SCE_Node_UpdateRootRecursive (SCE_SNode*);
void SCE_Node_FastUpdateRecursive (SCE_SNode*, unsigned int);
void SCE_Node_FastUpdateRootRecursive (SCE_SNode*, unsigned int);

int SCE_Node_HasParent (SCE_SNode*);
SCE_SNode* SCE_Node_GetParent (SCE_SNode*);

SCE_SOctreeElement* SCE_Node_GetElement (SCE_SNode*);

SCE_SList* SCE_Node_GetChildrenList (SCE_SNode*);

void SCE_Node_SetData (SCE_SNode*, void*);
#if 0
void* SCE_Node_GetData (SCE_SNode*);
#endif
#define SCE_Node_GetData(n) (((SCE_SNode*)(n))->data)

void SCE_Node_Use (SCE_SNode*) SCE_GNUC_DEPRECATED;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
