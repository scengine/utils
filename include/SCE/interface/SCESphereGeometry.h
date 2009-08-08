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

/* created: 02/06/2009
   updated: 03/08/2009 */

#ifndef SCESPHEREGEOMETRY_H
#define SCESPHEREGEOMETRY_H

#include <SCE/utils/SCEVector.h>
#include <SCE/interface/SCESphere.h>
#include <SCE/interface/SCEGeometry.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_SPHEREGEOMETRY_SEGMENTS 16
#define SCE_SPHEREGEOMETRY_RINGS 16

int SCE_SphereGeom_GenerateUV (SCE_SSphere*, SCEuint, SCEuint, SCE_SGeometry*);
SCE_SGeometry* SCE_SphereGeom_CreateUV (SCE_SSphere*, SCEuint, SCEuint);
SCE_SGeometry* SCE_SphereGeom_CreateDefaultUV (SCE_SSphere*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
