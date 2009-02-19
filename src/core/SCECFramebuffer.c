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
   updated: 05/02/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECSupport.h>
#include <SCE/core/SCECTexture.h>
#include <SCE/core/SCECFramebuffer.h>

/*
 * \file SCECFramebuffer.c
 * \copydoc coreframebuffer
 * 
 * \file SCECFramebuffer.h
 * \copydoc coreframebuffer
 */

/**
 * \defgroup coreframebuffer Frame Buffer Objects and render to texture
 * \ingroup core
 * \internal
 *
 * Offers frame buffers' managment functions for render to texture
 */

/** @{ */

/* TODO: rename it... */
static SCE_CFramebuffer *binded = NULL;

static SCEint max_attachement_buffers = 0;

int SCE_CFramebufferInit (void)
{
    if (SCE_CHasCap (SCE_MRT))
        glGetIntegerv (GL_MAX_DRAW_BUFFERS, &max_attachement_buffers);
    return SCE_OK;
}


void SCE_CBindFramebuffer (SCE_CFramebuffer *fb)
{
    binded = fb;
    if (binded)
        glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, binded->id);
    else
        glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
}


/**
 * \brief Initializes a framebuffer structure
 * \param fb the structure to initialize
 */
void SCE_CInitFramebuffer (SCE_CFramebuffer *fb)
{
    unsigned int i;

    fb->id = 0;
    for (i=0; i<SCE_NUM_RENDER_BUFFERS; i++)
    {
        fb->buffers[i].id = 0;
        fb->buffers[i].tex = NULL;
        fb->buffers[i].user = SCE_TRUE;
        fb->buffers[i].actived = SCE_FALSE;
    }
    fb->x = fb->y = 0;
    fb->w = fb->h = 1;
}

/**
 * \brief Creates a new framebuffer
 * \returns a pointer to the new framebuffer
 */
SCE_CFramebuffer* SCE_CCreateFramebuffer (void)
{
    SCE_CFramebuffer *fb = NULL;

    SCE_btstart ();
    fb = SCE_malloc (sizeof *fb);
    if (!fb)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    SCE_CInitFramebuffer (fb);
    glGenFramebuffersEXT (1, &fb->id);

    SCE_btend ();
    return fb;
}

#define SCE_CDEFAULTFUNC(action)\
SCE_CFramebuffer *back = binded;\
SCE_CBindFramebuffer (fb);\
action;\
SCE_CBindFramebuffer (back);

#define SCE_CDEFAULTFUNCR(t, action)\
t r;\
SCE_CFramebuffer *back = binded;\
SCE_CBindFramebuffer (fb);\
r = action;\
SCE_CBindFramebuffer (back);\
return r;

/**
 * \brief Deletes an existing frame buffer
 * \param fb the frame buffer to delete
 */
void SCE_CDeleteFramebuffer (SCE_CFramebuffer *fb)
{
    SCE_CDEFAULTFUNC (SCE_CDeleteFramebuffer_ ())
}
void SCE_CDeleteFramebuffer_ (void)
{
    SCE_btstart ();
    if (binded)
    {
        unsigned int i;
        for (i=0; i<SCE_NUM_RENDER_BUFFERS; i++)
        {
            if (!binded->buffers[i].user)
                SCE_CDeleteTexture (binded->buffers[i].tex);

            if (binded->buffers[i].id)
                glDeleteRenderbuffersEXT (1, &binded->buffers[i].id);
        }

        glDeleteFramebuffersEXT (1, &binded->id);
        SCE_free (binded);
        binded = NULL;
    }
    SCE_btend ();
}


/* converti un simple ID de type d'attachement en type opengl */
static int SCE_CIDToGLBuffer (SCEuint type)
{
    switch (type)
    {
    case SCE_DEPTH_BUFFER: return GL_DEPTH_ATTACHMENT_EXT;
    case SCE_STENCIL_BUFFER: return GL_STENCIL_ATTACHMENT_EXT;
    default: return GL_COLOR_ATTACHMENT0_EXT + type;
    }
}

static const char* SCE_CGetFramebufferError (SCEenum err)
{
    switch (err)
    {
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        return "incomplete attachement";
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        return "incomplete draw buffer";
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        return "incomplete read buffer";
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        return "unsupported texture format";
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        return "incomplete formats";
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        return "wrong dimensions";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        return "incomplete missing attachement";
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        return "no error";
    default:
        return "unknown error code";
    }
}

/**
 * \brief Adds an existing texture as a new render target for \p fb
 * \param fb the frame buffer to which to add the new texture
 * \param id render target's identifier (SCE_COLOR_BUFFERn, SCE_DEPTH_BUFFER
 * or SCE_STENCIL_BUFFER)
 * \param target used only for cubemaps, determines the face
 * of the cubemap on which to make the render, can be 0
 * \param tex the texture that is the target of \p id. must be a 2D texture
 * or a cubemap
 * \param mipmap mipmap level on which to make the render (0 is recommanded)
 * \param canfree boolean which indicates if the frame buffer \p fb has right
 * to delete \p tex
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * Two or more calls of this function under one frame buffer will use
 * the Multiple Render Targets OpenGL's extension.
 * After adding a texture as a color buffer, it is recommanded to
 * call SCE_CAddRenderBuffer() to add a depth buffer.
 * This function will adapts the \p fb's viewport to the dimensions of \p tex.
 * \sa SCE_CAddRenderBuffer()
 */
int SCE_CAddRenderTexture (SCE_CFramebuffer *fb, SCEuint id, SCEenum target,
                           SCE_CTexture *tex, int mipmap, int canfree)
{
    SCE_CDEFAULTFUNCR (int,
                       SCE_CAddRenderTexture_(id, target, tex, mipmap, canfree))
}
int SCE_CAddRenderTexture_ (SCEuint id, SCEenum target, SCE_CTexture *tex,
                            int mipmap, int canfree /*, int layer*/)
{
    int type, status;

    /* TODO: verifier le nombre de render targets encore autorises et veiller
       a ne pas depasser SCE_CGetMaxAttachmentBuffers() */

    SCE_btstart ();
    type = SCE_CIDToGLBuffer (id);

    /* si la precedente texture n'a pas ete envoyee par l'utilisateur,
       on la supprime */
    if (!binded->buffers[id].user)
        SCE_CDeleteTexture (binded->buffers[id].tex);

    /* si un buffer existe deja, on le supprime */
    if (binded->buffers[id].id)
    {
        glDeleteRenderbuffersEXT (1, &binded->buffers[id].id);
        binded->buffers[id].id = 0;
    }

    binded->buffers[id].user = !canfree;
    binded->buffers[id].tex = tex;

    /* verification du target */
    if (target < SCE_TEX_POSX || target > SCE_TEX_NEGZ)
        target = SCE_CGetTextureTarget (tex);
    else if (tex->target != SCE_TEX_CUBE)
    {
        /* target designant une face de cubemap, mais la texture
           n'est pas de type cubemap : erreur */
        Logger_Log (SCE_INVALID_ARG);
        Logger_LogMsg ("invalid target, the texture is not a cubemap");
        SCE_btend ();
        return SCE_ERROR;
    }

    if (!SCE_CIsTextureUsingMipmaps (tex))
    {
        SCE_CSetTextureParam (tex, GL_TEXTURE_MAX_LEVEL, 0);
        mipmap = SCE_FALSE; /* very important */
    }
    /* TODO: useless only if CTexture manager set some parameters */
    /*
    SCE_CSetTextureParam (tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    SCE_CSetTextureParam (tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SCE_CSetTextureParam (tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    SCE_CSetTextureParam (tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    */
    /* TODO: add texture arrays, texture rectangle & 3D textures managment */
#if 0
    if (target == SCE_TEX_RECTANGLE_NV)
    {

    }
    else if (target == SCE_TEX_2D_ARRAY || target == SCE_TEX_3D)
        /* SCE_CIsTextureArray (tex) */
    {
        glFramebufferTextureLayerEXT (GL_FRAMEBUFFER_EXT, type, tex->id,
                                      mipmap, layer);
    }
    else
#endif
    /* et on lui assigne notre jolie render texture */
    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, type,
                               target, tex->id, mipmap);
    /* si aucune color render texture n'existe, on desactive le tampon */
    if (id == SCE_DEPTH_BUFFER &&
        !binded->buffers[SCE_COLOR_BUFFER0].tex &&
        !binded->buffers[SCE_COLOR_BUFFER1].tex &&
        !binded->buffers[SCE_COLOR_BUFFER2].tex) /* etc. */
    {
        glDrawBuffer (GL_NONE);
    }
    glReadBuffer (GL_NONE);
    /* TODO: wtf iz dat */
    /*status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);*/
    status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        Logger_Log (status);
        Logger_LogMsg ("framebuffer check failed: %s",
                       SCE_CGetFramebufferError (status));
        SCE_btend ();
        return SCE_ERROR;
    }

    /* recuperation des dimensions */
    binded->w = SCE_CGetTextureWidth (tex, target, 0);
    binded->h = SCE_CGetTextureHeight (tex, target, 0);
    /* activation */
    binded->buffers[id].actived = SCE_TRUE;

    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief Adds a new render buffer
 * \param fb the frame buffer to which to add the render buffer
 * \param id render buffer's identifier, can be SCE_DEPTH_BUFFER
 * or SCE_STENCIL_BUFFER
 * \param fmt the format of the new render buffer, can be 0 or lesser
 * \param w width of the new render buffer
 * \param h height of the new render buffer
 * \note If you called SCE_CAddRenderTexture() previously, \p w
 * and \p h can be set to 0 then the dimensions of the render
 * buffer are automatically set to those of the texture passed to
 * SCE_CAddRenderTexture().
 * \note If you set \p fmt at 0 or lesser, an adapted format is used
 * automatically.
 * \sa SCE_CAddRenderTexture()
 */
int SCE_CAddRenderBuffer (SCE_CFramebuffer *fb, SCEuint id,
                          int fmt, int w, int h)
{
    SCE_CDEFAULTFUNCR (int, SCE_CAddRenderBuffer_ (id, fmt, w, h))
}
int SCE_CAddRenderBuffer_ (SCEuint id, int fmt, int w, int h)
{
    int type, status;

    SCE_btstart ();
    type = SCE_CIDToGLBuffer (id);

    /* assignation des valeurs par defaut */
    if (fmt <= 0)
    {
        if (id == SCE_DEPTH_BUFFER)
            fmt = GL_DEPTH_COMPONENT24;
        else if (id == SCE_STENCIL_BUFFER)
            fmt = GL_STENCIL_INDEX8_EXT;
    }

    if (w <= 0)
        w = binded->w;
    else
        binded->w = w;
    if (h <= 0)
        h = binded->h;
    else
        binded->h = h;

    /* si une texture existait deja, on la supprime */
    if (!binded->buffers[id].user)
    {
        SCE_CDeleteTexture (binded->buffers[id].tex);
        binded->buffers[id].tex = NULL;  /* ! important ! */
    }

    /* si le buffer n'existe pas, il faut le creer */
    if (!glIsRenderbufferEXT (binded->buffers[id].id))
        glGenRenderbuffersEXT (1, &binded->buffers[id].id);

    /* creation du render buffer */
    glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, binded->buffers[id].id);
    glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT, fmt, w, h);
    glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, 0);

    /* on l'ajoute au FBO */
    glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, type, GL_RENDERBUFFER_EXT,
                                  binded->buffers[id].id);

    status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        Logger_Log (status);
        Logger_LogMsg ("framebuffer check failed: %s",
                       SCE_CGetFramebufferError (status));
        SCE_btend ();
        return SCE_ERROR;
    }

    /* activation */
    binded->buffers[id].actived = SCE_TRUE;

    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief Creates a new render texture and add it as a new render target to the
 * given frame buffer
 * \param fb the frame buffer to which to add the new render texture
 * \param id render target's identifier (SCE_COLOR_BUFFERn, SCE_DEPTH_BUFFER
 * or SCE_STENCIL_BUFFER)
 * \param pxf internal pixel format of the new texture,
 * can be set to 0 or lesser
 * \param fmt format of the new texture, can be set to 0 or lesser
 * \param type data type of the new texture (SCE_UNSIGNED_BYTE, ...)
 * \param w width of the new texture
 * \param h height of the new texture
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * If \p pxf and/or \p fmt is set to 0 or lesser then an adapted value
 * is automatically set.
 * This function makes only a call of SCE_CAddRenderTexture() like this:
 * SCE_CAddRenderTexture (\p fb, \p id, SCE_TEX_2D, newtex, 0, SCE_TRUE)
 * where 'newtex' is the new texture created from the given informations.
 * \sa SCE_CAddRenderTexture()
 * \todo rename this function to 'AddNew'
 */
int SCE_CCreateRenderTexture (SCE_CFramebuffer *fb, SCEuint id,
                              int pxf, int fmt, int type, int w, int h)
{
    SCE_CDEFAULTFUNCR (int,SCE_CCreateRenderTexture_ (id, pxf, fmt, type, w, h))
}
int SCE_CCreateRenderTexture_ (SCEuint id, int pxf, int fmt,
                               int type, int w, int h)
{
    SCE_CTexture *tex = NULL;
    SCE_CTexData data;

    SCE_btstart ();
    /* creation de la texture */
    tex = SCE_CCreateTexture (SCE_TEX_2D);
    if (!tex)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }

    SCE_CInitTexData (&data);

    /* utilisation des valeurs par defaut en cas de non specification */
    if (pxf <= 0)
    {
        if (id == SCE_DEPTH_BUFFER)
            pxf = GL_DEPTH_COMPONENT24;
        else
            pxf = GL_RGBA;
    }
    if (fmt <= 0)
    {
        if (id == SCE_DEPTH_BUFFER)
            fmt = GL_DEPTH_COMPONENT;
        else
            fmt = GL_RGBA;
    }
    if (type <= 0)
        type = SCE_UNSIGNED_BYTE;

    /* assignation des donnees */
    data.w = w;
    data.h = h;
    data.type = type;
    data.pxf = pxf;
    data.fmt = fmt;

    SCE_CBindTexture (tex);
    if (SCE_CAddTextureTexDataDup_ (0, &data) < 0)
    {
        Logger_LogSrc ();
        SCE_CDeleteTexture (tex);
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_CBuildTexture_ (0, 0) < 0)
    {
        Logger_LogSrc ();
        SCE_CDeleteTexture (tex);
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_CBindTexture (NULL);

    /* ajout de la texture en tant que render buffer */
    SCE_CAddRenderTexture_ (id, SCE_TEX_2D, tex, 0, SCE_TRUE);

    SCE_btend ();
    return SCE_OK;
}

/**
 * \brief Gets one of the render textures of a frame buffer
 * \param fb the frame buffer to which get the texture
 * \param id the render target's identifier (SCE_COLOR_BUFFERn, SCE_DEPTH_BUFFER
 * or SCE_STENCIL_BUFFER)
 * \returns the render texture targeted by \p id
 */
SCE_CTexture* SCE_CGetRenderTexture (SCE_CFramebuffer *fb, SCEuint id)
{
    return fb->buffers[id].tex;
}
SCE_CTexture* SCE_CGetRenderTexture_ (SCEuint id)
{
    return binded->buffers[id].tex;
}


/**
 * \brief Activate a render buffer
 * \param id the render target's identifier
 * \param actived boolean which indicates the status of \p id
 */
void SCE_CActivateRenderBuffer (SCE_CFramebuffer *fb, int id, int actived)
{
    fb->buffers[id].actived = actived;
}
/**
 * \brief Equivalent to SCE_CActivateRenderBuffer (\p fb, \p id, SCE_TRUE)
 * \sa SCE_CActivateRenderBuffer() SCE_CDisableRenderBuffer()
 */
void SCE_CEnableRenderBuffer (SCE_CFramebuffer *fb, int id)
{
    fb->buffers[id].actived = SCE_TRUE;
}
/**
 * \brief Equivalent to SCE_CActivateRenderBuffer (\p fb, \p id, SCE_FALSE)
 * \sa SCE_CActivateRenderBuffer() SCE_CEnableRenderBuffer()
 */
void SCE_CDisableRenderBuffer (SCE_CFramebuffer *fb, int id)
{
    fb->buffers[id].actived = SCE_FALSE;
}

/**
 * \brief Gets the maximum buffers supported for multiple render targets
 */
unsigned int SCE_CGetMaxAttachmentBuffers (void)
{
    return max_attachement_buffers;
}

static void SCE_CSetDrawBuffers (SCE_CFramebuffer *fb)
{
    unsigned int i, j = 0;
    SCEenum drawids[SCE_MAX_ATTACHMENT_BUFFERS] = {0};

    for (i=0; i<max_attachement_buffers; i++)
    {
        if (fb->buffers[i].actived)
            drawids[j++] = GL_COLOR_ATTACHMENT0_EXT + i;
    }
    glDrawBuffers (j, drawids);
}

/**
 * \brief Using a frame buffer instead of the default OpenGL's render buffer.
 * If \p fb is not NULL, then \p fb is the target of all the further renders,
 * otherwise the default OpenGL's render buffer is positioned back
 * \param fb the frame buffer on make the further renders,
 * set as NULL to set up the default OpenGL's render buffer
 * \param r gives the new viewport for the renders, can be NULL
 *
 * Calling this function when none frame buffer is used saves the viewport,
 * then the first call of this function with a null frame buffer will restore
 * the viewport.
 */
void SCE_CUseFramebuffer (SCE_CFramebuffer *fb, SCE_SIntRect *r)
{
    static int viewport[4];         /* viewport avant l'utilisation du fbo */
    static int binded_ = SCE_FALSE; /* booleen, true: un fbo est deja binde */

    if (fb)
    {
        int *p = SCE_Rectangle_GetBottomLeftPoint (r);
        /* on recupere d'abord le viewport, pour ensuite le restituer
           (uniquement si aucun fbo n'est deja binde,
            sinon on prendrait son viewport) */
        if (!binded_)
            glGetIntegerv (GL_VIEWPORT, viewport);

        glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fb->id);
        if (r)
            glViewport (p[0], p[1], SCE_Rectangle_GetWidth (r),
                        SCE_Rectangle_GetHeight (r));
        else
            glViewport (fb->x, fb->y, fb->w, fb->h);

        if (SCE_CHasCap (SCE_MRT))
            SCE_CSetDrawBuffers (fb);

        binded_ = SCE_TRUE;
    }
    else if (binded_)
    {
        glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
        glViewport (viewport[0], viewport[1], viewport[2], viewport[3]);
        binded_ = SCE_FALSE;
    }
}

/** @} */
