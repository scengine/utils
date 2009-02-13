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
 
/* Cree le : 10 janvier 2007
   derniere modification le 02/07/2007 */

#ifndef SCECMATRIX_H
#define SCECMATRIX_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constante symbolique representant la matrice active */
#define SCE_CURRENT_MATRIX 0

/* charge la matrice d'identite */
void SCE_CLoadIdentityMatrix (void);

void SCE_CPushMatrix (void);
void SCE_CPopMatrix (void);

/* selectionne la matrice specifiee en argument comme matrice active */
void SCE_CSetActiveMatrix (SCEenum);
/* renvoie le mode de la matrice active */
GLint SCE_CGetActiveMatrix (void);

/* multiplie la matrice active par celle specifiee */
void SCE_CMultMatrix (const float*);
/* charge la matrice specifiee */
void SCE_CLoadMatrix (const float*);

/* retourne la matrice demandee */
void SCE_CGetMatrix (int, float*);


/* translation */
void SCE_CTranslateMatrix (float, float, float);

/* rotation */
void SCE_CRotateMatrix (float, float, float, float);

/* echelle */
void SCE_CScaleMatrix (float, float, float);

/* versions vectorielles */
void SCE_CTranslateMatrixv (float*);
void SCE_CRotateMatrixv (float, float*);
void SCE_CScaleMatrixv (float*);
/***/

#if 0
/* definit un point de vue, une position et un axe vertical */
void SCE_CLook3D (
  float, float, float,
  float, float, float,
  float, float, float);

/* version vectorielle */
void SCE_CLook3Dv (const float*, const float*, const float*);
#endif

/* cree une matrice de frustum de vision en perspective */
void SCE_CSetFrustumMatrix (
  GLdouble, GLdouble,
  GLdouble, GLdouble,
  GLdouble, GLdouble);

/* cree une matrice de projection en perspective cavaliere */
void SCE_CSetOrthoMatrix (
  GLdouble, GLdouble,
  GLdouble, GLdouble,
  GLdouble, GLdouble);

/* definit le cadrage actif dans lequel dessiner */
void SCE_CViewport (int, int, int, int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
