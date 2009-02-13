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
 
/* created: 02/07/2007
   updated: 25/09/2008 */

#ifndef SCEFRAMEBUFFER_H
#define SCEFRAMEBUFFER_H

#include <SCE/utils/SCERectangle.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup framebuffer
 * @{
 */

/* types de buffers */
#define SCE_COLOR_BUFFER0 0
#define SCE_COLOR_BUFFER1 1
#define SCE_COLOR_BUFFER2 2
#define SCE_COLOR_BUFFER3 3
#define SCE_COLOR_BUFFER4 4
#define SCE_COLOR_BUFFER5 5
#define SCE_COLOR_BUFFER6 6
#define SCE_COLOR_BUFFER7 7
#define SCE_DEPTH_BUFFER 8
#define SCE_STENCIL_BUFFER 9
/* nombre de render buffer differents */
#define SCE_NUM_RENDER_BUFFERS 10
/* nombre maximum de buffers pouvant etre attaches */
#define SCE_MAX_ATTACHMENT_BUFFERS 8 /* TODO: a mettre a jour regulierement */

/* pour la compatibilite */
#define SCE_COLOR_BUFFER SCE_COLOR_BUFFER0


/* buffer de rendu, soit vers un ID, soit vers une texture */
/**
 * \copydoc sce_crenderbuffer
 */
typedef struct sce_crenderbuffer SCE_CRenderBuffer;
/**
 * \brief A render buffer structure
 *
 * This structure stores a texture or a OpenGL render buffer.
 */
struct sce_crenderbuffer
{
    SCEuint id;        /**< OpenGL render buffer's identifier */
    SCE_CTexture *tex; /**< The render texture */
    int user;          /**< Is user the owner of \c tex ? */
    int actived;       /**< Is render buffer actived ? */
};

/**
 * \copydoc sce_cframebuffer
 */
typedef struct sce_cframebuffer SCE_CFramebuffer;
/**
 * \brief A frame buffer structure
 *
 * This is a FBO.
 */
struct sce_cframebuffer
{
    SCEuint id;             /**< OpenGL identifier of the FBO */
    /** All the targets of \c id, used for Multiple Render Targets extension */
    SCE_CRenderBuffer buffers[SCE_NUM_RENDER_BUFFERS];
    int x, y, w, h; /* TODO: use rectangle ? */
};

/** @} */

/* initialise le gestionnaire */
int SCE_CFramebufferInit (void);

/* defini le FBO actif pour les modifications */
void SCE_CBindFramebuffer (SCE_CFramebuffer*);

/* cree un FBO */
SCE_CFramebuffer* SCE_CCreateFramebuffer (void);
/* supprime un FBO */
void SCE_CDeleteFramebuffer (SCE_CFramebuffer*);
void SCE_CDeleteFramebuffer_ (void);

/* ajoute un tampon de rendu vers une texture */
int SCE_CAddRenderTexture (SCE_CFramebuffer*, SCEuint, SCEenum,
                           SCE_CTexture*, int, int);
int SCE_CAddRenderTexture_ (SCEuint, SCEenum, SCE_CTexture*, int, int);

/* ajoute un tampon de rendu vers ... rien :D */
int SCE_CAddRenderBuffer (SCE_CFramebuffer*, SCEuint, int, int, int);
int SCE_CAddRenderBuffer_ (SCEuint, int, int, int);

/* cree une texture et l'ajoute comme render texture */
int SCE_CCreateRenderTexture (SCE_CFramebuffer*, SCEuint,
                              int, int, int, int, int);
int SCE_CCreateRenderTexture_ (SCEuint, int, int, int, int, int);

/* recupere la texture de rendu du render buffer specifie */
SCE_CTexture* SCE_CGetRenderTexture (SCE_CFramebuffer*, SCEuint);
SCE_CTexture* SCE_CGetRenderTexture_ (SCEuint);

/* active/desactive un render buffer */
void SCE_CActivateRenderBuffer (SCE_CFramebuffer*, int, int);
void SCE_CEnableRenderBuffer (SCE_CFramebuffer*, int);
void SCE_CDisableRenderBuffer (SCE_CFramebuffer*, int);

/* defini si le buffer specifie du FBO doit etre vide avant le rendu */
void SCE_CClearRenderbuffer (SCE_CFramebuffer*, SCEuint, int);
void SCE_CClearRenderbuffer_ (SCEuint, int);

/* retourne le nombre maximum de buffers supporte pour le MRT */
unsigned int SCE_CGetMaxAttachmentBuffers (void);

/* defini le frame buffer actif */
void SCE_CUseFramebuffer (SCE_CFramebuffer*, SCE_SIntRect*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
