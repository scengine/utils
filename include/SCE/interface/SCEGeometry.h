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
   updated: 01/08/2009 */

#ifndef SCEGEOMETRY_H
#define SCEGEOMETRY_H

#include <SCE/core/SCECVertexArray.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
enum {
    SCE_GEOMETRY_POSITION_ARRAY,
    SCE_GEOMETRY_NORMAL_ARRAY,
    SCE_GEOMETRY_TANGENT_ARRAY,
    SCE_GEOMETRY_BINORMAL_ARRAY,
    SCE_GEOMETRY_TEXCOORD0_ARRAY,
    SCE_GEOMETRY_TEXCOORD1_ARRAY,
    SCE_GEOMETRY_TEXCOORD2_ARRAY,
    SCE_GEOMETRY_TEXCOORD3_ARRAY,
    SCE_GEOMETRY_TEXCOORD4_ARRAY,
    SCE_GEOMETRY_TEXCOORD5_ARRAY,
    SCE_GEOMETRY_TEXCOORD6_ARRAY,
    SCE_GEOMETRY_TEXCOORD7_ARRAY,
    SCE_GEOMETRY_TEXCOORD8_ARRAY
};
#endif

#define SCE_TANGENT SCE_TEXCOORD1
#define SCE_BINORMAL SCE_TEXCOORD2

#define SCE_GEN_TANGENTS (0x00000001)
#define SCE_GEN_BINORMALS (0x00000002)
#define SCE_GEN_NORMALS (0x00000004)

/* TODO: will be removed soon (sort algorithm implemented in SCEList) */
#define SCE_GEOMETRY_SORT_NEAR_TO_FAR 1
#define SCE_GEOMETRY_SORT_FAR_TO_NEAR 2

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
    SCE_CVertexArrayData data;  /**< Data structure */
    int canfree_data;           /**< Can this structure free \c data.data ? */
    SCE_SListIterator it;
    SCE_SList users;            /**< SCE_SGeometryArrayUser */
    size_t range[2];            /**< Modified vertices range */
    size_t *rangeptr;          /**< Pointer to the range to use (can be NULL) */
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

/**
 * \brief Contains geometry of a mesh
 * \sa SCE_SMesh
 */
struct sce_sgeometry {
    SCE_SList arrays;                 /**< All vertex arrays */
    SCE_SList modified;               /**< Modified vertex arrays */
    SCE_SGeometryArray *index_array;  /**< Index array */
    int canfree_index;
    unsigned int n_vertices, n_indices;
};

int SCE_Init_Geometry (void);
void SCE_Quit_Geometry (void);

int SCE_Geometry_GetResourceType (void);

SCE_SGeometryArray* SCE_Geometry_CreateArray (void);
SCE_SGeometryArray* SCE_Geometry_CreateArrayFrom (SCE_CVertexAttributeType,
                                                  int, int, void*, int);
void SCE_Geometry_DeleteArray (SCE_SGeometryArray*);

void SCE_Geometry_InitArrayUser (SCE_SGeometryArrayUser*);
SCE_SGeometryArrayUser* SCE_Geometry_CreateArrayUser (void);
void SCE_Geometry_ClearArrayUser (SCE_SGeometryArrayUser*);
void SCE_Geometry_DeleteArrayUser (SCE_SGeometryArrayUser*);

SCE_SGeometry* SCE_Geometry_Create (void);
void SCE_Geometry_Delete (SCE_SGeometry*);

SCE_SGeometryArray* SCE_Geometry_GetUserArray (SCE_SGeometryArrayUser*);

void SCE_Geometry_AddUser (SCE_SGeometryArray*, SCE_SGeometryArrayUser*,
                           SCE_FUpdateGeometryArray, void*);
void SCE_Geometry_RemoveUser (SCE_SGeometryArrayUser*);

void SCE_Geometry_Modified (SCE_SGeometryArray*, size_t*);
void SCE_Geometry_UpdateArray (SCE_SGeometryArray*);
void SCE_Geometry_Update (SCE_SGeometry*);

void SCE_Geometry_SetArrayData (SCE_SGeometryArray*, SCE_CVertexAttributeType,
                                int, int, void*, int);
void SCE_Geometry_SetArrayPosition (SCE_SGeometryArray*, int, SCEvertices*,
                                    int);
void SCE_Geometry_SetArrayTexCoord (SCE_SGeometryArray*, unsigned int, int,
                                    SCEvertices*, int);
void SCE_Geometry_SetArrayNormal (SCE_SGeometryArray*, SCEvertices*, int);
void SCE_Geometry_SetArrayTangent (SCE_SGeometryArray*, SCEvertices*, int);
void SCE_Geometry_SetArrayBinormal (SCE_SGeometryArray*, SCEvertices*, int);
void SCE_Geometry_SetArrayIndices (SCE_SGeometryArray*, SCEindices*, int);

void* SCE_Geometry_GetData (SCE_SGeometryArray*);
SCE_CVertexArrayData* SCE_Geometry_GetArrayData (SCE_SGeometryArray*);

void SCE_Geometry_AddArray (SCE_SGeometry*, SCE_SGeometryArray*);
SCE_SGeometryArray* SCE_Geometry_AddArrayDup (SCE_SGeometry*,
                                              SCE_SGeometryArray*, int);
void SCE_Geometry_RemoveArray (SCE_SGeometryArray*);

void SCE_Geometry_SetIndexArray (SCE_SGeometry*, SCE_SGeometryArray*, int);
SCE_SGeometryArray* SCE_Geometry_GetIndexArray (SCE_SGeometry*);

void SCE_Geometry_SetNumVertices (SCE_SGeometry*, unsigned int);
unsigned int SCE_Geometry_GetNumVertices (SCE_SGeometry*);
void SCE_Geometry_SetNumIndices (SCE_SGeometry*, unsigned int);
unsigned int SCE_Geometry_GetNumIndices (SCE_SGeometry*);

SCE_SList* SCE_Geometry_GetArrays (SCE_SGeometry*);
SCE_SList* SCE_Geometry_GetModifiedArrays (SCE_SGeometry*);

int SCE_Geometry_IsModified (SCE_SGeometry*);

SCE_SGeometry* SCE_Geometry_Load (const char*, int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
