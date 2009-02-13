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
 
/* Cree le : 11 fevrier 2007
   derniere modification le 14/03/2008 */

#ifndef SCECSHADER_H
#define SCECSHADER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constantes propres a CShader */
#define SCE_VERTEX_SHADER 1
#define SCE_PIXEL_SHADER 2

/* structure d'un shader GLSL */
typedef struct sce_cshaderglsl SCE_CShaderGLSL;
struct sce_cshaderglsl
{
    GLuint id;     /* identifiant opengl */
    GLenum type;   /* type du shader (vertex ou fragment) */
    GLchar *data;  /* donnes */
    int is_pixelshader;
    int compiled;
};

#ifdef SCE_USE_CG
/* structure d'un shader Cg */
typedef struct sce_cshadercg SCE_CShaderCG;
struct sce_cshadercg
{
    CGprogram id;   /* identifiant cg */
    CGprofile type; /* type du shader (vertex / fragment), ou 'profile' pour Cg */
    char *data;     /* donnes */
    char **args;    /* arguments du shader */
    int is_pixelshader;
    int compiled;
};
#endif


/* structure d'un programme regroupant des shaders GLSL */
typedef struct sce_cprogram SCE_CProgram;
struct sce_cprogram
{
    GLuint id;    /* identifiant opengl */

    /* booleen indiquant si le programme a ete lie/relie
       depuis la dernire assignation de shader au program */
    int compiled;
    /* eventuellement, rajouter des donnees intelligentes pour le stockage
       intelligent de Uniform* ... */
};


/* initialise le gestionnaire de shaders
   le parametre est un booleen indiquant si l'on souhaite ou non utiliser Cg */
int SCE_CShaderInit (int);
/* quitte le gestionnaire de shaders */
void SCE_CShaderQuit (void);

#ifdef SCE_USE_CG
/* renvoie le profile par defaut du type (pixel/vertex) demande */
CGprofile SCE_CGetCgProfile (SCEenum);
#endif

/* cree un objet de shader */
SCE_CShaderGLSL* SCE_CCreateShaderGLSL (SCEenum);
#ifdef SCE_USE_CG
SCE_CShaderCG* SCE_CCreateShaderCG(SCEenum);
#endif

/* supprime un shader */
void SCE_CDeleteShaderGLSL (SCE_CShaderGLSL*);
#ifdef SCE_USE_CG
void SCE_CDeleteShaderCG (SCE_CShaderCG*);
#endif

/* associe un code source au shader specifie */
void SCE_CSetShaderGLSLSource (SCE_CShaderGLSL*, char*);
int SCE_CSetShaderGLSLSourceDup (SCE_CShaderGLSL*, char*);
#ifdef SCE_USE_CG
void SCE_CSetShaderCGSource (SCE_CShaderCG*, char*);
int SCE_CSetShaderCGSourceDup (SCE_CShaderCG*, char*);
/* petite specialisation pour Cg, la gestion des arguments */
void SCE_CSetShaderCGArgs (SCE_CShaderCG*, char**);
int SCE_CSetShaderCGArgsDup (SCE_CShaderCG*, char**);
#endif

/* construit et compile le shader specifie */
int SCE_CBuildShaderGLSL (SCE_CShaderGLSL*);
#ifdef SCE_USE_CG
int SCE_CBuildShaderCG (SCE_CShaderCG*);
#endif

/* definit le shader selectionne comme shader actif pour le rendu */
#ifdef SCE_USE_CG
void SCE_CUseShaderCG (SCE_CShaderCG*);
#endif


/** !! ** fonctions pour les shaders GLSL uniquement ** !! **/

/* cree un objet de programme */
SCE_CProgram* SCE_CCreateProgram (void);

/* supprime un programme */
void SCE_CDeleteProgram (SCE_CProgram*);

/* associe un shader a un programme */
int SCE_CSetProgramShader (SCE_CProgram*, SCE_CShaderGLSL*, int);

/* lie un programme a ses shaders associes (le construit) */
int SCE_CBuildProgram (SCE_CProgram*);

/* defini le programme (GLSL) actif */
void SCE_CUseProgram (SCE_CProgram*);

/** !! ** ****************************************** ** !! **/


/* fonctions de gestion des etats des shaders
   (activation/desactivation manuelle) */
#if 0
void SCE_CDisableShaderGLSL (void);
#ifdef SCE_USE_CG
void SCE_CDisableShaderCG (int);
#endif
#endif

/* fonctions d'envoie des parametres.
   attention: pour les shaders GLSL, le mot 'Shader' est remplace par 'Program',
   car les modifications s'effectuent sur un program et non sur un shader. */

/* fonctions renvoyant l'index du parametre dont on donne le nom */
SCEint SCE_CGetProgramIndex (SCE_CProgram*, const char*);
#ifdef SCE_USE_CG
CGparameter SCE_CGetShaderCGIndex (SCE_CShaderCG*, const char*);
#endif

/* fonction de renvoie de l'index d'attribut */
SCEint SCE_CGetProgramAttribIndex (SCE_CProgram*, const char*);
/* TODO: trouver de quoi satisfaire les autres types de shaders... */

/* fonctions d'envoie de parametres aux shaders */
/* GLSL */
void SCE_CSetProgramParam (SCEint, int);
void SCE_CSetProgramParamf (SCEint, float);
void SCE_CSetProgramParam1fv (SCEint, size_t, const float*);
void SCE_CSetProgramParam2fv (SCEint, size_t, const float*);
void SCE_CSetProgramParam3fv (SCEint, size_t, const float*);
void SCE_CSetProgramParam4fv (SCEint, size_t, const float*);
void SCE_CSetProgramMatrix2 (SCEint, size_t, const float*);
void SCE_CSetProgramMatrix3 (SCEint, size_t, const float*);
void SCE_CSetProgramMatrix4 (SCEint, size_t, const float*);

/* Cg */
#ifdef SCE_USE_CG
void SCE_CSetShaderCGParam (CGparameter, int);
void SCE_CSetShaderCGParamf (CGparameter, float);
void SCE_CSetShaderCGParam1fv (CGparameter, size_t, const float*);
void SCE_CSetShaderCGParam2fv (CGparameter, size_t, const float*);
void SCE_CSetShaderCGParam3fv (CGparameter, size_t, const float*);
void SCE_CSetShaderCGParam4fv (CGparameter, size_t, const float*);
void SCE_CSetShaderCGMatrix (CGparameter, const float*);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
