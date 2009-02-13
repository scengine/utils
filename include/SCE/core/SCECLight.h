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
 
/* Cree le : 08/03/2008
   derniere modification : 19/10/2008 */

#ifndef SCECLIGHT_H
#define SCECLIGHT_H

#include <SCE/utils/SCEVector.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sce_clight SCE_CLight;
struct sce_clight
{
    float color[4];   /**< Light's colors */
    SCE_TVector4 pos; /**< Light's position */
    SCE_TVector3 dir; /**< Light's spot direction */
    float angle;      /**< Light's spot angle */
    float gat;        /**< Global attenuation */
    float lat;        /**< Linear attenuation */
    float qat;        /**< Quadratic attenuation */
    int ccontrol;     /**< What is the color control ? */
};

/* initialise une structure de lumiere */
void SCE_CInitLight (SCE_CLight*);

/* cree une nouvelle lumiere */
SCE_CLight* SCE_CCreateLight (void);
/* supprimme une lumiere */
void SCE_CDeleteLight (SCE_CLight*);

/* defini la couleur d'une lumiere */
void SCE_CSetLightColor (SCE_CLight*, float, float, float);
void SCE_CSetLightColorv (SCE_CLight*, float*);
/* renvoie la couleur d'une lumiere */
float* SCE_CGetLightColor (SCE_CLight*);
void SCE_CGetLightColorv (SCE_CLight*, float*);

/* defini la position d'une lumiere */
void SCE_CSetLightPosition (SCE_CLight*, float, float, float);
void SCE_CSetLightPositionv (SCE_CLight*, SCE_TVector3);
/* renvoie la position d'une lumiere */
float* SCE_CGetLightPosition (SCE_CLight*);
void SCE_CGetLightPositionv (SCE_CLight*, SCE_TVector3);

/* defini une position infinie (soleil par ex.) pour une lumiere */
void SCE_CInfiniteLight (SCE_CLight*, int);
/* indique si la lumiere est infinie */
int SCE_CIsInfiniteLight (SCE_CLight*);

void SCE_CSetLightIndependantSpecular (SCE_CLight*, int);
int SCE_CIsLightIndependantSpecular (SCE_CLight*);

/* defini le vecteur de direction du spot */
void SCE_CSetLightDirection (SCE_CLight*, float, float, float);
void SCE_CSetLightDirectionv (SCE_CLight*, SCE_TVector3);
/* renvoie le vecteur de direction du spot */
float* SCE_CGetLightDirection (SCE_CLight*);
void SCE_CGetLightDirectionv (SCE_CLight*, SCE_TVector3);

/* defini l'angle du spot */
void SCE_CSetLightAngle (SCE_CLight*, float);
/* renvoie l'angle du spot */
float SCE_CGetLightAngle (SCE_CLight*);

/* fonctions de manipulation des facteurs d'attenuation */
void SCE_CSetLightGlobalAtt (SCE_CLight*, float);
float SCE_CGetLightGlobalAtt (SCE_CLight*);
void SCE_CSetLightLinearAtt (SCE_CLight*, float);
float SCE_CGetLightLinearAtt (SCE_CLight*);
void SCE_CSetLightQuadraticAtt (SCE_CLight*, float);
float SCE_CGetLightQuadraticAtt (SCE_CLight*);

/* active/desactive l'eclairage */
void SCE_CActivateLighting (int);

/* defini une lumiere active pour le rendu */
void SCE_CUseLight (SCE_CLight*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
