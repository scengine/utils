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
   updated: 01/11/2009 */

#ifndef SCENODE_H
#define SCENODE_H

#include <SCE/SCECommon.h>
#include <SCE/utils/SCEVector.h>
#include <SCE/utils/SCEMatrix.h>
#include <SCE/utils/SCEList.h>
#include <SCE/interface/SCEOctree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup node
 * @{
 */

typedef enum {
    /** \brief Those nodes have SCE_SNode::data pointing to a float that is
     * their matrix */
    SCE_SINGLE_MATRIX_NODE,
    /** \brief Those nodes are designed for being used into a tree structure */
    SCE_TREE_NODE
} SCE_ENodeType;


/** \copydoc sce_snode */
typedef struct sce_snode SCE_SNode;
typedef float* (*SCE_FGetNodeMatrix)(SCE_SNode*);
typedef void (*SCE_FNodeCallback)(SCE_SNode*, void*);
typedef void (*SCE_FNodeCallback2)(SCE_SNode*);

typedef struct sce_snodedata SCE_SNodeData;
struct sce_snodedata {
    SCE_TMatrix4 matrix;       /**< Node matrix */
    SCE_TMatrix4 fmatrix;      /**< Real node's transformation matrix, with all
                                *   parent nodes' matrices applyed */
};

/**
 * \brief Node definition structure
 */
struct sce_snode {
    SCE_SOctreeElement *element; /**< Element */
    SCE_SNode *parent;         /**< Parent node */
    /* yes: even a single matrix node can has children! */
    SCE_SList child;           /**< Children */
    SCE_SList toupdate;        /**< Children to update! */
    SCE_FGetNodeMatrix getmat; /**< Get matrix function */
    SCE_FGetNodeMatrix getfmat;/**< Get final matricx function */
    SCE_SNodeData *data;       /**< pointer to a float* in case of single
                                * matrix node type */
    size_t matrix;             /**< Offset of the matrix into data
                                * (if non-zero then the node is of type
                                * SCE_SINGLE_MATRIX_NODE) */
    SCE_SListIterator it, it2; /**< Own iterator */
    unsigned int marks;        /**< Has the node moved since the last update? */
    /* \deprecated */ /* TODO: kick that */
    SCE_FNodeCallback moved;   /**< Called when the node has moved */
    void *movedparam;          /**< \c moved parameter */
    void *udata;               /**< User-defined data */
};

typedef struct sce_snodegroup SCE_SNodeGroup;
struct sce_snodegroup {
    float **arrays;
    size_t n_arrays;
    SCE_SList nodes;
};

/** @} */

SCE_SNode* SCE_Node_Create (SCE_ENodeType);
void SCE_Node_Delete (SCE_SNode*);
void SCE_Node_DeleteRecursive (SCE_SNode*);

SCE_SNodeGroup* SCE_Node_CreateGroup (void);
void SCE_Node_DeleteGroup (SCE_SNodeGroup*);

void SCE_Node_AddNode (SCE_SNodeGroup*, SCE_SNode*);
void SCE_Node_RemoveNode (SCE_SNodeGroup*, SCE_SNode*);

SCE_ENodeType SCE_Node_GetType (SCE_SNode*);

void SCE_Node_Attach (SCE_SNode*, SCE_SNode*);
SCE_SNode* SCE_Node_AttachNew (SCE_SNode*);
void SCE_Node_Detach (SCE_SNode*);

void SCE_Node_Insert (SCE_SNode*, SCE_SNode*);

void SCE_Node_MultMatrix (SCE_SNode*);
void SCE_Node_LoadMatrix (SCE_SNode*);

float* SCE_Node_GetSingleMatrix (SCE_SNode*);
float* SCE_Node_GetTreeMatrix (SCE_SNode*);
float* SCE_Node_GetTreeFinalMatrix (SCE_SNode*);

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
#if 0
void SCE_Node_FastUpdateRecursive (SCE_SNode*, unsigned int);
void SCE_Node_FastUpdateRootRecursive (SCE_SNode*, unsigned int);
#endif

int SCE_Node_HasParent (SCE_SNode*);
SCE_SNode* SCE_Node_GetParent (SCE_SNode*);

SCE_SOctreeElement* SCE_Node_GetElement (SCE_SNode*);

SCE_SList* SCE_Node_GetChildrenList (SCE_SNode*);

void SCE_Node_SetData (SCE_SNode*, void*);
#if 1
void* SCE_Node_GetData (SCE_SNode*);
#else
#define SCE_Node_GetData(n) (((SCE_SNode*)(n))->udata)
#endif

void SCE_Node_Use (SCE_SNode*) SCE_GNUC_DEPRECATED;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
