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
 
/* created: 19/01/2007
   updated: 07/07/2009 */

#ifndef SCEMESH_H
#define SCEMESH_H

#include <SCE/utils/SCEVector.h>
#include <SCE/utils/SCEList.h>
#include <SCE/core/SCECBuffers.h>
#include <SCE/interface/SCEBoundingBox.h>
#include <SCE/interface/SCEBoundingSphere.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup mesh
 * @{
 */

#define SCE_TANGENT SCE_TEXCOORD1
#define SCE_BINORMAL SCE_TEXCOORD2

#define SCE_GEN_TANGENTS (0x00000001)
#define SCE_GEN_BINORMALS (0x00000002)
#define SCE_GEN_NORMALS (0x00000004)

#define SCE_MESH_SORT_NEAR_TO_FAR 1
#define SCE_MESH_SORT_FAR_TO_NEAR 2


/* vertices type */
typedef GLfloat SCEvertices;
#define SCE_VERTICES_TYPE SCE_FLOAT

/* indices type */
typedef GLushort SCEindices;
#define SCE_INDICES_TYPE SCE_UNSIGNED_SHORT

/** \copydoc sce_smeshvertexdata */
typedef struct sce_smeshvertexdata SCE_SMeshVertexData;
/**
 * \brief Structure that stores vertices' data
 * \sa SCE_CVertexDeclaration
 */
struct sce_smeshvertexdata
{
    SCE_CVertexDeclaration *dec; /**< Vertex declaration */
    void *data;                  /**< Data */
    int canfree;                 /**< Do we have right to delete \c data? */
    size_t size;                 /**< Size of \c data in bytes */
};

/** \copydoc sce_smeshvertexbuffer */
typedef struct sce_smeshvertexbuffer SCE_SMeshVertexBuffer;
/**
 * \brief Vertex buffer that stores many informations of the data's arrengment
 * \sa SCE_SMeshVertexData SCE_CVertexBuffer
 */
struct sce_smeshvertexbuffer
{
    SCEenum usage;              /**< Buffer's usage (GL_*_DRAW) */
    SCE_CVertexBuffer *buffer;  /**< The vertex buffer */
    SCE_SList *data;            /**< List of the \c buffer data, contains
                                 * SCE_SMeshVertexData structures */
    int active;                 /**< Is buffer activated? */
    int built;                  /**< Is buffer built? */
};

/** \copydoc sce_smeshindexbuffer */
typedef struct sce_smeshindexbuffer SCE_SMeshIndexBuffer;
/**
 * \brief Index buffer that stores many informations of the data's arrengment
 * \sa SCE_CIndexBuffer
 */
struct sce_smeshindexbuffer
{
    SCEenum usage;            /**< Buffer's usage (GL_*_DRAW) */
    SCE_CIndexBuffer *buffer; /**< The index buffer */
    SCEenum type;             /**< Data type (GL_UNSIGNED_INT, ...) */
    void *data;               /**< Data */
    int canfree;              /**< Do we have right to delete \c data? */
    size_t size;              /**< Size of \c data in bytes */
    int built;                /**< Is buffer built? */
};


/** \copydoc sce_smesh */
typedef struct sce_smesh SCE_SMesh;
/**
 * \brief A mesh structure that contains all geometry informations about a mesh
 * \sa SCE_SMeshVertexBuffer SCE_SMeshIndexBuffer
 */
struct sce_smesh
{
    SCE_SList *vertices;     /**< List that contains all the vertex buffers of
                              * the mesh (elements are SCE_SMeshVertexBuffer) */
    SCE_SMeshIndexBuffer ib; /**< Index buffer */
    int use_indices;         /**< Do we use indices for rendering? */

    unsigned int vcount;     /**< Number of vertices */
    unsigned int icount;     /**< Number of indices */

    SCEenum polygon_type;    /**< Polygons type (triangle, quad, ...) */

    int built;               /**< Is mesh built? */

    SCEvertices *position;   /**< Pointer to the vertices positions */
    SCEvertices *normal;     /**< Pointer to the vertices normals */
    SCEvertices *tangent;    /**< Pointer to the vertices normals */
    SCEvertices *binormal;   /**< Pointer to the vertices normals */

    SCE_SList *sortedfaces;  /**< Sorted list that stores all the faces from
                              * its indices */
};

/** \copydoc sce_smeshface */
typedef struct sce_smeshface SCE_SMeshFace;
/**
 * \brief A structure of a face
 */
struct sce_smeshface
{
    SCEindices index[4]; /**< Indices of the face, to get the n-th vertex's
                          * position of the face: pos[index[n]] */
    SCEvertices *pos,    /**< Pointer to the vertices' positions */
                *nor;    /**< Pointer to the vertices' normals */
    int num_vertices;    /**< Number of vertices of the mesh */
};

/**
 * \brief Function which is called for each face of a mesh
 */
typedef int (*SCE_FMeshFaceFunc)(SCE_SMesh*, SCE_SMeshFace*, int, void*);

/** @} */

int SCE_Init_Mesh (void);
void SCE_Quit_Mesh (void);

int SCE_Mesh_GetResourceType (void);

void SCE_Mesh_InitVB (SCE_SMeshVertexBuffer*);
void SCE_Mesh_InitIB (SCE_SMeshIndexBuffer*);

SCE_SMeshVertexBuffer* SCE_Mesh_CreateVB (void);
void SCE_Mesh_DeleteVB (void*);

int SCE_Mesh_BuildVB (SCE_SMeshVertexBuffer*);
int SCE_Mesh_BuildIB (SCE_SMeshIndexBuffer*);

SCE_SMesh* SCE_Mesh_Create (void);
void SCE_Mesh_Delete (SCE_SMesh*);
void SCE_Mesh_DeleteList (SCE_SMesh**);

int SCE_Mesh_AddVB (SCE_SMesh*, SCE_SMeshVertexBuffer*);
int SCE_Mesh_AddNewVB (SCE_SMesh*, int);
SCE_SMeshVertexBuffer* SCE_Mesh_RemoveVB (SCE_SMesh*, unsigned int);
void SCE_Mesh_EraseVB (SCE_SMesh*, unsigned int);

void SCE_Mesh_ActivateVB (SCE_SMesh*, unsigned int, int);
void SCE_Mesh_ActivateIB (SCE_SMesh*, int);

SCE_SMeshVertexData* SCE_Mesh_LocateData (SCE_SMesh*, int,
                                          SCE_SMeshVertexBuffer**);
SCEvertices* SCE_Mesh_GetVerticesPositions (SCE_SMesh*);
SCEvertices* SCE_Mesh_GetVerticesNormals (SCE_SMesh*);
SCEvertices* SCE_Mesh_GetVerticesTangents (SCE_SMesh*);
SCEvertices* SCE_Mesh_GetVerticesBinormals (SCE_SMesh*);
SCEindices* SCE_Mesh_GetIndices (SCE_SMesh*);

int SCE_Mesh_AddVertices (SCE_SMesh*, unsigned int, int, SCEenum,
                          unsigned int, unsigned int, void*, int);
int SCE_Mesh_AddVerticesDup (SCE_SMesh*, unsigned int, int, SCEenum,
                             unsigned int, unsigned int, const void*);

int SCE_Mesh_SetIndices (SCE_SMesh*, SCEenum,SCEenum, unsigned int, void*, int);
int SCE_Mesh_SetIndicesDup (SCE_SMesh*, SCEenum, SCEenum, unsigned int,
                            const void*);

int SCE_Mesh_GetNumVertices (SCE_SMesh*);
int SCE_Mesh_GetNumIndices (SCE_SMesh*);
int SCE_Mesh_GetNumVerticesPerFace (SCE_SMesh*);
int SCE_Mesh_GetNumFaces (SCE_SMesh*);

int SCE_Mesh_ForEachFace (SCE_SMesh*, SCE_FMeshFaceFunc, void*);

int SCE_Mesh_SortFaces (SCE_SMesh*, int, SCE_TVector3);

void SCE_Mesh_ComputeTriangleTBN (SCEvertices*, SCEvertices*, SCEindices*,
                                  SCEvertices*, SCEvertices*, SCEvertices*);

int SCE_Mesh_ComputeTBN (SCEenum, SCEvertices*, SCEvertices*, SCEindices*,
                         unsigned int, unsigned int, SCEvertices*, SCEvertices*,
                         SCEvertices*);

int SCE_Mesh_GenerateTBN (SCE_SMesh*, SCEvertices*, SCEvertices*, SCEvertices*,
                          unsigned int);

int SCE_Mesh_AddGenVertices (SCE_SMesh*, SCEuint, SCEenum, SCEuint);

void SCE_Mesh_ComputeBoundingBox (SCEvertices*, unsigned int,
                                  SCE_TVector3, float*, float*, float*);
void SCE_Mesh_ComputeBoundingBoxv (SCEvertices*, unsigned int,
                                   SCE_TVector3, SCE_TVector3);
int SCE_Mesh_GenerateBoundingBox (SCE_SMesh*, SCE_SBoundingBox*);

void SCE_Mesh_ComputeBoundingSphere (SCEvertices*, unsigned int,
                                     SCE_TVector3, float*);
int SCE_Mesh_GenerateBoundingSphere (SCE_SMesh*, SCE_SBoundingSphere*);

void SCE_Mesh_GenerateCubeVertices (SCEvertices[72], SCE_TVector3,
                                    float, float, float);
SCE_SMesh* SCE_Mesh_CreateCube (SCE_TVector3, float, float, float);
SCE_SMesh* SCE_Mesh_CreateCubev (SCE_TVector3, SCE_TVector3);

const SCEindices* SCE_Mesh_GetIndexedCubeIndices (void);
void SCE_Mesh_GenerateIndexedCubeVertices (SCEvertices[24], SCE_TVector3,
                                           float, float, float);
SCE_SMesh* SCE_Mesh_CreateIndexedCube (SCE_TVector3, float, float, float);
SCE_SMesh* SCE_Mesh_CreateIndexedCubev (SCE_TVector3, SCE_TVector3);
#if 0
SCE_SMesh* SCE_Mesh_CreateFromBoundingBox (SCE_SBoundingBox*);
#endif

int SCE_Mesh_Build (SCE_SMesh*);

void SCE_Mesh_SetRenderMode (SCE_SMesh*, SCEenum);

int SCE_Mesh_Update (SCE_SMesh*, unsigned int);
int SCE_Mesh_LogUpdate (SCE_SMesh*, unsigned int);
void SCE_Mesh_ApplyUpdates (void);

SCE_SMesh** SCE_Mesh_Load (const char*, int);

void SCE_Mesh_Use (SCE_SMesh*);
void SCE_Mesh_Draw (void);
void SCE_Mesh_Render (SCE_SMesh*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
