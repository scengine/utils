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
   updated: 31/07/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCEMesh.h>

void SCE_Mesh_InitArray (SCE_SMeshArray *marray)
{
    marray->vb = NULL;
    SCE_Geometry_InitArrayUser (&marray->auser);
    SCE_List_InitIt (&marray->it);
    SCE_List_SetData (&marray->it, marray);
}
SCE_SMeshArray* SCE_Mesh_CreateArray (void)
{
    SCE_SMeshArray *marray = NULL;
    if (!(marray = SCE_malloc (sizeof *marray)))
        SCEE_LogSrc ();
    else
        SCE_Mesh_InitArray (marray);
    return marray;
}
void SCE_Mesh_DeleteArray (SCE_SMeshArray *marray)
{
    if (marray) {
        SCE_CDeleteVertexBuffer (marray->vb);
        SCE_Geometry_ClearArrayUser (&marray->auser);
        SCE_free (marray);
    }
}

void SCE_Mesh_Init (SCE_SMesh *mesh)
{
    mesh->geom = NULL;
    SCE_List_Init (&mesh->arrays);
    mesh->ib = NULL;
}
SCE_SMesh* SCE_Mesh_Create (void)
{
    SCE_SMesh *mesh = NULL;
    if (!(mesh = SCE_malloc (sizeof *mesh)))
        SCEE_LogSrc ();
    else
        SCE_Mesh_Init (mesh);
    return mesh;
}
SCE_SMesh* SCE_Mesh_CreateFrom (SCE_SGeometry *geom)
{
    SCE_SMesh *mesh = NULL;
    if (!(mesh = SCE_Mesh_Create ()))
        SCEE_LogSrc ();
    else if (SCE_Mesh_MakeFrom (mesh, geom) < 0) {
        SCE_Mesh_Delete (mesh), mesh = NULL;
        SCEE_LogSrc ();
    }
    return mesh;
}
void SCE_Mesh_Delete (SCE_SMesh *mesh)
{
    if (mesh) {
        SCE_CDeleteIndexBuffer (mesh->ib);
        SCE_List_Clear (&mesh->arrays);
        SCE_free (mesh);
    }
}

int SCE_Mesh_MakeFrom (SCE_SMesh *mesh, SCE_SGeometry *geom)
{
}

int SCE_Mesh_Build (SCE_SMesh *mesh, SCE_EMeshBuildMode bmode,
                    SCE_EMeshRenderMode rmode)
{

}

void SCE_Mesh_SetRenderMode (SCE_SMesh*, SCE_EMeshRenderMode);

void SCE_Mesh_Bind (SCE_SMesh*);
void SCE_Mesh_Render (void);
void SCE_Mesh_Unbind (void);
