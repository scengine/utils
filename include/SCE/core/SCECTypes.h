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
   updated: 27/07/2009 */

#ifndef SCECTYPES_H
#define SCECTYPES_H

/* include APIs */
#include <SCE/SCEGLee.h>
#include <GL/glu.h> /* not included/redefined by GLee.h */
#include <IL/il.h>
#include <IL/ilu.h>

#ifdef SCE_USE_CG
 #include <Cg/cg.h>
 #include <Cg/cgGL.h>
#endif

#ifdef __cplusplus
extern "C"
{
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

/* types de variable */
#define SCE_BYTE            GL_BYTE
#define SCE_UNSIGNED_BYTE   GL_UNSIGNED_BYTE
#define SCE_SHORT           GL_SHORT
#define SCE_UNSIGNED_SHORT  GL_UNSIGNED_SHORT
#define SCE_INT             GL_INT
#define SCE_UNSIGNED_INT    GL_UNSIGNED_INT
#define SCE_FLOAT           GL_FLOAT
#define SCE_DOUBLE          GL_DOUBLE

/* types de polygone */
#define SCE_POINTS          GL_POINTS
#define SCE_LINES           GL_LINES
#define SCE_LINE_LOOP       GL_LINE_LOOP
#define SCE_LINE_STRIP      GL_LINE_STRIP
#define SCE_TRIANGLES       GL_TRIANGLES
#define SCE_TRIANGLE_STRIP  GL_TRIANGLE_STRIP
#define SCE_TRIANGLE_FAN    GL_TRIANGLE_FAN
/* {deprecated */
#define SCE_QUADS           GL_QUADS
#define SCE_QUAD_STRIP      GL_QUAD_STRIP
/* deprecated} */

/* types de culling */
#define SCE_FRONT           GL_FRONT
#define SCE_BACK            GL_BACK

/* comparaisons */
#define SCE_LESS            GL_LESS
#define SCE_LEQUAL          GL_LEQUAL
#define SCE_GREATER         GL_GREATER
#define SCE_GEQUAL          GL_GEQUAL

/* unites de texture */
#define SCE_TEX0            GL_TEXTURE0
#define SCE_TEX1            GL_TEXTURE1
#define SCE_TEX2            GL_TEXTURE2
#define SCE_TEX3            GL_TEXTURE3
#define SCE_TEX4            GL_TEXTURE4
#define SCE_TEX5            GL_TEXTURE5
#define SCE_TEX6            GL_TEXTURE6
#define SCE_TEX7            GL_TEXTURE7
#define SCE_TEX8            GL_TEXTURE8
#define SCE_TEX9            GL_TEXTURE9
#define SCE_TEX10           GL_TEXTURE10
#define SCE_TEX11           GL_TEXTURE11
/* types de texture */
#define SCE_TEX_1D          GL_TEXTURE_1D
#define SCE_TEX_2D          GL_TEXTURE_2D
#define SCE_TEX_3D          GL_TEXTURE_3D
#define SCE_TEX_CUBE        GL_TEXTURE_CUBE_MAP
#define SCE_TEX_POSX        GL_TEXTURE_CUBE_MAP_POSITIVE_X
#define SCE_TEX_NEGX        GL_TEXTURE_CUBE_MAP_NEGATIVE_X
#define SCE_TEX_POSY        GL_TEXTURE_CUBE_MAP_POSITIVE_Y
#define SCE_TEX_NEGY        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
#define SCE_TEX_POSZ        GL_TEXTURE_CUBE_MAP_POSITIVE_Z
#define SCE_TEX_NEGZ        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
/* filtrage des textures */
#define SCE_TEX_NEAREST     GL_NEAREST
#define SCE_TEX_LINEAR      GL_LINEAR
#define SCE_TEX_BILINEAR    GL_LINEAR_MIPMAP_NEAREST /* TODO: noms a revoir */
#define SCE_TEX_TRILINEAR   GL_LINEAR_MIPMAP_LINEAR
#define SCE_TEX_MAG_FILTER  GL_TEXTURE_MAG_FILTER
#define SCE_TEX_MIN_FILTER  GL_TEXTURE_MIN_FILTER

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
