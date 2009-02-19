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
   updated: 18/02/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCECollide.h>
#include <SCE/interface/SCEOctree.h>

/**
 * \file SCEOctree.c
 * \copydoc octree
 * \file SCEOctree.h
 * \copydoc octree
 */

/**
 * \defgroup octree Octree managment
 * \ingroup interface
 * \brief Octree managment
 */

/** @{ */

#define SCE_OCTREE_HAVE_CHILDREN (1u)
#define SCE_OCTREE_VISIBLE (SCE_OCTREE_HAVE_CHILDREN << 1)
#define SCE_OCTREE_PARTIALLY (SCE_OCTREE_VISIBLE << 1)
#define SCE_OCTREE_LOOSE (SCE_OCTREE_PARTIALLY << 1)

static void SCE_Octree_Init (SCE_SOctree *tree)
{
    int i;
    for (i=0; i<8; i++)
        tree->child[i] = NULL;
    tree->marks = 0;
    tree->parent = NULL;
    SCE_BoundingBox_Init (&tree->box);
    tree->elements = NULL;
}

static void SCE_Octree_ElementHasNotOctree (void *elem)
{
    ((SCE_SOctreeElement*)elem)->octree = NULL;
}

/**
 * \brief Creates an octree
 * \returns a newly allocated octree or NULL on error.
 */
SCE_SOctree* SCE_Octree_Create (void)
{
    SCE_SOctree *tree = NULL;
    SCE_btstart ();
    if (!(tree = SCE_malloc (sizeof *tree)))
        goto failure;
    SCE_Octree_Init (tree);
    if (!(tree->elements = SCE_List_Create (SCE_Octree_ElementHasNotOctree)))
        goto failure;
    SCE_List_CanDeleteIterators (tree->elements, SCE_FALSE);
    goto success;
failure:
    SCE_Octree_Delete (tree), tree = NULL;
    Logger_LogSrc ();
success:
    SCE_btend ();
    return tree;
}
/**
 * \brief Clears an octree by removing recusivly all its chlidren
 * \sa SCE_Octree_DeleteRecursive(), SCE_Octree_Delete()
 */
void SCE_Octree_Clear (SCE_SOctree *tree)
{
    if (tree->marks & SCE_OCTREE_HAVE_CHILDREN)
    {
        unsigned int i;
        for (i = 0; i < 8; i++)
            tree->child[i]->parent = NULL;
    }
}
/**
 * \brief Deletes an octree
 * \param tree an octree to delete
 * 
 * This function deletes the given octree and detachs it of any eventual child.
 * \sa SCE_Octree_DeleteRecursive(), SCE_Octree_Clear()
 */
void SCE_Octree_Delete (SCE_SOctree *tree)
{
    if (tree)
    {
        SCE_Octree_Clear (tree);
        SCE_List_Delete (tree->elements);
        SCE_free (tree);
    }
}
/**
 * \brief Deletes an octree and all its children
 * \param tree an octree
 * 
 * This function deletes recursivly an octree.
 * \see SCE_Octree_Delete()
 */
void SCE_Octree_DeleteRecursive (SCE_SOctree *tree)
{
    if (tree)
    {
        if (tree->marks & SCE_OCTREE_HAVE_CHILDREN)
        {
            unsigned int i;
            for (i=0; i<8; i++)
                SCE_Octree_DeleteRecursive (tree->child[i]);
        }
        SCE_Octree_Delete (tree);
    }
}

static void SCE_Octree_InitElement (SCE_SOctreeElement *el)
{
#if SCE_LIST_ITERATOR_NO_MALLOC
    el->it = &el->iterator;
    SCE_List_InitIt (el->it);
#else
    el->it = NULL;
#endif
    el->octree = NULL;
    el->owner = NULL;
}

/**
 * \brief Creates a new octree element
 */
SCE_SOctreeElement* SCE_Octree_CreateElement (void)
{
    SCE_SOctreeElement *el = NULL;

    SCE_btstart ();
    if (!(el = SCE_malloc (sizeof *el)))
        goto failure;
    SCE_Octree_InitElement (el);
#if !SCE_LIST_ITERATOR_NO_MALLOC
    if (!(el->it = SCE_List_CreateIt ()))
        goto failure;
#endif
    SCE_List_SetData (el->it, el);
    goto success;
failure:
    SCE_Octree_DeleteElement (el), el = NULL;
    Logger_LogSrc ();
success:
    SCE_btend ();
    return el;
}

/**
 * \brief Deletes an octree element
 */
void SCE_Octree_DeleteElement (SCE_SOctreeElement *el)
{
    if (el)
    {
        if (el->octree)
            SCE_List_Remove (el->octree->elements, el->it);
#if !SCE_LIST_ITERATOR_NO_MALLOC
        SCE_List_DeleteIt (el->it);
#endif
        SCE_free (el);
    }
}


/**
 * \brief Sets the center point of an octree
 * \param tree an octree
 * \param x,y,z the new center's coordinates
 * \see SCE_Octree_SetCenterv()
 */
void SCE_Octree_SetCenter (SCE_SOctree *tree, float x, float y, float z)
{
    SCE_BoundingBox_SetCenter (&tree->box, x, y, z);
}
/**
 * \brief Sets the center point of an octree
 * \param tree an octree
 * \param c vector of the new center's coordinates
 * \see SCE_Octree_SetCenter()
 */
void SCE_Octree_SetCenterv (SCE_SOctree *tree, SCE_TVector3 c)
{
    SCE_BoundingBox_SetCenterv (&tree->box, c);
}
/**
 * \brief Gets the center point of an octree
 * \param tree an octree
 * \param c vector to store center's coordinates
 */
void SCE_Octree_GetCenterv (SCE_SOctree *tree, SCE_TVector3 c)
{
    SCE_BoundingBox_GetCenterv (&tree->box, c);
}
/**
 * \brief Sets the size of an octree
 * \param tree an octree
 * \param w,h,d new width, height and depth of the octree
 */
void SCE_Octree_SetSize (SCE_SOctree *tree, float w, float h, float d)
{
    SCE_BoundingBox_SetSize (&tree->box, w, h, d);
}
/**
 * \brief Gets the width of an octree
 * \param tree an octree
 * \returns the width of the given octree
 * \see SCE_Octree_GetHeight(), SCE_Octree_GetDepth()
 */
float SCE_Octree_GetWidth (SCE_SOctree *tree)
{
    return SCE_BoundingBox_GetWidth (&tree->box);
}
/**
 * \brief Gets the height of an octree
 * \param tree an octree
 * \returns the height of the given octree
 * \see SCE_Octree_GetWidth(), SCE_Octree_GetDepth()
 */
float SCE_Octree_GetHeight (SCE_SOctree *tree)
{
    return SCE_BoundingBox_GetHeight (&tree->box);
}
/**
 * \brief Gets the depth of an octree
 * \param tree an octree
 * \returns the depth of the given octree
 * \see SCE_Octree_GetWidth(), SCE_Octree_GetHeight()
 */
float SCE_Octree_GetDepth (SCE_SOctree *tree)
{
    return SCE_BoundingBox_GetDepth (&tree->box);
}

/**
 * \brief Does \p tree have children?
 */
int SCE_Octree_HaveChildren (SCE_SOctree *tree)
{
    return tree->marks & SCE_OCTREE_HAVE_CHILDREN;
}
/**
 * \brief Does \p tree is visible?
 */
int SCE_Octree_IsVisible (SCE_SOctree *tree)
{
    return tree->marks & SCE_OCTREE_VISIBLE;
}
/**
 * \brief Does \p tree is partially visible?
 */
int SCE_Octree_IsPartiallyVisible (SCE_SOctree *tree)
{
    return tree->marks & SCE_OCTREE_PARTIALLY;
}

/**
 * \brief Gets the chlidren of an octree
 * \param tree an octree
 * \return an array of the given octree's chlidren
 */
SCE_SOctree** SCE_Octree_GetChildren (SCE_SOctree *tree)
{
    return tree->child;
}

/**
 * \brief Creates the eight chlidren of an octree
 * \param tree an octree
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * 
 * This function creates the eight children of the given octree.
 * \sa SCE_Octree_RecursiveMake()
 */
int SCE_Octree_MakeChildren (SCE_SOctree *tree, int useloose, float ratio)
{
    unsigned int i;
    SCE_TVector3 origins[8];
    float *origin = SCE_BoundingBox_GetOrigin (&tree->box);
    float w = SCE_BoundingBox_GetWidth (&tree->box);
    float h = SCE_BoundingBox_GetHeight (&tree->box);
    float d = SCE_BoundingBox_GetDepth (&tree->box);
    float w2 = w / 2.0;
    float h2 = h / 2.0;
    float d2 = 0.0;

    for (i = 0; i < 8; i += 4)
    {
        SCE_Vector3_Copy (origins[i], origin);
        origins[i][2] += d2 * (1.0 - ratio);
        SCE_Vector3_Copy (origins[i+1], origin);
        origins[i+1][0] += w2 * (1.0 - ratio);
        origins[i+1][2] += d2 * (1.0 - ratio);
        SCE_Vector3_Copy (origins[i+2], origin);
        origins[i+2][1] += h2 * (1.0 - ratio);
        origins[i+2][2] += d2 * (1.0 - ratio);
        SCE_Vector3_Copy (origins[i+3], origin);
        origins[i+3][0] += w2 * (1.0 - ratio);
        origins[i+3][1] += h2 * (1.0 - ratio);
        origins[i+3][2] += d2 * (1.0 - ratio);
        d2 = d / 2.;
    }
    for (i = 0; i < 8; i++)
    {
        tree->child[i] = SCE_Octree_Create ();
        if (!tree->child[i])
        {
            Logger_LogSrc ();
            return SCE_ERROR;
        }
        tree->child[i]->parent = tree;
        w2 *= (1.0 + ratio);
        h2 *= (1.0 + ratio);
        d2 *= (1.0 + ratio);
        SCE_BoundingBox_Set (&tree->child[i]->box, origins[i], w2, h2, d2);
    }
    tree->marks |= SCE_OCTREE_HAVE_CHILDREN;
    if (useloose)
        tree->marks |= SCE_OCTREE_LOOSE;
    return SCE_OK;
}

/**
 * \brief Creates octree's children recursivly
 * \param tree an octree
 * \param rec maximum recursions
 * \param stop a function to determines when stop children generation
 * (can be NULL)
 * \param param user data to pass to the \p stop function
 * \param useloose do we'll use loose octrees?
 * \param ratio ratio for extended
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * 
 * This function creates octrees's children recursivly. The recursion depth is
 * controlled by the \p stop function.
 * \see SCE_FOctreeLimitFunc, SCE_Octree_MakeChildren()
 */
int SCE_Octree_RecursiveMake (SCE_SOctree *tree, unsigned int rec,
                              SCE_FOctreeLimitFunc stop, void *param,
                              int useloose, float ratio)
{
    if ((!stop || !stop (tree, param)) && rec != 0)
    {
        unsigned int i;
        if (SCE_Octree_MakeChildren (tree, useloose, ratio) < 0)
        {
            Logger_LogSrc ();
            return SCE_ERROR;
        }
        for (i = 0; i < 8; i++)
        {
            if (SCE_Octree_RecursiveMake (tree->child[i], rec-1,
                                          stop, param, useloose, ratio) < 0)
            {
                Logger_LogSrc ();
                return SCE_ERROR;
            }
        }
    }
    return SCE_OK;
}

void SCE_Octree_SetElementOwner (SCE_SOctreeElement *el, void *owner)
{
    el->owner = owner;
}
void* SCE_Octree_GetElementOwner (SCE_SOctreeElement *el)
{
    return el->owner;
}
int SCE_Octree_IsElementVisible (SCE_SOctreeElement *el)
{
    return el->octree->marks & SCE_OCTREE_VISIBLE;
}
int SCE_Octree_IsElementPartiallyVisible (SCE_SOctreeElement *el)
{
    return el->octree->marks & SCE_OCTREE_PARTIALLY;
}

void SCE_Octree_SetElementBoundingSphere (SCE_SOctreeElement *el,
                                          SCE_SBoundingSphere *sphere)
{
    el->sphere = sphere;
}
SCE_SBoundingSphere* SCE_Octree_GetElementBoundingSphere(SCE_SOctreeElement *el)
{
    return el->sphere;
}

/**
 * \brief Inserts an element into an octree
 * \param tree an octree
 * \param el the element to insert
 * \param in indicates if \p el is already in the box of \p tree
 * \returns SCE_ERROR when \p el is out of \p tree, SCE_OK otherwise
 * \sa SCE_Octree_ReinsertElement()
 */
int SCE_Octree_InsertElement (SCE_SOctree *tree, SCE_SOctreeElement *el, int in)
{
    if (tree->marks & SCE_OCTREE_HAVE_CHILDREN)
    {
        unsigned int i;

        if (tree->marks & SCE_OCTREE_LOOSE)
        {
            for (i = 0; i < 8; i++)
            {
                if (SCE_Collide_AABBWithBS (&tree->child[i]->box, el->sphere)
                    == SCE_COLLIDE_IN)
                    return SCE_Octree_InsertElement(tree->child[i],el,SCE_TRUE);
            }
            goto alone;
        }
        else
        {
            for (i = 0; i < 8; i++)
            {
                switch (SCE_Collide_AABBWithBS(&tree->child[i]->box,el->sphere))
                {
                case SCE_COLLIDE_IN:
                    return SCE_Octree_InsertElement(tree->child[i],el,SCE_TRUE);
                case SCE_COLLIDE_PARTIALLY:
                    goto alone;
                }
            }
        }
    }
    else
    {
    alone:
        if (in ||
            SCE_Collide_AABBWithBS (&tree->box, el->sphere) == SCE_COLLIDE_IN)
        {
            if (el->octree)
                SCE_List_Remove (el->octree->elements, el->it);
            SCE_List_Prependl (tree->elements, el->it);
            el->octree = tree;
        }
        else
            return SCE_ERROR;
    }
    return SCE_OK;
}
/**
 * \brief Re-inserts an element into an octree
 * \param el the element to re-insert
 * \returns SCE_ERROR when \p el is out of its old octree, SCE_OK otherwise
 *
 * This function works like SCE_Octree_InsertElement() except that use the
 * current one octree where \p el is contained and finds its parent(s) when is
 * necessary. SCE_ERROR is returned if the root parent doesn't contains \p el.
 * \sa SCE_Octree_InsertElement()
 */
int SCE_Octree_ReinsertElement (SCE_SOctreeElement *el)
{
    SCE_SOctree *parent = el->octree;
#if 1
    while (parent->parent)
        parent = parent->parent;
    return SCE_Octree_InsertElement (parent, el, SCE_FALSE);
#else
    do
    {
        if (SCE_Collide_AABBWithBS (&parent->box, el->sphere) ==
            SCE_COLLIDE_IN)
            return SCE_Octree_InsertElement (parent, el, SCE_TRUE);
        parent = parent->parent;
    }
    while (parent);
    return SCE_ERROR;
#endif
}

/**
 * \brief Removes an element from its octree
 */
void SCE_Octree_RemoveElement (SCE_SOctreeElement *el)
{
    if (el->octree)
        SCE_List_Remove (el->octree->elements, el->it);
}


static void SCE_Octree_RecMark (SCE_SOctree *tree, int mark)
{
    tree->marks =
        mark | (tree->marks & SCE_OCTREE_HAVE_CHILDREN) |
        (tree->marks & SCE_OCTREE_LOOSE);
    if (tree->marks & SCE_OCTREE_HAVE_CHILDREN)
    {
        unsigned int i;
        for (i = 0; i < 8; i++)
            SCE_Octree_RecMark (tree->child[i], mark);
    }
}

/**
 * \brief Marks the visible octrees of \p tree from the camera \p cam
 * \sa SCE_Frustum_BoundingBoxIn()
 */
void SCE_Octree_MarkVisibles (SCE_SOctree *tree, SCE_SCamera *cam)
{
    int state = SCE_Frustum_BoundingBoxIn (SCE_Camera_GetFrustum (cam),
                                           &tree->box);
    tree->marks |= SCE_OCTREE_VISIBLE;
    tree->marks |= SCE_OCTREE_PARTIALLY;
    if (state == SCE_COLLIDE_OUT)
        SCE_Octree_RecMark (tree, 0);
    else
    {
        if (state == SCE_COLLIDE_IN)
            SCE_Octree_RecMark (tree, SCE_OCTREE_VISIBLE);
        else if (tree->marks & SCE_OCTREE_HAVE_CHILDREN)
        {
            unsigned int i;
            for (i = 0; i < 8; i++)
                SCE_Octree_MarkVisibles (tree->child[i], cam);
            tree->marks |= SCE_OCTREE_PARTIALLY;
        }
    }
}

/** @} */
