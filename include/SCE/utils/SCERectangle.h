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
 
/* created: 26/02/2008
   updated: 27/02/2008 */

#ifndef SCERECTANGLE_H
#define SCERECTANGLE_H

#include <SCE/utils/SCEVector.h>
#include <SCE/utils/SCELine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* type d'intersection de deux rectangles */
#define SCE_RECT_OUT 0
#define SCE_RECT_IN 1
#define SCE_RECT_INTERSECTS 2

typedef struct sce_sfloatrect SCE_SFloatRect;
struct sce_sfloatrect
{
    SCE_TVector2 p1, p2;
};

typedef struct sce_sintrect SCE_SIntRect;
struct sce_sintrect
{
    int p1[2], p2[2];
};

/* initialise une structure de rectangle */
void SCE_Rectangle_Init (SCE_SIntRect*);
void SCE_Rectangle_Initf (SCE_SFloatRect*);

/* defini les valeurs d'un rectangle */
void SCE_Rectangle_Set (SCE_SIntRect*, int, int, int, int);
void SCE_Rectangle_Setf (SCE_SFloatRect*, float, float, float, float);

void SCE_Rectangle_SetFromOrigin (SCE_SIntRect*, int, int,
                                  unsigned int, unsigned int);
void SCE_Rectangle_SetFromOriginf (SCE_SFloatRect*, float, float, float, float);

/* deplace un rectangle */
void SCE_Rectangle_Move (SCE_SIntRect*, int, int);
void SCE_Rectangle_Movef (SCE_SFloatRect*, float, float);
void SCE_Rectangle_Movev (SCE_SIntRect*, int*);
void SCE_Rectangle_Movefv (SCE_SFloatRect*, SCE_TVector2);

/* retourne l'aire d'un rectangle */
int SCE_Rectangle_GetArea (SCE_SIntRect*);
float SCE_Rectangle_GetAreaf (SCE_SFloatRect*);

/* retourne la longueur d'un rectangle */
int SCE_Rectangle_GetWidth (SCE_SIntRect*);
float SCE_Rectangle_GetWidthf (SCE_SFloatRect*);
/* retourne la hauteur d'un rectangle */
int SCE_Rectangle_GetHeight (SCE_SIntRect*);
float SCE_Rectangle_GetHeightf (SCE_SFloatRect*);

void SCE_Rectangle_GetPoints (SCE_SIntRect*, int*, int*, int*, int*);
void SCE_Rectangle_GetPointsv (SCE_SIntRect*, int*, int*);
void SCE_Rectangle_GetPointsf (SCE_SIntRect*, float*, float*, float*, float*);
void SCE_Rectangle_GetPointsfv (SCE_SIntRect*, float*, float*);

int* SCE_Rectangle_GetBottomLeftPoint (SCE_SIntRect*);
int* SCE_Rectangle_GetTopRightPoint (SCE_SIntRect*);
float* SCE_Rectangle_GetBottomLeftPointf (SCE_SFloatRect*);
float* SCE_Rectangle_GetTopRightPointf (SCE_SFloatRect*);

/* indique si deux rectangles sont identiques */
int SCE_Rectangle_Equal (SCE_SIntRect*, SCE_SIntRect*);
int SCE_Rectangle_Equalf (SCE_SFloatRect*, SCE_SFloatRect*);

/* indique si un point se trouve a l'interieur d'un rectangle */
int SCE_Rectangle_IsIn (SCE_SIntRect*, int, int);
int SCE_Rectangle_IsInf (SCE_SFloatRect*, float, float);
int SCE_Rectangle_IsInv (SCE_SIntRect*, int*);
int SCE_Rectangle_IsInfv (SCE_SFloatRect*, float*);

/* construit les segments de droite qui forment le rectangle
   dans l'ordre : left, right, bottom, top */
void SCE_Rectangle_MakeLines (SCE_SIntRect*, SCE_SLine*, SCE_SLine*,
                              SCE_SLine*, SCE_SLine*);
void SCE_Rectangle_MakeLinesf (SCE_SFloatRect*, SCE_SLine*, SCE_SLine*,
                               SCE_SLine*, SCE_SLine*);
void SCE_Rectangle_MakeLinesv (SCE_SIntRect*, SCE_SLine*);
void SCE_Rectangle_MakeLinesfv (SCE_SFloatRect*, SCE_SLine*);

/* indique si deux rectangles sont en collision */
int SCE_Rectangle_Intersects (SCE_SIntRect*, SCE_SIntRect*);
int SCE_Rectangle_Intersectsf (SCE_SFloatRect*, SCE_SFloatRect*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
