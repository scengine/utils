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
   updated: 01/07/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCEModel.h>


static void SCE_Model_InitEntity (SCE_SModelEntity *entity)
{
    entity->entity = NULL;
    entity->is_instance = SCE_TRUE;
    SCE_List_InitIt (&entity->it);
    SCE_List_SetData (&entity->it, entity);
}
static void SCE_Model_DeleteEntity (SCE_SModelEntity*);
static SCE_SModelEntity* SCE_Model_CreateEntity (SCE_SSceneEntity *e)
{
    SCE_SModelEntity *entity = NULL;
    if (!(entity = SCE_malloc (sizeof *entity)))
        goto fail;
    SCE_Model_InitEntity (entity);
    if (!e)
    {
        if (!(e = SCE_SceneEntity_Create ()))
            goto fail;
        entity->is_instance = SCE_FALSE;
    }
    entity->entity = e;
    SCE_btend ();
    return entity;
fail:
    SCE_Model_DeleteEntity (entity);
    Logger_LogSrc ();
    SCE_btend ();
    return NULL;
}
static void SCE_Model_DeleteEntity (SCE_SModelEntity *entity)
{
    if (entity)
    {
        if (!entity->is_instance)
            SCE_SceneEntity_Delete (entity->entity);
        SCE_free (entity);
    }
}

static void SCE_Model_InitEntityGroup (SCE_SModelEntityGroup *group)
{
    group->group = NULL;
    group->is_instance = SCE_TRUE;
    SCE_List_InitIt (&group->it);
    SCE_List_SetData (&group->it, group);
}
static void SCE_Model_DeleteEntityGroup (SCE_SModelEntityGroup*);
static SCE_SModelEntityGroup*
SCE_Model_CreateEntityGroup (SCE_SSceneEntityGroup *g)
{
    SCE_SModelEntityGroup *group = NULL;
    if (!(group = SCE_malloc (sizeof *group)))
        goto fail;
    SCE_Model_InitEntityGroup (group);
    if (!g)
    {
        if (!(g = SCE_SceneEntity_CreateGroup ()))
            goto fail;
        group->is_instance = SCE_FALSE;
    }
    group->group = g;
    return group;
fail:
    SCE_Model_DeleteEntityGroup (group);
    Logger_LogSrc ();
    return NULL;
}
static void SCE_Model_DeleteEntityGroup (SCE_SModelEntityGroup *group)
{
    if (group)
    {
        if (!group->is_instance)
        {
            SCE_SceneEntity_DeleteGroup (group->group);
            SCE_free (group);
        }
    }
}

static void SCE_Model_Init (SCE_SModel *mdl)
{
    unsigned int i;
    for (i = 0; i < SCE_MAX_MODEL_ENTITIES; i++)
        mdl->entities[i] = NULL;
    mdl->groups = NULL;
    mdl->instances = NULL;
    mdl->instance_type = SCE_MODEL_NOT_INSTANCE;
}
SCE_SModel* SCE_Model_Create (void)
{
    unsigned int i;
    SCE_SModel *mdl = NULL;
    SCE_btstart ();
    if (!(mdl = SCE_malloc (sizeof *mdl)))
        goto fail;
    SCE_Model_Init (mdl);
    if (!(mdl->instances = SCE_List_Create (
              (SCE_FListFreeFunc)SCE_SceneEntity_DeleteInstance)))
        goto fail;
    SCE_btend ();
    return mdl;
fail:
    SCE_Model_Delete (mdl);
    Logger_LogSrc ();
    SCE_btend ();
    return NULL;
}
void SCE_Model_Delete (SCE_SModel *mdl)
{
    if (mdl)
    {
        unsigned int i;
        for (i = 0; i < SCE_MAX_MODEL_ENTITIES; i++)
            SCE_List_Delete (mdl->entities[i]);
        SCE_List_Delete (mdl->instances);
        SCE_List_Delete (mdl->groups);
        SCE_free (mdl);
    }
}


static int SCE_Model_BuildEntityArg (SCE_SModelEntity *entity, SCE_SMesh *mesh,
                                     SCE_SShader *shader, va_list args)
{
    SCE_STexture *tex = NULL;

    SCE_SceneEntity_SetMesh (entity->entity, mesh);
    if (shader)
    {
        SCE_SSceneResource *res = SCE_SceneResource_Create ();
        if (!res)
            goto fail;
        SCE_SceneResource_SetResource (res, shader);
        SCE_SceneEntity_SetShader (entity->entity, res);
    }
    tex = va_arg (args, SCE_STexture*);
    while (tex)
    {
        SCE_SSceneResource *res = SCE_SceneResource_Create ();
        if (!res)
            goto fail;
        SCE_SceneResource_SetResource (res, tex);
        SCE_SceneEntity_AddTexture (entity->entity, res);
        tex = va_arg (args, SCE_STexture*);
    }
    return SCE_OK;
fail:
    Logger_LogSrc ();
    return SCE_ERROR;
}

#define SCE_CHECK_LEVEL(level) do {                                     \
        if (level >= SCE_MAX_MODEL_ENTITIES)                            \
        {                                                               \
            Logger_Log (SCE_INVALID_ARG);                               \
            Logger_LogMsg ("parameter 'level' is too high (%d), maximum is %u",\
                           level, SCE_MAX_MODEL_ENTITIES);              \
            return;                 /* lol. */                          \
        }                                                               \
    } while (0)

/**
 * \brief Builds and adds a scene entity to a model
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * If \p level is lesser than 0, then using the latest level of detail.
 */
int SCE_Model_AddEntityArg (SCE_SModel *mdl, int level, SCE_SMesh *mesh,
                            SCE_SShader *shader, va_list args)
{
    SCE_SModelEntity *entity = NULL;
    unsigned int n;

    /* NOTE: can returns > MAX_MODEL_ENTITIES */
    if (level < 0)
        level = SCE_Model_GetNumLOD (mdl);
#ifdef SCE_DEBUG
    SCE_CHECK_LEVEL (level);
#endif

    if (!mdl->entities[level])
    {
        if (!(mdl->entities[level] = SCE_List_Create (
                  (SCE_FListFreeFunc)SCE_Model_DeleteEntity)))
            goto fail;
    }
    if (!mdl->groups)
    {
        if (!(mdl->groups = SCE_List_Create (
                  (SCE_FListFreeFunc)SCE_Model_DeleteEntityGroup)))
            goto fail;
    }
    n = SCE_List_GetLength (mdl->entities[level]);
    if (SCE_List_GetLength (mdl->groups) <= n)
    {
        SCE_SModelEntityGroup *mgroup = NULL;
        /* one is enough */
        if (!(mgroup = SCE_Model_CreateEntityGroup (NULL)))
            goto fail;
        SCE_List_Appendl (mdl->groups, &mgroup->it);
    }
    if (!(entity = SCE_Model_CreateEntity (NULL)))
        goto fail;
    if (SCE_Model_BuildEntityArg (entity, mesh, shader, args) < 0)
        goto fail;
    SCE_List_Appendl (mdl->entities[level], &entity->it);
    {
        /* can't fail */
        SCE_SListIterator *it = SCE_List_GetIterator (mdl->groups, n);
        SCE_SModelEntityGroup *mgroup = SCE_List_GetData (it);
        SCE_SceneEntity_AddEntity (mgroup->group, entity->entity);
    }
    /* returns identifier of the added scene entity */
    return n;
fail:
    SCE_Model_DeleteEntity (entity);
    Logger_LogSrc ();
    return SCE_ERROR;
}
int SCE_Model_AddEntity (SCE_SModel *mdl, int level, SCE_SMesh *mesh,
                         SCE_SShader *shader, ...)
{
    va_list args;
    int code;
    va_start (args, shader);
    code = SCE_Model_AddEntityArg (mdl, level, mesh, shader, args);
    va_end (args);
    return code;
}


int SCE_Model_AddInstance (SCE_SModel *mdl, unsigned int n,
                           SCE_SSceneEntityInstance *einst)
{
    SCE_SModelEntityGroup *mgroup = NULL;
    SCE_SListIterator *it = SCE_List_GetIterator (mdl->groups, n);
    if (!it)
    {
        Logger_Log (SCE_INVALID_ARG);
        Logger_LogMsg ("no group number %u in this model", n);
        return SCE_ERROR;
    }
    mgroup = SCE_List_GetData (it);
    SCE_SceneEntity_AddInstance (mgroup->group, einst);
    return SCE_OK;
}
int SCE_Model_AddNewInstance (SCE_SModel *mdl, unsigned int n)
{
    SCE_SSceneEntityInstance *einst = NULL;
    if (!(einst = SCE_SceneEntity_CreateInstance ()))
        goto fail;
    if (SCE_Model_AddInstance (mdl, n, einst) < 0)
        goto fail;
    return SCE_OK;
fail:
    SCE_SceneEntity_DeleteInstance (einst);
    Logger_LogSrc ();
    return SCE_ERROR;
}


/**
 * \brief Gets the number of LOD
 */
unsigned int SCE_Model_GetNumLOD (SCE_SModel *mdl)
{
    unsigned int i, n = 0;
    for (i = 0; i < SCE_MAX_MODEL_ENTITIES; i++)
    {
        if (mdl->entities[i] && SCE_List_HasElements (mdl->entities[i]))
            n++;
    }
    return n;
}
/**
 * \brief Gets the required entity
 * \param level LOD of the required entity
 * \param n the number of the required entity
 * \returns the scene entity required, if no on have been found, returns NULL
 *
 * If \p level is lesser than 0, then using the latest level of detail.
 */
SCE_SSceneEntity* SCE_Model_GetEntity (SCE_SModel *mdl, int level,
                                       unsigned int n)
{
    SCE_SListIterator *it = NULL;
    SCE_SModelEntity *entity = NULL;
    if (level < 0)
        level = SCE_Model_GetNumLOD (mdl);
    if (level == 0)
        return NULL;
    it = SCE_List_GetIterator (mdl->entities[level], n);
    if (!it)
        return NULL;
    entity = SCE_List_GetData (it);
    return entity->entity;
}
/**
 * \brief Gets the list of the entities of the LOD \p level
 *
 * The returned list contains pointers to SCE_SModelEntity structures.
 * If \p level is lesser than 0, then using the latest level of detail.
 */
SCE_SList* SCE_Model_GetEntitiesList (SCE_SModel *mdl, int level)
{
    if (level < 0)
        level = SCE_Model_GetNumLOD (mdl);
    return mdl->entities[level];
}

/**
 * \brief Gets the scene entity of a model entity
 * \returns \p entity::entity
 */
SCE_SSceneEntity* SCE_Model_GetEntityEntity (SCE_SModelEntity *entity)
{
    return entity->entity;
}


static SCE_SModelEntity* SCE_Model_CopyDupEntity (SCE_SModelEntity *in)
{
    SCE_SModelEntity *entity = NULL;
    if (!(entity = SCE_Model_CreateEntity (in->entity)))
    {
        Logger_LogSrc ();
        return NULL;
    }
    return entity;
}
static int SCE_Model_InstanciateSoft (SCE_SModel *mdl, SCE_SModel *mdl2)
{
    unsigned int i;
    mdl2->groups = mdl->groups;
    for (i = 0; i < SCE_MAX_MODEL_ENTITIES; i++)
        mdl2->entities[i] = mdl2->entities[i];
    mdl2->instance_type = SCE_MODEL_HARD_INSTANCE;
    return SCE_OK;
}
static int SCE_Model_InstanciateHard (SCE_SModel *mdl, SCE_SModel *mdl2)
{
    SCE_SListIterator *it = NULL;
    unsigned int i;

    if (!mdl2->groups)
    {
        if (!(mdl2->groups = SCE_List_Create (
                  (SCE_FListFreeFunc)SCE_Model_DeleteEntityGroup)))
            goto fail;
    }
    /* duplicate SCE_SModelEntityGroup */
    SCE_List_ForEach (it, mdl->groups)
    {
        SCE_SModelEntityGroup *mgroup = NULL, *newg = NULL;
        mgroup = SCE_List_GetData (it);
        if (!(newg = SCE_Model_CreateEntityGroup (mgroup->group)))
            goto fail;
        SCE_List_Appendl (mdl2->groups, &newg->it);
    }

    for (i = 0; i < SCE_MAX_MODEL_ENTITIES; i++)
    {
        if (!SCE_List_HasElements (mdl->entities[i]))
            break;
        else
        {
            if (!mdl2->entities[i])
            {
                if (!(mdl2->entities[i] = SCE_List_Create (
                          (SCE_FListFreeFunc)SCE_Model_DeleteEntity)))
                    goto fail;
            }
        }
        SCE_List_ForEach (it, mdl->entities[i])
        {
            SCE_SModelEntity *entity = NULL;
            if (!(entity = SCE_Model_CopyDupEntity (SCE_List_GetData (it))))
                goto fail;
            SCE_List_Appendl (mdl2->entities[i], &entity->it);
        }
    }

    mdl2->instance_type = SCE_MODEL_SOFT_INSTANCE;

    return SCE_OK;
fail:
    Logger_LogSrc ();
    return SCE_ERROR;
}
/**
 * \brief Instanciates a model (can only instanciate a built model)
 * \param mode the instance type, read SCE_Model_GetInstanceType()'s
 * documentation for more details about available modes
 *
 * \p mdl2 must just be allocated by SCE_Model_Create(). (bad english here)
 * \sa SCE_Model_CreateInstanciate()
 */
int SCE_Model_Instanciate (SCE_SModel *mdl, SCE_SModel *mdl2, int mode)
{
    int code = SCE_OK;
    switch (mode)
    {
    case SCE_MODEL_SOFT_INSTANCE:
        code = SCE_Model_InstanciateSoft (mdl, mdl2);
        break;
    case SCE_MODEL_HARD_INSTANCE:
        code = SCE_Model_InstanciateHard (mdl, mdl2);
    }

    if (code < 0)
        goto fail;

    return SCE_OK;
fail:
    Logger_LogSrc ();
    return SCE_ERROR;
}
/**
 * \brief Instanciates a model
 *
 * This function does like SCE_Model_Instanciate() except that it first creates
 * a new model.
 * \sa SCE_Model_Instanciate()
 */
SCE_SModel* SCE_Model_CreateInstanciate (SCE_SModel *mdl, int mode)
{
    SCE_SModel *instance = NULL;

    if (!(instance = SCE_Model_Create ()))
    {
        Logger_LogSrc ();
        return NULL;
    }
    SCE_Model_Instanciate (mdl, instance, mode);
    return instance;
}

/**
 * \brief Gets the mode of \p mdl
 *
 * If \p mdl is an instance created by SCE_Model_Instanciate(), this function
 * returns the following:
 * - SCE_MODEL_SOFT_INSTANCE: the data of the model structure have been
 *   duplicated;
 * - SCE_MODEL_HARD_INSTANCE: the content of the model structure have just been
 *   copied, like a simple '=' on C struct.
 * If \p mdl isn't an instance, this functions returns SCE_MODEL_NOT_INSTANCE.
 */
int SCE_Model_GetInstanceType (SCE_SModel *mdl)
{
    return mdl->instance_type;
}


SCE_SList* SCE_Model_GetInstancesList (SCE_SModel *mdl)
{
    return mdl->instances;
}

SCE_SSceneEntityGroup* SCE_Model_GetSceneEntityGroup (SCE_SModel *mdl,
                                                      unsigned int n)
{
    SCE_SListIterator *it = SCE_List_GetIterator (mdl->groups, n);
    if (!it)
        return NULL;
    else
        return ((SCE_SModelEntityGroup*)SCE_List_GetData (it))->group;
}
