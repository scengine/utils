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
   updated: 27/02/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECore.h>
#include <SCE/core/SCECSupport.h>
#include <SCE/interface/SCEGeometryInstance.h>

typedef void (*SCE_FInstanceGroupRenderFunc)(SCE_SGeometryInstanceGroup*);

static void SCE_Instance_RenderSimple (SCE_SGeometryInstanceGroup*);
static void SCE_Instance_RenderPseudo (SCE_SGeometryInstanceGroup*);
static void SCE_Instance_RenderHardware (SCE_SGeometryInstanceGroup*);

static SCE_FInstanceGroupRenderFunc renderfuncs[3] =
{
    SCE_Instance_RenderSimple,
    SCE_Instance_RenderPseudo,
    SCE_Instance_RenderHardware
};

void SCE_Instance_Init (SCE_SGeometryInstance *inst)
{
    inst->m = NULL;
    inst->group = NULL;
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
    if (type != SCE_HARDWARE_INSTANCING || SCE_CHasCap (SCE_HW_INSTANCING))
        group->type = type;
    else
    {
        group->type = SCE_SIMPLE_INSTANCING;
#ifdef SCE_DEBUG
        Logger_PrintMsg ("hardware instancing is not supported,"
                         "using simple instancing.\n");
#endif
    }
}
/**
 * \brief Defines the vertex attributes for giving the modelview matrix
 * \param a1 attribute index of the vector for the matrix row 1
 * \param a2 attribute index of the vector for the matrix row 2
 * \param a3 attribute index of the vector for the matrix row 3
 */
void SCE_Instance_SetAttribIndices (SCE_SGeometryInstanceGroup *group,
                                    int a1, int a2, int a3)
{
    group->attrib1 = a1; group->attrib2 = a2; group->attrib3 = a3;
}
/**
 * \brief Gets the list of the instances of \p group
 * \returns \p group->instances
 */
SCE_SList* SCE_Instance_GetInstancesList (SCE_SGeometryInstanceGroup *group)
{
    return group->instances;
}

/**
 * \brief Adds an instance into a group of instances
 * \param group the group where add the instance
 * \param inst the instance to add
 */
void SCE_Instance_AddInstance (SCE_SGeometryInstanceGroup *group,
                               SCE_SGeometryInstance *inst)
{
    SCE_List_Prependl (group->instances, inst->it);
}
/**
 * \brief Removes an instance from its group
 * \param inst the instance to remove
 */
void SCE_Instance_RemoveInstance (SCE_SGeometryInstance *inst)
{
    SCE_List_Removel (inst->it);
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
int SCE_Instance_HasGroupInstance (SCE_SGeometryInstanceGroup *group)
{
    return (SCE_List_HasElements (group->instances));
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


void SCE_Instance_SetData (SCE_SGeometryInstance *inst, void *data)
{
    inst->data = data;
}
void* SCE_Instance_GetData (SCE_SGeometryInstance *inst)
{
    return inst->data;
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
        SCE_Mesh_Draw ();
        SCE_CPopMatrix ();
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
        SCE_Mesh_Draw ();
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
