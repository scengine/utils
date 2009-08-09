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
 
/* created: 10/01/2007
   updated: 02/08/2009 */

#ifndef SCECBUFFER_H
#define SCECBUFFER_H

#include <SCE/utils/SCEList.h>
#include <SCE/core/SCECVertexArray.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup corebuffers
 * @{
 */

/**
 * \brief Usage modes for a buffer
 */
enum sce_cbufferusage {
    SCE_BUFFER_STREAM_DRAW = GL_STREAM_DRAW,
    SCE_BUFFER_STREAM_READ = GL_STREAM_READ,
    SCE_BUFFER_STREAM_COPY = GL_STREAM_COPY,
    SCE_BUFFER_STATIC_DRAW = GL_STATIC_DRAW,
    SCE_BUFFER_STATIC_READ = GL_STATIC_READ,
    SCE_BUFFER_STATIC_COPY = GL_STATIC_COPY,
    SCE_BUFFER_DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
    SCE_BUFFER_DYNAMIC_READ = GL_DYNAMIC_READ,
    SCE_BUFFER_DYNAMIC_COPY = GL_DYNAMIC_COPY
};
/** \copydoc sce_cbufferusage */
typedef enum sce_cbufferusage SCE_CBufferUsage;

/** \copydoc sce_cbuffer */
typedef struct sce_cbuffer SCE_CBuffer;

/** \copydoc sce_cbufferdata */
typedef struct sce_cbufferdata SCE_CBufferData;
/**
 * \brief Data of a buffer
 */
struct sce_cbufferdata {
    SCEuint first;              /**< Offset of the data in the buffer */
    size_t size;                /**< Bytes of \c data */
    void *data;                 /**< Data, user is the owner of */
    size_t range[2];            /**< Range of modified bytes \c range[0] is the
                                 *   offset and \c range[1] the number of
                                 *   modified bytes */
    SCE_SListIterator it;
    int modified;               /**< Is this buffer data waiting for update? */
    SCE_CBuffer *buf;
    int user;
};

/**
 * \brief A GL buffer
 */
struct sce_cbuffer {
    SCEuint id;                 /**< GL identifier */
    SCEenum target;             /**< GL target (type of the buffer) */
    size_t size;                /**< Total bytes of the buffer */
    SCE_SList data;             /**< Buffer data (SCE_CBufferData) */
    SCE_SList modified;         /**< Data stored here when need update */
    size_t range[2];            /**< Range of modified bytes
                                 *   (min/max of \c modified).
                                 *   Used for glMapBufferRange() */
    void *mapptr;               /**< Buffer address saved here on locking */
    SCE_SListIterator it;       /**< Own iterator for modified buffers list */
};

/** @} */

int SCE_CBufferInit (void);
void SCE_CBufferQuit (void);

void SCE_CInitBufferData (SCE_CBufferData*);
SCE_CBufferData* SCE_CCreateBufferData (void);
void SCE_CClearBufferData (SCE_CBufferData*);
void SCE_CDeleteBufferData (SCE_CBufferData*);

void SCE_CInitBuffer (SCE_CBuffer*);
SCE_CBuffer* SCE_CCreateBuffer (void);
void SCE_CClearBuffer (SCE_CBuffer*);
void SCE_CDeleteBuffer (SCE_CBuffer*);

void SCE_CModifiedBufferData (SCE_CBufferData*, size_t*);
void SCE_CUnmodifiedBufferData (SCE_CBufferData*);

void SCE_CAddBufferData (SCE_CBuffer*, SCE_CBufferData*);
SCE_CBufferData* SCE_CAddBufferNewData (SCE_CBuffer*, size_t, void*);
void SCE_CRemoveBufferData (SCE_CBufferData*);

void SCE_CBuildBuffer (SCE_CBuffer*, SCEenum, SCE_CBufferUsage);
extern void (*SCE_CUpdateBuffer) (SCE_CBuffer*);
void SCE_CUpdateModifiedBuffers (void);
void SCE_CUseBuffer (SCE_CBuffer*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
