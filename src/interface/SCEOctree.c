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
   updated: 20/06/2009 */

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

static void SCE_Octree_InsertLoose (SCE_SOctree*, SCE_SOctreeElement*);
static void SCE_Octree_InsertNormal (SCE_SOctree*, SCE_SOctreeElement*);
static void SCE_Octree_Insert (SCE_SOctree*, SCE_SOctreeElement*);

static void SCE_Octree_Init (SCE_SOctree *tree)
{
    int i;
    for (i=0; i<8; i++)
        tree->child[i] = NULL;
    tree->visible = SCE_FALSE;
    tree->partially = SCE_FALSE;
    tree->insert = SCE_Octree_Insert;
    tree->parent = NULL;
    SCE_BoundingBox_Init (&tree->box);
    tree->elements = NULL;
    tree->data = NULL;
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
    if (!(tree->elements = SCE_List_Create (NULL)))
        goto failure;
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
    if (tree->child[0])
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
        unsigned int i;
        for (i = 0; i < 8; i++)
        {
            SCE_Octree_DeleteRecursive (tree->child[i]);
            tree->child[i] = NULL;
        }
        SCE_Octree_Delete (tree);
    }
}

/**
 * \brief Initializes an octree element
 */
void SCE_Octree_InitElement (SCE_SOctreeElement *el)
{
    SCE_List_InitIt (&el->it);
    el->insert = SCE_Octree_DefaultInsertFunc;
    el->octree = NULL;
    el->sphere = NULL;
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
        SCE_Octree_RemoveElement (el);
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
 * \brief Returns the bounding box of an octree
 */
SCE_SBoundingBox* SCE_Octree_GetBoundingBox (SCE_SOctree *tree)
{
    return &tree->box;
}

/**
 * \brief Sets user defined data to an octree
 */
void SCE_Octree_SetData (SCE_SOctree *tree, void *data)
{
    tree->data = data;
}
/**
 * \brief Gets user defined data from an octree
 */
void* SCE_Octree_GetData (SCE_SOctree *tree)
{
    return tree->data;
}

/**
 * \brief Is \p tree visible?
 */
int SCE_Octree_IsVisible (SCE_SOctree *tree)
{
    return tree->visible;
}
/**
 * \brief Is \p tree partially visible?
 */
int SCE_Octree_IsPartiallyVisible (SCE_SOctree *tree)
{
    return tree->partially;
}
/**
 * \brief Gets the recursion level of an octree
 */
unsigned int SCE_Octree_GetLevel (SCE_SOctree *tree)
{
    unsigned int level = 0;
    while (tree->parent)
    {
        tree = tree->parent;
        level++;
    }
    return level;
}

/**
 * \brief Is \p tree having children?
 */
int SCE_Octree_HasChildren (SCE_SOctree *tree)
{
    return (tree->child[0] ? SCE_TRUE : SCE_FALSE);
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
 * \brief Creates the eight children of an octree
 * \param tree an octree
 * \param useloose use loose octrees?
 * \param ratio extend ratio for loose octrees
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
    if (useloose)
        tree->insert = SCE_Octree_InsertLoose;
    else
        tree->insert = SCE_Octree_InsertNormal;
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


/**
 * \brief Default function used to insert an element into an octree
 * \sa SCE_SOctreeElement::insert
 */
void SCE_Octree_DefaultInsertFunc (SCE_SOctree *tree, SCE_SOctreeElement *el)
{
    SCE_List_Prependl (tree->elements, &el->it);
}

static void SCE_Octree_InsertLoose (SCE_SOctree *tree, SCE_SOctreeElement *el)
{
    unsigned int i;
    for (i = 0; i < 8; i++)
    {
        if (SCE_Collide_AABBWithBS (&tree->child[i]->box, el->sphere)
            == SCE_COLLIDE_IN)
        {
            tree->child[i]->insert (tree->child[i], el);
            return;
        }
    }
    SCE_Octree_Insert (tree, el);
}

static void SCE_Octree_InsertNormal (SCE_SOctree *tree, SCE_SOctreeElement *el)
{
    unsigned int i;
    int r;
    for (i = 0; i < 8; i++)
    {
        r = SCE_Collide_AABBWithBS (&tree->child[i]->box, el->sphere);
        if (r == SCE_COLLIDE_PARTIALLY)
            break;
        else if (r == SCE_COLLIDE_IN)
        {
            tree->child[i]->insert (tree->child[i], el);
            return;
        }
    }
    SCE_Octree_Insert (tree, el);
}

static void SCE_Octree_Insert (SCE_SOctree *tree, SCE_SOctreeElement *el)
{
    el->insert (tree, el);
    el->octree = tree;
}

/**
 * \brief Inserts an element into an octree
 * \param tree an octree
 * \param el the element to insert
 * \note \p el must have been previously added by SCE_Octree_AddElement()
 * \sa SCE_Octree_ReinsertElement(), SCE_Octree_RemoveElement()
 */
void SCE_Octree_InsertElement (SCE_SOctree *tree, SCE_SOctreeElement *el)
{
/*    SCE_Octree_RemoveElement (el); */
    tree->insert (tree, el);
}
/**
 * \brief Re-inserts an element into an octree
 * \param el the element to re-insert
 *
 * This function works like SCE_Octree_InsertElement() except that use the
 * current one octree where \p el is contained and finds its parent(s) when it's
 * necessary.
 * \note \p el must have been previously added by SCE_Octree_InsertElement()
 * \sa SCE_Octree_InsertElement(), SCE_Octree_RemoveElement()
 */
void SCE_Octree_ReinsertElement (SCE_SOctreeElement *el)
{
    SCE_SOctree *parent = el->octree;
    do
    {
        if (SCE_Collide_AABBWithBS (&parent->box, el->sphere) ==
            SCE_COLLIDE_IN)
        {
            SCE_List_Removel (&el->it);
            SCE_Octree_InsertElement (parent, el);
            break;
        }
        parent = parent->parent;
    }
    while (parent);
#ifdef SCE_DEBUG
    if (!parent)
    {
        /* element hasn't be inserted lol. */
        Logger_PrintMsg ("octree element reinsertion failure: out of the box!");
    }
#endif
}
/**
 * \brief Removes an element from its octree (SCE_Octree_ReinsertElement() will
 * not works after that)
 */
void SCE_Octree_RemoveElement (SCE_SOctreeElement *el)
{
    if (el->octree)
    {
        SCE_List_Removel (&el->it);
        el->octree = NULL;
    }
}


/* TODO: useless... ? */
static void SCE_Octree_RecMark (SCE_SOctree *tree, int visible, int partially)
{
    tree->visible = visible;
    tree->partially = partially;
    /* useless... ? */
#if 0
    if (tree->child[0])
    {
        unsigned int i;
        for (i = 0; i < 8; i++)
            SCE_Octree_RecMark (tree->child[i], visible, partially);
    }
#endif
}

/**
 * \brief Marks the visible octrees of \p tree from the frustum \p frustum
 * \sa SCE_Frustum_BoundingBoxIn()
 */
void SCE_Octree_MarkVisibles (SCE_SOctree *tree, SCE_SFrustum *frustum)
{
    int state = SCE_Frustum_BoundingBoxIn (frustum, &tree->box);
    if (state == SCE_COLLIDE_OUT)
        SCE_Octree_RecMark (tree, SCE_FALSE, SCE_FALSE);
    else if (state == SCE_COLLIDE_IN)
        SCE_Octree_RecMark (tree, SCE_TRUE, SCE_FALSE);
    else
    {
        if (tree->child[0])
        {
            unsigned int i;
            for (i = 0; i < 8; i++)
                SCE_Octree_MarkVisibles (tree->child[i], frustum);
        }
        tree->visible = SCE_TRUE;
        tree->partially = SCE_TRUE;
    }
}

/** @} */
