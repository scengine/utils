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
 
/* created: 15/12/2006
   updated: 02/08/2009 */

#ifndef SCECORE_H
#define SCECORE_H

#include <SCE/core/SCECSupport.h>
#include <SCE/core/SCECMatrix.h>
#include <SCE/core/SCECBuffer.h>
#include <SCE/core/SCECVertexArray.h>
#include <SCE/core/SCECVertexBuffer.h>
#include <SCE/core/SCECTexture.h>
#include <SCE/core/SCECFramebuffer.h>
#include <SCE/core/SCECShader.h>
#include <SCE/core/SCECMaterial.h>
#include <SCE/core/SCECLight.h>
#include <SCE/core/SCECOcclusionQuery.h>
#include <SCE/core/SCECPointSprite.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_CINIT_CG_SHADERS (0x00000001)

int SCE_CInit (SCEflags);
void SCE_CQuit (void);

void SCE_CClearColor (float, float, float, float);
void SCE_CClearDepth (float);
void SCE_CClear (const SCEbitfield);
void SCE_CFlush (void);

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
