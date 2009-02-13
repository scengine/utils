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
 
/* Cree le : 06/04/2008
   derniere modification : 25/08/2008 */

#ifndef SCEQUAD_H
#define SCEQUAD_H

#include <SCE/utils/SCERectangle.h>
#include <SCE/interface/SCEMesh.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SCE_QUAD_MESHS_NAME "sce_quad_mesh"
#define SCE_QUAD_MESH_NAME "sce_quad_mesh_lod0"

/* initialise le gestionnaire */
int SCE_Init_Quad (void);
/* quitte le gestionnaire */
void SCE_Quit_Quad (void);

/* dessine le quad par defaut */
void SCE_Quad_DrawDefault (void);

/* construit une matrice pour configurer le bon rendu d'un quad */
void SCE_Quad_MakeMatrix (SCE_TMatrix4, float, float, float, float);
/* idem, mais a partir d'un rectangle */
void SCE_Quad_MakeMatrixFromRectangle (SCE_TMatrix4, SCE_SIntRect*);
void SCE_Quad_MakeMatrixFromRectanglef (SCE_TMatrix4, SCE_SFloatRect*);

/* fonctions de rendu */
void SCE_Quad_Draw (float, float, float, float);
void SCE_Quad_DrawFromRectangle (SCE_SIntRect*);
void SCE_Quad_DrawFromRectanglef (SCE_SFloatRect*);

/* retourne un pointeur direct sur le mesh du quad */
SCE_SMesh* SCE_Quad_GetMesh (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
