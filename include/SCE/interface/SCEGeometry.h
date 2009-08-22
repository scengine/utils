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

/* created: 25/07/2009
   updated: 22/08/2009 */

#ifndef SCEGEOMETRY_H
#define SCEGEOMETRY_H

#include <SCE/utils/SCEVector.h>
#include <SCE/core/SCECVertexArray.h>
#include <SCE/interface/SCEBox.h>
#include <SCE/interface/SCESphere.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup geometry
 * @{
 */

#define SCE_TANGENT SCE_TEXCOORD1
#define SCE_BINORMAL SCE_TEXCOORD2

#define SCE_GEN_TANGENTS (0x00000001)
#define SCE_GEN_BINORMALS (0x00000002)
#define SCE_GEN_NORMALS (0x00000004)

enum sce_esortorder {
    SCE_SORT_NEAR_TO_FAR,
    SCE_SORT_FAR_TO_NEAR
};
typedef enum sce_esortorder SCE_ESortOrder;

/**
 * \brief Default (and HIGHLY recommanded for compatibility reasons) vertices
 * data type
 */
typedef GLfloat SCEvertices;
/**
 * \brief Default (and HIGHLY recommanded for compatibility reasons) vertices
 * data type
 */
#define SCE_VERTICES_TYPE SCE_FLOAT

/**
 * \brief Default (and HIGHLY recommanded for compatibility reasons) indices
 * data type
 */
typedef GLushort SCEindices;
/**
 * \brief Default (and HIGHLY recommanded for compatibility reasons) indices
 * data type
 */
#define SCE_INDICES_TYPE SCE_UNSIGNED_SHORT

/** \copydoc sce_sgeometry */
typedef struct sce_sgeometry SCE_SGeometry;
/** \copydoc sce_sgeometryarray */
typedef struct sce_sgeometryarray SCE_SGeometryArray;
/**
 * \brief A geometry array
 */
struct sce_sgeometryarray {
    SCE_CVertexArrayData data;/**< Associated data */
    SCE_SGeometryArray *root, *child; /**< Interleaved */
    int canfree_data;         /**< Can this structure free \c array.data.data?*/
    SCE_SListIterator it;     /**< Own iterator */
    SCE_SList users;          /**< SCE_SGeometryArrayUser */
    size_t range[2];          /**< Modified vertices range */
    size_t *rangeptr;         /**< Pointer to the range to use (can be NULL) */
    SCE_SGeometry *geom;
};

/** \copydoc sce_sgeometryarrayuser */
typedef struct sce_sgeometryarrayuser SCE_SGeometryArrayUser;
/**
 * \brief Prototype of the called callbacks when an array is updated
 * (happens when its geometry is updated)
 * \sa SCE_SGeometryArrayUser, SCE_Geometry_Update()
 */
typedef void (*SCE_FUpdateGeometryArray)(void*, size_t*);
/**
 * \brief User of a geometry
 * \sa SCE_SGeometryArray SCE_FUpdatedGeometryArray
 */
struct sce_sgeometryarrayuser {
    SCE_SGeometryArray *array;       /**< Attached array */
    SCE_FUpdateGeometryArray update; /**< Called on each modified array */
    void *arg;                       /**< Argument of \c update */
    SCE_SListIterator it;
};


typedef struct sce_sgeometryprimitivesort SCE_SGeometryPrimitiveSort;
/**
 * \brief Used to define a primitive in a sort algorithm case
 */
struct sce_sgeometryprimitivesort {
    float dist;
    SCEindices index;
};
/**
 * \brief Contains geometry of a mesh
 * \sa SCE_SGeometryArray, SCE_SGeometryArrayUser, SCE_SMesh
 */
struct sce_sgeometry {
    SCE_CPrimitiveType prim;
    SCE_SList arrays;                 /**< All vertex arrays */
    SCE_SList modified;               /**< Modified vertex arrays */
    SCE_SGeometryArray *index_array;  /**< Index array */
    int canfree_index;
    size_t n_vertices, n_indices;

    SCE_SGeometryArray *pos_array, *nor_array, *tex_array;
    SCEvertices *pos_data, *nor_data, *tex_data;
    SCEindices *index_data;

    SCE_SGeometryPrimitiveSort *sorted;
    size_t sorted_length;

    SCE_SBox box;
    SCE_SSphere sphere;
    int box_uptodate, sphere_uptodate; /* Bounding volumes state */
};

/** @} */

int SCE_Init_Geometry (void);
void SCE_Quit_Geometry (void);

int SCE_Geometry_GetResourceType (void);

void SCE_Geometry_InitArray (SCE_SGeometryArray*);
SCE_SGeometryArray* SCE_Geometry_CreateArray (void);
SCE_SGeometryArray* SCE_Geometry_CreateArrayFrom (SCE_CVertexAttributeType,
                                                  SCE_CType, size_t, int,
                                                  void*, int);
void SCE_Geometry_DeleteArray (SCE_SGeometryArray*);
void SCE_Geometry_CopyArray (SCE_SGeometryArray*, const SCE_SGeometryArray*);

void SCE_Geometry_InitArrayUser (SCE_SGeometryArrayUser*);
SCE_SGeometryArrayUser* SCE_Geometry_CreateArrayUser (void);
void SCE_Geometry_ClearArrayUser (SCE_SGeometryArrayUser*);
void SCE_Geometry_DeleteArrayUser (SCE_SGeometryArrayUser*);

SCE_SGeometry* SCE_Geometry_Create (void);
void SCE_Geometry_Delete (SCE_SGeometry*);

SCE_SGeometryArray* SCE_Geometry_GetUserArray (SCE_SGeometryArrayUser*);
void SCE_Geometry_AttachArray (SCE_SGeometryArray*, SCE_SGeometryArray*);
SCE_SGeometryArray* SCE_Geometry_GetRoot (SCE_SGeometryArray*);
SCE_SGeometryArray* SCE_Geometry_GetChild (SCE_SGeometryArray*);

void SCE_Geometry_AddUser (SCE_SGeometryArray*, SCE_SGeometryArrayUser*,
                           SCE_FUpdateGeometryArray, void*);
void SCE_Geometry_RemoveUser (SCE_SGeometryArrayUser*);

void SCE_Geometry_Modified (SCE_SGeometryArray*, const size_t*);
void SCE_Geometry_UpdateArray (SCE_SGeometryArray*);
void SCE_Geometry_Update (SCE_SGeometry*);

void SCE_Geometry_SetArrayData (SCE_SGeometryArray*, SCE_CVertexAttributeType,
                                SCE_CType, size_t, int, void*, int);
void SCE_Geometry_SetArrayPosition (SCE_SGeometryArray*, size_t, int,
                                    SCEvertices*, int);
void SCE_Geometry_SetArrayTexCoord (SCE_SGeometryArray*, unsigned int, size_t,
                                    int, SCEvertices*, int);
void SCE_Geometry_SetArrayNormal (SCE_SGeometryArray*, size_t, SCEvertices*,
                                  int);
void SCE_Geometry_SetArrayTangent (SCE_SGeometryArray*, size_t, SCEvertices*,
                                   int);
void SCE_Geometry_SetArrayBinormal (SCE_SGeometryArray*, size_t, SCEvertices*,
                                    int);
void SCE_Geometry_SetArrayIndices (SCE_SGeometryArray*, SCE_CType, void*, int);

void* SCE_Geometry_GetData (SCE_SGeometryArray*);
SCE_CVertexAttributeType
SCE_Geometry_GetArrayAttributeType (SCE_SGeometryArray*);
SCE_CVertexArrayData* SCE_Geometry_GetArrayData (SCE_SGeometryArray*);

void SCE_Geometry_AddArray (SCE_SGeometry*, SCE_SGeometryArray*);
void SCE_Geometry_AddArrayRec (SCE_SGeometry*, SCE_SGeometryArray*);
SCE_SGeometryArray*
SCE_Geometry_AddNewArray (SCE_SGeometry*, SCE_CVertexAttributeType,
                          SCE_CType, size_t, int, void*, int);
SCE_SGeometryArray* SCE_Geometry_AddArrayDup (SCE_SGeometry*,
                                              SCE_SGeometryArray*, int);
SCE_SGeometryArray* SCE_Geometry_AddArrayDupDup (SCE_SGeometry*,
                                                 SCE_SGeometryArray*, int);
void SCE_Geometry_RemoveArray (SCE_SGeometryArray*);

void SCE_Geometry_SetIndexArray (SCE_SGeometry*, SCE_SGeometryArray*, int);
SCE_SGeometryArray* SCE_Geometry_SetIndexArrayDup (SCE_SGeometry*,
                                                   SCE_SGeometryArray*, int);
SCE_SGeometryArray* SCE_Geometry_SetIndexArrayDupDup (SCE_SGeometry*,
                                                      SCE_SGeometryArray*, int);
SCE_SGeometryArray* SCE_Geometry_GetIndexArray (SCE_SGeometry*);

int SCE_Geometry_SetData (SCE_SGeometry*, SCEvertices*, SCEvertices*,
                          SCEvertices*, SCEindices*, SCEuint, SCEuint);
int SCE_Geometry_SetDataDup (SCE_SGeometry*, SCEvertices*, SCEvertices*,
                             SCEvertices*, SCEindices*, SCEuint, SCEuint);

SCE_SGeometryArray* SCE_Geometry_GetPositionsArray (SCE_SGeometry*);
SCE_SGeometryArray* SCE_Geometry_GetNormalsArray (SCE_SGeometry*);
SCE_SGeometryArray* SCE_Geometry_GetTexCoordsArray (SCE_SGeometry*);
SCEvertices* SCE_Geometry_GetPositions (SCE_SGeometry*);
SCEvertices* SCE_Geometry_GetNormals (SCE_SGeometry*);
SCEvertices* SCE_Geometry_GetTexCoords (SCE_SGeometry*);
SCEindices* SCE_Geometry_GetIndices (SCE_SGeometry*);

void SCE_Geometry_SetPrimitiveType (SCE_SGeometry*, SCE_CPrimitiveType);
SCEenum SCE_Geometry_GetPrimitiveType (SCE_SGeometry*);

void SCE_Geometry_SetNumVertices (SCE_SGeometry*, size_t);
void SCE_Geometry_SetNumIndices (SCE_SGeometry*, size_t);

size_t SCE_Geometry_GetNumVertices (SCE_SGeometry*);
size_t SCE_Geometry_GetNumIndices (SCE_SGeometry*);
size_t SCE_Geometry_GetNumVerticesPerPrimitive (SCE_SGeometry*);
size_t SCE_Geometry_GetNumPrimitives (SCE_SGeometry*);

SCE_SList* SCE_Geometry_GetArrays (SCE_SGeometry*);
SCE_SList* SCE_Geometry_GetModifiedArrays (SCE_SGeometry*);

int SCE_Geometry_IsModified (SCE_SGeometry*);

SCE_SGeometry* SCE_Geometry_Load (const char*, int);

void SCE_Geometry_ComputeBoundingBox (SCEvertices*, size_t, SCE_SBox*);
void SCE_Geometry_ComputeBoundingSphere (SCEvertices*, size_t, SCE_SBox*,
                                         SCE_SSphere*);
void SCE_Geometry_GenerateBoundingBox (SCE_SGeometry*);
void SCE_Geometry_GenerateBoundingSphere (SCE_SGeometry*);
void SCE_Geometry_GenerateBoundingVolumes (SCE_SGeometry*);

SCE_SBox* SCE_Geometry_GetBox (SCE_SGeometry*);
SCE_SSphere* SCE_Geometry_GetSphere (SCE_SGeometry*);
void SCE_Geometry_BoxUpToDate (SCE_SGeometry*);
void SCE_Geometry_SphereUpToDate (SCE_SGeometry*);

/* bonus functions */
int SCE_Geometry_SortPrimitives (SCE_SGeometry*, SCE_ESortOrder, SCE_TVector3);

void SCE_Mesh_ComputeTriangleTBN (SCEvertices*, SCEvertices*, size_t*,
                                  SCEvertices*, SCEvertices*, SCEvertices*);
int SCE_Geometry_ComputeTBN (SCE_CPrimitiveType, SCEvertices*, SCEvertices*,
                             SCE_CType, void*, size_t, size_t, SCEvertices*,
                             SCEvertices*, SCEvertices*);
int SCE_Geometry_GenerateTBN (SCE_SGeometry*, SCEvertices**, SCEvertices**,
                              SCEvertices**, unsigned int);
int SCE_Geometry_AddGenerateTBN (SCE_SGeometry*, unsigned int, int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
