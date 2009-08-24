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
   updated: 24/08/2009 */

#ifndef SCELINE_H
#define SCELINE_H

#include <SCE/utils/SCEVector.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_LINE_COLINEAR 1
#define SCE_LINE_EQUAL 2

typedef struct sce_sline SCE_SLine;
struct sce_sline {
    SCE_TVector2 a, b;
};

void SCE_Line_Init (SCE_SLine*);

void SCE_Line_Set (SCE_SLine*, float, float, float, float);
void SCE_Line_Setv (SCE_SLine*, SCE_TVector2, SCE_TVector2);

void SCE_Line_MovePoint1 (SCE_SLine*, float, float);
void SCE_Line_MovePoint1v (SCE_SLine*, float*);
void SCE_Line_MovePoint2 (SCE_SLine*, float, float);
void SCE_Line_MovePoint2v (SCE_SLine*, float*);

int SCE_Line_IsInXInterval (SCE_SLine*, float);
int SCE_Line_IsInYInterval (SCE_SLine*, float);

void SCE_Line_GetEquation(SCE_SLine*, float*, float*);

int SCE_Line_GetIntersection (SCE_SLine*, SCE_SLine*, float*, float*);
int SCE_Line_GetIntersectionv (SCE_SLine*, SCE_SLine*, SCE_TVector2);

int SCE_Line_Intersects (SCE_SLine*, SCE_SLine*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
