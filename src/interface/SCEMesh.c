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

/* created: 31/07/2009
   updated: 01/08/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEResource.h>
#include <SCE/core/SCECSupport.h>
#include <SCE/interface/SCEMesh.h>

typedef void (*SCE_SMeshRenderFunc)(SCEenum);
typedef void (*SCE_SMeshRenderInstancedFunc)(SCEenum, SCEuint);

static int is_init = SCE_FALSE;

static int resource_type = 0;

static SCE_SMesh *mesh_bound = NULL;
static SCE_SMeshRenderFunc render_func = NULL;
static SCE_SMeshRenderInstancedFunc render_func_instanced = NULL;

static void* SCE_Mesh_LoadResource (const char*, int, void*);

int SCE_Init_Mesh (void)
{
    if (is_init)
        return SCE_OK;
    resource_type = SCE_Resource_RegisterType (SCE_FALSE,
                                               SCE_Mesh_LoadResource, NULL);
    if (resource_type < 0)
        goto fail;
    is_init = SCE_TRUE;
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("failed to initialize mesh module");
    return SCE_ERROR;
}
void SCE_Quit_Mesh (void)
{
    is_init = SCE_FALSE;
}

void SCE_Mesh_InitArray (SCE_SMeshArray *marray)
{
    SCE_CInitVertexBufferData (&marray->data);
    SCE_Geometry_InitArrayUser (&marray->auser);
    SCE_List_InitIt (&marray->it);
    SCE_List_SetData (&marray->it, marray);
}
SCE_SMeshArray* SCE_Mesh_CreateArray (void)
{
    SCE_SMeshArray *marray = NULL;
    if (!(marray = SCE_malloc (sizeof *marray)))
        SCEE_LogSrc ();
    else
        SCE_Mesh_InitArray (marray);
    return marray;
}
static void SCE_Mesh_RemoveArray (SCE_SMeshArray*);
void SCE_Mesh_ClearArray (SCE_SMeshArray *marray)
{
    SCE_CClearVertexBufferData (&marray->data);
    SCE_Geometry_ClearArrayUser (&marray->auser);
    SCE_Mesh_RemoveArray (marray);
}
void SCE_Mesh_DeleteArray (SCE_SMeshArray *marray)
{
    if (marray) {
        SCE_Mesh_ClearArray (marray);
        SCE_free (marray);
    }
}

void SCE_Mesh_InitBuffer (SCE_SMeshBuffer *mbuf)
{
    SCE_CInitVertexBuffer (&mbuf->vb);
    SCE_List_InitIt (&mbuf->it);
    SCE_List_SetData (&mbuf->it, mbuf);
}
SCE_SMeshBuffer* SCE_Mesh_CreateBuffer (void)
{
    SCE_SMeshBuffer *mbuf = NULL;
    return mbuf;
}
static void SCE_Mesh_RemoveBuffer (SCE_SMeshBuffer*);
void SCE_Mesh_ClearBuffer (SCE_SMeshBuffer *mbuf)
{
    SCE_CClearVertexBuffer (&mbuf->vb);
    SCE_Mesh_RemoveBuffer (mbuf);
}
void SCE_Mesh_DeleteBuffer (SCE_SMeshBuffer *buf)
{
    if (buf) {
        SCE_Mesh_ClearBuffer (buf);
        SCE_free (buf);
    }
}

static void SCE_Mesh_FreeArray (void *marray)
{
    SCE_Mesh_RemoveArray (marray);
}
static void SCE_Mesh_FreeBuffer (void *mbuf)
{
    SCE_Mesh_DeleteBuffer (mbuf);
}
void SCE_Mesh_Init (SCE_SMesh *mesh)
{
    mesh->geom = NULL;
    mesh->canfree_geom = SCE_FALSE;
    mesh->prim = SCE_POINTS;    /* do not take any risk. */
    SCE_List_Init (&mesh->arrays);
    SCE_List_SetFreeFunc (&mesh->arrays, SCE_Mesh_FreeArray);
    SCE_List_Init (&mesh->buffers);
    SCE_List_SetFreeFunc (&mesh->buffers, SCE_Mesh_FreeBuffer);
    SCE_CInitIndexBuffer (&mesh->ib);
    mesh->use_ib = SCE_FALSE;
    mesh->rmode = SCE_VA_RENDER_MODE;
    mesh->bmode = 0;            /* TODO: will do. */
}
SCE_SMesh* SCE_Mesh_Create (void)
{
    SCE_SMesh *mesh = NULL;
    if (!(mesh = SCE_malloc (sizeof *mesh)))
        SCEE_LogSrc ();
    else
        SCE_Mesh_Init (mesh);
    return mesh;
}
SCE_SMesh* SCE_Mesh_CreateFrom (SCE_SGeometry *geom)
{
    SCE_SMesh *mesh = NULL;
    if (!(mesh = SCE_Mesh_Create ()))
        SCEE_LogSrc ();
    else if (SCE_Mesh_MakeFrom (mesh, geom) < 0) {
        SCE_Mesh_Delete (mesh), mesh = NULL;
        SCEE_LogSrc ();
    }
    return mesh;
}
void SCE_Mesh_Clear (SCE_SMesh *mesh)
{
    SCE_CClearIndexBuffer (&mesh->ib);
    SCE_List_Clear (&mesh->arrays);
    SCE_List_Clear (&mesh->buffers);
}
void SCE_Mesh_Delete (SCE_SMesh *mesh)
{
    if (mesh) {
        SCE_Mesh_Clear (mesh);
        SCE_free (mesh);
    }
}


static void SCE_Mesh_UpdateArrayCallback (void *data, size_t *range)
{
    SCE_CModifiedVertexBufferData (data, range);
}
/**
 * \internal
 * \brief Sets the geometry array of a mesh array
 * \sa SCE_Mesh_AddArray(), SCE_Mesh_AddArrayFrom()
 */
static void SCE_Mesh_SetArrayArray (SCE_SMeshArray *marray,
                                    SCE_SGeometryArray *array,
                                    unsigned int n_vertices)
{
    SCE_CSetVertexBufferDataArrayData (&marray->data,
                                       SCE_Geometry_GetArrayData (array),
                                       n_vertices);
    SCE_Geometry_AddUser (array, &marray->auser, SCE_Mesh_UpdateArrayCallback,
                          &marray->data);
}


/**
 * \internal
 */
static void SCE_Mesh_AddArray (SCE_SMesh *mesh, SCE_SMeshArray *marray)
{
/*     if (marray->mesh) */
    SCE_List_Appendl (&mesh->arrays, &marray->it);
}
/**
 * \internal
 */
static SCE_SMeshArray* SCE_Mesh_AddNewArray (SCE_SMesh *mesh)
{
    SCE_SMeshArray *marray = NULL;
    if (!(marray = SCE_Mesh_CreateArray ()))
        SCEE_LogSrc ();
    else
        SCE_Mesh_AddArray (mesh, marray);
    return marray;
}
/**
 * \internal
 */
static SCE_SMeshArray* SCE_Mesh_AddNewArrayFrom (SCE_SMesh *mesh,
                                                 SCE_SGeometryArray *array,
                                                 unsigned int n_vertices)
{
    SCE_SMeshArray *marray = NULL;
    if (!(marray = SCE_Mesh_AddNewArray (mesh)))
        SCEE_LogSrc ();
    else
        SCE_Mesh_SetArrayArray (marray, array, n_vertices);
    return marray;
}
/**
 * \internal
 */
static void SCE_Mesh_RemoveArray (SCE_SMeshArray *marray)
{
    SCE_List_Remove (&marray->it);
}

/**
 * \brief Sets the geometry of a mesh
 * \sa SCE_Mesh_CreateFrom(), SCE_Mesh_Build(), SCE_SGeometry
 */
int SCE_Mesh_SetGeometry (SCE_SMesh *mesh, SCE_SGeometry *geom, int canfree)
{
    unsigned int n_vertices = 0;
    SCE_SGeometryArray *index_array = NULL;
    SCE_SListIterator *it = NULL;
    SCE_SList *arrays = NULL;

    n_vertices = SCE_Geometry_GetNumVertices (geom);
    arrays = SCE_Geometry_GetArrays (geom);
    SCE_List_ForEach (it, arrays) {
        if (!SCE_Mesh_AddNewArrayFrom (mesh, SCE_List_GetData (it), n_vertices))
            goto fail;
    }
    arrays = SCE_Geometry_GetModifiedArrays (geom);
    SCE_List_ForEach (it, arrays) {
        if (!SCE_Mesh_AddNewArrayFrom (mesh, SCE_List_GetData (it), n_vertices))
            goto fail;
    }
    index_array = SCE_Geometry_GetIndexArray (geom);
    if (index_array) {
        /* NOTE: not very beautiful... */
        SCE_CIndexArray ia;
        SCE_CVertexArrayData *vdata = SCE_Geometry_GetArrayData (index_array);
        ia.type = vdata->type;
        ia.data = vdata->data;
        SCE_CSetIndexBufferIndexIndexArray (&mesh->ib, ia,
                                            SCE_Geometry_GetNumIndices (geom));
        mesh->use_ib = SCE_TRUE;
    }
    mesh->prim = SCE_Geometry_GetPrimitiveType (geom);
    mesh->geom = geom;
    mesh->canfree_geom = canfree;
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}


/**
 * \internal
 */
static void SCE_Mesh_AddBufferArray (SCE_SMeshBuffer *mbuf,
                                     SCE_SMeshArray *marray)
{
    SCE_CAddVertexBufferData (&mbuf->vb, &marray->data);
}

/**
 * \internal
 * \brief Adds a buffer to a mesh
 * \sa SCE_Mesh_RemoveBuffer()
 */
static void SCE_Mesh_AddBuffer (SCE_SMesh *mesh, SCE_SMeshBuffer *mbuf)
{
    SCE_List_Appendl (&mesh->buffers, &mbuf->it);
}
/**
 * \internal
 * \brief Adds a new buffer to a mesh
 */
static SCE_SMeshBuffer* SCE_Mesh_AddNewBuffer (SCE_SMesh *mesh)
{
    SCE_SMeshBuffer *mbuf = NULL;
    if (!(mbuf = SCE_Mesh_CreateBuffer ()))
        SCEE_LogSrc ();
    else
        SCE_Mesh_AddBuffer (mesh, mbuf);
    return mbuf;
}
/**
 * \internal
 * \brief Adds a new buffer to a mesh from a mesh array
 */
static SCE_SMeshBuffer* SCE_Mesh_AddNewBufferFrom (SCE_SMesh *mesh,
                                                   SCE_SMeshArray *marray)
{
    SCE_SMeshBuffer *mbuf = NULL;
    if (!(mbuf = SCE_Mesh_AddNewBuffer (mesh)))
        SCEE_LogSrc ();
    else
        SCE_Mesh_AddBufferArray (mbuf, marray);
    return mbuf;
}
/**
 * \internal
 * \brief Removes a buffer from a mesh
 * \sa SCE_Mesh_AddBuffer()
 */
static void SCE_Mesh_RemoveBuffer (SCE_SMeshBuffer *mbuf)
{
    SCE_List_Remove (&mbuf->it);
}


static int SCE_Mesh_MakeIndependantVB (SCE_SMesh *mesh)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, &mesh->arrays) {
        if (!SCE_Mesh_AddNewBufferFrom (mesh, SCE_List_GetData (it)))
            goto fail;
    }
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}
static int SCE_Mesh_MakeGlobalVB (SCE_SMesh *mesh)
{
    SCE_SListIterator *it = NULL;
    SCE_SMeshBuffer *mbuf = NULL;

    if (!(mbuf = SCE_Mesh_AddNewBuffer (mesh))) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    SCE_List_ForEach (it, &mesh->arrays)
        SCE_Mesh_AddBufferArray (mbuf, SCE_List_GetData (it));
    return SCE_OK;
}

/**
 * \internal
 * \todo remove "usage" parameter, defines specific usage for each stream
 * according to mesh usage (animated or not)
 */
static void SCE_Mesh_BuildBuffers (SCE_SMesh *mesh, SCE_CBufferRenderMode mode,
                                   SCE_CBufferUsage usage)
{
    SCE_SListIterator *it = NULL;

    if (mesh->use_ib)
        SCE_CBuildIndexBuffer (&mesh->ib, SCE_BUFFER_STATIC_DRAW);
    if (mode != SCE_UNIFIED_VBO_RENDER_MODE) {
        SCE_List_ForEach (it, &mesh->buffers) {
            SCE_SMeshBuffer *mbuf = SCE_List_GetData (it);
            SCE_CBuildVertexBuffer (&mbuf->vb, usage, mode);
        }
    } else {
        SCE_SMeshBuffer *rootbuf = NULL;
        SCE_SListIterator *it2 = NULL;
        it = SCE_List_GetFirst (&mesh->buffers);
        rootbuf = SCE_List_GetData (it);
        it = it2 = SCE_List_GetNext (it);
        SCE_List_ForEachNext (it) {
            SCE_SMeshBuffer *mbuf = SCE_List_GetData (it);
            SCE_CBuildVertexBuffer (&mbuf->vb, usage, SCE_VBO_RENDER_MODE);
        }
        SCE_CBuildVertexBuffer (&rootbuf->vb, usage, mode);
        SCE_List_ForEachNext (it2)
            SCE_CUseVertexBuffer (SCE_List_GetData (it));
        if (mesh->use_ib)
            SCE_CUseIndexBuffer (&mesh->ib);
        SCE_CFinishVertexBufferRender (); /* it calls CEndVertexArraySequence */
    }
}
/**
 * \brief Builds a mesh by creating vertex buffers with requested modes
 *
 * If \p rmode is not supported (according to SCE_VAO and SCE_VBO, declared in
 * SCECSupport), a more common render mode is used (generally simple vertex
 * arrays).
 * \sa SCE_Mesh_AutoBuild()
 * \todo use stream concept, so kick \p bmode param
 */
int SCE_Mesh_Build (SCE_SMesh *mesh, SCE_EMeshBuildMode bmode,
                    SCE_CBufferRenderMode rmode, int animated)
{
    int err = SCE_ERROR;

    /* SCE_Mesh_Unbuild (mesh); */

    /* manage extension support */
    if (rmode > SCE_VBO_RENDER_MODE && !SCE_CHasCap (SCE_VAO))
        rmode = SCE_VBO_RENDER_MODE;
    if (rmode == SCE_VBO_RENDER_MODE && !SCE_CHasCap (SCE_VBO))
        rmode = SCE_VA_RENDER_MODE;
/*     if (rmode == SCE_VA_RENDER_MODE && !SCE_CHasCap (SCE_VA)) */ /* lulz */
    switch (bmode) {
    case SCE_INDEPENDANT_VERTEX_BUFFER:
        err = SCE_Mesh_MakeIndependantVB (mesh);
        break;
    case SCE_GLOBAL_VERTEX_BUFFER:
        err = SCE_Mesh_MakeGlobalVB (mesh);
        break;
    }
    if (err < 0)
        goto fail;
    {
        SCE_CBufferUsage usage;
        usage = (animated ? SCE_BUFFER_DYNAMIC_DRAW : SCE_BUFFER_STATIC_DRAW);
        SCE_Mesh_BuildBuffers (mesh, rmode, usage);
    }
    mesh->bmode = bmode;
    mesh->rmode = rmode;
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    return SCE_ERROR;
}

/**
 * \todo use stream instead of "buffers"
 * \brief Uses default value for mesh build mode and determine the best buffer
 * render mode
 *
 * It is recommanded to use this function to build meshes so the SCEngine can
 * do some optimization under data structure to prevent for rendering
 * situations that needs specific mesh data structure.
 * \sa SCE_Mesh_Build()
 */
int SCE_Mesh_AutoBuild (SCE_SMesh *mesh, int animated)
{
    if (SCE_CHasCap (SCE_VAO))
        return SCE_Mesh_Build (mesh, SCE_INDEPENDANT_VERTEX_BUFFER,
                               SCE_UNIFIED_VAO_RENDER_MODE, animated);
    else
        return SCE_Mesh_Build (mesh, SCE_INDEPENDANT_VERTEX_BUFFER,
                               SCE_VBO_RENDER_MODE, animated);
}

/* why change mode on live? */
#if 0
void SCE_Mesh_SetRenderMode (SCE_SMesh *mesh, SCE_CBufferRenderMode rmode)
{
}
#endif

/* why se breaker les balls, les fonctions inutiles c'est chiant
 * TODO: ah non spas inutile, apres generation TBN, besoin de mettre a jour */
#if 0
/**
 * \brief Takes new (if any) arrays from the geometry of a mesh, and rebuild it
 * \sa SCE_Mesh_SetGeometry(), SCE_Mesh_Build()
 */
int SCE_Mesh_Update (SCE_SMesh *mesh)
{
    /* not yet implemented, trop difficulte. (et trop chiant) */
}
#endif

/**
 * \internal
 */
static void* SCE_Mesh_LoadResource (const char *fname, int force, void *data)
{
    SCE_SMesh *mesh = NULL;
    SCE_SGeometry *geom = NULL;
    (void)data;

    if (!(mesh = SCE_Mesh_Create ()))
        goto fail;
    if (force > 0)
        force--;
    if (!(geom = SCE_Geometry_Load (fname, force)))
        goto fail;
    if (SCE_Mesh_SetGeometry (mesh, geom, SCE_TRUE) < 0)
        goto fail;
    return mesh;
fail:
    SCE_Mesh_Delete (mesh);
    SCEE_LogSrc ();
    return NULL;
}
/**
 * \brief Loads a mesh from a file containing geometry
 * \param fname name of the geometry file
 * \param force see SCE_Resource_Load()
 * \returns a new or existing mesh, depends on resource availability
 * and on \p force
 *
 * This function loads a mesh resource by loading a geometry resource using
 * SCE_Geometry_Load() and set it to the mesh. This function doesn't build
 * the returned mesh itself, so you have to do it before using the mesh.
 * \sa SCE_Mesh_SetGeometry(), SCE_Geometry_Load(), SCE_Resource_Load()
 */
SCE_SMesh* SCE_Mesh_Load (const char *fname, int force)
{
    return SCE_Resource_Load (resource_type, fname, force, NULL);
}


/**
 * \brief Declares a mesh as activated for rendering
 * \sa SCE_Mesh_Render(), SCE_Mesh_RenderInstanced(), SCE_Mesh_Unuse()
 */
void SCE_Mesh_Use (SCE_SMesh *mesh)
{
    if (mesh->rmode == SCE_UNIFIED_VBO_RENDER_MODE) {
        SCE_SMeshBuffer *mbuf = NULL;
        mbuf = SCE_List_GetData (SCE_List_GetFirst (&mesh->buffers));
        SCE_CUseVertexBuffer (&mbuf->vb);
    } else {
        SCE_SListIterator *it = NULL;
        SCE_List_ForEach (it, &mesh->buffers) {
            SCE_SMeshBuffer *mbuf = SCE_List_GetData (it);
            SCE_CUseVertexBuffer (&mbuf->vb);
        }
    }
    if (mesh->use_ib) {
        SCE_CUseIndexBuffer (&mesh->ib);
        render_func = SCE_CRenderVertexBufferIndexed;
        render_func_instanced = SCE_CRenderVertexBufferIndexedInstanced;
    } else {
        render_func = SCE_CRenderVertexBuffer;
        render_func_instanced = SCE_CRenderVertexBufferInstanced;
    }
    mesh_bound = mesh;
}
/**
 * \brief Render an instance of the mesh bound with SCE_Mesh_Use()
 * \sa SCE_Mesh_Use(), SCE_Mesh_RenderInstanced(), SCE_Mesh_Unuse()
 */
void SCE_Mesh_Render (void)
{
    render_func (mesh_bound->prim);
}
/**
 * \brief Render \p n_instances instances of the mesh bound with
 * SCE_Mesh_Use() using hardware geometry instancing
 * \sa SCE_Mesh_Use(), SCE_Mesh_Render(), SCE_Mesh_Unuse()
 */
void SCE_Mesh_RenderInstanced (SCEuint n_instances)
{
    render_func_instanced (mesh_bound->prim, n_instances);
}
/**
 * \brief Kicks the bound mesh
 * \note Useless in a pure GL 3 context
 * \sa SCE_Mesh_Use(), SCE_Mesh_Render(), SCE_Mesh_RenderInstanced(),
 * SCE_CFinishVertexBufferRender()
 */
void SCE_Mesh_Unuse (void)
{
    if (SCE_TRUE/*non_full_gl3*/)
        SCE_CFinishVertexBufferRender ();
    mesh_bound = NULL;
}
