/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2012  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
   updated: 24/03/2012 */

#ifndef SCERECTANGLE_H
#define SCERECTANGLE_H

#include <SCE/utils/SCEVector.h>
#include <SCE/utils/SCELine.h>

#ifdef __cplusplus
extern "C" {
#endif

/* type d'intersection de deux rectangles */
#define SCE_RECT_OUT 0
#define SCE_RECT_IN 1
#define SCE_RECT_INTERSECTS 2

typedef struct sce_sfloatrect SCE_SFloatRect;
struct sce_sfloatrect {
    SCE_TVector2 p1, p2;
};

typedef struct sce_sintrect SCE_SIntRect;
struct sce_sintrect {
    int p1[2], p2[2];
};

typedef struct sce_sintrect3 SCE_SIntRect3;
struct sce_sintrect3 {
    int p1[3], p2[3];
};

void SCE_Rectangle_Init (SCE_SIntRect*);
void SCE_Rectangle3_Init (SCE_SIntRect3*);
void SCE_Rectangle_Initf (SCE_SFloatRect*);

void SCE_Rectangle_Set (SCE_SIntRect*, int, int, int, int);
void SCE_Rectangle3_Set (SCE_SIntRect3*, int, int, int, int, int, int);
void SCE_Rectangle3_Setv (SCE_SIntRect3*, const int*, const int*);
void SCE_Rectangle_Setf (SCE_SFloatRect*, float, float, float, float);

void SCE_Rectangle_SetFromOrigin (SCE_SIntRect*, int, int,
                                  unsigned int, unsigned int);
void SCE_Rectangle_SetFromOriginf (SCE_SFloatRect*, float, float, float, float);
void SCE_Rectangle_SetFromCenter (SCE_SIntRect*, int, int,
                                  unsigned int, unsigned int);
void SCE_Rectangle_SetFromCenterf (SCE_SFloatRect*, float, float, float, float);
void SCE_Rectangle_SetFromCenterfv (SCE_SFloatRect*, SCE_TVector2,
                                    float, float);

void SCE_Rectangle_Move (SCE_SIntRect*, int, int);
void SCE_Rectangle3_Move (SCE_SIntRect3*, int, int, int);
void SCE_Rectangle_Movef (SCE_SFloatRect*, float, float);
void SCE_Rectangle_Movev (SCE_SIntRect*, int*);
void SCE_Rectangle_Movefv (SCE_SFloatRect*, SCE_TVector2);
void SCE_Rectangle_Resize (SCE_SIntRect*, int, int);
void SCE_Rectangle_Resizef (SCE_SFloatRect*, float, float);
void SCE_Rectangle_Add (SCE_SIntRect*, int, int);
void SCE_Rectangle_Addf (SCE_SFloatRect*, float, float);

int SCE_Rectangle_GetArea (const SCE_SIntRect*);
float SCE_Rectangle_GetAreaf (const SCE_SFloatRect*);

int SCE_Rectangle_GetWidth (const SCE_SIntRect*);
int SCE_Rectangle_GetHeight (const SCE_SIntRect*);
int SCE_Rectangle3_GetWidth (const SCE_SIntRect3*);
int SCE_Rectangle3_GetHeight (const SCE_SIntRect3*);
int SCE_Rectangle3_GetDepth (const SCE_SIntRect3*);
float SCE_Rectangle_GetWidthf (const SCE_SFloatRect*);
float SCE_Rectangle_GetHeightf (const SCE_SFloatRect*);

void SCE_Rectangle_GetPoints (const SCE_SIntRect*, int*, int*, int*, int*);
void SCE_Rectangle_GetPointsv (const SCE_SIntRect*, int*, int*);
void SCE_Rectangle3_GetPointsv (const SCE_SIntRect3*, int*, int*);
void SCE_Rectangle_GetPointsf (const SCE_SIntRect*, float*, float*,
                               float*, float*);
void SCE_Rectangle_GetPointsfv (const SCE_SIntRect*, float*, float*);

int* SCE_Rectangle_GetBottomLeftPoint (SCE_SIntRect*);
int* SCE_Rectangle_GetTopRightPoint (SCE_SIntRect*);
float* SCE_Rectangle_GetBottomLeftPointf (SCE_SFloatRect*);
float* SCE_Rectangle_GetTopRightPointf (SCE_SFloatRect*);

int SCE_Rectangle_Equal (SCE_SIntRect*, SCE_SIntRect*);
int SCE_Rectangle_Equalf (SCE_SFloatRect*, SCE_SFloatRect*);

int SCE_Rectangle_IsIn (SCE_SIntRect*, int, int);
int SCE_Rectangle_IsInf (SCE_SFloatRect*, float, float);
int SCE_Rectangle_IsInv (SCE_SIntRect*, int*);
int SCE_Rectangle_IsInfv (SCE_SFloatRect*, float*);

void SCE_Rectangle_MakeLines (SCE_SIntRect*, SCE_SLine*, SCE_SLine*,
                              SCE_SLine*, SCE_SLine*);
void SCE_Rectangle_MakeLinesf (SCE_SFloatRect*, SCE_SLine*, SCE_SLine*,
                               SCE_SLine*, SCE_SLine*);
void SCE_Rectangle_MakeLinesv (SCE_SIntRect*, SCE_SLine*);
void SCE_Rectangle_MakeLinesfv (SCE_SFloatRect*, SCE_SLine*);

int SCE_Rectangle_Intersects (SCE_SIntRect*, SCE_SIntRect*);
int SCE_Rectangle_Intersectsf (SCE_SFloatRect*, SCE_SFloatRect*);

void SCE_Rectangle3_Union (const SCE_SIntRect3*, const SCE_SIntRect3*,
                           SCE_SIntRect3*);
int SCE_Rectangle3_Intersection (const SCE_SIntRect3*, const SCE_SIntRect3*,
                                 SCE_SIntRect3*);

int SCE_Rectangle3_IsInside (const SCE_SIntRect3*, const SCE_SIntRect3*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
