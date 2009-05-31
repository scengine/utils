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

/* created: 17/02/2009
   updated: 17/02/2009 */

#ifndef SCESKYBOX_H
#define SCESKYBOX_H

#include <SCE/interface/SCESceneEntity.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup skybox
 * @{
 */

/** \copydoc sce_sskybox */
typedef struct sce_sskybox SCE_SSkybox;
/**
 * \brief A skybox, represented by its resources and its entity instance
 */
struct sce_sskybox
{
    SCE_SMesh *mesh;                    /**< Skybox cube mesh */
    SCE_SSceneResource *tex;            /**< Texture */
    int mode;                           /**< Texture mode */
    int textype;
    SCE_SSceneResource *shader;         /**< Shader */
#if 0
    SCE_SSceneEntityGroup *group;       /**< Entity group */
#endif
    SCE_SSceneEntity *entity;           /**< Entity */
    SCE_SSceneEntityInstance *instance; /**< Instance */
};

/** @} */

SCE_SSkybox* SCE_Skybox_Create (void);
void SCE_Skybox_Delete (SCE_SSkybox*);

void SCE_Skybox_SetSize (SCE_SSkybox*, float);
int SCE_Skybox_SetTexture (SCE_SSkybox*, SCE_SSceneResource*, int);
void SCE_Skybox_SetShader (SCE_SSkybox*, SCE_SSceneResource*);

#if 0
SCE_SSceneEntityGroup* SCE_Skybox_GetEntityGroup (SCE_SSkybox*);
#endif
SCE_SSceneEntity* SCE_Skybox_GetEntity (SCE_SSkybox*);
SCE_SSceneEntityInstance* SCE_Skybox_GetInstance (SCE_SSkybox*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
