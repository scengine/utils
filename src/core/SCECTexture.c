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
 
/* created: 14/01/2007
   updated: 29/01/2009 */

#include <string.h>

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEMath.h>
#include <SCE/utils/SCEResource.h>

#include <SCE/core/SCECSupport.h>
#include <SCE/core/SCECTexture.h>

/**
 * \file SCECTexture.c
 * \copydoc coretexture
 * 
 * \file SCECTexture.h
 * \copydoc coretexture
 */

/**
 * \defgroup coretexture Core textures managment
 * \ingroup core
 * \internal
 * \brief Functions that gives a full support of textures managment
 */

/** @{ */

static int resource_type = 0;

#if 0
static unsigned int nbatchs = 0;

unsigned int SCE_CGetTextureNumBatchs (void)
{
    unsigned int n = nbatchs;
    nbatchs = 0;
    return n;
}
#endif

/* booleen: true = reduction des images lors d'une redimension automatique */
static int sce_tex_reduce = SCE_TRUE;

static int texsub = 0;  /* indique glTexImage (0) ou glTexSubImage (1) */

/* texture actuellement bindee */
static SCE_CTexture *bound = NULL;

/* stocke les textures utilisees (via Use) */
static SCE_CTexture **texused = NULL;

static int max_tex_units = 0; /* nombre maximum d'unites de texture */

static int max_dimensions = 64; /* dimensions maximales des textures 2D et 1D */
static int max_cube_dimensions = 16;
static int max_3d_dimensions = 16;
static int max_mipmap_level = 1; /* niveau de mipmap maximum par defaut */

/* force certaines valeurs lors d'un appel a AddTexData() */
static int force_pxf = SCE_FALSE, forced_pxf;
static int force_type = SCE_FALSE, forced_type;
static int force_fmt = SCE_FALSE, forced_fmt;


/* nombre de textures utilisees de chaque type */
static int n_textype[4];

static void* SCE_CLoadTextureResource (const char*, int, void*);

/**
 * \internal
 * \brief Initializes the textures manager
 */
int SCE_CTextureInit (void)
{
    size_t i;

    /* recuperation du nombre maximum d'unites de texture */
    SCE_CGetIntegerv (GL_MAX_TEXTURE_UNITS, &max_tex_units);
    /* recuperation de la taille maximale d'une texture */
    SCE_CGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_dimensions);
    SCE_CGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE, &max_cube_dimensions);
    SCE_CGetIntegerv (GL_MAX_3D_TEXTURE_SIZE, &max_3d_dimensions);
    /* ...et assignation au mipmap maximum */
    max_mipmap_level = SCE_Math_PowerOfTwo (max_dimensions);

    /* creation du tableau de stockage des textures en cours d'utilisation */
    texused = SCE_malloc (max_tex_units * sizeof *texused);
    if (!texused)
        goto fail;
    for (i = 0; i < max_tex_units; i++)
        texused[i] = NULL;
    resource_type = SCE_Resource_RegisterType (SCE_FALSE,
                                               SCE_CLoadTextureResource, NULL);
    if (resource_type < 0)
        goto fail;

    SCE_btend ();
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("failed to initialize textures manager");
    return SCE_ERROR;
}

/**
 * \internal
 * \brief Quits the textures manager
 */
void SCE_CTextureQuit (void)
{
    SCE_free (texused);
    texused = NULL;
}


int SCE_CGetTextureResourceType (void)
{
    return resource_type;
}


/**
 * \deprecated
 * \todo remove it
 */
void SCE_CBindTexture (SCE_CTexture *tex)
{
    bound = tex;
    if (bound)
        glBindTexture (bound->target, bound->id);
}


/**
 * \brief Initializes a texture data structure
 */
void SCE_CInitTexData (SCE_CTexData *d)
{
    d->img = NULL;
    d->canfree = SCE_FALSE; /* par defaut on ne supprime rien */
    d->user = SCE_TRUE;     /* idem */
    d->target = SCE_TEX_1D;
    d->level = 0;
    d->w = d->h = d->d = 1;
    d->pxf = d->fmt = GL_RGBA;
    d->type = SCE_UNSIGNED_BYTE;
    d->data_size = 0;
    d->data_user = SCE_TRUE;
    d->data = NULL;
    d->comp = SCE_FALSE;
}

/**
 * \brief Creates a texture data structure
 * \returns the new texture data
 */
SCE_CTexData* SCE_CCreateTexData (void)
{
    SCE_CTexData *d = NULL;
    SCE_btstart ();
    d = SCE_malloc (sizeof *d);
    if (!d)
        SCEE_LogSrc ();
    else
        SCE_CInitTexData (d);
    SCE_btend ();
    return d;
}

/**
 * \brief Creates a texture data from the bound SCE image
 * \returns the new texture data
 * \sa SCE_CImage
 */
SCE_CTexData* SCE_CCreateTexDataFromImage (void)
{
    SCE_CTexData *d = NULL;

    SCE_btstart ();
    d = SCE_malloc (sizeof *d);
    if (!d)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    d->data_size = SCE_CGetImageDataSize_ ();
    d->data_user = SCE_FALSE;
    d->data = SCE_malloc (d->data_size);
    if (!d->data)
    {
        SCE_free (d);
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }
    memcpy (d->data, SCE_CGetImageData_ (), d->data_size);
    d->img = SCE_CGetImageBound ();
    d->target = SCE_CGetImageType_ ();
    d->level = SCE_CGetImageMipmapLevel_ ();
    d->w = SCE_CGetImageWidth_ ();
    d->h = SCE_CGetImageHeight_ ();
    d->d = SCE_CGetImageDepth_ ();
    d->pxf = SCE_CGetImagePixelFormat_ ();
    d->fmt = SCE_CGetImageFormat_ ();
    d->type = SCE_CGetImageDataType_ ();
    d->comp = SCE_CGetImageIsCompressed_ ();

    SCE_btend ();
    return d;
}

/**
 * \brief Deletes a texture data
 */
void SCE_CDeleteTexData (void *data)
{
    SCE_btstart ();
    if (data)
    {
        SCE_CTexData *d = data;
        if (!d->data_user)
            SCE_free (d->data);
        if (d->canfree && SCE_Resource_Free (d->img))
            SCE_CDeleteImage (d->img);
        SCE_free (d);
    }
    SCE_btend ();
}
static void SCE_CDeleteTexData_ (void *data)
{
    SCE_btstart ();
    if (data)
    {
        SCE_CTexData *d = data;
        if (!d->user)
        {
            SCE_free (d->data);
            if (d->canfree && SCE_Resource_Free (d->img))
                SCE_CDeleteImage (d->img);
            SCE_free (d);
        }
    }
    SCE_btend ();
}

/**
 * \brief Duplicates a texture data
 * \param d the texture data to copy
 * \returns the copy of \p d
 */
SCE_CTexData* SCE_CDupTexData (SCE_CTexData *d)
{
    SCE_CTexData *data = NULL;

    SCE_btstart ();
    data = SCE_malloc (sizeof *data);
    if (!data)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    *data = *d;
    if (data->data_size > 0)
    {
        data->data = SCE_malloc (data->data_size);
        if (!data->data)
        {
            SCE_free (data);
            SCEE_LogSrc ();
            SCE_btend ();
            return NULL;
        }
        memcpy (data->data, d->data, data->data_size);
    }
    SCE_btend ();
    return data;
}



static void SCE_CInitTexture (SCE_CTexture *tex)
{
    unsigned int i;
    tex->id = 0;
    tex->target = 0;
    tex->have_data = SCE_FALSE;
    tex->use_mipmap = tex->hw_mipmap = SCE_FALSE;
    for (i=0; i<6; i++)
        tex->data[i] = NULL;
}

/**
 * \brief Creates a core texture
 * \param target the type of the texture to create (can set SCE_TEX_1D,
 * SCE_TEX_2D, SCE_TEX_3D or SCE_TEX_CUBE)
 * \note If \p target is a non valid target, then... what ?
 * \todo que fait cette fonction quand \p target est non valide ?
 */
SCE_CTexture* SCE_CCreateTexture (SCEenum target)
{
    unsigned int i;
    SCE_CTexture *tex = NULL;

    SCE_btstart ();
    tex = SCE_malloc (sizeof *tex);
    if (!tex)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    SCE_CInitTexture (tex);

    for (i=0; i<6; i++)
    {
        tex->data[i] = SCE_List_Create (SCE_CDeleteTexData_);
        if (!tex->data[i])
        {
            SCE_CDeleteTexture (tex);
            SCEE_LogSrc ();
            SCE_btend ();
            return NULL;
        }
    }

    tex->target = target;
    /* assignation du type */
    switch (target)
    {
    case SCE_TEX_1D: tex->type = SCE_TEXTYPE_1D; break;
    case SCE_TEX_2D: tex->type = SCE_TEXTYPE_2D; break;
    case SCE_TEX_3D: tex->type = SCE_TEXTYPE_3D; break;
    case SCE_TEX_CUBE: tex->type = SCE_TEXTYPE_CUBE;
    }

    glGenTextures (1, &tex->id);

    SCE_btend ();
    return tex;
}


/* ... */
#define SCE_CDEFAULTFUNC(action)\
SCE_CTexture *back = bound;\
SCE_CBindTexture (tex);\
action;\
SCE_CBindTexture (back);

#define SCE_CDEFAULTFUNCR(t, action)\
t r;\
SCE_CTexture *back = bound;\
SCE_CBindTexture (tex);\
r = action;\
SCE_CBindTexture (back);\
return r;


/**
 * \brief Deletes a core texture
 * \param tex the texture to delete
 */
void SCE_CDeleteTexture (SCE_CTexture *tex)
{
    SCE_btstart ();
    if (tex)
    {
        unsigned int i;
        for (i=0; i<6; i++)
            SCE_List_Delete (tex->data[i]);
        glDeleteTextures (1, &tex->id);
        SCE_free (tex);
    }
    SCE_btend ();
}
void SCE_CDeleteTexture_ (void)
{
    SCE_CDeleteTexture (bound);
    bound = NULL;
}


/**
 * \brief Gets the number of texture units available
 * \returns the number of texture units
 */
int SCE_CGetMaxTextureUnits (void)
{
    return max_tex_units;
}
/**
 * \brief Gets the maximum textures size
 */
int SCE_CGetMaxTextureSize (void)
{
    return max_dimensions;
}
/**
 * \brief Gets the maximum textures cubemap size
 */
int SCE_CGetMaxTextureCubeSize (void)
{
    return max_cube_dimensions;
}
/**
 * \brief Gets the maximum 3D textures size
 */
int SCE_CGetMaxTexture3DSize (void)
{
    return max_3d_dimensions;
}


/**
 * \brief Sets the filter of a texture when is far
 * \param level is the level of filtering, can set SCE_TEX_NEAREST,
 * SCE_TEX_LINEAR, SCE_TEX_BILINEAR, SCE_TEX_TRILINEAR.
 *
 * \sa SCE_Texture_SetFilter() SCE_CPixelizeTexture()
 */
void SCE_CSetTextureFilter (SCE_CTexture *tex, SCEint level)
{
    SCE_CDEFAULTFUNC (SCE_CSetTextureFilter_ (level))
}
void SCE_CSetTextureFilter_ (SCEint filter)
{
    SCE_CSetTextureParam_ (GL_TEXTURE_MIN_FILTER, filter);
}

/**
 * \brief Defines if a texture is pixelized when is near
 * \param p can be SCE_TRUE or SCE_FALSE
 *
 * \sa SCE_Texture_Pixelize() SCE_CSetTextureFilter()
 */
void SCE_CPixelizeTexture (SCE_CTexture *tex, int p)
{
    SCE_CDEFAULTFUNC (SCE_CPixelizeTexture_ (p))
}
void SCE_CPixelizeTexture_ (int p)
{
    SCE_CSetTextureParam_ (GL_TEXTURE_MAG_FILTER, (p ? GL_NEAREST : GL_LINEAR));
}


/**
 * \brief Sets a parameter to a core texture
 * \param tex the texture to assign the parameter
 * \param pname the type of the parameter
 * \param param the value of the parameter
 *
 * This function calls glTexParameteri()
 *
 * \sa SCE_CSetTextureParamf()
 */
void SCE_CSetTextureParam (SCE_CTexture *tex, SCEenum pname, SCEint param)
{
    SCE_CDEFAULTFUNC (SCE_CSetTextureParam_ (pname, param))
}
void SCE_CSetTextureParam_ (SCEenum pname, SCEint param)
{
    glTexParameteri (bound->target, pname, param);
}

/**
 * \brief Sets a parameter to a core texture
 * \param tex the texture to assign the parameter
 * \param pname the type of the parameter
 * \param param the value of the parameter
 *
 * This function calls glTexParameterf
 *
 * \sa SCE_CSetTextureParam()
 */
void SCE_CSetTextureParamf (SCE_CTexture *tex, SCEenum pname, SCEfloat param)
{
    SCE_CDEFAULTFUNC (SCE_CSetTextureParamf_ (pname, param))
}
void SCE_CSetTextureParamf_ (SCEenum pname, SCEfloat param)
{
    glTexParameterf (bound->target, pname, param);
}

/**
 * \brief Sets the parameters of texture coordinates generation
 * 
 * This function calls glTexGeni(\p a, \p b, \p c). See the documentation of
 * this GL function for more details :D
 *
 * \sa SCE_CSetTextureGenf() SCE_CSetTextureGenfv()
 */
void SCE_CSetTextureGen (SCE_CTexture *tex, SCEenum a, SCEenum b, int c)
{
    SCE_CDEFAULTFUNC (SCE_CSetTextureGen_ (a, b, c))
}
void SCE_CSetTextureGen_ (SCEenum a, SCEenum b, int c)
{
    glTexGeni (a, b, c);
}

/**
 * \brief Sets the parameters of texture coordinates generation
 * 
 * This function calls glTexGenf(\p a, \p b, \p c). See the documentation of
 * this GL function for more details :D
 *
 * \sa SCE_CSetTextureGen() SCE_CSetTextureGenfv()
 */
void SCE_CSetTextureGenf (SCE_CTexture *tex, SCEenum a, SCEenum b, float c)
{
    SCE_CDEFAULTFUNC (SCE_CSetTextureGenf_ (a, b, c))
}
void SCE_CSetTextureGenf_ (SCEenum a, SCEenum b, float c)
{
    glTexGenf (a, b, c);
}

/**
 * \brief Sets the parameters of texture coordinates generation
 * 
 * This function calls glTexGenfv(\p a, \p b, \p c). See the documentation of
 * this GL function for more details :D
 *
 * \sa SCE_CSetTextureGen() SCE_CSetTextureGenf()
 */
void SCE_CSetTextureGenfv (SCE_CTexture *tex, SCEenum a, SCEenum b, float *c)
{
    SCE_CDEFAULTFUNC (SCE_CSetTextureGenfv_ (a, b, c))
}
void SCE_CSetTextureGenfv_ (SCEenum a, SCEenum b, float *c)
{
    glTexGenfv (a, b, c);
}

/**
 * \brief Forces the pixel format when calling SCE_CAddTextureTexData()
 * \param force do we force the pixel format ?
 * \param pxf forced pixel format
 */
void SCE_CForceTexturePixelFormat (int force, int pxf)
{
    force_pxf = force;
    forced_pxf = pxf;
}
/**
 * \brief Forces the type when calling SCE_CAddTextureTexData()
 * \param force do we force the type ?
 * \param type forced type
 */
void SCE_CForceTextureType (int force, int type)
{
    force_type = force;
    forced_type = type;
}
/**
 * \brief Forces the format when calling SCE_CAddTextureTexData()
 * \param force do we force the format ?
 * \param fmt forced format
 */
void SCE_CForceTextureFormat (int force, int fmt)
{
    force_fmt = force;
    forced_fmt = fmt;
}


/**
 * \brief Gets the type of a core texture
 * \returns the type of \p tex
 */
SCEenum SCE_CGetTextureTarget (SCE_CTexture *tex)
{
    return tex->target;
}
SCEenum SCE_CGetTextureTarget_ (void)
{
    return bound->target;
}


/* retourne un identifiant valide pour le tableau des donnees
   et assigne a 'target' la valeur adequat */
static unsigned int SCE_CGetTextureTargetID (SCE_CTexture *tex, int *target)
{
    unsigned int i = 0;
    if (*target >= SCE_TEX_POSX && *target <= SCE_TEX_NEGZ &&
        tex->target == SCE_TEX_CUBE)
    {
        i = *target - SCE_TEX_POSX;
    }
    else
        *target = tex->target;
    return i;
}


/**
 * \brief Gets the texture's data of the specified target and mipmap level
 * \param target target of the texture, used only for cubemaps, determines
 * the cube face, can be 0
 * \param level mipmap level of the asked data
 * \returns the texture's data corresponding to the given parameters
 * \sa SCE_CTexData
 */
SCE_CTexData* SCE_CGetTextureTexData (SCE_CTexture *tex, int target, int level)
{
    unsigned int i;
    SCE_CTexData *data = NULL;

    SCE_btstart ();
    i = SCE_CGetTextureTargetID (tex, &target);
    if (level > SCE_List_GetSize (tex->data[i]))
        data = SCE_List_GetData (SCE_List_GetLast (tex->data[i]));
    else
        data = SCE_List_GetData (SCE_List_GetIterator (tex->data[i], level));
    SCE_btend ();
    return data;
}
SCE_CTexData* SCE_CGetTextureTexData_ (int target, int level)
{
    return SCE_CGetTextureTexData (bound, target, level);
}

/**
 * \brief Indicates if a core texture has any data
 */
int SCE_CHasTextureData (SCE_CTexture *tex)
{
    return tex->have_data;
}

/**
 * \brief Indicates if a texture is using mipmapping
 */
int SCE_CIsTextureUsingMipmaps (SCE_CTexture *tex)
{
    return tex->use_mipmap;
}
int SCE_CIsTextureUsingMipmaps_ (void)
{
    return bound->use_mipmap;
}

/**
 * \brief Gets the number of mipmap levels of \p tex
 * \param target used only for cubemaps, determines the cube face, can be 0
 * \returns the number of mipmap levels of \p tex
 */
int SCE_CGetTextureNumMipmaps (SCE_CTexture *tex, int target)
{
    unsigned int i;
    SCE_btstart ();
    i = SCE_CGetTextureTargetID (tex, &target);
    SCE_btend ();
    return SCE_List_GetSize (tex->data[i]);
}
int SCE_CGetTextureNumMipmaps_ (int target)
{
    return SCE_CGetTextureNumMipmaps (bound, target);
}

/**
 * \brief Gets the width of a texture
 *
 * This function returns SCE_CGetTextureTexData (\p tex, \p target, \p level)->w
 */
int SCE_CGetTextureWidth (SCE_CTexture *tex, int target, int level)
{
    return SCE_CGetTextureTexData (tex, target, level)->w;
}
/**
 * \brief Gets the height of a texture
 *
 * This function returns SCE_CGetTextureTexData (\p tex, \p target, \p level)->h
 */
int SCE_CGetTextureHeight (SCE_CTexture *tex, int target, int level)
{
    return SCE_CGetTextureTexData (tex, target, level)->h;
}


/**
 * \brief Gets hardware compatible size from anything texture's size
 * \param min set as SCE_TRUE, the returned value is lesser than \p s,
 * greather otherwise
 * \param s the initial size
 * \returns hardware compatible size that is the most closer of \p s
 */
int SCE_CGetTextureValidSize (int min, int s)
{
    if (s > max_dimensions)
        return max_dimensions;
    if (!SCE_CHasCap (SCE_TEX_NON_POWER_OF_TWO))
    {
        int tmp = 1;
        while (tmp < s)
            tmp *= 2;
        if (min)
            tmp /= 2;
        s = tmp;
    }
    return s;
}


/**
 * \brief Resize an image with hardware compatibles dimentions
 * \param img the image to resize
 * \param w the new width (0 or lesser keep the current value)
 * \param h the new height (0 or lesser keep the current value)
 * \param d the new depth (0 or lesser keep the current value)
 *
 * Calls SCE_CResizeImage() over \p img after the check of \p w, \p h and \p d
 * with SCE_CGetTextureValidSize().
 */
void SCE_CResizeTextureImage (SCE_CImage *img, int w, int h, int d)
{
    SCE_CBindImage (img);
    SCE_CResizeTextureImage_ (w, h, d);
    SCE_CBindImage (NULL);
}
void SCE_CResizeTextureImage_ (int w, int h, int d)
{
    SCE_btstart ();
    /* on verifie, pour chaque composante, si elle a ete specifiee */
    if (w <= 0)
        w = SCE_CGetImageWidth_ ();
    if (h <= 0)
        h = SCE_CGetImageHeight_ ();
    if (d <= 0)
        d = SCE_CGetImageDepth_ ();
    /* recuperation de tailles valides */
    /* -> ici SCE_TRUE indique qu'on recupere la dimension inferieur */
    w = SCE_CGetTextureValidSize (sce_tex_reduce, w);
    h = SCE_CGetTextureValidSize (sce_tex_reduce, h);
    d = SCE_CGetTextureValidSize (sce_tex_reduce, d);
    /* mise a l'echelle (uniquement si besoin est) */
    if (w != SCE_CGetImageWidth_ () || h != SCE_CGetImageHeight_ () ||
        d != SCE_CGetImageDepth_ ())
        SCE_CResizeImage_ (w, h, d);
    SCE_btend ();
}


/**
 * \brief Adds an image to a texture
 * \param tex the texture where add the image
 * \param target the target where bind the image
 * \param img the image to add
 * \param canfree can \p tex's deletion deletes \p img ?
 *
 * Add an image to a texture and bind it as \p target, this parameter can be 0
 * then is automatically set, or can be SCE_TEX_nD for n dimension texture or
 * SCE_TEX_POSX + n where n is the cube face of the cubemap (requires that
 * \p tex is a cubemap). \p img is automatically resized to hardware compatibles
 * dimentions.
 */
int SCE_CAddTextureImage (SCE_CTexture *tex, int target,
                          SCE_CImage *img, int canfree)
{
    SCE_CTexData *d = NULL;
    unsigned int i, j, n_mipmaps;
    int old_level;

    SCE_btstart ();
    /* verification du target */
    i = SCE_CGetTextureTargetID (tex, &target);

    SCE_CBindImage (img);
    old_level = SCE_CGetImageMipmapLevel_ ();

    /* verification des dimensions de l'image */
    SCE_CResizeTextureImage_ (0, 0, 0);
#if 0
    /* log de l'operation */
    SCEE_SendMsg ("your hardware doesn't support non-power of two "
                  "textures.\n%s of '%s' forced.\nnew dimentions: "
                  "%d*%d\n", sce_img_reduce ? "shrinkage":"extention",
                  fname, w, h);
#endif

    /**
     * \todo fucking hack de merde, le mipmapping DDS chie "un peu"
     *       avec DevIL, le nombre de mipmap semble changer...
     */
    n_mipmaps = /*SCE_CGetImageNumMipmaps_ ()*/ 1;
    /* assignation des donnees de l'image */
    for (j=0; j<n_mipmaps; j++)
    {
        SCE_CSetImageMipmapLevel_ (j);
        /* creation des donnees a partir du niveau de mipmap j de l'image img */
        d = SCE_CCreateTexDataFromImage ();
        if (!d)
        {
            SCE_CBindImage (NULL);
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        d->target = target;  /* assignation du target */
        d->canfree = canfree;
        d->user = SCE_FALSE;

        /* ajout des donnees a la texture */
        if (SCE_List_AppendNewl (tex->data[i], d) < 0)
        {
            SCE_CBindImage (NULL);
            SCE_CDeleteTexData (d);
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        d = NULL;
    }

    SCE_CSetImageMipmapLevel_ (old_level);
    SCE_CBindImage (NULL);
    tex->have_data = SCE_TRUE;

    SCE_btend ();
    return SCE_OK;
}
int SCE_CAddTextureImage_ (int target, SCE_CImage *img, int canfree)
{
    return SCE_CAddTextureImage (bound, target, img, canfree);
}


/**
 * \brief Adds a new texture data to a texture
 * \param target the target where bind the new data
 * \param d the new data
 * \param canfree defines if \p d can be deleted on \p tex deletion
 *
 * If this function fails, then the forced values will not set.
 * 
 * \sa SCE_CAddTextureTexDataDup() SCE_CTexData
 */
int SCE_CAddTextureTexData (SCE_CTexture *tex, int target,
                            SCE_CTexData *d, int canfree)
{
    unsigned int i;

    SCE_btstart ();
    i = SCE_CGetTextureTargetID (tex, &target);
    d->target = target;
    d->user = !canfree;
    if (SCE_List_AppendNewl (tex->data[i], d) < 0)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    tex->have_data = SCE_TRUE;
    /* on place les valeurs forcees (si elles le sont) */
    if (force_pxf) d->pxf = forced_pxf;
    if (force_type) d->type = forced_type;
    if (force_fmt) d->fmt = forced_fmt;
    SCE_btend ();
    return SCE_OK;
}
int SCE_CAddTextureTexData_ (int target, SCE_CTexData *d, int canfree)
{
    return SCE_CAddTextureTexData (bound, target, d, canfree);
}

/**
 * \brief Duplicates and adds a new texture data to a texture
 *
 * This function works like SCE_CAddTextureTexData() except that duplicates
 * \p d before adding.
 *
 * \sa SCE_CAddTextureTexData() SCE_CTexData
 */
int SCE_CAddTextureTexDataDup (SCE_CTexture *tex, int target, SCE_CTexData *d)
{
    SCE_CTexData *data = NULL;

    SCE_btstart ();
    data = SCE_CDupTexData (d);
    if (!data)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_btend ();
    return SCE_CAddTextureTexData (tex, target, data, SCE_TRUE);
}
int SCE_CAddTextureTexDataDup_ (int target, SCE_CTexData *d)
{
    return SCE_CAddTextureTexDataDup (bound, target, d);
}

/**
 * \brief 
 * \param 
 */
SCE_CImage* SCE_CRemoveTextureImage (SCE_CTexture *tex, int target, int level)
{
    SCE_CImage *img = NULL;
    SCE_CTexData *d = NULL;
    SCE_SListIterator *it = NULL;
    unsigned int i;

    SCE_btstart ();
    i = SCE_CGetTextureTargetID (tex, &target);

    /* si level n'existe pas, on prend le dernier */
    /* >= parce que level commence a 0 */
    if (level >= SCE_List_GetSize (tex->data[i]))
        it = SCE_List_GetLast (tex->data[i]);
    else
        it = SCE_List_GetIterator (tex->data[i], level);

    d = SCE_List_GetData (it);
    img = d->img;
    d->canfree = SCE_FALSE;

    if (img)
    {
        SCE_List_ForEach (it, tex->data[i])
        {
            d = SCE_List_GetData (it);
            if (d->img == img)
                d->img = NULL;
        }
    }

    SCE_btend ();
    return img;
}
SCE_CImage* SCE_CRemoveTextureImage_ (int target, int level)
{
    return SCE_CRemoveTextureImage (bound, target, level);
}

/**
 * \brief 
 * \param 
 */
void SCE_CEraseTextureImage (SCE_CTexture *tex, int target, int level)
{
    SCE_CImage *img = NULL;
    SCE_btstart ();
    img = SCE_CRemoveTextureImage (tex, target, level);
    if (SCE_Resource_Free (img))
        SCE_CDeleteImage (img);
    SCE_btend ();
}
void SCE_CEraseTextureImage_ (int target, int level)
{
    SCE_CEraseTextureImage (bound, target, level);
}

/**
 * \brief 
 * \param 
 */
SCE_CTexData* SCE_CRemoveTextureTexData (SCE_CTexture *tex,
                                         int target, int level)
{
    SCE_CTexData *d = NULL, *data = NULL;
    SCE_SListIterator *pro = NULL, *it = NULL;
    unsigned int i;

    SCE_btstart ();
    i = SCE_CGetTextureTargetID (tex, &target);

    /* si level n'existe pas, on prend le dernier */
    if (level > SCE_List_GetSize (tex->data[i]))
        d = SCE_List_GetData (SCE_List_GetLast (tex->data[i]));
    else
        d = SCE_List_GetData (SCE_List_GetIterator (tex->data[i], level));

    d->user = SCE_TRUE;
    data = d;

    SCE_List_ForEachProtected (pro, it, tex->data[i])
    {
        d = SCE_List_GetData (it);
        if (d == data)
        {
            SCE_List_Removel (it);
            SCE_List_DeleteIt (it);
        }
    }

    SCE_btend ();
    return data;

}
SCE_CTexData* SCE_CRemoveTextureTexData_ (int target, int level)
{
    return SCE_CRemoveTextureTexData (bound, target, level);
}

/**
 * \brief 
 * \param 
 */
void SCE_CEraseTextureTexData (SCE_CTexture *tex, int target, int level)
{
    SCE_CTexData *d = NULL;
    SCE_btstart ();
    d = SCE_CRemoveTextureTexData (tex, target, level);
    /* les TexData ne sont pas des ressources, donc... */
    SCE_CDeleteTexData (d);
    SCE_btend ();
}
void SCE_CEraseTextureTexData_ (int target, int level)
{
    SCE_CEraseTextureTexData (bound, target, level);
}


typedef struct
{
    int type, w, h, d;
    const char **names;
} SCE_CTexResInfo;

static void* SCE_CLoadTextureResource (const char *name, int force, void *data)
{
    unsigned int i = 0, j;
    SCE_CImage *img = NULL;
    SCE_CTexture *tex = NULL;
    int resize;
    int type, w, h, d;
    SCE_CTexResInfo *rinfo = data;

    SCE_btstart ();
    (void)name;
    type = rinfo->type;
    w = rinfo->w; h = rinfo->h; d = rinfo->d;
    resize = (w > 0 || h > 0 || d > 0);

    /* si type est egal a SCE_TEX_CUBE, 6 const char* seront recuperes,
     * representant respectivement les 6 faces du cube. si des parametres
     * manquent, seul le premier sera construit lors de la construction.
     * sinon, chaque parametre representera un niveau de mipmap
     */

    if (force > 0)
        force--;
    for (j = 0; rinfo->names[j]; j++)
    {
        const char *fname = rinfo->names[j];
        img = SCE_Resource_Load (SCE_CGetImageResourceType(), fname,force,NULL);
        if (!img)
            goto fail;
        SCE_CBindImage (img);

        if (resize)
            SCE_CResizeTextureImage_ (w, h, d);

        if (!tex)
        {
            if (type <= 0)
                type = SCE_CGetImageType_ ();
            tex = SCE_CCreateTexture (type);
            if (!tex)
                goto fail;
        }

        if (SCE_CAddTextureImage (tex, (type == SCE_TEX_CUBE) ?
                                  SCE_TEX_POSX + i : 0, img, SCE_TRUE) < 0)
            goto fail;

        /* cubemap..? */
        if (type == SCE_TEX_CUBE)
            i++;
        else if (resize)
        {
            /* mipmapping */
            w = SCE_CGetImageWidth_ () / 2;
            h = SCE_CGetImageHeight_ () / 2;
            d = SCE_CGetImageDepth_ () / 2;
        }

        if (i == 6)             /* cube map completed */
            break;
    }

    SCE_CBindImage (NULL);
    SCE_btend ();
    return tex;
fail:
    SCE_CDeleteTexture (tex);
    SCEE_LogSrc ();
    SCE_btend ();
    return NULL;
}

/**
 * \brief 
 * \param force force a new texture to be loaded
 */
SCE_CTexture* SCE_CLoadTexturev (int type, int w, int h, int d, int force,
                                 const char **names)
{
    unsigned int i;
    char buf[2048] = {0};
    SCE_CTexResInfo info;
    SCE_CTexture *tex;

    info.type = type;
    info.w = w; info.h = h; info.d = d;
    info.names = names;
    for (i = 0; names[i]; i++)
        strcat (buf, names[i]);

    if (!(tex = SCE_Resource_Load (resource_type, buf, force, &info)))
        goto fail;

    return tex;
fail:
    SCEE_LogSrc ();
    return NULL;
}
/**
 * \brief 
 * \param 
 */
SCE_CTexture* SCE_CLoadTexture (int type, int w, int h, int d, int force, ...)
{
    va_list args;
    unsigned int i = 0;
    const char *name = NULL;
    const char *names[42];
    SCE_CTexture *tex = NULL;

    SCE_btstart ();
    va_start (args, force);
    name = va_arg (args, const char*);
    while (name && i < 42 - 1)
    {
        names[i] = name;
        name = va_arg (args, const char*);
        i++;
    }
    va_end (args);
    names[i] = NULL;
#ifdef SCE_DEBUG
    if (i == 0)
    {
        SCEE_Log (SCE_INVALID_ARG);
        SCEE_LogMsg ("excpected at least 1 file name, but 0 given");
        SCE_btend ();
        return NULL;
    }
#endif
    tex = SCE_CLoadTexturev (type, w, h, d, force, names);
    SCE_btend ();
    return tex;
}


/**
 * \brief 
 * \param 
 */
int SCE_CBuildTexture (SCE_CTexture *tex, int use_mipmap, int hw_mipmap)
{
    SCE_CDEFAULTFUNCR (int, SCE_CBuildTexture_ (use_mipmap, hw_mipmap))
}
/* ajoutees le 28/09/2007 */
typedef void (*SCE_CMakeTextureFunc)(SCE_CTexData*);
/* callbacks a prototype generique pour etre
   utilisees par un pointeur de fonction */
static void SCE_CMakeTexture1DComp (SCE_CTexData *d)
{
    glCompressedTexImage1D (d->target, d->level, d->pxf, d->w, 0,
                            d->data_size, d->data);
}
static void SCE_CMakeTexture2DComp (SCE_CTexData *d)
{
    glCompressedTexImage2D (d->target, d->level, d->pxf, d->w, d->h,
                            0, d->data_size, d->data);
}
static void SCE_CMakeTexture3DComp (SCE_CTexData *d)
{
    glCompressedTexImage3D (d->target, d->level, d->pxf, d->w, d->h, d->d,
                            0, d->data_size, d->data);
}
static void SCE_CMakeTexture1D (SCE_CTexData *d)
{
    glTexImage1D (d->target, d->level, d->pxf, d->w, 0, d->fmt, d->type, d->data);
}
static void SCE_CMakeTexture2D (SCE_CTexData *d)
{
    glTexImage2D (d->target, d->level, d->pxf, d->w, d->h,
                  0, d->fmt, d->type, d->data);
}
static void SCE_CMakeTexture3D (SCE_CTexData *d)
{
    glTexImage3D (d->target, d->level, d->pxf, d->w, d->h, d->d,
                  0, d->fmt, d->type, d->data);
}
/* fonctions de mise a jour */
static void SCE_CMakeTexture1DCompUp (SCE_CTexData *d)
{
    glCompressedTexSubImage1D (d->target, d->level, 0, d->w,
                               d->pxf, d->data_size, d->data);
}
static void SCE_CMakeTexture2DCompUp (SCE_CTexData *d)
{
    glCompressedTexSubImage2D (d->target, d->level, 0, 0, d->w, d->h, d->pxf,
                               d->data_size, d->data);
}
static void SCE_CMakeTexture3DCompUp (SCE_CTexData *d)
{
    glCompressedTexSubImage3D (d->target, d->level, 0, 0, 0, d->w, d->h,
                               d->d, d->pxf, d->data_size, d->data);
}
static void SCE_CMakeTexture1DUp (SCE_CTexData *d)
{
    glTexSubImage1D (d->target, d->level, 0, d->w, d->fmt, d->type, d->data);
}
static void SCE_CMakeTexture2DUp (SCE_CTexData *d)
{
    glTexSubImage2D (d->target, d->level, 0, 0, d->w, d->h, d->fmt,
                     d->type, d->data);
}
static void SCE_CMakeTexture3DUp (SCE_CTexData *d)
{
    glTexSubImage3D (d->target, d->level, 0, 0, 0, d->w, d->h, d->d, d->fmt,
                     d->type, d->data);
}
static void SCE_CMakeTexture1DMp (SCE_CTexData *d)
{
    int err = gluBuild1DMipmaps (d->target, d->pxf, d->w,
                                 d->fmt, d->type, d->data);
    if (err != 0)
    {
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("gluBuild1DMipmaps fails : %s\n", gluErrorString (err));
    }
}
static void SCE_CMakeTexture2DMp (SCE_CTexData *d)
{
    int err = gluBuild2DMipmaps (d->target, d->pxf, d->w, d->h,
                                 d->fmt, d->type, d->data);
    if (err != 0)
    {
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("gluBuild2DMipmaps fails : %s\n", gluErrorString (err));
    }
}
static void SCE_CMakeTexture3DMp (SCE_CTexData *d)
{
    int err;
    SCE_btstart ();
    err = gluBuild3DMipmaps (d->target, d->pxf, d->w, d->h, d->d,
                             d->fmt, d->type, d->data);
    if (err != 0)
    {
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("gluBuild3DMipmaps fails : %s\n", gluErrorString (err));
    }
    SCE_btend ();
}
/* determine quelle fonction utiliser pour le stockage des donnees */
static SCE_CMakeTextureFunc SCE_CGetMakeTextureFunc (int type, int comp, int mp)
{
    SCE_CMakeTextureFunc make = NULL;

    if (mp)
    {
        if (type == SCE_TEX_1D)
            make = SCE_CMakeTexture1DMp;
        else if (type == SCE_TEX_2D || type == SCE_TEX_CUBE)
            make = SCE_CMakeTexture2DMp;
        else if (type == SCE_TEX_3D)
            make = SCE_CMakeTexture3DMp;
    }
    else if (comp)
    {
        if (type == SCE_TEX_1D)
            make = (texsub ? SCE_CMakeTexture1DCompUp : SCE_CMakeTexture1DComp);
        else if (type == SCE_TEX_2D || type == SCE_TEX_CUBE)
            make = (texsub ? SCE_CMakeTexture2DCompUp : SCE_CMakeTexture2DComp);
        else if (type == SCE_TEX_3D)
            make = (texsub ? SCE_CMakeTexture3DCompUp : SCE_CMakeTexture3DComp);
    }
    else
    {
        if (type == SCE_TEX_1D)
            make = (texsub ? SCE_CMakeTexture1DUp : SCE_CMakeTexture1D);
        else if (type == SCE_TEX_2D || type == SCE_TEX_CUBE)
            make = (texsub ? SCE_CMakeTexture2DUp : SCE_CMakeTexture2D);
        else if (type == SCE_TEX_3D)
            make = (texsub ? SCE_CMakeTexture3DUp : SCE_CMakeTexture3D);
    }
    return make;
}
/* construit une texture avec les infos minimales */
static void SCE_CMakeTexture (SCEenum textype, SCE_SList *data,
                              SCEenum target, int use_mipmap)
{
    SCE_SListIterator *i = NULL;
    SCE_CTexData *d = NULL;
    SCE_CMakeTextureFunc make = NULL;
    int mp = SCE_FALSE;

    SCE_btstart ();
    /* si du mipmap est demande est qu'on en a en reserve,
       on ne va pas le generer via GLU */
    if (use_mipmap)
        mp = ((SCE_List_GetSize (data) > 1) ? SCE_FALSE : SCE_TRUE);

    SCE_List_ForEach (i, data)
    {
        d = SCE_List_GetData (i);
        make = SCE_CGetMakeTextureFunc (textype, d->comp, mp);
        /* si un target specifique a ete specifie */
        if (target != 0)
            d->target = target;
        make (d);
        if (!use_mipmap)
            break;
        /* TODO: penser a generer les niveaux non-existants dans 'data' */
    }
    SCE_btend ();
}
int SCE_CBuildTexture_ (int use_mipmap, int hw_mipmap)
{
    unsigned int i, n = 1;
    SCEenum t = bound->target;
    void (*make)(SCEenum, SCE_SList*, SCEenum, int) = SCE_CMakeTexture;

    SCE_btstart ();
    if (!bound->have_data)
    {
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("you must specify data before texture build");
        SCE_btend ();
        return SCE_ERROR;
    }

    /* si un parametre est non-specifie, on lui attribue la valeur
       deja presente dans la texture, sinon c'est l'inverse */
    if (use_mipmap < 0)
        use_mipmap = bound->use_mipmap;
    else
        bound->use_mipmap = use_mipmap;
    if (hw_mipmap < 0)
        hw_mipmap = bound->hw_mipmap;
    else
        bound->hw_mipmap = hw_mipmap;

    if (bound->target == SCE_TEX_CUBE)
        n = 6;

    if (use_mipmap)
    {
        if (hw_mipmap && SCE_CHasCap (SCE_TEX_HW_GEN_MIPMAP))
        {
            SCE_CSetTextureParam_ (GL_GENERATE_MIPMAP_SGIS, SCE_TRUE);
            for (i=0; i<n; i++)
                make (t, bound->data[i], ((n>1) ? SCE_TEX_POSX+i:0),SCE_FALSE);
        }
        else
        {
            if (hw_mipmap)
                SCEE_SendMsg ("hardware mipmap generation isn't supported");
            for (i=0; i<n; i++)
                make (t, bound->data[i], (n > 1 ? SCE_TEX_POSX+i:0), SCE_TRUE);
        }
        SCE_CSetTextureParam_ (GL_TEXTURE_MAX_LEVEL, max_mipmap_level);
        SCE_CSetTextureFilter_ (SCE_TEX_TRILINEAR);
    }
    else
    {
        for (i=0; i<n; i++)
            make (t, bound->data[i], (n > 1 ? SCE_TEX_POSX+i : 0), SCE_FALSE);
        SCE_CSetTextureParam_ (GL_TEXTURE_MAX_LEVEL, 0);
    }

    SCE_CPixelizeTexture_ (SCE_FALSE);
    texsub = SCE_FALSE;

    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief 
 * \param 
 */
int SCE_CUpdateTexture (SCE_CTexture *tex, int use_mipmap, int hw_mipmap)
{
    SCE_CDEFAULTFUNCR (int, SCE_CUpdateTexture_ (use_mipmap, hw_mipmap))
}
int SCE_CUpdateTexture_ (int use_mipmap, int hw_mipmap)
{
    texsub = SCE_TRUE;
    return SCE_CBuildTexture_ (use_mipmap, hw_mipmap);
}

void SCE_CSetActiveTextureUnit (unsigned int unit)
{
    glActiveTexture (SCE_TEX0 + unit);
}

/**
 * \todo l'organisation des deux prochaines fonctions pue peut-etre :
 *       je suppose qu'il y a un BindTexture a faire pour chaque unite
 *       de texturage, or c'est peut-etre faux... a voir.
 *       et y'a-t-il un glDisable/Enable a faire pour chaque unite ?
 */
static void SCE_CSetTextureUsed (SCE_CTexture *tex, int unit)
{
    if (tex)
    {
        glActiveTexture (SCE_TEX0 + unit);
        glEnable (tex->target);
        glBindTexture (tex->target, tex->id);
        n_textype[tex->type]++;
        texused[unit] = tex;
#if 0
        nbatchs++;
#endif
    }
    else if (texused[unit])
    {
        glActiveTexture (SCE_TEX0 + unit);
        glBindTexture (texused[unit]->target, 0);
        n_textype[texused[unit]->type]--;
        if (n_textype[texused[unit]->type] <= 0)
        {
            n_textype[texused[unit]->type] = 0;
            glDisable (texused[unit]->target);
        }
        texused[unit] = NULL;
    }
}
/**
 * \brief 
 * \param 
 * \todo reviser
 */
void SCE_CUseTexture (SCE_CTexture *tex, int unit)
{
    /* invalid texture unit */
    if (unit >= max_tex_units)
        return;
    else if (unit < 0)
    {
        glDisable (SCE_TEX_1D);
        glDisable (SCE_TEX_2D);
        glDisable (SCE_TEX_3D);
        glDisable (SCE_TEX_CUBE);

        for (unit = 0; unit < max_tex_units; unit++)
        {
            texused[unit] = NULL;
            glActiveTexture (SCE_TEX0 + unit);
            glBindTexture (SCE_TEX_1D, 0);
            glBindTexture (SCE_TEX_2D, 0);
            glBindTexture (SCE_TEX_3D, 0);
            glBindTexture (SCE_TEX_CUBE, 0);
        }
    }
    else
        SCE_CSetTextureUsed (tex, unit);
}

/** @} */
