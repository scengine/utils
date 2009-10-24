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
 
/* created: 12/10/2006
   updated: 12/08/2009 */

#ifndef SCENGINE_H
#define SCENGINE_H

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEUtils.h>
#include <SCE/core/SCECore.h>

#include <SCE/interface/SCEBoundingBox.h>
#include <SCE/interface/SCEBoundingSphere.h>
#include <SCE/interface/SCEFrustum.h>
#include <SCE/interface/SCEOctree.h>
#include <SCE/interface/SCENode.h>
#include <SCE/interface/SCEGeometry.h>
#include <SCE/interface/SCEOBJLoader.h>
#include <SCE/interface/SCESphereGeometry.h>
#include <SCE/interface/SCEBoxGeometry.h>
#include <SCE/interface/SCEParticle.h>
#include <SCE/interface/SCEParticleEmitter.h>
#include <SCE/interface/SCEParticleModifier.h>
#include <SCE/interface/SCEParticleProcessor.h>
#include <SCE/interface/SCEParticleSystem.h>
#include <SCE/interface/SCEJoint.h>
#include <SCE/interface/SCESkeleton.h>
#include <SCE/interface/SCEAnimatedGeometry.h>
#include <SCE/interface/SCEAnimation.h>
#include <SCE/interface/SCEMD5Loader.h>
#include <SCE/interface/SCEQuad.h>
#include <SCE/interface/SCEMesh.h>

#include <SCE/interface/SCELight.h>
#include <SCE/interface/SCECamera.h>
#include <SCE/interface/SCELevelOfDetail.h>
#include <SCE/interface/SCEGeometryInstance.h>
#include <SCE/interface/SCESceneResource.h>
#include <SCE/interface/SCESceneEntity.h>
#include <SCE/interface/SCEModel.h>
#include <SCE/interface/SCESkybox.h>
#include <SCE/interface/SCEScene.h>

/* set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

int SCE_Init (FILE*, SCEflags);
void SCE_Quit (void);

const char* SCE_GetVersionString (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
