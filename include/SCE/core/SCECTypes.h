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
 
/* created: 06/03/2007
   updated: 07/08/2009 */

#ifndef SCECTYPES_H
#define SCECTYPES_H

/* include APIs */
#include <SCE/SCEGLee.h>
/* TODO: kick GLU dependence */
#include <GL/glu.h> /* not included/redefined by GLee.h */
#include <IL/il.h>
#include <IL/ilu.h>

#ifdef SCE_USE_CG
 #include <Cg/cg.h>
 #include <Cg/cgGL.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* (re)definition des types du SCEngine */
typedef unsigned int    SCEflags;
typedef GLenum          SCEenum;
typedef GLbitfield      SCEbitfield;
typedef GLsizei         SCEsizei;
typedef GLint           SCEint;
typedef GLuint          SCEuint;
typedef GLfloat         SCEfloat;
typedef GLdouble        SCEdouble;

/* constantes que peuvent renvoyer les fonctions du coeur
   (et autres) du SCEngine */
#define SCE_OK 0
#define SCE_ERROR (-1)
#define SCE_TRUE 1
#define SCE_FALSE 0

enum sce_ctype {
    SCE_BYTE = GL_BYTE,
    SCE_UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
    SCE_SHORT = GL_SHORT,
    SCE_UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
    SCE_INT = GL_INT,
    SCE_UNSIGNED_INT = GL_UNSIGNED_INT,
    SCE_FLOAT = GL_FLOAT,
    SCE_DOUBLE = GL_DOUBLE
};
typedef enum sce_ctype SCE_CType;

/* types de culling */
#define SCE_FRONT           GL_FRONT
#define SCE_BACK            GL_BACK

/* comparaisons */
#define SCE_LESS            GL_LESS
#define SCE_LEQUAL          GL_LEQUAL
#define SCE_GREATER         GL_GREATER
#define SCE_GEQUAL          GL_GEQUAL

/* matrices */
#define SCE_MAT_MODELVIEW   GL_MODELVIEW
#define SCE_MAT_PROJECTION  GL_PROJECTION
#define SCE_MAT_TEXTURE     GL_TEXTURE

#define SCE_CSizeof SCE_CSizeofType
size_t SCE_CSizeofType (SCEenum);

int SCE_CGetInteger (SCEenum);
float SCE_CGetFloat (SCEenum);
void SCE_CGetIntegerv (SCEenum, int*);
void SCE_CGetFloatv (SCEenum, float*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
