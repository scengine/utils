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
   updated: 02/08/2009 */

#ifndef SCEMESH_H
#define SCEMESH_H

#include <SCE/core/SCECVertexBuffer.h>
#include <SCE/interface/SCEGeometry.h>

#ifdef __cplusplus
extern "C" {
#endif

enum sce_emeshbuildmode {
    SCE_INDEPENDANT_VERTEX_BUFFER = 1,
    SCE_GLOBAL_VERTEX_BUFFER,
    SCE_GLOBAL_TBN_VERTEX_BUFFER
};
typedef enum sce_emeshbuildmode SCE_EMeshBuildMode;

typedef struct sce_smesharray SCE_SMeshArray;
struct sce_smesharray {
    SCE_CVertexBufferData data;
    SCE_SGeometryArrayUser auser;
    SCE_SListIterator it;
};

typedef struct sce_smeshbuffer SCE_SMeshBuffer;
struct sce_smeshbuffer {
    SCE_CVertexBuffer vb;
    SCE_SListIterator it;
};

typedef struct sce_smesh SCE_SMesh;
/**
 * \brief Structure of a renderable mesh
 * \sa SCE_SGeometry, SCE_CVertexBuffer
 */
struct sce_smesh {
    SCE_SGeometry *geom;
    int canfree_geom;
    SCEenum prim;               /**< Primitive type (SCE_TRIANGLES
                                 * recommanded) */
    SCE_SList arrays;           /**< SCE_SMeshArray */
    SCE_SList buffers;          /**< SCE_SMeshBuffer */
    SCE_CIndexBuffer ib;
    int use_ib;
    SCE_CBufferRenderMode rmode; /**< Render mode */
    SCE_EMeshBuildMode bmode;   /**< Build mode */
};

int SCE_Init_Mesh (void);
void SCE_Quit_Mesh (void);

void SCE_Mesh_InitArray (SCE_SMeshArray*);
SCE_SMeshArray* SCE_Mesh_CreateArray (void);
void SCE_Mesh_ClearArray (SCE_SMeshArray*);
void SCE_Mesh_DeleteArray (SCE_SMeshArray*);

void SCE_Mesh_InitBuffer (SCE_SMeshBuffer*);
/*SCE_SMeshBuffer* SCE_Mesh_CreateBuffer (void);*/
void SCE_Mesh_ClearBuffer (SCE_SMeshBuffer*);
/*void SCE_Mesh_DeleteBuffer (SCE_SMeshBuffer*);*/

void SCE_Mesh_Init (SCE_SMesh*);
SCE_SMesh* SCE_Mesh_Create (void);
SCE_SMesh* SCE_Mesh_CreateFrom (SCE_SGeometry*);
void SCE_Mesh_Delete (SCE_SMesh*);

int SCE_Mesh_SetGeometry (SCE_SMesh*, SCE_SGeometry*, int);
int SCE_Mesh_Build (SCE_SMesh*, SCE_EMeshBuildMode, SCE_CBufferRenderMode);
int SCE_Mesh_AutoBuild (SCE_SMesh*, int);
void SCE_Mesh_SetRenderMode (SCE_SMesh*, SCE_CBufferRenderMode);

int SCE_Mesh_Update (SCE_SMesh*);

SCE_SMesh* SCE_Mesh_Load (const char*, int);

void SCE_Mesh_Bind (SCE_SMesh*);
void SCE_Mesh_Render (void);
void SCE_Mesh_RenderInstanced (SCEuint);
void SCE_Mesh_Unbind (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
