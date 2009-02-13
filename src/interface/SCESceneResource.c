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
 
/* created: 03/11/2008 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCESceneResource.h>


void SCE_SceneResource_Init (SCE_SSceneResource *res)
{
    res->resource = NULL;
    res->owners = NULL;
    res->group = NULL;
    res->removed = SCE_TRUE;
#if SCE_LIST_ITERATOR_NO_MALLOC
    SCE_List_InitIt (&res->iterator);
    res->it = &res->iterator;
#else
    res->it = NULL;
#endif
}

SCE_SSceneResource* SCE_SceneResource_Create (void)
{
    SCE_SSceneResource *res = NULL;

    SCE_btstart ();
    if (!(res = SCE_malloc (sizeof *res)))
        goto failure;
    SCE_SceneResource_Init (res);
    if (!(res->owners = SCE_List_Create (NULL)))
        goto failure;
#if !SCE_LIST_ITERATOR_NO_MALLOC
    if (!(res->it = SCE_List_CreateIt ()))
        goto failure;
#endif
    /* for compatibility with group functions */
    SCE_List_SetData (res->it, res);
    goto success;

failure:
    SCE_SceneResource_Delete (res), res = NULL;
    Logger_LogSrc ();
success:
    SCE_btend ();
    return res;
}

void SCE_SceneResource_Delete (SCE_SSceneResource *res)
{
    if (res)
    {
        SCE_SceneResource_RemoveResource (res);
#if !SCE_LIST_ITERATOR_NO_MALLOC
        SCE_List_DeleteIt (res->it);
#endif
        SCE_List_Delete (res->owners);
        SCE_free (res);
    }
}


void SCE_SceneResource_InitGroup (SCE_SSceneResourceGroup *group)
{
    group->resources = NULL;
    group->type = 0;
}

/* used for SCE_List_Create() */
static void SCE_SceneResource_YouDontHaveGroup (void *r)
{
    SCE_SSceneResource *res = r;
    res->group = NULL;
    res->removed = SCE_TRUE;
}

SCE_SSceneResourceGroup* SCE_SceneResource_CreateGroup (void)
{
    SCE_SSceneResourceGroup *group = NULL;

    SCE_btstart ();
    if (!(group = SCE_malloc (sizeof *group)))
        goto failure;
    SCE_SceneResource_InitGroup (group);
    /* don't delete the resources on group deletion */
    if (!(group->resources = SCE_List_Create (
              SCE_SceneResource_YouDontHaveGroup)))
        goto failure;
    /* each resource manages its own iterator */
    SCE_List_CanDeleteIterators (group->resources, SCE_FALSE);
    goto success;

failure:
    SCE_SceneResource_DeleteGroup (group), group = NULL;
    Logger_LogSrc ();
success:
    SCE_btend ();
    return group;
}

void SCE_SceneResource_DeleteGroup (SCE_SSceneResourceGroup *group)
{
    if (group)
    {
        SCE_List_Delete (group->resources);
        SCE_free (group);
    }
}

/**
 * \brief Sets the type of a group
 */
void SCE_SceneResource_SetGroupType (SCE_SSceneResourceGroup *group, int type)
{
    group->type = type;
}

/**
 * \brief Gets the type of the given group
 */
int SCE_SceneResource_GetGroupType (SCE_SSceneResourceGroup *group)
{
    return group->type;
}

/**
 * \brief Gets the type of the group of the given resource
 */
int SCE_SceneResource_GetType (SCE_SSceneResource *res)
{
    return res->group->type;
}


/**
 * \brief Adds a resource into a group
 * \param group the group where add the resource (can be NULL)
 * \param res the resource to add
 *
 * If \p group is NULL, \p res is added to the previous group where it was,
 * if \p res was never added to a group, calling this function with \p group
 * NULL generates a segmentation fault (in the better case).
 * \sa SCE_SceneResource_RemoveResource()
 */
void SCE_SceneResource_AddResource (SCE_SSceneResourceGroup *group,
                                    SCE_SSceneResource *res)
{
    if (!group || group == res->group)
    {
        if (res->removed) /* in this case, res should have a group... */
            SCE_List_Prependl (res->group->resources, res->it);
    }
    else
    {
        SCE_SceneResource_RemoveResource (res);
        SCE_List_Prependl (group->resources, res->it);
        res->group = group;
    }
    res->removed = SCE_FALSE;
}

/**
 * \brief Removes a resource from its current group
 * \param res the resource to remove from
 * \sa SCE_SceneResource_AddResource()
 */
void SCE_SceneResource_RemoveResource (SCE_SSceneResource *res)
{
    if (!res->removed)
    {
        SCE_List_Remove (res->group->resources, res->it);
        res->removed = SCE_TRUE;
    }
}


/**
 * \brief Gets the list of the resource of a resources group
 */
SCE_SList* SCE_SceneResource_GetResourcesList (SCE_SSceneResourceGroup *group)
{
    return group->resources;
}

/**
 * \brief Calls a function for each resource of a group
 * \param group the group where check the resources
 * \param f the function to call for each resource
 * \param param an user defined parameter sent to \p f
 */
void SCE_SceneResource_ForEachResource (SCE_SSceneResourceGroup *group,
                                        SCE_FForEachSceneResourceFunc f,
                                        void *param)
{
    SCE_SListIterator *it, *pro;
    SCE_List_ForEachProtected (pro, it, group->resources)
    {
        if (!f (group, SCE_List_GetData (it), param))
            break;
    }
}


void SCE_SceneResource_SetResource (SCE_SSceneResource *res, void *resource)
{
    res->resource = resource;
}

void* SCE_SceneResource_GetResource (SCE_SSceneResource *res)
{
    return res->resource;
}


int SCE_SceneResource_AddOwner (SCE_SSceneResource *res, void *owner)
{
    if (SCE_List_PrependNewl (res->owners, owner) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    return SCE_OK;
}

void SCE_SceneResource_RemoveOwner (SCE_SSceneResource *res, void *owner)
{
    SCE_SListIterator *it;
    it = SCE_List_LocateIterator (res->owners, owner, NULL);
    if (it)
    {
        SCE_List_Remove (res->owners, it);
        SCE_List_DeleteIt (it);
    }
}


SCE_SList* SCE_SceneResource_GetOwnersList (SCE_SSceneResource *res)
{
    return res->owners;
}

#if 0
void
Sound_SetListenerMatrix (SCE_TMatrix4 m)
{
    float data[6];              /* final data (sent to alListener()) */
    SCE_TVector3 pos;           /* listener position */

    /* save translation vector */
    SCE_Matrix4_GetTranslation (m, pos);
    /* delete translation */
    m[3] = m[7] = m[11] = 0.0f;
    /* initialize directionnal vectors */
    SCE_Vector3_Set (&data[0], 0.0, 0.0, -1.0); /* & and [0] used for the example */
    SCE_Vector3_Set (&data[3], 0.0, 1.0, 0.0);
    /* project vectors into the _rotation_ matrix
       (because translation was deleted) */
    SCE_Matrix4_MulV3 (m, &data[0]); /* & and [0] used for the example */
    SCE_Matrix4_MulV3 (m, &data[3]);
    /* restores matrix translation */
    m[3] = pos[0]; m[7] = pos[1]; m[11] = pos[3];
    /* send data to openal */
    alListenerfv (AL_ORIENTATION, data);
    alListenerfv (AL_POSITION, pos);
}
#endif
