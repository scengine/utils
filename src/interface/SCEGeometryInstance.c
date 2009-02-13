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
 
/* created: 25/10/2008
   updated: 02/12/2008 */

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECore.h>
#include <SCE/core/SCECSupport.h>
#include <SCE/interface/SCEGeometryInstance.h>

static unsigned int ninstances = 0;

typedef void (*SCE_FInstanceGroupRenderFunc)(SCE_SGeometryInstanceGroup*);

static void SCE_Instance_RenderSimple (SCE_SGeometryInstanceGroup*);
static void SCE_Instance_RenderPseudo (SCE_SGeometryInstanceGroup*);
static void SCE_Instance_RenderHardware (SCE_SGeometryInstanceGroup*);

void SCE_Instance_InitCount (void)
{
    ninstances = 0;
}
unsigned int SCE_Instance_GetCount (void)
{
    return ninstances;
}

static SCE_FInstanceGroupRenderFunc renderfuncs[3] =
{
    SCE_Instance_RenderSimple,
    SCE_Instance_RenderPseudo,
    SCE_Instance_RenderHardware
};

static void SCE_Instance_DefaultRenderFunc (SCE_SGeometryInstance*);

void SCE_Instance_Init (SCE_SGeometryInstance *inst)
{
    inst->m = NULL;
    inst->renderfunc = SCE_Instance_DefaultRenderFunc;
    inst->group = NULL;
    inst->removed = SCE_TRUE;
#if SCE_LIST_ITERATOR_NO_MALLOC
    SCE_List_InitIt (&inst->iterator);
    inst->it = &inst->iterator;
#else
    inst->it = NULL;
#endif
    inst->data = NULL;
}

void SCE_Instance_InitGroup (SCE_SGeometryInstanceGroup *group)
{
    group->mesh = NULL;
    group->instances = NULL;
    group->type = SCE_SIMPLE_INSTANCING;
    group->attrib1 = 3; /* lulz */
    group->attrib1 = 4;
    group->attrib1 = 5;
}


SCE_SGeometryInstance* SCE_Instance_Create (void)
{
    SCE_SGeometryInstance *inst = NULL;

    SCE_btstart ();
    if (!(inst = SCE_malloc (sizeof *inst)))
        goto failure;
    SCE_Instance_Init (inst);
#if !SCE_LIST_ITERATOR_NO_MALLOC
    if (!(inst->it = SCE_List_CreateIt ()))
        goto failure;
#endif
    /* for compatibility with the group functions */
    SCE_List_SetData (inst->it, inst);
    goto success;

failure:
    SCE_Instance_Delete (inst), inst = NULL;
    Logger_LogSrc ();
success:
    SCE_btend ();
    return inst;
}

void SCE_Instance_Delete (SCE_SGeometryInstance *inst)
{
    if (inst)
    {
        SCE_Instance_RemoveInstance (inst);
#if !SCE_LIST_ITERATOR_NO_MALLOC
        SCE_List_DeleteIt (inst->it);
#endif
        SCE_free (inst);
    }
}

/* used for SCE_List_Create() */
static void SCE_Instance_YouDontHaveGroup (void *i)
{
    SCE_SGeometryInstance *inst = i;
    inst->group = NULL;
    inst->removed = SCE_TRUE;
}

SCE_SGeometryInstanceGroup* SCE_Instance_CreateGroup (void)
{
    SCE_SGeometryInstanceGroup *group = NULL;

    SCE_btstart ();
    if (!(group = SCE_malloc (sizeof *group)))
        goto failure;
    SCE_Instance_InitGroup (group);
    if (!(group->instances = SCE_List_Create (SCE_Instance_YouDontHaveGroup)))
        goto failure;
    /* each instance manages its own iterator */
    SCE_List_CanDeleteIterators (group->instances, SCE_FALSE);
    goto success;

failure:
    SCE_Instance_DeleteGroup (group), group = NULL;
    Logger_LogSrc ();
success:
    SCE_btend ();
    return group;
}

void SCE_Instance_DeleteGroup (SCE_SGeometryInstanceGroup *group)
{
    if (group)
    {
        SCE_List_Delete (group->instances);
        SCE_free (group);
    }
}

/**
 * \brief Defines the instancing method for a group
 * \param type can be SCE_SIMPLE_INSTANCING, SCE_PSEUDO_INSTANCING or
 * SCE_HARDWARE_INSTANCING
 */
void SCE_Instance_SetInstancingType (SCE_SGeometryInstanceGroup *group,
                                     int type)
{
    if (type == SCE_HARDWARE_INSTANCING && !SCE_CHasCap (SCE_HW_INSTANCING))
    {
        group->type = SCE_SIMPLE_INSTANCING;
#ifdef SCE_DEBUG
        Logger_PrintMsg ("hardware instancing is not supported,"
                         "using simple instancing");
#endif
    }
    else
        group->type = type;
}

void SCE_Instance_SetAttribIndices (SCE_SGeometryInstanceGroup *group,
                                    int a1, int a2, int a3)
{
    group->attrib1 = a1; group->attrib2 = a2; group->attrib3 = a3;
}

/**
 * \brief Adds an instance into a group of instances
 * \param group the group where add the instance (can be NULL)
 * \param inst the instance to add
 *
 * If \p group is NULL, \p inst is added to the previous group where it was,
 * if \p inst was never added to a group, calling this function with \p group
 * NULL generates a segmentation fault (in the better case).
 */
void SCE_Instance_AddInstance (SCE_SGeometryInstanceGroup *group,
                               SCE_SGeometryInstance *inst)
{
    if (!group || group == inst->group)
    {
        if (inst->removed) /* in this case, inst should have a group... */
            SCE_List_Prependl (inst->group->instances, inst->it);
    }
    else
    {
        SCE_Instance_RemoveInstance (inst);
        SCE_List_Prependl (group->instances, inst->it);
        inst->group = group;
    }
    inst->removed = SCE_FALSE;
}
/**
 * \brief Removes an instance from its group
 * \param inst the instance to remove
 */
void SCE_Instance_RemoveInstance (SCE_SGeometryInstance *inst)
{
    if (!inst->removed)
    {
        SCE_List_Remove (inst->group->instances, inst->it);
        inst->removed = SCE_TRUE;
    }
}

/**
 * \brief Gets the list of the instances of the given group
 */
SCE_SList* SCE_Instance_GetGroupInstancesList(SCE_SGeometryInstanceGroup *group)
{
    return group->instances;
}

/**
 * \brief Indicates if an instances group have any instance
 */
int SCE_Instance_HaveGroupInstance (SCE_SGeometryInstanceGroup *group)
{
    return (SCE_List_GetFirst (group->instances) != NULL);
}

/**
 * \brief Defines the mesh of an instances group
 */
void SCE_Instance_SetGroupMesh (SCE_SGeometryInstanceGroup *group,
                                SCE_SMesh *mesh)
{
    group->mesh = mesh;
}
/**
 * \brief Gets the mesh of an instances group
 */
SCE_SMesh* SCE_Instance_GetGroupMesh (SCE_SGeometryInstanceGroup *group)
{
    return group->mesh;
}


void SCE_Instance_SetMatrix (SCE_SGeometryInstance *inst, SCE_TMatrix4 m)
{
    inst->m = m;
}
float* SCE_Instance_GetMatrix (SCE_SGeometryInstance *inst)
{
    return inst->m;
}

SCE_SGeometryInstanceGroup* SCE_Instance_GetGroup (SCE_SGeometryInstance *inst)
{
    return inst->group;
}

/**
 * \brief Defines the render callback function for rendering the given instance
 * \param f the callback function
 *
 * If \p f is NULL the default render function is specified instead.
 */
void SCE_Instance_SetRenderCallback (SCE_SGeometryInstance *inst,
                                     SCE_FGeometryInstanceRenderFunc f)
{
    inst->renderfunc = (f ? f : SCE_Instance_DefaultRenderFunc);
}


void SCE_Instance_SetData (SCE_SGeometryInstance *inst, void *data)
{
    inst->data = data;
}
void* SCE_Instance_GetData (SCE_SGeometryInstance *inst)
{
    return inst->data;
}


static void SCE_Instance_DefaultRenderFunc (SCE_SGeometryInstance *inst)
{
    (void)inst;
    SCE_Mesh_Draw ();
}

static void SCE_Instance_RenderSimple (SCE_SGeometryInstanceGroup *group)
{
    SCE_SListIterator *it = NULL;
    SCE_SGeometryInstance *inst = NULL;

    /* bind geometry */
    SCE_Mesh_Use (group->mesh);
    /* then for each instance, render it */
    SCE_List_ForEach (it, group->instances)
    {
        inst = SCE_List_GetData (it);
        SCE_CPushMatrix ();
        SCE_CMultMatrix (inst->m);
        inst->renderfunc (inst);
        SCE_CPopMatrix ();
        ninstances++;
    }
}

static void SCE_Instance_RenderPseudo (SCE_SGeometryInstanceGroup *group)
{
    SCE_TMatrix4 modelview, final;
    SCE_SListIterator *it = NULL;
    SCE_SGeometryInstance *inst = NULL;

    /* get the current matrix */
    SCE_CGetMatrix (SCE_MAT_MODELVIEW, modelview);

    /* bind geometry */
    SCE_Mesh_Use (group->mesh);
    /* then for each instance, render it */
    SCE_List_ForEach (it, group->instances)
    {
        inst = SCE_List_GetData (it);
        /* compute final matrix */
        SCE_Matrix4_Mul (modelview, inst->m, final);
        /* set persistent vertex attributes */ /* TODO: pouha */
        glVertexAttrib4fv (group->attrib1, &final[0]);
        glVertexAttrib4fv (group->attrib2, &final[4]);
        glVertexAttrib4fv (group->attrib3, &final[8]);
        inst->renderfunc (inst);
    }
}

static void SCE_Instance_RenderHardware (SCE_SGeometryInstanceGroup *group)
{
    /* not yet implemented */
}

void SCE_Instance_RenderGroup (SCE_SGeometryInstanceGroup *group)
{
    renderfuncs[group->type] (group);
}
