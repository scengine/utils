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

/* created: 27/06/2009
   updated: 27/06/2009 */

#ifndef SCEMODEL_H
#define SCEMODEL_H

#include <SCE/utils/SCEList.h>
#include <SCE/interface/SCESceneEntity.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sce_smodelgroup SCE_SModelGroup;
struct sce_smodelgroup
{
    SCE_SSceneEntityGroup *group;
    int canfree;                /* YES!1ONE */
};

typedef struct sce_smodel SCE_SModel;
struct sce_smodel
{
    SCE_SList *groups;
    SCE_SList *entities;

};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
