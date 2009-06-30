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
   updated: 27/06/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCEModel.h>


static void SCE_Model_InitEntity (SCE_SModelEntity *entity)
{
    entity->entity = NULL;
    entity->mesh = NULL;
    entity->textures = NULL;
    entity->shader = NULL;
    entity->locales = NULL;
    entity->is_instance = SCE_FALSE;
    entity->it = &entity->iterator;
    SCE_List_InitIt (entity->it);
    SCE_List_SetData (entity->it, entity);
}
static void SCE_Model_DeleteTexture (void *tex)
{
    if (tex)
    {
        SCE_Texture_Delete (tex);
    }
}
static void SCE_Model_DeleteEntity (SCE_SModelEntity*);
static SCE_SModelEntity* SCE_Model_CreateEntity (int createlocales,
                                                 int createtextures)
{
    SCE_SModelEntity *entity = NULL;
    if (!(entity = SCE_malloc (sizeof *entity)))
        goto fail;
    SCE_Model_InitEntity (entity);
    if (createtextures)
    {
        if (!(entity->textures = SCE_List_Create (SCE_Model_DeleteTexture)))
            goto fail;
    }
    if (createlocales)
    {
        if (!(entity->locales = SCE_List_Create (SCE_free)))
            goto fail;
    }
    SCE_List_CanDeleteIterators (entity->textures, SCE_TRUE);
    SCE_List_CanDeleteIterators (entity->locales, SCE_TRUE);
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
            SCE_List_Delete (entity->textures);
        SCE_List_Delete (entity->locales);
        SCE_free (entity);
    }
}

static void SCE_Model_InitEntityGroup (SCE_SModelEntityGroup *group)
{
    group->group = NULL;
    group->is_instance = SCE_TRUE;
    group->it = &group->iterator;
    SCE_List_InitIt (group->it);
    SCE_List_SetData (group->it, group);
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
    SCE_Matrix4_Identity (mdl->matrix);
    mdl->is_instance = SCE_FALSE;
}
SCE_SModel* SCE_Model_Create (void)
{
    unsigned int i;
    SCE_SModel *mdl = NULL;
    SCE_btstart ();
    if (!(mdl = SCE_malloc (sizeof *mdl)))
        goto fail;
    SCE_Model_Init (mdl);
    for (i = 0; i < SCE_MAX_MODEL_ENTITIES; i++)
    {
        if (!(mdl->entities[i] = SCE_List_Create (
              (SCE_FListFreeFunc)SCE_Model_DeleteEntity)))
            goto fail;
    }
    if (!(mdl->groups = SCE_List_Create (
              (SCE_FListFreeFunc)SCE_Model_DeleteEntityGroup)))
        goto fail;
    if (!(mdl->instances = SCE_List_Create (NULL)))
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

#define SCE_CHECK_LEVEL(level) do {                                     \
        if (level >= SCE_MAX_MODEL_ENTITIES)                            \
        {                                                               \
            Logger_Log (SCE_INVALID_ARG);                               \
            Logger_LogMsg ("parameter 'level' is too high (%d), maximum is %u",\
                           level, SCE_MAX_MODEL_ENTITIES);              \
            return;                 /* lol. */                          \
        }                                                               \
    } while (0)


int SCE_Model_AddEntityArg (SCE_SModel *mdl, int level, SCE_SMesh *mesh,
                            SCE_SShader *shader, va_list args)
{
    SCE_STexture *tex = NULL;
    SCE_SModelEntity *entity = NULL;
    int createlocales = SCE_FALSE;

    /* NOTE: can returns > MAX_MODEL_ENTITIES */
    if (level < 0)
        level = SCE_Model_GetNumLOD (mdl);
#ifdef SCE_DEBUG
    SCE_CHECK_LEVEL (level);
#endif

    if (level == 0)
        createlocales = SCE_TRUE;
    if (!(entity = SCE_Model_CreateEntity (createlocales, SCE_TRUE)))
        goto fail;
    entity->mesh = mesh;
    entity->shader = shader;
    tex = va_arg (args, SCE_STexture*);
    while (tex)
    {
        if (SCE_List_AppendNewl (entity->textures, tex) < 0)
            goto fail;
        tex = va_arg (args, SCE_STexture*);
    }
    SCE_List_Appendl (mdl->entities[level], entity->it);
    /* returns identifier of the added scene entity */
    return SCE_List_GetLength (mdl->entities[level]) - 1;
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
    code = SCE_Model_AddEntity (mdl, level, mesh, shader, args);
    va_end (args);
    return code;
}

/**
 * \brief Adds an instance to an model entity
 * \param n the \p (n-1) th added entity, see SCE_Model_AddEntity()
 * \param mat the local space matrix of the instance (local to the model)
 *
 * The memory of \p mat will be freed.
 * \sa SCE_Model_AddInstanceDup()
 */
int SCE_Model_AddInstance (SCE_SModel *mdl, unsigned int n, SCE_TMatrix4 mat)
{
    SCE_SModelEntity *entity = NULL;
    SCE_SListIterator *it = NULL;

    if (!SCE_List_HasElement (mdl->entities[0]))
    {
#ifdef SCE_DEBUG
        Logger_Log (SCE_INVALID_ARG);
        Logger_LogMsg("cannot add instance: none elements for LOD %u.", level);
        return SCE_ERROR;
#else
        return SCE_OK;
#endif
    }

    it = SCE_List_GetIterator (mdl->entities[0], n);
    if (!it)
        it = SCE_List_GetLast (mdl->entities[0]);
    entity = SCE_List_GetData (it); /* can't fail */
    if (SCE_List_AppendNewl (entity->locales, mat) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    return SCE_OK;
}
/**
 * \brief Adds an instance to an model entity
 *
 * Calls SCE_Model_AddInstance() but with a memory duplication of \p mat
 * \sa SCE_Model_AddInstance()
 */
int SCE_Model_AddInstanceDup (SCE_SModel *mdl, unsigned int n, SCE_TMatrix4 mat)
{
    float *newmat = NULL;       /* FIXME: matrix type (float) */
    if (!(newmat = SCE_malloc (sizeof (SCE_TMatrix4))))
        goto fail;
    SCE_Matrix4_Copy (newmat, mat);
    if (SCE_Model_AddInstance (mdl, n, newmat) < 0)
        goto fail;
    return SCE_OK;
fail:
    SCE_free (newmat);
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
        if (SCE_List_HasElement (mdl->entities[i]))
            n++;
    }
    return n;
}

/**
 * \brief Gets the global space matrix of a model
 */
float* SCE_Model_GetMatrix (SCE_SModel *mdl)
{
    return mdl->matrix;
}


static int SCE_Model_BuildEntity (SCE_SModelEntity *entity)
{
    SCE_SListIterator *it = NULL;

    if (!entity->entity)
    {
        if (!(entity->entity = SCE_SceneEntity_Create ()))
            goto fail;
    }
    SCE_SceneEntity_SetMesh (entity->entity, entity->mesh);
    if (entity->shader)
    {
        SCE_SSceneResource *res = SCE_SceneResource_Create ();
        if (!res)
            goto fail;
        SCE_SceneResource_SetResource (res, entity->shader);
        if (SCE_SceneResource_AddOwner (res, entity->entity) < 0)
            goto fail;
        SCE_SceneEntity_SetShader (entity->entity, res);
    }
    SCE_List_ForEach (it, entity->textures)
    {
        SCE_SSceneResource *res = SCE_SceneResource_Create ();
        if (!res)
            goto fail;
        SCE_SceneResource_SetResource (res, SCE_List_GetData (it));
        if (SCE_SceneResource_AddOwner (res, entity->entity) < 0)
            goto fail;
        SCE_SceneEntity_AddTexture (entity->entity, res);
    }
    return SCE_OK;
fail:
    Logger_LogSrc ();
    return SCE_ERROR;
}

static void SCE_Model_RemoveEmptyEntities (SCE_SModel *mdl)
{
    unsigned int i;
    SCE_SModelEntity *entity = NULL;

    for (i = 0; i < SCE_MAX_MODEL_ENTITIES; i++)
    {
        if (!SCE_List_HasElement (mdl->entities[i]))
        {
            /* move elements */
            unsigned int j;
            for (j = i; j < SCE_MAX_MODEL_ENTITIES - 1; j++)
                mdl->entities[j] = mdl->entities[j + 1];
            mdl->entities[SCE_MAX_MODEL_ENTITIES - 1] = NULL;
        }
    }
}

static int SCE_Model_BuildInstances (SCE_SModel *mdl)
{
    SCE_SListIterator *it = NULL, *it2 = NULL;
    unsigned int n = 0;

    it2 = SCE_List_GetFirst (mdl->groups);
    SCE_List_ForEach (it, mdl->entities[0])
    {
        SCE_SListIterator *it3 = NULL;
        SCE_SModelEntity *entity = SCE_List_GetData (it);
        SCE_List_ForEach (it3, entity->locales)
        {
            SCE_SSceneEntityInstance *einst = NULL;
            if (!(einst = SCE_SceneEntity_CreateInstance ()))
                goto fail;
            SCE_Matrix4_Mul (mdl->matrix, SCE_List_GetData (it3),
                             SCE_Node_GetMatrix (SCE_SceneEntity_GetInstanceNode
                                                 (einst)));
            /* NOTE: 2nd iterator of entity instances is for the users */
            SCE_List_Appendl (mdl->instances,
                              SCE_SceneEntity_GetInstanceIterator2 (einst));
            SCE_SceneEntity_AddInstance (SCE_List_GetData (it2), einst);
        }
        it2 = SCE_List_GetNext (it2);
    }

    return SCE_OK;
fail:
    Logger_LogSrc ();
    return SCE_ERROR;
}

int SCE_Model_Build (SCE_SModel *mdl)
{
    SCE_SListIterator *it = NULL;
    unsigned int i, n = 0;

    SCE_Model_RemoveEmptyEntities (mdl);

    /* build entities */
    for (i = 0; i < SCE_MAX_MODEL_ENTITIES; i++)
    {
        if (!SCE_List_HasElement (mdl->entities[i]))
            break;              /* next are NULL */
        SCE_List_ForEach (it, mdl->entities[i])
        {
            SCE_SModelEntity *entity = SCE_List_GetData (it);
            if (SCE_Model_BuildEntity (entity) < 0)
                goto fail;
        }
    }

    /* build groups */
    /* hope this list is the biggest */
    SCE_List_ForEach (it, mdl->entities[0])
    {
        SCE_SModelEntity *entity = SCE_List_GetData (it);
        if (!entity->is_instance)
        {
            SCE_SModelEntityGroup *mgroup = NULL;
            if (!(mgroup = SCE_Model_CreateEntityGroup (NULL)))
                goto fail;
            SCE_List_Appendl (mdl->groups, mgroup->it);
            /* add entities */
            for (i = 0; i < SCE_MAX_MODEL_ENTITIES; i++)
            {
                SCE_SListIterator *it2 = NULL;
                it2 = SCE_List_GetIterator (mdl->entities[i], n);
                if (it2)
                {
                    SCE_SModelEntity *entity = SCE_List_GetData (it2);
                    SCE_SceneEntity_AddEntity (mgroup->group, entity->entity);
                }
            }
        }
        n++;                    /* the entity we are checking */
    }

    if (SCE_Model_BuildInstances (mdl) < 0)
        goto fail;

    return SCE_OK;
fail:
    Logger_LogSrc ();
    return SCE_ERROR;
}


static SCE_SModelEntity* SCE_Model_CopyDupEntity (SCE_SModelEntity *in)
{
    SCE_SModelEntity *entity = NULL;
    if (!(entity = SCE_Model_CreateEntity (SCE_TRUE, SCE_FALSE)))
    {
        Logger_LogSrc ();
        return NULL;
    }
    entity->mesh = in->mesh;
    entity->textures = in->textures;
    entity->shader = in->shader;
    entity->is_instance = SCE_TRUE;
    return entity;
}
/**
 * \brief Instanciates a model (can only instanciates a built model)
 *
 * \p mdl2 must just be allocated by SCE_Model_Create(). (bad english here)
 */
int SCE_Model_Instanciate (SCE_SModel *mdl, SCE_SModel *mdl2)
{
    SCE_SListIterator *it = NULL;
    unsigned int i;

    /* duplicate SCE_SModelEntityGroup */
    SCE_List_ForEach (it, mdl->groups)
    {
        SCE_SModelEntityGroup *mgroup = NULL, *newg = NULL;
        mgroup = SCE_List_GetData (it);
        if (!(newg = SCE_Model_CreateEntityGroup (mgroup->group)))
            goto fail;
        SCE_List_Appendl (mdl2->groups, newg->it);
    }

    for (i = 0; i < SCE_MAX_MODEL_ENTITIES; i++)
    {
        SCE_List_ForEach (it, mdl->entities[i])
        {
            SCE_SModelEntity *entity = NULL;
            if (!(entity = SCE_Model_CopyDupEntity (SCE_List_GetData (it))))
                goto fail;
            SCE_List_Appendl (mdl2->entities[i], entity);
        }
    }

    if (SCE_Model_BuildInstances (mdl2) < 0)
        goto fail;
    mdl2->is_instance = SCE_TRUE;

    return SCE_OK;
fail:
    Logger_LogSrc ();
    return SCE_ERROR;
}
SCE_SModel* SCE_Model_CreateInstanciate (SCE_SModel *mdl)
{
    SCE_SModel *instance = NULL;

    if (!(instance = SCE_Model_Create ()))
    {
        Logger_LogSrc ();
        return NULL;
    }
    SCE_Model_Instanciate (mdl, instance);
    return instance;
}


SCE_SList* SCE_Model_GetInstances (SCE_SModel *mdl)
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
