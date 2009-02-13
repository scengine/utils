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
 
/* Cree le : 13/03/2008
   derniere modification : 11/07/2008 */

#ifndef SCELIGHT_H
#define SCELIGHT_H

#include <SCE/core/SCECLight.h>
#include <SCE/interface/SCENode.h>
#include <SCE/interface/SCEShaders.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* nombre maximal de lumieres de type shader */
#define SCE_MAX_SHADER_LIGHTS 16

/* nombre de parametres differents pour la lumiere */
#define SCE_NUM_LIGHT_PARAMS 6

/* types de lumiere (c'est un peu debile...) */
#define SCE_SHADER_LIGHT SCE_FALSE
#define SCE_OPENGL_LIGHT SCE_TRUE

/* noms des variables de parametres pour la lumiere */
#define SCE_COLOR_NAME     "sce_ColorLight"
#define SCE_POSITION_NAME  "sce_PositionLight"
#define SCE_DIRECTION_NAME "sce_DirectionLight"
#define SCE_ANGLE_NAME     "sce_AngleLight"
#define SCE_RADIUS_NAME    "sce_RadiusLight"
#define SCE_NUMLIGHTS_NAME "sce_NumLights"

typedef struct sce_slight SCE_SLight;
struct sce_slight
{
    SCE_CLight *clight; /* lumiere coeur */
    float intensity;    /* coefficient d'intensite */
    float radius;       /* rayon de portee de la lumiere */
    int gl;             /* booleen indiquant si la lumiere est geree par gl */
    int actived;        /* defini si la lumiere est active */
    SCE_SNode *node;    /* noeud de la lumiere */
};

/* initialise une structure de lumiere */
void SCE_Light_Init (SCE_SLight*);

/* cree une lumiere */
SCE_SLight* SCE_Light_Create (void);
/* supprime une lumiere */
void SCE_Light_Delete (SCE_SLight*);

/* renvoie le nombre de lumieres activees */
int SCE_Light_GetNumActived (void);

/* permet de specifier la matrice de la camera */
void SCE_Light_SetCameraMatrix (const SCE_TMatrix4);

/* active/desactive une lumiere */
void SCE_Light_Activate (SCE_SLight*, int);
/* retourne l'etat d'une lumiere */
int SCE_Light_IsActived (SCE_SLight*);

/* defini le type de la lumiere */
void SCE_Light_SetType (SCE_SLight*, int);
/* renvoie le type d'une lumiere */
int SCE_Light_GetType (SCE_SLight*);

/* renvoie le noeud d'une lumiere */
SCE_SNode* SCE_Light_GetNode (SCE_SLight*);
/* renvoie la matrice du noeud d'une lumiere */
float* SCE_Light_GetNodeMatrix (SCE_SLight*);

/* defini la couleur d'une lumiere */
void SCE_Light_SetColor (SCE_SLight*, float, float, float);
void SCE_Light_SetColorv (SCE_SLight*, float*);
/* renvoie la couleur d'une lumiere */
float* SCE_Light_GetColor (SCE_SLight*);
void SCE_Light_GetColorv (SCE_SLight*, float*);

/* copie la position de la lumiere */
void SCE_Light_CopyPosition (SCE_SLight*, float*);

/* copie la direction de la lumiere */
void SCE_Light_CopyDirection (SCE_SLight*, float*);

/* defini une position infinie (soleil par ex.) pour une lumiere */
void SCE_Light_Infinite (SCE_SLight*, int);
/* indique si la lumiere est infinie */
int SCE_Light_IsInfinite (SCE_SLight*);

/* defini l'angle du spot */
void SCE_Light_SetAngle (SCE_SLight*, float);
/* renvoie l'angle du spot */
float SCE_Light_GetAngle (SCE_SLight*);

/* defini l'intensite d'une lumiere */
void SCE_Light_SetIntensity (SCE_SLight*, float);
/* renvoie l'intensite d'une lumiere */
float SCE_Light_GetIntensity (SCE_SLight*);

/* defini le rayon d'une lumiere */
void SCE_Light_SetRadius (SCE_SLight*, float);
/* renvoie le rayon d'une lumiere */
float SCE_Light_GetRadius (SCE_SLight*);

/* active/desactive l'eclairage */
void SCE_Light_ActivateLighting (int);

/* defini une lumiere comme etant active pour le rendu */
void SCE_Light_Use (SCE_SLight*);

/* construit le tableau des indices des parametres de shader pour la lumiere */
void SCE_Light_MakeIndices (SCE_SShader*, int*);

/* invoque les fonctions de specification des parametres de shader GLSL */
void SCE_Light_Apply (int*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
