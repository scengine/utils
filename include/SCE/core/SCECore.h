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
 
/* Cree le : 15 decembre 2006
   derniere modification le 25/09/2008 */

#ifndef SCECORE_H
#define SCECORE_H

/* inclusion des en-tetes de tous les modules du coeur */
#include <SCE/core/SCECSupport.h>
#include <SCE/core/SCECMatrix.h>
#include <SCE/core/SCECBuffers.h>
#include <SCE/core/SCECTexture.h>
#include <SCE/core/SCECFramebuffer.h>
#include <SCE/core/SCECShader.h>
#include <SCE/core/SCECMaterial.h>
#include <SCE/core/SCECLight.h>
#include <SCE/core/SCECOcclusionQuery.h>
#include <SCE/core/SCECPointSprite.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* flags d'initialisation du coeur */
#define SCE_CINIT_CG_SHADERS (0x00000001)

/* fonction d'initialisation du coeur et d'opengl et ses extensions.
   initialise les modules tiers (Cg) par demande explicite via ses flags */
int SCE_CInit (SCEflags);
/* quitte le coeur du SCEngine (desactive tous les modules du core) */
void SCE_CQuit (void);

/* specifie les valeurs de vidange */
void SCE_CClearColor (float, float, float, float);
void SCE_CClearDepth (float);
/* fonction de vidange d'un ou plusieurs tampons d'image */
void SCE_CClear (const SCEbitfield);
/* glFlush */
void SCE_CFlush (void);

/* positionne un (ou plusieurs) etat opengl suivant la valeur booleenne (int) */
void SCE_CSetState (SCEenum, int);
void SCE_CSetState2 (SCEenum, SCEenum, int);
void SCE_CSetState3 (SCEenum, SCEenum, SCEenum, int);
void SCE_CSetState4 (SCEenum, SCEenum, SCEenum, SCEenum, int);
void SCE_CSetState5 (SCEenum, SCEenum, SCEenum, SCEenum, SCEenum, int);
void SCE_CSetState6 (SCEenum, SCEenum, SCEenum, SCEenum, SCEenum, SCEenum, int);

void SCE_CSetBlending (SCEenum, SCEenum);

void SCE_CActivateColorBuffer (int);
void SCE_CActivateDepthBuffer (int);

void SCE_CSetCulledFaces (SCEenum);
void SCE_CSetValidPixels (SCEenum);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
