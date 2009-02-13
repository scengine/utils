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
 
/* Cree le : 6 mars 2006
   derniere modification le 23/09/2008 */

#ifndef SCESHADERS_H
#define SCESHADERS_H

#include <SCE/utils/SCEList.h>
#include <SCE/utils/SCEMatrix.h>

#include <SCE/core/SCECShader.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* constantes propres a Shaders */
#define SCE_UNKNOWN_SHADER -1 /* NOTE: hum... */
#ifdef SCE_USE_CG
 #define SCE_CG_SHADER 0
#endif
#define SCE_GLSL_SHADER 1

#define SCE_SHADER_BAD_INDEX -1

/* :-' */
typedef void (*SCE_FShaderSetParamfv)(int, size_t, float*);
typedef void (*SCE_FShaderSetMatrix)(int, SCE_TMatrix4);

typedef struct sce_sshaderparam SCE_SShaderParam;
struct sce_sshaderparam
{
    void *param;
    int index;
    int size;
    SCE_FShaderSetParamfv setfv;
    SCE_FShaderSetMatrix setm;
};

typedef struct sce_sshader SCE_SShader;
struct sce_sshader
{
    void *v, *p;                 /* vertex/pixel shader */
    SCE_CProgram *p_glsl;        /* program GLSL */

    unsigned int type;           /* type du shader (langage; GLSL, ASM, Cg) */

    char **res[2];               /* ressources */
    char *vs_source, *ps_source; /* sources principales */
    char *vs_addsrc, *ps_addsrc; /* code source additif (defines, ...) */
    int ready;                   /* true; le shader peut etre utilise */

    /* parametres a envoyer a chaque utilisation du shader (automatiques)
       (pointeurs dont les donnees pointees sont susceptibles de changer) */
    SCE_SList *params_i;         /* entiers */
    SCE_SList *params_f;         /* flottants */
    SCE_SList *params_m;         /* matrices */
};


/* initialise le gestionnaire de shaders */
int SCE_Init_Shader (void);
/* quitte le gestionnaire de shaders */
void SCE_Quit_Shader (void);

/* retourne l'identifiant de type du media manager */
int SCE_Shader_GetMediaTypeID (void);

/* initialise une structure de shader */
void SCE_Shader_Init (SCE_SShader*);
/* cree un shader */
SCE_SShader* SCE_Shader_Create (int);
/* detruit un shader */
void SCE_Shader_Delete (SCE_SShader*);

/* retourne le langage du shader (Cg, GLSL ou ASM) */
int SCE_Shader_GetLanguage (SCE_SShader*);

/* retourne le type du shader (vertex, pixel, ou les deux) */
int SCE_Shader_GetType (SCE_SShader*);


/* charge (char**) un fichier contenant le code du vertex shader
   et/ou le code du pixel shader (prefixes : [Vertex Shader] [Pixel Shader]) */
void* SCE_Shader_LoadSourceFromFile (FILE*, const char*, void*);

/* cree un shader a partir de deux fichiers, le premier contient le code du
   vertex shader, le second le code du pixel shader. un des deux fichiers peut
   contenir les deux types de code ensemble */
SCE_SShader* SCE_Shader_CreateFromFile (const char*, const char*);

/* construit un shader */
int SCE_Shader_Build (SCE_SShader*);

/* ajoute un morceau de code source qui sera ajoute
   au debut du code source par defaut */
int SCE_Shader_AddSource (SCE_SShader*, int, char*);


/* retourne l'index d'une variable de shader */
int SCE_Shader_GetIndex (SCE_SShader*, int, const char*);
/* retourne l'index d'une variable d'attribut (fonctionne que pour GLSL) */
int SCE_Shader_GetAttribIndex (SCE_SShader*, const char*);
/*
int SCE_Shader_GetParam(SCE_SShader*, const char*);
float SCE_Shader_GetParamf(SCE_SShader*, const char*);
float* SCE_Shader_GetParamfv(SCE_SShader*, const char*);
*/

void SCE_Shader_Param (int, const char*, int);
void SCE_Shader_Paramf (int, const char*, float);
#define SCE_Shader_Paramfv(a, b, c, d) SCE_Shader_Param1fv(a, b, c, d)
void SCE_Shader_Param1fv (int, const char*, size_t, float*);
void SCE_Shader_Param2fv (int, const char*, size_t, float*);
void SCE_Shader_Param3fv (int, const char*, size_t, float*);
void SCE_Shader_Param4fv (int, const char*, size_t, float*);

void SCE_Shader_SetParam (int, int);
void SCE_Shader_SetParamf (int, float);
#define SCE_Shader_SetParamfv(a, b, d) SCE_Shader_SetParam1fv(a, b, d)
void SCE_Shader_SetParam1fv (int, size_t, float*);
void SCE_Shader_SetParam2fv (int, size_t, float*);
void SCE_Shader_SetParam3fv (int, size_t, float*);
void SCE_Shader_SetParam4fv (int, size_t, float*);

/* ajoute des parametres variables a envoyer a chaque utilisation du shader */
int SCE_Shader_AddParamv (SCE_SShader*, int, const char*, void*);
int SCE_Shader_AddParamfv (SCE_SShader*, int, const char*, int, int, void*);
/*int SCE_Shader_AddParamMatrix (SCE_SShader*, int, const char*, void*);*/

/* defini si la modification de shader via Use est possible */
void SCE_Shader_Active (int);
/* versions verbeuses */
void SCE_Shader_Enable (void);
void SCE_Shader_Disable (void);

/* defini le shader comme etant actif (fonction generique) */
void SCE_Shader_Use (SCE_SShader*);

/*
void SCE_Shader_UsePixelShader(SCE_SShader*);

void SCE_Shader_UseVertexShader(SCE_SShader*);
*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
