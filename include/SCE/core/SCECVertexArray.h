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
 
/* created: 26/07/2009
   updated: 02/08/2009 */

#ifndef SCECVERTEXARRAY_H
#define SCECVERTEXARRAY_H

#include <SCE/utils/SCEList.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup vertexarray
 * @{
 */

/**
 * \brief Vertex attributes
 */
enum sce_cvertexattributetype {
    SCE_POSITION = 1,
    SCE_COLOR,
    SCE_NORMAL,

    SCE_TEXCOORD0,
    SCE_TEXCOORD1,
    SCE_TEXCOORD2,
    SCE_TEXCOORD3,
    SCE_TEXCOORD4,
    SCE_TEXCOORD5,
    SCE_TEXCOORD6,
    SCE_TEXCOORD7,

    SCE_ATTRIB0 = 128,
    SCE_ATTRIB1,
    SCE_ATTRIB2,
    SCE_ATTRIB3,
    SCE_ATTRIB4,
    SCE_ATTRIB5,
    SCE_ATTRIB6,
    SCE_ATTRIB7,
    SCE_ATTRIB8,
    SCE_ATTRIB9,
    SCE_ATTRIB10,
    SCE_ATTRIB11,
    SCE_ATTRIB12,
    SCE_ATTRIB13,
    SCE_ATTRIB14,
    SCE_ATTRIB15
};
/** \copydoc sce_cvertexattributetype */
typedef enum sce_cvertexattributetype SCE_CVertexAttributeType;

/** \copydoc sce_cvertexarraydata */
typedef struct sce_cvertexarraydata SCE_CVertexArrayData;
/**
 * \brief Data of a vertex array
 */
struct sce_cvertexarraydata {
    SCE_CVertexAttributeType attrib; /**< Vertices' attribute */
    SCEenum type;         /**< Data type (SCE_FLOAT, SCE_INT, ...) */
    SCEint size;          /**< Number of dimensions of the vectors */
    void *data;           /**< User is always the owner of the data */
};

typedef void (*SCE_FSetVA)(SCE_CVertexArrayData*);

/** \copydoc sce_cvertexarray */
typedef struct sce_cvertexarray SCE_CVertexArray;
/**
 * \brief A vertex array
 */
struct sce_cvertexarray {
    SCEuint id;                 /**< Vertex array object identifier */
    SCE_FSetVA set, unset;      /**< Functions to set/unset the vertex array */
    SCE_CVertexArrayData data;
    SCE_SListIterator it;       /**< Own iterator */
};

typedef struct sce_cindexarray SCE_CIndexArray;
/**
 * \brief An index array
 */
struct sce_cindexarray {
    SCEenum type;
    void *data;
};

/** @} */

int SCE_CVertexArrayInit (void);
void SCE_CVertexArrayQuit (void);

void SCE_CInitVertexArrayData (SCE_CVertexArrayData*);
SCE_CVertexArrayData* SCE_CCreateVertexArrayData (void);
void SCE_CDeleteVertexArrayData (SCE_CVertexArrayData*);

void SCE_CInitVertexArray (SCE_CVertexArray*);
SCE_CVertexArray* SCE_CCreateVertexArray (void);
void SCE_CClearVertexArray (SCE_CVertexArray*);
void SCE_CDeleteVertexArray (SCE_CVertexArray*);

void SCE_CInitIndexArray (SCE_CIndexArray*);
SCE_CIndexArray* SCE_CCreateIndexArray (void);
void SCE_CDeleteIndexArray (SCE_CIndexArray*);

SCE_CVertexArrayData* SCE_CGetVertexArrayData (SCE_CVertexArray*);
void SCE_CSetVertexArrayData (SCE_CVertexArray*, SCE_CVertexArrayData*);
void SCE_CSetVertexArrayNewData (SCE_CVertexArray*, SCE_CVertexAttributeType,
                                 SCEenum, SCEint, void*);

void SCE_CBeginVertexArraySequence (SCE_CVertexArray*);
void SCE_CCallVertexArraySequence (SCE_CVertexArray*);
void SCE_CEndVertexArraySequence (void);

void SCE_CUseVertexArray (SCE_CVertexArray*);
void SCE_CRender (SCEenum, SCEuint);
void SCE_CRenderInstanced (SCEenum, SCEuint, SCEuint);
void SCE_CRenderIndexed (SCEenum, SCE_CIndexArray*, SCEuint);
void SCE_CRenderIndexedInstanced (SCEenum, SCE_CIndexArray*, SCEuint, SCEuint);
void SCE_CFinishVertexArrayRender (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
