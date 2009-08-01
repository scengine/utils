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
   updated: 24/08/2008 */

#ifndef SCECBUFFERS_H
#define SCECBUFFERS_H

#include <SCE/utils/SCEList.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* constantes de definition des types de sommets. (attrib)
   la constante SCE_POSITION peut valoire n'importe quelle valeur entiere
   positive, les autre doivent imperativement valoires PREC+1
   ou PREC represente la constante precedemment definie. */
#define SCE_POSITION 1
#define SCE_COLOR 2
#define SCE_NORMAL 3

/* coordonnees de texture */
#define SCE_TEXCOORD0 4
#define SCE_TEXCOORD1 5
#define SCE_TEXCOORD2 6
#define SCE_TEXCOORD3 7
#define SCE_TEXCOORD4 8
#define SCE_TEXCOORD5 9
#define SCE_TEXCOORD6 10
#define SCE_TEXCOORD7 11
/* s'etend jusqu'au maximum supporte par ma machine, cf CBuffers.c */

/* attributs de sommet */
#define SCE_ATTRIB0 100
#define SCE_ATTRIB1 101
#define SCE_ATTRIB2 102
#define SCE_ATTRIB3 103
#define SCE_ATTRIB4 104
#define SCE_ATTRIB5 105
#define SCE_ATTRIB6 106
#define SCE_ATTRIB7 107
#define SCE_ATTRIB8 108
#define SCE_ATTRIB9 109
#define SCE_ATTRIB10 110
#define SCE_ATTRIB11 111
#define SCE_ATTRIB12 112
#define SCE_ATTRIB13 113
#define SCE_ATTRIB14 114
#define SCE_ATTRIB15 115
/* s'etend jusqu'au maximum supporte par ma machine, cf CBuffers.c */

typedef void (*SCE_FSetDeclaration)(void);

/** \copydoc sce_cvertexdeclaration */
typedef struct sce_cvertexdeclaration SCE_CVertexDeclaration;
/**
 * \brief A vertex declaration structure
 * 
 * Declare a vertex's data type.
 */
struct sce_cvertexdeclaration
{
    SCE_FSetDeclaration set; /**< function to send the declaration to OpenGL */
    int activated;           /**< Is vertex declaration activated ? */
    unsigned int attrib;  /**< vertices' type (SCE_POSITION, SCE_NORMAL, ...) */
    SCEenum type;   /**< data type (SCE_FLOAT, SCE_INT, ...) */
    GLint size;     /**< number of dimensions of the vectors */
    GLint first;    /**< position of the first byte of data */
    GLsizei stride; /**< stride between two consecutive vertices (in bytes) */
    int user;       /**< Indicates the owner of the declaration */
};


/** \copydoc sce_cbufferdata */
typedef struct sce_cbufferdata SCE_CBufferData;
/**
 * \brief A buffer data structure
 *
 * Contains a part of a buffer's data.
 */
struct sce_cbufferdata
{
    void *data;   /**< data */
    size_t data_size; /**< data size (in bytes) */
    size_t first; /**< first byte in the buffer to copy \p data into it */
    int user;     /**< boolean wich indicates the owner of the data */
};


/** \copydoc sce_cvertexbuffer */
typedef struct sce_cvertexbuffer SCE_CVertexBuffer;
/**
 * \brief A vertex buffer
 *
 * Contains some data of various vertices' types.
 */
struct sce_cvertexbuffer
{
    GLuint id;       /**< OpenGL id */
    SCE_SList *data; /**< all the buffer's data */
    SCE_SList *decs; /**< all the buffer's vertex declarations */
    size_t size;     /**< total size of stored data, at end,
		       \p size is the buffer's length (in bytes) */
    void *mapptr;    /**< pointer to the mapped buffer's data */
};


/** \copydoc sce_cindexbuffer */
typedef struct sce_cindexbuffer SCE_CIndexBuffer;
/**
 * \brief An index buffer
 */
struct sce_cindexbuffer
{
    GLuint id;       /**< OpenGL id */
    SCE_SList *data; /**< all the buffer's data */
    size_t size;     /**< total size of stored data, at end,
		       \p size is the buffer's length (in bytes) */
    void *mapptr;    /**< pointer to the mapped buffer's data */
};

int SCE_CBufferInit (void);

void SCE_CBindVertexBuffer (SCE_CVertexBuffer*);
void SCE_CBindIndexBuffer (SCE_CIndexBuffer*);

void SCE_CInitVertexDeclaration (SCE_CVertexDeclaration*);
SCE_CVertexDeclaration* SCE_CCreateVertexDeclaration (void);
void SCE_CDeleteVertexDeclaration (void*);

void SCE_CInitVertexBuffer (SCE_CVertexBuffer*);
void SCE_CInitIndexBuffer (SCE_CIndexBuffer*);

void SCE_CInitBufferData (SCE_CBufferData*);
SCE_CBufferData* SCE_CCreateBufferData (void);
void SCE_CDeleteBufferData (void*);

SCE_CVertexBuffer* SCE_CCreateVertexBuffer (void);
SCE_CIndexBuffer* SCE_CCreateIndexBuffer (void);

void SCE_CDeleteVertexBuffer (SCE_CVertexBuffer*);
void SCE_CDeleteVertexBuffer_ (void);
void SCE_CDeleteIndexBuffer (SCE_CIndexBuffer*);
void SCE_CDeleteIndexBuffer_ (void);

void SCE_CClearVertexBuffer (SCE_CVertexBuffer*);
void SCE_CClearVertexBuffer_ (void);
void SCE_CClearIndexBuffer (SCE_CIndexBuffer*);
void SCE_CClearIndexBuffer_ (void);

int SCE_CAddVertexBufferData (SCE_CVertexBuffer*, size_t, void*);
int SCE_CAddVertexBufferData_ (size_t, void*);
int SCE_CAddVertexBufferDataDup (SCE_CVertexBuffer*, size_t, void*);
int SCE_CAddVertexBufferDataDup_ (size_t, void*);

int SCE_CAddIndexBufferData (SCE_CIndexBuffer*, size_t, void*);
int SCE_CAddIndexBufferData_ (size_t, void*);
int SCE_CAddIndexBufferDataDup (SCE_CIndexBuffer*, size_t, void*);
int SCE_CAddIndexBufferDataDup_ (size_t, void*);

size_t SCE_CGetVertexBufferFirst (SCE_CVertexBuffer*);

void SCE_CActiveBufferType (SCEenum, int);
void SCE_CEnableBufferType (SCEenum);
void SCE_CDisableBufferType (SCEenum);

void SCE_CBuildVertexDeclaration (SCE_CVertexDeclaration*);

int SCE_CAddVertexDeclaration (SCE_CVertexBuffer*, SCE_CVertexDeclaration*);
int SCE_CAddVertexDeclaration_ (SCE_CVertexDeclaration*);
int SCE_CAddVertexDeclarationDup (SCE_CVertexBuffer*, SCE_CVertexDeclaration*);
int SCE_CAddVertexDeclarationDup_ (SCE_CVertexDeclaration*);

void SCE_CBuildVertexBuffer (SCE_CVertexBuffer*, SCEenum);
void SCE_CBuildVertexBuffer_ (SCEenum);
void SCE_CBuildIndexBuffer (SCE_CIndexBuffer*, SCEenum);
void SCE_CBuildIndexBuffer_ (SCEenum);


void SCE_CLockVertexBuffer (SCE_CVertexBuffer*, SCEenum);
void SCE_CUnlockVertexBuffer (SCE_CVertexBuffer*);

void SCE_CLockIndexBuffer (SCE_CIndexBuffer*, SCEenum);
void SCE_CUnlockIndexBuffer (SCE_CIndexBuffer*);

void SCE_CUpdateVertexBuffer (SCE_CVertexBuffer*);
void SCE_CUpdateIndexBuffer (SCE_CIndexBuffer*);

void SCE_CUseVertexBuffer (SCE_CVertexBuffer*);
void SCE_CUseIndexBuffer (SCE_CIndexBuffer*);

void SCE_CDrawBuffer (SCEenum, GLint, GLsizei);
void SCE_CDrawIndexedBuffer (SCEenum, SCEenum, size_t, GLsizei);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
