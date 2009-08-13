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
 
/* created: 28/07/2007
   updated: 30/11/2008 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEString.h>
#include <SCE/utils/SCEResource.h>
#include <SCE/utils/SCEMedia.h>

#include <SCE/core/SCECSupport.h>
#include <SCE/core/SCECImage.h>

/**
 * \file SCECImage.c
 * \copydoc coreimage
 * 
 * \file SCECImage.h
 * \copydoc coreimage
 */

/**
 * \defgroup coreimage Images managment using DevIL library
 * \ingroup core
 * \internal
 * \brief Interfacing the DevIL's API and implement some features to be ready
 * for the core textures manager \c coretexture
 */

/** @{ */

static int resource_type = 0;
/*static SCEenum resize_filter = ILU_NEAREST;*/ /* unused */

static SCE_CImage *bound = NULL;

#define SCE_IMG_FORCE_PONCTUAL 1
#define SCE_IMG_FORCE_PERSISTENT 2

static int rescaleforced = SCE_FALSE;
static float scale_w, scale_h, scale_d;

static int resizeforced = SCE_FALSE;
static int size_w, size_h, size_d;


/**
 * \brief Initializes the images manager
 * \returns SCE_OK on success, SCE_ERROR on error
 */
int SCE_CImageInit (void)
{
    /* initialisation de DevIL */
    ilInit ();
    iluInit ();

    ilEnable (IL_ORIGIN_SET);
    ilOriginFunc (IL_ORIGIN_LOWER_LEFT);
    /* on sauvegarde les donnees compressees */
    ilSetInteger (IL_KEEP_DXTC_DATA, IL_TRUE);

    resource_type = SCE_Resource_RegisterType (SCE_TRUE, NULL, NULL);
    if (resource_type < 0)
        goto fail;
    if (SCE_Media_Register (resource_type,
                            ".bmp .gif .jpg .dds .png .tga .jpeg .ico .mn"
                            "g .pcx .rgb .rgba .tif", SCE_CLoadImage, NULL) < 0)
        goto fail;
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("failed to initialize images manager");
    return SCE_ERROR;
}

/**
 * \brief Quit the images manager
 */
void SCE_CImageQuit (void)
{
    ilShutDown ();
}


int SCE_CGetImageResourceType (void)
{
    return resource_type;
}


void SCE_CBindImage (SCE_CImage *img)
{
    SCE_btstart ();
    bound = img;
    if (bound)
    {
        ilBindImage (0);
        ilBindImage (bound->id);
        ilActiveMipmap (bound->level);
    }
    else
        ilBindImage (0);
    SCE_btend ();
}
SCE_CImage* SCE_CGetImageBound (void)
{
    return bound;
}


static void SCE_CInitImageData (SCE_CImageData *d)
{
    d->free_data = SCE_FALSE;
    d->data = NULL;
    d->pxf = IL_RGBA;
    d->updated = SCE_FALSE;
}

static SCE_CImageData* SCE_CCreateImageData (void)
{
    SCE_CImageData *d = NULL;

    SCE_btstart ();
    d = SCE_malloc (sizeof *d);
    if (!d)
        SCEE_LogSrc ();
    else
        SCE_CInitImageData (d);
    SCE_btend ();
    return d;
}

static void SCE_CDeleteImageData (void *data)
{
    SCE_btstart ();
    if (data)
    {
        SCE_CImageData *d = data;
        if (d->free_data)
            SCE_free (d->data);
        SCE_free (d);
    }
    SCE_btend ();
}


static void SCE_CInitImage (SCE_CImage *img)
{
    img->id = 0;
    img->mipmaps = NULL;
    img->level = 0;
    img->data = NULL;
}

/**
 * \brief Creates a new image
 * \returns a pointer to the new image structure
 */
SCE_CImage* SCE_CCreateImage (void)
{
    SCE_CImage *img = NULL;

    SCE_btstart ();
    img = SCE_malloc (sizeof *img);
    if (!img)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    SCE_CInitImage (img);
    img->mipmaps = SCE_List_Create (SCE_CDeleteImageData);
    if (!img->mipmaps)
    {
        SCE_free (img);
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    ilGenImages (1, &img->id);

    SCE_btend ();
    return img;
}

/**
 * \brief Deletes an image
 * \param img image to delete
 */
void SCE_CDeleteImage (SCE_CImage *img)
{
    SCE_CImage *back = bound;
    SCE_CBindImage (img);
    SCE_CDeleteImage_ ();
    SCE_CBindImage (back);
}
void SCE_CDeleteImage_ (void)
{
    SCE_btstart ();
    if (bound)
    {
	ilBindImage (0);
        SCE_List_Delete (bound->mipmaps);
        ilDeleteImages (1, &bound->id);
        SCE_free (bound);
        bound = NULL;
    }
    SCE_btend ();
}


/**
 * \brief Force image resizing at loading
 * \param persistent defines if the resizing is persistent or not, set SCE_TRUE
 * or SCE_FALSE
 * \param w,h,d news dimensions to set
 * \sa SCE_CForceImageRescale()
 * 
 * To avoid the resizing, so... I don't know how you do :D
 */
void SCE_CForceImageResize (int persistent, int w, int h, int d)
{
    rescaleforced = SCE_FALSE; /* on desactive l'autre */
    resizeforced = (persistent ? SCE_IMG_FORCE_PERSISTENT :
                    SCE_IMG_FORCE_PONCTUAL);
    size_w = w; size_h = h; size_d = d;
}
/**
 * \brief Force image rescaling at loading
 * \param persistent defines if the rescaling is persistent or not, set SCE_TRUE
 * or SCE_FALSE
 * \param w,h,d news scales to set
 * \sa SCE_CForceImageResize()
 * 
 * To avoid the rescaling, so... I don't know how you do :D
 */
void SCE_CForceImageRescale (int persistent, float w, float h, float d)
{
    resizeforced = SCE_FALSE; /* on desactive l'autre */
    rescaleforced = (persistent ? SCE_IMG_FORCE_PERSISTENT :
                     SCE_IMG_FORCE_PONCTUAL);
    scale_w = w; scale_h = h; scale_d = d;
}

/**
 * \brief Get the number of mipmap levels of an image
 * \param img the image
 * \returns the number of mipmap levels
 */
unsigned int SCE_CGetImageNumMipmaps (SCE_CImage *img)
{
    return SCE_List_GetSize (img->mipmaps);
}
unsigned int SCE_CGetImageNumMipmaps_ (void)
{
    return SCE_List_GetSize (bound->mipmaps);
}

/**
 * \brief Indicates if an image contains mipmap levels
 * \returns a boolean
 */
int SCE_CHaveImageMipmaps (SCE_CImage *img)
{
    return (SCE_List_GetSize (img->mipmaps) > 1) ? 1 : 0;
}
int SCE_CHaveImageMipmaps_ (void)
{
    return (SCE_List_GetSize (bound->mipmaps) > 1) ? 1 : 0;
}

/**
 * \brief Get the number of the active mipmap level
 */
unsigned int SCE_CGetImageMipmapLevel (SCE_CImage *img)
{
    return img->level;
}
unsigned int SCE_CGetImageMipmapLevel_ (void)
{
    return bound->level;
}


/* retourne les donnees du niveau de mipmap actif */
static SCE_CImageData* SCE_CGetImageCurrentMipmapData_ (void)
{
    SCE_SListIterator *it = NULL;

    SCE_btstart ();
    it = SCE_List_GetIterator (bound->mipmaps, bound->level);
    if (!it)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }
    SCE_btend ();
    return SCE_List_GetData (it);
}

/**
 * \brief Set the active mipmap level
 * \param level the mipmap level to activate
 * \returns SCE_OK on success, SCE_ERROR on error
 * \todo fix this fucking hack
 */
int SCE_CSetImageMipmapLevel (SCE_CImage *img, unsigned int level)
{
    SCE_CImage *back = bound;
    int r;
    SCE_CBindImage (img);
    r = SCE_CSetImageMipmapLevel_ (level);
    SCE_CBindImage (back);
    return r;
}
int SCE_CSetImageMipmapLevel_ (unsigned int level)
{
    int max_level;

    SCE_btstart ();
    /*ilBindImage (0);
      ilBindImage (bound->id);*/
    /*max_level = SCE_CGetImageNumMipmaps_ ();*/
    ilGetIntegerv (IL_NUM_MIPMAPS, &max_level);
    max_level++;
    if (level >= max_level)
    {
        SCEE_Log (SCE_INVALID_ARG);
        SCEE_LogMsg ("you can't active this mipmap level (%d), the maximum "
                       "mipmap level for this image is %d", level, max_level);
        SCE_btend ();
        return SCE_ERROR;
    }
    bound->level = level;
    bound->data = SCE_CGetImageCurrentMipmapData_ ();
    ilBindImage (0);
    ilBindImage (bound->id);
    ilActiveMipmap (level);
    SCE_btend ();
    return SCE_OK;
}



static int SCE_CIsCompressedPixelFormat (SCEenum fmt)
{
    if (fmt == IL_DXT1 || fmt == IL_DXT2 ||
        fmt == IL_DXT3 || fmt == IL_DXT4 ||
        fmt == IL_DXT5 || fmt == IL_3DC)
        return SCE_TRUE;
    else
        return SCE_FALSE;
}

/**
 * \brief Updates the active mipmap level of an image
 * \returns SCE_OK on success, SCE_ERROR on error
 */
int SCE_CUpdateImageMipmap (SCE_CImage *img)
{
    SCE_CImage *back = bound;
    int r;
    SCE_CBindImage (img);
    r = SCE_CUpdateImageMipmap_ ();
    SCE_CBindImage (back);
    return r;
}
int SCE_CUpdateImageMipmap_ (void)
{
    SCE_CImageData *d = bound->data;

    SCE_btstart ();
    if (!d)
    {
        SCEE_Log (SCE_INVALID_OPERATION);
        SCE_btend ();
        return SCE_ERROR;
    }

    if (d->updated)
    {
        SCE_btend ();
        return SCE_OK;
    }

    if (d->free_data)
        SCE_free (d->data);
    d->data = NULL;

    if (SCE_CGetImageIsCompressed_ ())
    {
        size_t data_size = SCE_CGetImageDataSize_ ();
        d->free_data = 1;
        d->data = SCE_malloc (data_size);
        if (!d->data)
        {
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        ilGetDXTCData (d->data, data_size, d->pxf);
    }
    else
    {
        d->free_data = 0;
        ilConvertImage (d->pxf, SCE_CGetImageDataType_ ());
        d->data = ilGetData ();
    }

    d->updated = SCE_TRUE;

    SCE_btend ();
    return SCE_OK;
}


/* met a jour la liste chainee des mipmaps */
/**
 * \todo faire en sorte de conserver les formats de pixel precedemment
 *       envoyes pour chaque niveau de mipmap
 */
static int SCE_CUpdateImageMipmapList_ (void)
{
    SCE_CImageData *d = NULL;
    int num_mipmaps, i;

    SCE_btstart ();
    /* on se place au premier niveau de mipmap
       (j'ai pas confiance en ilActiveMipmap (0)) */
    ilBindImage (0);
    ilBindImage (bound->id);

    /* on detruit la liste */
    SCE_List_Clear (bound->mipmaps);

    /* puis on la recree */
    ilGetIntegerv (IL_NUM_MIPMAPS, &num_mipmaps);
    num_mipmaps++;  /* parce qu'aucun mipmap = 0 (et moi je veux au moins 1) */
    for (i=0; i<num_mipmaps; i++)
    {
        ilBindImage (0);
        ilBindImage (bound->id);
        ilActiveMipmap (i);
        
        d = SCE_CCreateImageData ();
        if (!d)
        {
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        ilGetIntegerv (IL_DXTC_DATA_FORMAT, &d->pxf);
        if (d->pxf == IL_DXT_NO_COMP)
            ilGetIntegerv (IL_IMAGE_FORMAT, &d->pxf);
        if (SCE_List_AppendNewl (bound->mipmaps, d) < 0)
        {
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        d = NULL;
    }

    /* si level est trop grand, on le defini
       au niveau de mipmap le plus petit */
    if (bound->level >= num_mipmaps)
        bound->level = SCE_List_GetIndex (SCE_List_GetLast (bound->mipmaps));

    /* recuperation du niveau de mipmap */
    if (SCE_CSetImageMipmapLevel_ (bound->level) < 0)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }

    SCE_btend ();
    return SCE_OK;
}

/**
 * \brief Updates the mipmap levels list of \p img
 * 
 * Clears the mipmap levels list and rebuild it from the DevIL's image
 * informations. The informations are the number of mipmaps and the pixel format
 * or each level.
 * It updates only the list, doesn't call SCE_CUpdateImageMipmap() for each
 * level.
 */
int SCE_CUpdateImage (SCE_CImage *img)
{
    SCE_CImage *back = bound;
    int r;
    SCE_CBindImage (img);
    r = SCE_CUpdateImage_ ();
    SCE_CBindImage (back);
    return r;
}
int SCE_CUpdateImage_ (void)
{
    return SCE_CUpdateImageMipmapList_ ();
}


#define SCE_CIMGGET(name, ienum)\
int SCE_CGetImage##name (SCE_CImage *img)\
{\
    SCE_CImage *back = bound;\
    int r;\
    SCE_CBindImage (img);\
    ilGetIntegerv (ienum, &r);\
    SCE_CBindImage (back);\
    return r;\
}\
int SCE_CGetImage##name##_ (void)\
{\
    return ilGetInteger (ienum);\
}

SCE_CIMGGET (Width,     IL_IMAGE_WIDTH)
SCE_CIMGGET (Height,    IL_IMAGE_HEIGHT)
SCE_CIMGGET (Depth,     IL_IMAGE_DEPTH)
SCE_CIMGGET (Format,    IL_IMAGE_FORMAT)
SCE_CIMGGET (PixelSize, IL_IMAGE_BYTES_PER_PIXEL)
SCE_CIMGGET (DataType,  IL_IMAGE_TYPE)

#undef SCE_CIMGGET


/**
 * \brief Gets the type on an image
 * \returns the image type
 */
int SCE_CGetImageType (SCE_CImage *img)
{
    SCE_CImage *back = bound;
    int r;
    SCE_CBindImage (img);
    r = SCE_CGetImageType_ ();
    SCE_CBindImage (back);
    return r;
}
int SCE_CGetImageType_ (void)
{
    int type = SCE_TEX_1D;
    SCE_btstart ();
    if (SCE_CGetImageDepth_ () > 1)
        type = SCE_TEX_3D;
    else if (SCE_CGetImageHeight_ () > 1)
        type = SCE_TEX_2D;
    SCE_btend ();
    return type;
}

/* converti un format de pixel DevIL en format OpenGL */
static int SCE_CConvertPxfIlToGl (int pxf)
{
    switch (pxf)
    {
        case IL_DXT1:
            return SCE_PXF_DXT1;
        case IL_DXT2:
        case IL_DXT3:
            return SCE_PXF_DXT3;
        case IL_DXT4:
        case IL_DXT5:
            return SCE_PXF_DXT5;
        case IL_3DC:
            return SCE_PXF_3DC;
        default:
            return pxf;
    }
}
/* converti un format de pixel OpenGL en format DevIL */
static int SCE_CConvertPxfGlToIl (int pxf)
{
    switch (pxf)
    {
        case SCE_PXF_DXT1:
            return IL_DXT1;
        case SCE_PXF_DXT3:
            return IL_DXT3;
        case SCE_PXF_DXT5:
            return IL_DXT5;
        case SCE_PXF_3DC:
            return IL_3DC;
        default:
            return pxf;
    }
}

/**
 * \brief Gets the image pixel format
 * \returns the pixel format \sa defines de merde
 * \todo améliorer cette doc en ajoutant le support des defines
 */
int SCE_CGetImagePixelFormat (SCE_CImage *img)
{
    return SCE_CConvertPxfIlToGl (img->data->pxf);
}
int SCE_CGetImagePixelFormat_ (void)
{
    return SCE_CConvertPxfIlToGl (bound->data->pxf);
}


/**
 * \brief Gets the size of the data of the active mipmap level
 * \returns the size of the data (bytes)
 */
size_t SCE_CGetImageDataSize (SCE_CImage *img)
{
    SCE_CImage *back = bound;
    size_t r;
    SCE_CBindImage (img);
    r = SCE_CGetImageDataSize_ ();
    SCE_CBindImage (back);
    return r;
}
size_t SCE_CGetImageDataSize_ (void)
{
    if (SCE_CGetImageIsCompressed_ ())
        return ilGetDXTCData (NULL, 0, bound->data->pxf);
    else
        return ilGetInteger (IL_IMAGE_SIZE_OF_DATA);
}

/**
 * \brief Indicates if the active mipmap level has a compressed pixel format
 * \returns a boolean
 */
int SCE_CGetImageIsCompressed (SCE_CImage *img)
{
    return SCE_CIsCompressedPixelFormat (img->data->pxf);
}
int SCE_CGetImageIsCompressed_ (void)
{
    return SCE_CIsCompressedPixelFormat (bound->data->pxf);
}

/**
 * \brief Gets the pointer to the data of te active mipmap level
 * \returns the pointer to the data
 */
void* SCE_CGetImageData (SCE_CImage *img)
{
    SCE_btstart ();
    /* mise a jour du niveau de mipmap actif */
    SCE_CUpdateImageMipmap (img);
    SCE_btend ();
    return img->data->data;
}
void* SCE_CGetImageData_ (void)
{
    SCE_btstart ();
    /* mise a jour du niveau de mipmap actif */
    SCE_CUpdateImageMipmap_ ();
    SCE_btend ();
    return bound->data->data;
}


/**
 * \brief Resizes an image, take its new dimensions
 * \param w the new width
 * \param h the new height
 * \param d the new depth (only for 3D images)
 * \note Dimensions less than 1 are not modified.
 * \sa SCE_CRescaleImage()
 * \todo add filters managment
 */
void SCE_CResizeImage (SCE_CImage *img, int w, int h, int d)
{
    SCE_CImage *back = bound;
    SCE_CBindImage (img);
    SCE_CResizeImage_ (w, h, d);
    SCE_CBindImage (back);
}
void SCE_CResizeImage_ (int w, int h, int d)
{
    SCE_btstart ();
    if (w < 1)
        w = SCE_CGetImageWidth_ ();
    if (h < 1)
        h = SCE_CGetImageHeight_ ();
    if (d < 1)
        d = SCE_CGetImageDepth_ ();

    iluScale (w, h, d);
    bound->data->updated = SCE_FALSE;
    SCE_btend ();
}

/**
 * \brief Rescale an image, take its new scales
 * \param w the new width factor
 * \param h the new height factor
 * \param d the new depth factor (only on 3D images)
 * \note Set parameter at 0 at your own risk
 * \sa SCE_CResizeImage()
 * \todo add filters managment
 */
void SCE_CRescaleImage (SCE_CImage *img, float w, float h, float d)
{
    SCE_CImage *back = bound;
    SCE_CBindImage (img);
    SCE_CResizeImage_ (w, h, d);
    SCE_CBindImage (back);
}
void SCE_CRescaleImage_ (float w, float h, float d)
{
    SCE_btstart ();
    w = SCE_CGetImageWidth_ () * w;
    h = SCE_CGetImageHeight_ () * h;
    d = SCE_CGetImageDepth_ () * d;

    iluScale (w, h, d);
    bound->data->updated = SCE_FALSE;
    SCE_btend ();
}

/**
 * \brief Flip an image, inverse it from the y axis
 * \todo this function do not works, DevIL can't flip an image (pd)
 */
void SCE_CFlipImage (SCE_CImage *img)
{
    SCE_CImage *back = bound;
    SCE_CBindImage (img);
    SCE_CFlipImage_ ();
    SCE_CBindImage (back);
}
void SCE_CFlipImage_ (void)
{
    SCE_btstart ();
    iluFlipImage (); /* seg fault §§ */
    bound->data->updated = SCE_FALSE;
    SCE_btend ();
}


/* repete une operation pour chaque niveau de mipmap */
#define SCE_CSETALLMIPMAPS(action)\
{\
    unsigned int level = SCE_CGetImageMipmapLevel_ (), i, n;\
    n = SCE_CGetImageNumMipmaps_ ();\
    for (i=0; i<n; i++)\
    {\
        if (SCE_CSetImageMipmapLevel_ (i) < 0)\
        {\
            SCEE_LogSrc ();\
            break;\
        }\
        action;\
        i++;\
    }\
    SCE_CSetImageMipmapLevel_ (level);\
}

/**
 * \brief Sets the pixel format of the active mipmap level of \p img
 * \param fmt the new pixel format
 * 
 * \sa SCE_CSetImageAllPixelFormat()
 */
void SCE_CSetImagePixelFormat (SCE_CImage *img, SCEenum fmt)
{
    img->data->pxf = SCE_CConvertPxfGlToIl (fmt);
    img->data->updated = SCE_FALSE;
}
void SCE_CSetImagePixelFormat_ (SCEenum fmt)
{
    bound->data->pxf = SCE_CConvertPxfGlToIl (fmt);
    bound->data->updated = SCE_FALSE;
}
/**
 * \brief Sets the pixel format of all mipmap levels of \p img
 * \param fmt the new pixel format
 * 
 * \sa SCE_CSetImagePixelFormat()
 */
void SCE_CSetImageAllPixelFormat (SCE_CImage *img, SCEenum fmt)
{
    SCE_CImage *back = bound;
    SCE_CBindImage (img);
    SCE_CSetImageAllPixelFormat_ (fmt);
    SCE_CBindImage (back);
}
void SCE_CSetImageAllPixelFormat_ (SCEenum fmt)
{
    SCE_CSETALLMIPMAPS (SCE_CSetImagePixelFormat_ (fmt))
}


/**
 * \brief Makes mipmap levels for an image based on its first image
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_CBuildImageMipmaps (SCE_CImage *img)
{
    SCE_CImage *back = bound;
    int r;
    SCE_CBindImage (img);
    r = SCE_CBuildImageMipmaps_ ();
    SCE_CBindImage (back);
    return r;
}
int SCE_CBuildImageMipmaps_ (void)
{
    SCE_btstart ();
    /* ilActiveLevel (0) ? */
    if (iluBuildMipmaps () == IL_FALSE)
    {
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("DevIL fails to build mipmaps : %s",
                       iluErrorString (ilGetError ()));
        SCE_btend ();
        return SCE_ERROR;
    }

    /* mise a jour de la liste des mipmaps */
    SCE_CUpdateImage_ ();

    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief Loads a new image. This function is the callback for the media manager
 * \returns a new SCE_CImage*
 */
void* SCE_CLoadImage (FILE *fp, const char *fname, void *unused)
{
    SCE_CImage *img = NULL;

    SCE_btstart ();
    img = SCE_CCreateImage ();
    if (!img)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    unused = NULL;

    SCE_CBindImage (img);

    /* chargement de l'image */
    if (!ilLoadImage ((char*)fname))
    {
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("DevIL can't load '%s': %s",
                       fname, iluErrorString (ilGetError ()));
        SCE_CDeleteImage_ ();
        SCE_btend ();
        return NULL;
    }

    /* application des redimensions */
    if (resizeforced)
        SCE_CResizeImage_ (size_w, size_h, size_d);
    else if (rescaleforced)
        SCE_CRescaleImage_ (scale_w, scale_h, scale_d);

    if (SCE_CUpdateImage_ () < 0)
    {
        SCEE_LogSrc ();
        SCE_CDeleteImage_ ();
        SCE_btend ();
        return NULL;
    }

    /* annulation si la demande etait ponctuelle */
    if (resizeforced == SCE_IMG_FORCE_PONCTUAL)
        resizeforced = SCE_FALSE;
    else if (rescaleforced == SCE_IMG_FORCE_PONCTUAL)
        rescaleforced = SCE_FALSE;

    SCE_CBindImage (NULL);

    SCE_btend ();
    return img;
}

/** @} */
