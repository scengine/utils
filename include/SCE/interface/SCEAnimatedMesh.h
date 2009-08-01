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

/* created: 06/04/2009
   updated: 20/06/2009 */

#ifndef SCEANIMATEDMESH_H
#define SCEANIMATEDMESH_H

#include <SCE/interface/SCESkeleton.h>
#include <SCE/interface/SCEMesh.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_MAX_ANIMATED_VERTEX_ATTRIBUTES 4

/* vertex buffers organization modes */
#define SCE_INDEPENDANT_VERTEX_BUFFER 1
#define SCE_GLOBAL_VERTEX_BUFFER 2
#define SCE_GLOBAL_TBN_VERTEX_BUFFER 3

typedef struct sce_svertex SCE_SVertex;
struct sce_svertex
{
    unsigned int weight_id;     /**< First weight ID */
    unsigned int weight_count;  /**< Num attached weights */
};

typedef struct sce_svertexweight SCE_SVertexWeight;
struct sce_svertexweight
{
    float weight;               /**< Weight */
    unsigned int joint_id;      /**< Joint ID */
    unsigned int next_vertex_id;/**< ID of the first weight of the next vertex*/
};

typedef struct sce_sanimatedmesh SCE_SAnimatedMesh;

typedef void (*SCE_FApplySkeletonFunc)(SCE_SAnimatedMesh*, SCE_SSkeleton*);

struct sce_sanimatedmesh
{
    SCE_SMesh *mesh;
    SCE_SSkeleton *baseskel;    /* bind pose skeleton */
    SCE_SSkeleton *animskel;    /* skeleton to apply for animations */
    int canfree_baseskel, canfree_animskel;

    SCE_SVertex *vertices;
    SCE_SVertexWeight *weights;
    SCEindices *indices;
    unsigned int n_vertices;
    unsigned int n_weights;
    unsigned int n_indices;

    SCEvertices *base[SCE_MAX_ANIMATED_VERTEX_ATTRIBUTES];
    int local[SCE_MAX_ANIMATED_VERTEX_ATTRIBUTES];
    SCEvertices *output[SCE_MAX_ANIMATED_VERTEX_ATTRIBUTES];
    int size[SCE_MAX_ANIMATED_VERTEX_ATTRIBUTES];

    SCE_FApplySkeletonFunc applyskel;
};

int SCE_Init_AnimMesh (void);
void SCE_Quit_AnimMesh (void);

int SCE_AnimMesh_GetResourceType (void);

SCE_SAnimatedMesh* SCE_AnimMesh_Create (void);
void SCE_AnimMesh_Delete (SCE_SAnimatedMesh*);

void SCE_AnimMesh_SetBaseSkeleton (SCE_SAnimatedMesh*, SCE_SSkeleton*, int);
SCE_SSkeleton* SCE_AnimMesh_GetBaseSkeleton (SCE_SAnimatedMesh*);
void SCE_AnimMesh_SetAnimSkeleton (SCE_SAnimatedMesh*, SCE_SSkeleton*, int);
SCE_SSkeleton* SCE_AnimMesh_GetAnimSkeleton (SCE_SAnimatedMesh*);
int SCE_AnimMesh_AllocateAnimSkeleton (SCE_SAnimatedMesh*);

void SCE_AnimMesh_SetVertices (SCE_SAnimatedMesh*, SCE_SVertex*, unsigned int);
void SCE_AnimMesh_SetWeights (SCE_SAnimatedMesh*, SCE_SVertexWeight*,
                              unsigned int);

int SCE_AnimMesh_AllocateVertices (SCE_SAnimatedMesh*, unsigned int);
int SCE_AnimMesh_AllocateWeights (SCE_SAnimatedMesh*, unsigned int);

void SCE_AnimMesh_SetIndices (SCE_SAnimatedMesh*, SCEindices*, unsigned int);

SCE_SVertex* SCE_AnimMesh_GetVertices (SCE_SAnimatedMesh*);
SCE_SVertexWeight* SCE_AnimMesh_GetWeights (SCE_SAnimatedMesh*);

void SCE_AnimMesh_SetBaseVertices (SCE_SAnimatedMesh*, int, SCEvertices*, int);
int SCE_AnimMesh_SetBaseVerticesDup (SCE_SAnimatedMesh*, int, SCEvertices*,
                                     int, int, float);
void SCE_AnimMesh_SetOutputVertices (SCE_SAnimatedMesh*, int, SCEvertices*,int);

int SCE_AnimMesh_AllocateBaseVertices (SCE_SAnimatedMesh*, int, int);

SCEvertices* SCE_AnimMesh_GetBaseVertices (SCE_SAnimatedMesh*, int);
SCEvertices* SCE_AnimMesh_GetOutputVertices (SCE_SAnimatedMesh*, int, int*);

void SCE_AnimMesh_ApplySkeleton (SCE_SAnimatedMesh*, SCE_SSkeleton*);
void SCE_AnimMesh_ApplyBaseSkeleton (SCE_SAnimatedMesh*);
void SCE_AnimMesh_ApplyAnimSkeleton (SCE_SAnimatedMesh*);

/*void SCE_AnimMesh_SetLocal (SCE_SAnimatedMesh*, SCE_SSkeleton*);*/
int SCE_AnimMesh_SetGlobal (SCE_SAnimatedMesh*);

int SCE_AnimMesh_BuildMesh (SCE_SAnimatedMesh*, int, int*);
SCE_SMesh* SCE_AnimMesh_GetMesh (SCE_SAnimatedMesh*);
int SCE_AnimMesh_UpdateMesh (SCE_SAnimatedMesh*);

SCE_SAnimatedMesh* SCE_AnimMesh_Load (const char*, const char*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
