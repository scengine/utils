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
 
/* Cree le : 24/03/2008
   derniere modification : 24/03/2008 */

#ifndef SCECPOINTSPRITE_H
#define SCECPOINTSPRITE_H

#include <SCE/utils/SCEVector.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sce_cpointsprite SCE_CPointSprite;
struct sce_cpointsprite
{
    float size;    /* taille du point */
    int smooth;    /* point lisse ou non (booleen) */
    int textured;  /* point texture ou non */
    float att[3];  /* facteurs d'attenuation du point */
    float minsize; /* tailles maximales et minimales du point */
    float maxsize;
};

/* initialise le gestionnaire */
int SCE_CPointSpriteInit (void);

/* initialise une sructure */
void SCE_CInitPointSprite (SCE_CPointSprite*);

/* cree un point sprite */
SCE_CPointSprite* SCE_CCreatePointSprite (void);
/* supprime un point sprite */
void SCE_CDeletePointSprite (SCE_CPointSprite*);

/* defini la taille d'un point */
void SCE_CSetPointSpriteSize (SCE_CPointSprite*, float);
float SCE_CGetPointSpriteSize (SCE_CPointSprite*);

/* defini si un point doit etre lisse */
void SCE_CSmoothPointSprite (SCE_CPointSprite*, int);
int SCE_CIsPointSpriteSmoothed (SCE_CPointSprite*);

/* defini si on utilise une texture sur les points */
void SCE_CActivatePointSpriteTexture (SCE_CPointSprite*, int);
void SCE_CEnablePointSpriteTexture (SCE_CPointSprite*);
void SCE_CDisablePointSpriteTexture (SCE_CPointSprite*);

/* defini les attenuations de taille d'un point */
void SCE_CSetPointSpriteAttenuations (SCE_CPointSprite*, float, float, float);
void SCE_CSetPointSpriteAttenuationsv (SCE_CPointSprite*, SCE_TVector3);

/* defini les parametres d'un point sprites comme actifs
   pour les prochains rendus de points */
void SCE_CUsePointSprite (SCE_CPointSprite*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
