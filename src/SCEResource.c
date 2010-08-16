/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2010  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
 
/* created: 02/01/2007
   updated: 10/04/2010 */

#include <stdlib.h>
#include <stdio.h>

#include "SCE/utils/SCEMemory.h"
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEString.h"
#include "SCE/utils/SCEList.h"
#include "SCE/utils/SCEMedia.h"
#include "SCE/utils/SCEResource.h"

/**
 * \file SCEResource.c
 * \copydoc resources
 * \file SCEResource.h
 * \copydoc resources
 */

/**
 * \defgroup resources Resources managment
 * \ingroup utils
 * \brief Resources managment
 */

/** @{ */

/** \internal */
/**
 * \brief Abstract type for a resource, independant to the file type
 */
typedef struct sce_sresourcetype SCE_SResourceType;
struct sce_sresourcetype
{
    int type;
    int media;
    SCE_FLoadResourceFunc load;
    SCE_FSaveResourceFunc save;
    /*void (*delete)(void*);*/
    SCE_SListIterator it;
};

/** \internal */
typedef struct sce_sresource SCE_SResource;
struct sce_sresource
{
    SCE_SResourceType *type;
    void *data;
    char *name;
    unsigned int nb_used;       /* number of utilisations */
    SCE_SListIterator it;
};

static SCE_SList resources_type;
static SCE_SList resources;

static int res_type_id = 0;     /* type 0 is unused */


static void SCE_Resource_InitType (SCE_SResourceType *r)
{
    r->type = 0;
    r->media = SCE_FALSE;
    r->load = NULL;
    r->save = NULL;
    SCE_List_InitIt (&r->it);
    SCE_List_SetData (&r->it, r);
}
static SCE_SResourceType* SCE_Resource_CreateType (void)
{
    SCE_SResourceType *res = NULL;
    if (!(res = SCE_malloc (sizeof *res)))
        SCEE_LogSrc ();
    else
        SCE_Resource_InitType (res);
    return res;
}
static void SCE_Resource_DeleteType (void *r)
{
    if (r)
    {
        SCE_SResourceType *res = r;
        SCE_free (res);
    }
}

static void SCE_Resource_Init (SCE_SResource *r)
{
    r->type = NULL;
    r->data = NULL;
    r->name = NULL;
    r->nb_used = 0;
    SCE_List_InitIt (&r->it);
    SCE_List_SetData (&r->it, r);
}
static SCE_SResource* SCE_Resource_Create (void)
{
    SCE_SResource *res = NULL;
    res = SCE_malloc (sizeof *res);
    if (!res)
        SCEE_LogSrc ();
    else
        SCE_Resource_Init (res);
    return res;
}
static void SCE_Resource_Delete (void *r)
{
    if (r)
    {
        SCE_SResource *res = r;
        SCE_free (res->name);
        SCE_free (res);
    }
}


/**
 * \brief Initialize the resources manager
 * \returns SCE_OK on succes or SCE_ERROR on failure. This function actually
 * returns always SCE_OK
 */
int SCE_Init_Resource (void)
{
    SCE_List_Init (&resources);
    SCE_List_SetFreeFunc (&resources, SCE_Resource_Delete);
    SCE_List_Init (&resources_type);
    SCE_List_SetFreeFunc (&resources_type, SCE_Resource_DeleteType);
    return SCE_OK;
}
/**
 * \brief Exits the resource manager
 */
void SCE_Quit_Resource (void)
{
    SCE_List_Clear (&resources);
    SCE_List_Clear (&resources_type);
}


/**
 * \brief Registers a new type of resource
 * \param media load directly from media manager?
 * \param load load function for the new type
 * \param save save function for the new type
 * \returns the resource type identifier, used on resource loading
 */
int SCE_Resource_RegisterType (int media, SCE_FLoadResourceFunc load,
                               SCE_FSaveResourceFunc save)
{
    SCE_SResourceType *res = NULL;
    if (!(res = SCE_Resource_CreateType ()))
        goto fail;
    res_type_id++;
    res->type = res_type_id;
    res->media = media;
    res->load = load;
    res->save = save;
    SCE_List_Appendl (&resources_type, &res->it);
    return res_type_id;
fail:
    SCE_Resource_DeleteType (res);
    SCEE_LogSrc ();
    return SCE_ERROR;
}


static SCE_SResourceType* SCE_Resource_LocateType (int type)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, &resources_type) {
        SCE_SResourceType *t = SCE_List_GetData (it);
        if (t->type == type)
            return t;
    }
    SCEE_Log (SCE_INVALID_ARG);
    SCEE_LogMsg ("resource type %d is not registered", type);
    return NULL;
}

static SCE_SResource* SCE_Resource_LocateFromName (const char *name)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, &resources) {
        SCE_SResource *res = SCE_List_GetData (it);
        if (SCE_String_Cmp (name, res->name, SCE_TRUE) == 0)
            return res;
    }
    return NULL;
}
static SCE_SResource* SCE_Resource_LocateFromData (void *data)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, &resources) {
        SCE_SResource *res = SCE_List_GetData (it);
        if (res->data == data)
            return res;
    }
    return NULL;
}
static SCE_SResource* SCE_Resource_LocateFromTypeAndName (int type,
                                                          const char *name)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, &resources) {
        SCE_SResource *res = SCE_List_GetData (it);
        if (res->type->type == type &&
            SCE_String_Cmp (name, res->name, SCE_TRUE) == 0)
            return res;
    }
    return NULL;
}


static SCE_SResource* SCE_Resource_SafeAdd (SCE_SResourceType *t,
                                            const char *name, void *resource)
{
    SCE_SResource *res = NULL;
    if (!(res = SCE_Resource_Create ()))
        goto fail;
    if (!(res->name = SCE_String_Dup (name)))
        goto fail;
    res->data = resource;
    res->type = t;
    SCE_List_Appendl (&resources, &res->it);
    return res;
fail:
    SCEE_LogSrc ();
    return NULL;
}

/**
 * \brief Allows the user to add his own resources (not recommanded)
 *
 * It is recommanded to specify a load function an use it through
 * SCE_Resource_Load()
 * \sa SCE_Resource_Load()
 */
int SCE_Resource_Add (int type, const char *name, void *data)
{
    SCE_SResource *res = NULL;
    SCE_SResourceType *t = NULL;
    t = SCE_Resource_LocateType (type);
    if (!t) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    res = SCE_Resource_LocateFromTypeAndName (type, name);
    if (!res) {
        if (!SCE_Resource_SafeAdd (t, name, data)) {
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
    }
    else {
        if (res->data == data)
            res->nb_used++;
        else {
            SCEE_Log (SCE_INVALID_OPERATION);
            SCEE_LogMsg ("resource named '%s' of type %d already exists!",
                         name, type);
            return SCE_ERROR;
        }
    }
    return SCE_OK;
}

static void* SCE_Resource_LoadNew (int type, const char *name, int force,
                                   void *data)
{
    void *resource = NULL;
    SCE_SResourceType *t = NULL;
    SCE_SResource *res = NULL;

    t = SCE_Resource_LocateType (type);
    if (!t) goto fail;
    if (!force) {
        if (!(res = SCE_Resource_SafeAdd (t, name, NULL)))
            goto fail;
        name = res->name;
    }
    if (t->media)
        resource = SCE_Media_Load (t->type, name, data);
    else
        resource = t->load (name, force, data);
    if (!resource)
        goto fail;
    if (res)
        res->data = resource;
    return resource;
fail:
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("failed to create new resource '%s' of type %d",
                    name, type);
    return NULL;
}
/**
 * \brief Loads a resource
 * \param type Data type ID
 * \param name the name of the resource to load
 * \param forcenew force a new loading? (without getting an existing resource if
 * any)
 * \returns the required resource
 * \sa SCE_Resource_LoadNew()
 */
void* SCE_Resource_Load (int type, const char *name, int forcenew, void *data)
{
    void *resource = NULL;
    SCE_SResource *res = NULL;

    if (!forcenew)
        res = SCE_Resource_LocateFromTypeAndName (type, name);
    if (res) {
        resource = res->data;
        res->nb_used++;
    } else {
        if (!(resource = SCE_Resource_LoadNew (type, name, forcenew, data)))
            SCEE_LogSrc ();
    }
    return resource;
}

/* TODO: doc sux */
/**
 * \brief Decrements the number of uses of a resource and returns if it can be
 * freed or not
 * \param data a pointer to the resource (from a function like
 * SCE_Resource_Load())
 * \returns 1 if the resource can be freed and 0 if it's always used
 * 
 * This function decrements the number of uses of a resource and returns a
 * boolean that indicates if the resource can be freed or not.
 */
int SCE_Resource_Free (void *data)
{
    int ret = SCE_TRUE;         /* 'coz the user could want to delete a forced
                                   resource */
    SCE_SResource *res = NULL;

    if (!data)
        ret = SCE_FALSE;
    else {
        res = SCE_Resource_LocateFromData (data);
        if (res) {
            res->nb_used--;
            if (res->nb_used == 0)
                SCE_List_Erase (&resources, &res->it);
        }
    }

    return ret;
}


/**
 * \brief Get the number of uses of a resource
 * \param name the resource's name, or NULL
 * \param data the resource's pointer, or NULL
 * \returns the number of uses of the resource
 * 
 * This function get the number of uses of a resource from its name or its
 * pointer. \p name or \p data can be NULL. It both \p name and \p data are
 * NULL, this function will return 0.
 */
unsigned int SCE_Resource_NumUsed (const char *name, void *data)
{
    SCE_SResource *res = NULL;

    if (name)
        res = SCE_Resource_LocateFromName (name);
    if (!res)
        res = SCE_Resource_LocateFromData (data);
    return (res ? res->nb_used : 0);
}

/**
 * \brief Get the number of loaded resources
 * \returns the number of loaded resources
 * 
 * Returns the number of loaded reources in the resource manager
 */
unsigned int SCE_Resource_NumLoaded (void)
{
    return SCE_List_GetLength (&resources);
}

/**
 * \brief Get a resource name from its pointer
 * \param data resource pointer
 * \returns the name of the resource that contains \p data or NULL if no
 * resource contains \p data
 * 
 * This function get a resource name from a resource pointer.
 */
char* SCE_Resource_GetName (void *data)
{
    SCE_SResource *res = NULL;
    res = SCE_Resource_LocateFromData (data);
    return (res ? res->name : NULL);
}

/** @} */
