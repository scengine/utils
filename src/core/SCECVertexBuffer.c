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
 
/* created: 29/07/2009
   updated: 22/08/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECVertexBuffer.h>

/* NOTE: GLee doesn't define these names.. ? */
#define glDrawArraysInstanced glDrawArraysInstancedARB
#define glDrawElementsInstanced glDrawElementsInstancedARB

#define SCE_BUFFER_OFFSET(p) ((char*)NULL + (p))

static SCE_CVertexBuffer *vb_bound = NULL;
static SCE_CIndexBuffer *ib_bound = NULL;

void SCE_CInitVertexBufferData (SCE_CVertexBufferData *data)
{
    SCE_CInitBufferData (&data->data);
    SCE_CInitVertexArraySequence (&data->seq);
    SCE_List_Init (&data->arrays);
    SCE_List_CanDeleteIterators (&data->arrays, SCE_TRUE);
    data->stride = 0;
    SCE_List_InitIt (&data->it);
    SCE_List_SetData (&data->it, data);
    data->vb = NULL;
}
SCE_CVertexBufferData* SCE_CCreateVertexBufferData (void)
{
    SCE_CVertexBufferData *data = NULL;
    if (!(data = SCE_malloc (sizeof *data)))
        SCEE_LogSrc ();
    else
        SCE_CInitVertexBufferData (data);
    return data;
}
void SCE_CClearVertexBufferData (SCE_CVertexBufferData *data)
{
    SCE_CRemoveVertexBufferData (data);
    SCE_CDeleteVertexArraySequence (&data->seq);
    SCE_List_Clear (&data->arrays);
    SCE_CClearBufferData (&data->data);
}
void SCE_CDeleteVertexBufferData (SCE_CVertexBufferData *data)
{
    if (data) {
        SCE_CClearVertexBufferData (data);
        SCE_free (data);
    }
}

static void SCE_CFreeVertexBufferData (void *vbd)
{
    /* not useless: CRemove set the vb pointer of vbd to NULL */
    SCE_CRemoveVertexBufferData (vbd);
}
void SCE_CInitVertexBuffer (SCE_CVertexBuffer *vb)
{
    SCE_CInitVertexArraySequence (&vb->seq);
    SCE_CInitBuffer (&vb->buf);
    SCE_List_Init (&vb->data);
    SCE_List_SetFreeFunc (&vb->data, SCE_CFreeVertexBufferData);
    vb->use = NULL;
    vb->rmode = SCE_VA_RENDER_MODE;
    vb->n_vertices = 0;
}
SCE_CVertexBuffer* SCE_CCreateVertexBuffer (void)
{
    SCE_CVertexBuffer *vb = NULL;
    if (!(vb = SCE_malloc (sizeof *vb)))
        SCEE_LogSrc ();
    else
        SCE_CInitVertexBuffer (vb);
    return vb;
}
void SCE_CClearVertexBuffer (SCE_CVertexBuffer *vb)
{
    SCE_List_Clear (&vb->data);
    SCE_CClearBuffer (&vb->buf);
    SCE_CDeleteVertexArraySequence (&vb->seq);
}
void SCE_CDeleteVertexBuffer (SCE_CVertexBuffer *vb)
{
    if (vb) {
        SCE_CClearVertexBuffer (vb);
        SCE_free (vb);
    }
}

void SCE_CInitIndexBuffer (SCE_CIndexBuffer *ib)
{
    SCE_CInitBuffer (&ib->buf);
    SCE_CInitBufferData (&ib->data);
    SCE_CInitIndexArray (&ib->ia);
    ib->ia.data = SCE_BUFFER_OFFSET (0);
    ib->n_indices = 0;
}
SCE_CIndexBuffer* SCE_CCreateIndexBuffer (void)
{
    SCE_CIndexBuffer *ib = NULL;
    if (!(ib = SCE_malloc (sizeof *ib)))
        SCEE_LogSrc ();
    else
        SCE_CInitIndexBuffer (ib);
    return ib;
}
void SCE_CClearIndexBuffer (SCE_CIndexBuffer *ib)
{
    SCE_CClearBufferData (&ib->data);
    SCE_CClearBuffer (&ib->buf);
}
void SCE_CDeleteIndexBuffer (SCE_CIndexBuffer *ib)
{
    if (ib) {
        SCE_CClearIndexBuffer (ib);
        SCE_free (ib);
    }
}


/**
 * \brief Adds a vertex array to a vertex buffer data
 *
 * The structure \p data is given to the vertex array of \p vbd
 * calling SCE_CSetVertexArrayData().
 * \sa SCE_CSetVertexArrayData(), SCE_CDeleteVertexBufferDataArrays()
 * SCE_CAddVertexBufferData
 */
int SCE_CAddVertexBufferDataArray (SCE_CVertexBufferData *vbd,
                                   SCE_CVertexArray *va,
                                   size_t n_vertices)
{
    size_t stride;
    SCE_CVertexArrayData *data;
    if (SCE_List_AppendNewl (&vbd->arrays, va) < 0) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    /* get main pointer of the interleaved array */
    data = SCE_CGetVertexArrayData (va);
    if (!vbd->data.data)
        vbd->data.data = data->data;
    else {
        vbd->data.data = (data->data < vbd->data.data ?
                          data->data : vbd->data.data);
    }
    stride = SCE_CSizeof (data->type) * data->size;
    vbd->stride += stride;
    vbd->data.size += stride * n_vertices;
    return SCE_OK;
}
static void SCE_CFreeDataArray (void *a)
{
    SCE_CDeleteVertexArray (a);
}
/**
 * \brief Deletes the arrays of a vertex buffer data
 * \sa SCE_CAddVertexBufferDataArray()
 */
void SCE_CDeleteVertexBufferDataArrays (SCE_CVertexBufferData *vbd)
{
    SCE_List_SetFreeFunc (&vbd->arrays, SCE_CFreeDataArray);
    SCE_List_Clear (&vbd->arrays);
    SCE_List_SetFreeFunc (&vbd->arrays, NULL);
}

/**
 * \brief Set modified vertices range
 * \param range range of modified vertices, [0] is the first modified vertex
 * and [1] the number of modified vertices, if NULL the whole buffer data will
 * be updated.
 * \note the vertex buffer of \p vbd must be built before calling this function
 * \sa SCE_CModifiedBufferData()
 */
void SCE_CModifiedVertexBufferData (SCE_CVertexBufferData *vbd,
                                    const size_t *range)
{
    if (!range)
        SCE_CModifiedBufferData (&vbd->data, NULL);
    else {
        /* consider all arrays */
        size_t r[2];
        r[0] = range[0] * vbd->stride;
        r[1] = range[1] * vbd->stride;
        SCE_CModifiedBufferData (&vbd->data, r);
    }
}
#if 0
/**
 * \brief Enables the given vertex buffer data for the render
 * \sa SCE_CSetVertexBufferRenderMode()
 */
void SCE_CEnableVertexBufferData (SCE_CVertexBufferData *vbd)
{
    SCE_List_Remove (&vbd->it);
    SCE_List_Appendl (&vbd->vb->data, &vbd->it);
}
/**
 * \brief Disables the given vertex buffer data for the render
 * \sa SCE_CSetVertexBufferRenderMode()
 */
void SCE_CDisableVertexBufferData (SCE_CVertexBufferData *vbd)
{
    SCE_List_Remove (&vbd->it);
}
#endif


/**
 * \brief Gets the buffer of a vertex buffer
 */
SCE_CBuffer* SCE_CGetVertexBufferBuffer (SCE_CVertexBuffer *vb)
{
    return &vb->buf;
}

/**
 * \brief Adds data to a vertex buffer
 *
 * The memory of \p d is never freed by module CVertexBuffer.
 * \sa SCE_CSetVertexBufferDataArrayData(), SCE_CAddBufferData()
 */
void SCE_CAddVertexBufferData (SCE_CVertexBuffer *vb, SCE_CVertexBufferData *d)
{
    SCE_CAddBufferData (&vb->buf, &d->data);
    SCE_List_Appendl (&vb->data, &d->it);
    d->vb = vb;
}
/**
 * \brief Removes a vertex buffer data from its buffer
 * \sa SCE_CAddVertexBufferData(), SCE_CClearVertexBufferData(),
 * SCE_CRemoveBufferData()
 */
void SCE_CRemoveVertexBufferData (SCE_CVertexBufferData *data)
{
    if (data->vb) {
        SCE_CRemoveBufferData (&data->data);
        SCE_List_Remove (&data->it);
        data->vb = NULL;
    }
}

/**
 * \brief Sets the number of vertices in a vertex buffer
 */
void SCE_CSetVertexBufferNumVertices (SCE_CVertexBuffer *vb, size_t n)
{
    vb->n_vertices = n;
}

static void SCE_CUseVAMode (SCE_CVertexBuffer *vb)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, &vb->data) {
        SCE_SListIterator *it2;
        SCE_CVertexBufferData *data = SCE_List_GetData (it);
        SCE_List_ForEach (it2, &data->arrays)
            SCE_CUseVertexArray (SCE_List_GetData (it2));
    }
}
static void SCE_CUseVBOMode (SCE_CVertexBuffer *vb)
{
    SCE_SListIterator *it = NULL;
    glBindBuffer (GL_ARRAY_BUFFER, vb->buf.id);
    SCE_List_ForEach (it, &vb->data) {
        SCE_SListIterator *it2;
        SCE_CVertexBufferData *data = SCE_List_GetData (it);
        SCE_List_ForEach (it2, &data->arrays)
            SCE_CUseVertexArray (SCE_List_GetData (it2));
    }
}
/* TODO: deprecated too... ? */
static void SCE_CUseVAOMode (SCE_CVertexBuffer *vb)
{
    SCE_SListIterator *it = NULL;
    glBindBuffer (GL_ARRAY_BUFFER, vb->buf.id);
    SCE_List_ForEach (it, &vb->data) {
        SCE_CVertexBufferData *data = SCE_List_GetData (it);
        SCE_CCallVertexArraySequence (data->seq);
    }
}
static void SCE_CUseUnifiedVAOMode (SCE_CVertexBuffer *vb)
{
    SCE_CCallVertexArraySequence (vb->seq);
}
/**
 * \brief Builds a vertex buffer
 * \param usage GL usage of the buffer
 * \param mode rendering method to use with the vertex buffer
 *
 * If \p mode is SCE_UNIFIED_VBO_RENDER_MODE, you may call
 * SCE_CUseVertexBuffer()/SCE_CUseIndexBuffer() for each vertex/index buffer
 * you want to link to \p vb and terminate with SCE_CEndVertexArraySequence().
 * Then using \p vb will do the same as using one by one each vertex buffer you
 * specified.
 * \sa SCE_CSetVertexBufferRenderMode(), SCE_CBufferRenderMode
 */
void SCE_CBuildVertexBuffer (SCE_CVertexBuffer *vb, SCE_CBufferUsage usage,
                             SCE_CBufferRenderMode mode)
{
    SCE_CVertexBufferData *data = NULL;

    vb->rmode = mode;
    if (usage == SCE_BUFFER_DEFAULT_USAGE)
        usage = SCE_BUFFER_STREAM_DRAW;
    if (mode >= SCE_VBO_RENDER_MODE)
        SCE_CBuildBuffer (&vb->buf, GL_ARRAY_BUFFER, usage);

    SCE_CSetVertexBufferRenderMode (vb, mode);
    if (mode == SCE_VAO_RENDER_MODE) {
        SCE_SListIterator *it = NULL;
        /* create one VAO for each vertex buffer data in the vertex buffer */
        SCE_List_ForEach (it, &vb->data) {
            SCE_SListIterator *it2;
            data = SCE_List_GetData (it);
            SCE_CBeginVertexArraySequence (&data->seq);
            SCE_List_ForEach (it2, &data->arrays)
                SCE_CUseVertexArray (SCE_List_GetData (it2));
            SCE_CEndVertexArraySequence ();
        }
    } else if (mode == SCE_UNIFIED_VAO_RENDER_MODE) {
        SCE_CBeginVertexArraySequence (&vb->seq);
        SCE_CUseVBOMode (vb);
        SCE_CEndVertexArraySequence ();
    }
}

/**
 * \brief Sets up the given render mode
 * \param mode desired render mode
 *
 * Note that if you wish a VAO mode, it has to be specified to
 * SCE_CBuildVertexBuffer().
 * \sa SCE_CBuildVertexBuffer(), SCE_CBufferRenderMode
 */
void SCE_CSetVertexBufferRenderMode (SCE_CVertexBuffer *vb,
                                     SCE_CBufferRenderMode mode)
{
    SCE_FUseVBFunc fun = NULL;
    SCE_SListIterator *it = NULL;

    switch (mode) {
    case SCE_VA_RENDER_MODE:
        fun = SCE_CUseVAMode;
        if (vb->use != fun && vb->use) {
            SCE_List_ForEach (it, &vb->data) {
                SCE_SListIterator *it2;
                SCE_CVertexBufferData *vbd = SCE_List_GetData (it);
                SCE_List_ForEach (it2, &vbd->arrays) {
                    SCE_CVertexArrayData *data;
                    data = SCE_CGetVertexArrayData (SCE_List_GetData (it2));
#if 1
                    data->data = &((char*)vbd->data.data)[(size_t)data->data];
#else
                    /* TODO: WTF?? */
                    data->data = (char*)vbd->data.data + (char*)data->data;
#endif
                }
            }
        }
        break;
    case SCE_VBO_RENDER_MODE:
        fun = SCE_CUseVBOMode;
    case SCE_VAO_RENDER_MODE:
        if (!fun)
            fun = SCE_CUseVAOMode;
    case SCE_UNIFIED_VAO_RENDER_MODE:
        if (!fun)
            fun = SCE_CUseUnifiedVAOMode;
        SCE_List_ForEach (it, &vb->data) {
            SCE_SListIterator *it2;
            SCE_CVertexArrayData *data = NULL;
            SCE_CVertexBufferData *vbd = SCE_List_GetData (it);
            SCE_List_ForEach (it2, &vbd->arrays) {
                data = SCE_CGetVertexArrayData (SCE_List_GetData (it2));
                data->data = SCE_BUFFER_OFFSET (vbd->data.first
                                                + (char*)data->data
                                                - (char*)vbd->data.data);
                data->stride = vbd->stride;
            }
        }
        break;
    default:
#ifdef SCE_DEBUG
        SCEE_SendMsg ("unknown buffer render mode %d\n", mode);
#endif
    }
    if (fun)
        vb->use = fun;
}

/**
 * \brief Indicates if a vertex buffer is built
 */
int SCE_CIsVertexBufferBuilt (SCE_CVertexBuffer *vb)
{
    return (vb->use ? SCE_TRUE : SCE_FALSE);
}

/**
 * \brief 
 */
void SCE_CUseVertexBuffer (SCE_CVertexBuffer *vb)
{
    vb->use (vb);
    vb_bound = vb;
}


/**
 * \brief 
 */
void SCE_CRenderVertexBuffer (SCEenum prim)
{
    glDrawArrays (prim, 0, vb_bound->n_vertices);
}
/**
 * \brief 
 */
void SCE_CRenderVertexBufferInstanced (SCEenum prim, SCEuint num)
{
    glDrawArraysInstanced (prim, 0, vb_bound->n_vertices, num);
}


/**
 * \brief Sets the modified range of an index buffer
 * \param range range of modified indices
 * \sa SCE_CModifiedVertexBufferData(), SCE_CModifiedBuffer()
 */
void SCE_CModifiedIndexBuffer (SCE_CIndexBuffer *ib, const size_t *range)
{
    if (!range)
        SCE_CModifiedBuffer (&ib->buf, NULL);
    else {
        size_t r[2];
        size_t size = SCE_CSizeof (ib->ia.type);
        r[0] = range[0] * size;
        r[1] = range[1] * size;
        SCE_CModifiedBuffer (&ib->buf, r);
    }
}
/**
 * \brief Gets the core buffer of an index buffer
 */
SCE_CBuffer* SCE_CGetIndexBufferBuffer (SCE_CIndexBuffer *ib)
{
    return &ib->buf;
}

/**
 * \brief Sets the array data of an index buffer
 *
 * The structure \p ia is copied into \p ib so don't worry about memory
 * management, \p ia can be a static structure.
 * \sa SCE_CSetIndexBufferIndices()
 */
void SCE_CSetIndexBufferIndexArray (SCE_CIndexBuffer *ib, SCE_CIndexArray *ia,
                                    SCEuint n_indices)
{
    ib->data.size = n_indices * SCE_CSizeof (ia->type);
    ib->data.data = ia->data;
    ib->ia.type = ia->type;
    /* don't set ib->ia.data, coz it's just an offset */
    ib->n_indices = n_indices;
}
/**
 * \brief Sets the index array of an index buffer
 *
 * \p indices will never be freed by the vertex buffer module.
 * \sa SCE_CSetIndexBufferIndexArray()
 */
void SCE_CSetIndexBufferIndices (SCE_CIndexBuffer *ib, SCEenum type,
                                 unsigned int n_indices, void *indices)
{
    ib->data.size = n_indices * SCE_CSizeof (type);
    ib->data.data = indices;
    ib->ia.type = type;
    /* don't set ib->ia.data, coz it's just an offset */
    ib->n_indices = n_indices;
}

/**
 * \brief 
 */
void SCE_CBuildIndexBuffer (SCE_CIndexBuffer *ib, SCE_CBufferUsage usage)
{
    if (usage == SCE_BUFFER_DEFAULT_USAGE)
        usage = SCE_BUFFER_STATIC_DRAW;
    SCE_CAddBufferData (&ib->buf, &ib->data);
    SCE_CBuildBuffer (&ib->buf, GL_ELEMENT_ARRAY_BUFFER, usage);
}

/**
 * \brief 
 */
void SCE_CUseIndexBuffer (SCE_CIndexBuffer *ib)
{
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ib->buf.id);
    ib_bound = ib;
}

/**
 * \brief 
 */
void SCE_CRenderVertexBufferIndexed (SCEenum prim)
{
    SCE_CRenderIndexed (prim, &ib_bound->ia, ib_bound->n_indices);
}
/**
 * \brief 
 */
void SCE_CRenderVertexBufferIndexedInstanced (SCEenum prim, SCEuint num)
{
    SCE_CRenderIndexedInstanced (prim, &ib_bound->ia, ib_bound->n_indices, num);
}


/**
 * \brief Deactivate rendering states setup by SCE_CUseVertexBuffer() and
 * SCE_CUseIndexBuffer()
 * \note Useless in a pure GL 3 context.. and for Unbind index buffer?
 */
void SCE_CFinishVertexBufferRender (void)
{
    SCE_CFinishVertexArrayRender ();
    if (vb_bound->rmode == SCE_VBO_RENDER_MODE ||
        vb_bound->rmode == SCE_VAO_RENDER_MODE) {
        glBindBuffer (GL_ARRAY_BUFFER, 0);
        /* otherwise there is no vertex buffer object or the VAO already
           deactivated the vertex buffer */
    }
    if (ib_bound) { /* if NULL, no index buffer */
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
        ib_bound = NULL;
    }
    vb_bound = NULL;
}
