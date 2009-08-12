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
   updated: 11/08/2009 */

#ifndef SCECVERTEXBUFFER_H
#define SCECVERTEXBUFFER_H

#include <SCE/utils/SCEList.h>
#include <SCE/core/SCECBuffer.h>
#include <SCE/core/SCECVertexArray.h>
#include <SCE/SCECommon.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup vertexbuffer
 * @{
 */

/**
 * \brief Render modes for a vertex buffer
 */
enum sce_cbufferrendermode {
    SCE_VA_RENDER_MODE,         /**< Use simple vertex arrays */
    SCE_VBO_RENDER_MODE,        /**< Use vertex buffer objects */
    SCE_VAO_RENDER_MODE,        /**< Use one vertex array object for each
                                 * vertex array in the vertex buffer */
    SCE_UNIFIED_VAO_RENDER_MODE,/**< Use one single vertex array object
                                 * for the entire vertex buffer. It disables
                                 * the ability of enable/disable vertex arrays
                                 * of the vertex buffer (see
                                 * SCE_CEnableVertexBufferData()) but improves
                                 * performances when rendering it. */
};
/** \copydoc sce_cbufferrendermode */
typedef enum sce_cbufferrendermode SCE_CBufferRenderMode;

/** \copydoc sce_cvertexbuffer  */
typedef struct sce_cvertexbuffer SCE_CVertexBuffer;

/** \copydoc sce_cvertexbufferdata */
typedef struct sce_cvertexbufferdata SCE_CVertexBufferData;
/**
 * \brief Data of a vertex buffer
 */
struct sce_cvertexbufferdata {
    SCE_CBufferData data;       /**< Buffer data */
    SCE_CVertexArraySequence seq; /**< Global setup sequence (VAO) */
    SCE_SList arrays;           /**< Vertex arrays (many means interleaved) */
    size_t stride;              /**< Final stride */
    SCE_SListIterator it;       /**< Used to store the structure into a vertex
                                 * buffer */
    SCE_CVertexBuffer *vb;      /**< The vertex buffer using this structure */
};

typedef void (*SCE_FUseVBFunc)(SCE_CVertexBuffer*);
/**
 * \brief A vertex buffer
 */
struct sce_cvertexbuffer {
    SCE_CVertexArraySequence seq; /**< Global setup sequence (VAO) */
    SCE_CBuffer buf;            /**< Core buffer */
    SCE_SList data;             /**< CVertexBufferData, memory managed by the
                                 * vertex buffer */
    SCE_FUseVBFunc use;         /**< Setup function */
    SCE_CBufferRenderMode rmode;/**< Render mode set when built */
    unsigned int n_vertices;    /**< Number of vertices in the vertex buffer */
};
/** \copydoc sce_cindexbuffer */
typedef struct sce_cindexbuffer SCE_CIndexBuffer;
/**
 * \brief An index buffer
 */
struct sce_cindexbuffer {
    SCE_CBuffer buf;            /**< Core buffer */
    SCE_CBufferData data;
    SCE_CIndexArray ia;
    unsigned int n_indices;     /**< Number of indices */
};

/** @} */

void SCE_CInitVertexBufferData (SCE_CVertexBufferData*);
SCE_CVertexBufferData* SCE_CCreateVertexBufferData (void);
void SCE_CClearVertexBufferData (SCE_CVertexBufferData*);
void SCE_CDeleteVertexBufferData (SCE_CVertexBufferData*);

void SCE_CInitVertexBuffer (SCE_CVertexBuffer*);
SCE_CVertexBuffer* SCE_CCreateVertexBuffer (void);
void SCE_CClearVertexBuffer (SCE_CVertexBuffer*);
void SCE_CDeleteVertexBuffer (SCE_CVertexBuffer*);

void SCE_CInitIndexBuffer (SCE_CIndexBuffer*);
SCE_CIndexBuffer* SCE_CCreateIndexBuffer (void);
void SCE_CClearIndexBuffer (SCE_CIndexBuffer*);
void SCE_CDeleteIndexBuffer (SCE_CIndexBuffer*);

void SCE_CAddVertexBufferDataArray (SCE_CVertexBufferData*,
                                    SCE_CVertexArray*, size_t);
SCE_CVertexArray* SCE_CAddVertexBufferDataNewArray (SCE_CVertexBufferData*,
                                                    SCE_CVertexArrayData*);
void SCE_CModifiedVertexBufferData (SCE_CVertexBufferData*, const size_t*);
#if 0
void SCE_CEnableVertexBufferData (SCE_CVertexBufferData*);
void SCE_CDisableVertexBufferData (SCE_CVertexBufferData*);
#endif

SCE_CBuffer* SCE_CGetVertexBufferBuffer (SCE_CVertexBuffer*);
void SCE_CAddVertexBufferData (SCE_CVertexBuffer*, SCE_CVertexBufferData*);
void SCE_CRemoveVertexBufferData (SCE_CVertexBufferData*);

void SCE_CBuildVertexBuffer (SCE_CVertexBuffer*, SCE_CBufferUsage,
                             SCE_CBufferRenderMode);
void SCE_CSetVertexBufferRenderMode (SCE_CVertexBuffer*, SCE_CBufferRenderMode);
void SCE_CUpdateVertexBuffer (SCE_CVertexBuffer*) SCE_GNUC_DEPRECATED;
void SCE_CUseVertexBuffer (SCE_CVertexBuffer*);
void SCE_CRenderVertexBuffer (SCEenum);
void SCE_CRenderVertexBufferInstanced (SCEenum, SCEuint);

void SCE_CModifiedIndexBuffer (SCE_CIndexBuffer*, const size_t*);
SCE_CBuffer* SCE_CGetIndexBufferBuffer (SCE_CIndexBuffer*);
void SCE_CSetIndexBufferIndexArray (SCE_CIndexBuffer*, SCE_CIndexArray*,
                                    SCEuint);
void SCE_CSetIndexBufferIndices (SCE_CIndexBuffer*, SCEenum, unsigned int,
                                 void*);
void SCE_CBuildIndexBuffer (SCE_CIndexBuffer*, SCE_CBufferUsage);
void SCE_CUseIndexBuffer (SCE_CIndexBuffer*);
void SCE_CRenderVertexBufferIndexed (SCEenum);
void SCE_CRenderVertexBufferIndexedInstanced (SCEenum, SCEuint);

void SCE_CFinishVertexBufferRender (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
