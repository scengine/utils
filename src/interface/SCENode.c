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
    GNU General Public License for more details

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -----------------------------------------------------------------------------*/
 
/* created: 10/07/2007
   updated: 01/02/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEListFastForeach.h>
#include <SCE/core/SCECMatrix.h>
#include <SCE/interface/SCENode.h>

/**
 * \file SCENode.c
 * \copydoc node
 * \file SCENode.h
 * \copydoc node
 */

/**
 * \defgroup node Nodes managment
 * \ingroup interface
 * \brief Nodes and node trees managment
 */

/** @{ */

#define SCE_NODE_HAS_MOVED (1u)
#define SCE_NODE_FORCE (SCE_NODE_HAS_MOVED << 1)

/* used to kick "if" instructions. it's more probably that the nodes have
   user-defined callbacks, so an useless "if" can be kicked */
static void SCE_Node_NullFunc (SCE_SNode *n, void *p)
{
}

static void SCE_Node_Init (SCE_SNode *node)
{
    node->parent = NULL;
    node->child = NULL;
    node->element = NULL;
#if SCE_LIST_ITERATOR_NO_MALLOC
    node->it = &node->iterator;
    SCE_List_InitIt (node->it);
#else
    node->it = NULL;
#endif
    SCE_List_SetData (node->it, node);
    SCE_Matrix4_Identity (node->matrix);
    SCE_Matrix4_Identity (node->fmatrix);
    node->data = NULL;
    node->marks = 0;
    node->moved = SCE_Node_NullFunc;
    node->movedparam = NULL;
}

static void SCE_Node_YouDontHaveParent (void *n)
{
    SCE_SNode *node = n;
    node->parent = NULL;
}

/**
 * \brief Creates a new node
 * \returns a newly allocated SCE_SNode on success or NULL on error
 */
SCE_SNode* SCE_Node_Create (void)
{
    SCE_SNode *node = NULL;
    if (!(node = SCE_malloc (sizeof *node)))
        goto failure;
    SCE_Node_Init (node);
    if (!(node->child = SCE_List_Create (SCE_Node_YouDontHaveParent)))
        goto failure;
    if (!(node->element = SCE_Octree_CreateElement ()))
        goto failure;
#if !SCE_LIST_ITERATOR_NO_MALLOC
    if (!(node->it = SCE_List_CreateIt ()))
        goto failure;
#endif
    /* by default, the data of the element is the node */
    SCE_List_SetData (&node->element->it, node);
    goto success;
failure:
    SCE_Node_Delete (node), node = NULL;
    Logger_LogSrc ();
success:
    return node;
}

/**
 * \brief Deletes a node
 * \param node a node
 * \note This function doesn't do anything with the node's data.
 */
void SCE_Node_Delete (SCE_SNode *node)
{
    if (node)
    {
        SCE_Node_Detach (node);
#if !SCE_LIST_ITERATOR_NO_MALLOC
        SCE_List_DeleteIt (node->it);
#endif
        SCE_List_Delete (node->child);
        SCE_free (node);
    }
}
/**
 * \brief Deletes a node and all its children
 * \param node a node
 * 
 * This function deletes a node recursivly.
 * \sa SCE_Node_Delete()
 */
void SCE_Node_DeleteRecursive (SCE_SNode *node)
{
    if (node)
    {
        SCE_SListIterator *i = NULL;
        SCE_List_ForEach (i, node->child)
            SCE_Node_DeleteRecursive (SCE_List_GetData (i));
        SCE_Node_Delete (node);
    }
}


/**
 * \brief Attaches a node to another
 * \param node a node
 * \param child a node to make child of \p node
 * \returns SCE_ERROR on error, SCE_OK otherwise.
 * 
 * This function makes the \p child node child of the \p node node. If the \p
 * child node has already a parent node, it will be detached before from its
 * current parent.
 * 
 * \warning If this function fails, the node will became orphean, even if it
 *          has a parent before.
 */
void SCE_Node_Attach (SCE_SNode *node, SCE_SNode *child)
{
    SCE_Node_Detach (child);
    SCE_List_Prependl (node->child, child->it);
    child->parent = node;
    SCE_Node_HasMoved (child);
}

/**
 * \brief Creates a new child to a node
 * \param node a node to which add a child
 * \returns the new child of \p node of NULL on error
 * 
 * This function creates a new node and attaches it as a child of the given
 * node.
 * \see SCE_Node_Create(), SCE_Node_Attach()
 */
SCE_SNode* SCE_Node_AttachNew (SCE_SNode *node)
{
    SCE_SNode *new = NULL;
    if (!(new = SCE_Node_Create ()))
    {
        Logger_LogSrc ();
        return NULL;
    }
    SCE_Node_Attach (node, new);
    return new;
}

/**
 * \brief Detaches a node from its parent
 * \param node the node to detach
 */
void SCE_Node_Detach (SCE_SNode *node)
{
    if (node->parent)
    {
        SCE_List_Removel (node->it);
        node->parent = NULL;
        SCE_Node_HasMoved (node);
    }
}

/**
 * \brief Inserts a node between another and its parent
 * \param node the node to assert
 * \param child 
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * Inserts \p node between \p child and its parent. If \p child hasn't a parent,
 * this functions attach \p child to \p node.
 * \sa SCE_Node_Attach()
 */
void SCE_Node_Insert (SCE_SNode *node, SCE_SNode *child)
{
    if (child->parent)
        SCE_Node_Attach (child->parent, node);
    SCE_Node_Attach (node, child);
}


/**
 * \brief Calls SCE_CMultMatrix() with the final matrix of the given node
 */
void SCE_Node_MultMatrix (SCE_SNode *node)
{
    SCE_CMultMatrix (node->fmatrix);
}
/**
 * \brief Calls SCE_CLoadMatrix() with the final matrix of the given node
 */
void SCE_Node_LoadMatrix (SCE_SNode *node)
{
    SCE_CLoadMatrix (node->fmatrix);
}

/**
 * \brief Gets the node's matrix
 * \returns the matrix of the given node
 * 
 * This function gets the matrix of the given node. This gets the node's matrix
 * without any modification of its parent nodes. If you want to get the fully
 * transformed node's matrix, use SCE_Node_GetFinalMatrix() instead of this
 * function. This function declare \p node as non-updated by calling
 * SCE_Node_IsNotUpdated().
 * \see SCE_Node_GetFinalMatrix(), SCE_Node_CopyMatrix()
 */
float* SCE_Node_GetMatrix (SCE_SNode *node)
{
    return node->matrix;
}
/**
 * \brief Copies the node's matrix
 * \param node a node
 * \param m a matrix where copy the node's matrix
 * 
 * See SCE_Node_GetMatrix() for more details about what is the node's matrix.
 * \see SCE_Node_CopyFinalMatrix(), SCE_Node_GetMatrix()
 */
void SCE_Node_CopyMatrix (SCE_SNode *node, SCE_TMatrix4 m)
{
    SCE_Matrix4_Copy (m, node->matrix);
}

/**
 * \brief Gets the real node's matrix
 * \param node a node
 * \returns the real node's matrix
 * 
 * This function gets the matrix of the given node with all parent node's matrix
 * transformations applied. This is the real transformation matrix for the node,
 * not only the node's one.
 * If you just want the node's transofrmation matrix, without parent
 * transformations, use SCE_Node_GetMatrix()
 * \see SCE_Node_GetMatrix(), SCE_Node_CopyFinalMatrix()
 */
float* SCE_Node_GetFinalMatrix (SCE_SNode *node)
{
    return node->fmatrix;
}
/**
 * \brief Copies the real node's matrix
 * \param node a node
 * \param m a matrix where copy the real node's matrix
 * 
 * See SCE_Node_GetFinalMatrix() for more details about what is the real node's
 * matrix.
 * \see SCE_Node_GetFinalMatrix()
 */
void SCE_Node_CopyFinalMatrix (SCE_SNode *node, SCE_TMatrix4 m)
{
    SCE_Matrix4_Copy (m, node->fmatrix);
}

/**
 * \brief 
 * \param node a node
 * \param f the callback function
 * \param p user-defined parameter to send to \p f
 */
void SCE_Node_SetOnMovedCallback (SCE_SNode *node, SCE_FNodeCallback f, void *p)
{
    if (f)
        node->moved = f;
    else
        node->moved = SCE_Node_NullFunc;
    node->movedparam = p;
}

/**
 * \brief Marks the node \p node and its children
 * as a moved node since the last update
 */
void SCE_Node_HasMoved (SCE_SNode *node)
{
    SCE_SListIterator *it;
    node->marks |= SCE_NODE_HAS_MOVED;
    SCE_List_ForEach (it, node->child)
        SCE_Node_HasMoved (SCE_List_GetData (it));
}
/**
 * \brief Defines the node \p node as an unmoved node since the last update
 */
void SCE_Node_HasNotMoved (SCE_SNode *node)
{
    node->marks |= SCE_NODE_HAS_MOVED;
    node->marks ^= SCE_NODE_HAS_MOVED;
}
/**
 * \brief Indicates if the given node has moved since the last update
 */
int SCE_Node_IsMoved (SCE_SNode *node)
{
    return node->marks & SCE_NODE_HAS_MOVED;
}

/**
 * \brief Forces the updating of \p node and its children
 */
void SCE_Node_Force (SCE_SNode *node)
{
    SCE_SListIterator *it;
    node->marks |= SCE_NODE_FORCE;
    SCE_List_ForEach (it, node->child)
        SCE_Node_Force (SCE_List_GetData (it));
}
/**
 * \brief Disables the update forcing of \p node
 */
void SCE_Node_DontForce (SCE_SNode *node)
{
    node->marks |= SCE_NODE_FORCE;
    node->marks ^= SCE_NODE_FORCE;
}
/**
 * \brief Have we forced the updating of \p node?
 */
int SCE_Node_IsForced (SCE_SNode *node)
{
    return node->marks & SCE_NODE_FORCE;
}


/**
 * \internal
 * \brief Updates a node by multiplying its matrix with the final matrix of its
 * parent
 * \param node the node to update
 * \see SCE_Node_UpdateRecursive()
 */
static void SCE_Node_Update (SCE_SNode *node)
{
    if (node->marks)
    {
        SCE_Matrix4_Mul (node->parent->fmatrix, node->matrix, node->fmatrix);
        if (node->marks & SCE_NODE_HAS_MOVED)
            node->moved (node, node->movedparam);
        node->marks = 0;
    }
}

/**
 * \brief Updates a node and its children
 * \param node the node to update recursivly
 * \see SCE_Node_UpdateRootRecursive(), SCE_Node_FastUpdateRecursive()
 */
void SCE_Node_UpdateRecursive (SCE_SNode *node)
{
    SCE_SListIterator *i = NULL;
    SCE_Node_Update (node);
    SCE_List_ForEach (i, node->child)
        SCE_Node_UpdateRecursive (SCE_List_GetData (i));
}

/**
 * \internal
 * \brief Updates a root node of a tree
 * \param node the node to update
 * \see SCE_Node_UpdateRootRecursive()
 */
static void SCE_Node_UpdateRoot (SCE_SNode *node)
{
    if (node->marks)
    {
        SCE_Matrix4_Copy (node->fmatrix, node->matrix);
        /* TODO */
        if (node->marks & SCE_NODE_HAS_MOVED)
            node->moved (node, node->movedparam);
        node->marks = 0;
    }
}

/**
 * \brief Updates a root node and its children
 * \param node the node to update recursivly
 * \see SCE_Node_UpdateRecursive(), SCE_Node_FastUpdateRootRecursive()
 */
void SCE_Node_UpdateRootRecursive (SCE_SNode *node)
{
    SCE_SListIterator *i = NULL;
    SCE_Node_UpdateRoot (node);
    SCE_List_ForEach (i, node->child)
        SCE_Node_UpdateRecursive (SCE_List_GetData (i));
}

static void SCE_Node_FastRec (void *node)
{
    SCE_SListIterator *i = NULL;
    SCE_Node_Update (node);
    SCE_List_ForEach (i, ((SCE_SNode*)node)->child)
        SCE_Node_UpdateRecursive (SCE_List_GetData (i));
}
/**
 * \brief Updates a node and its children
 * \param node the node to update recursivly
 * \see SCE_Node_UpdateRecursive(), SCE_Node_FastUpdateRootRecursive()
 */
void SCE_Node_FastUpdateRecursive (SCE_SNode *node, unsigned int n)
{
    SCE_Node_Update (node);
    SCE_List_FastForEach4 (node->child, n, SCE_Node_FastRec);
}
/**
 * \brief Updates a node and all its child
 * \param node the node to update recursivly
 * \see SCE_Node_FastUpdateRecursive(), SCE_Node_UpdateRootRecursive()
 */
void SCE_Node_FastUpdateRootRecursive (SCE_SNode *node, unsigned int n)
{
    SCE_Node_UpdateRoot (node);
    SCE_List_FastForEach4 (node->child, n, SCE_Node_FastRec);
}


/**
 * \brief Checks if a node has a parent
 * \param node a node
 * \returns SCE_TRUE if the \p node node have a parent, SCE_FALSE otherwise.
 */
int SCE_Node_HasParent (SCE_SNode *node)
{
    return (node->parent ? SCE_TRUE : SCE_FALSE);
}

/**
 * \brief Gets the parent of a node
 * \returns the parent node of \p node
 */
SCE_SNode* SCE_Node_GetParent (SCE_SNode *node)
{
    return node->parent;
}

/**
 * \brief Returns the element of a node
 * \sa SCE_SNode::element
 */
SCE_SOctreeElement* SCE_Node_GetElement (SCE_SNode *node)
{
    return node->element;
}

/**
 * \brief Sets data of a node
 * \param data the data to set to \p node
 */
void SCE_Node_SetData (SCE_SNode *node, void *data)
{
    node->data = data;
    SCE_List_SetData (&node->element->it, data);
}

#if 0
/**
 * \brief Gets data of a node
 * \returns the data of the \p node node
 */
void* SCE_Node_GetData (SCE_SNode *node)
{
    return node->data;
}
#endif


/**
 * \deprecated
 * \brief Calls SCE_CLoadIdentityMatrix() if \p node is null,
 *  SCE_Node_MultMatrix() otherwise
 */
void SCE_Node_Use (SCE_SNode *node)
{
    if (node)
        SCE_Node_MultMatrix (node);
    else
        SCE_CLoadIdentityMatrix ();
}

/** @} */
