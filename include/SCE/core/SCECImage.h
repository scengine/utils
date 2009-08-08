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
   updated: 08/07/2009 */

#ifndef SCECIMAGE_H
#define SCECIMAGE_H

#include <SCE/SCEMinimal.h>
#include <SCE/utils/SCEList.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Types of texture
 */
enum sce_ctextype {
    SCE_TEX_1D = GL_TEXTURE_1D,
    SCE_TEX_2D = GL_TEXTURE_2D,
    SCE_TEX_3D = GL_TEXTURE_3D,
    SCE_TEX_CUBE = GL_TEXTURE_CUBE_MAP
};
typedef enum sce_ctextype SCE_CTexType;

enum sce_cpixelformat {
    SCE_PXF_LUMINANCE = GL_LUMINANCE,
    SCE_PXF_LUMINANCE_ALPHA = GL_LUMINANCE_ALPHA,
    SCE_PXF_RGB = GL_RGB,
    SCE_PXF_RGBA = GL_RGBA,
    SCE_PXF_BGR = GL_BGR,
    SCE_PXF_BGRA = GL_BGRA,
    SCE_PXF_DXT1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    SCE_PXF_DXT3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    SCE_PXF_DXT5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};
typedef enum sce_cpixelformat SCE_CPixelFormat;

/* TODO: tmp hack, GLee doesn't support this extension */
#define SCE_PXF_3DC /*GL_COMPRESSED_RGB_3DC_ATI*/ 0

/**
 * \brief An image data
 * 
 * Contains the data of one image's mipmap level.
 */
typedef struct sce_cimagedata SCE_CImageData;
struct sce_cimagedata
{
    int free_data;  /**< Boolean, true = can free \c data */
    void *data;     /**< Raw data */
    int pxf;        /**< Pixel format */
    int updated;    /**< Boolean, true when \c data is up to date */
};

/**
 * \brief An SCE image
 * 
 * Contains an image with his own data stored into mipmap levels. It stores
 * too a DevIL identifier to manage all the image's data.
 */
typedef struct sce_cimage SCE_CImage;
struct sce_cimage
{
    ILuint id;            /**< DevIL's identifier */
    SCE_SList *mipmaps;   /**< All mipmap levels \sa SCE_CImageData  */
    unsigned int level;   /**< Active mipmap level */
    SCE_CImageData *data; /**< Active mipmap level's data */
};


int SCE_CImageInit (void);
void SCE_CImageQuit (void);

int SCE_CGetImageResourceType (void);

void SCE_CBindImage (SCE_CImage*);
SCE_CImage* SCE_CGetImageBound (void);

/* cree une nouvelle image */
SCE_CImage* SCE_CCreateImage (void);
/* detruit une image */
void SCE_CDeleteImage (SCE_CImage*);
void SCE_CDeleteImage_ (void);

/* active/desactive le redimensionnement automatique
   des images a leur chargement */
void SCE_CForceImageResize (int, int, int, int);
void SCE_CForceImageRescale (int, float, float, float);

/* retourne le nombre de mipmaps */
unsigned int SCE_CGetImageNumMipmaps (SCE_CImage*);
unsigned int SCE_CGetImageNumMipmaps_ (void);

/* renvoie un booleen indiquant si l'image dispose de mipmaps */
int SCE_CHaveImageMipmaps (SCE_CImage*);
int SCE_CHaveImageMipmaps_ (void);

/* retourne le niveau de mipmap actif */
unsigned int SCE_CGetImageMipmapLevel (SCE_CImage*);
unsigned int SCE_CGetImageMipmapLevel_ (void);

/* defini le niveau de mipmap actif */
int SCE_CSetImageMipmapLevel (SCE_CImage*, unsigned int);
int SCE_CSetImageMipmapLevel_ (unsigned int);

/* met a jour le niveau de mipmap courant */
int SCE_CUpdateImageMipmap (SCE_CImage*);
int SCE_CUpdateImageMipmap_ (void);

/* met a jour tous les niveaux de mipmap */
int SCE_CUpdateImage (SCE_CImage*);
int SCE_CUpdateImage_ (void);

int SCE_CGetImageWidth (SCE_CImage*);
int SCE_CGetImageWidth_ (void);
int SCE_CGetImageHeight (SCE_CImage*);
int SCE_CGetImageHeight_ (void);
int SCE_CGetImageDepth (SCE_CImage*);
int SCE_CGetImageDepth_ (void);
int SCE_CGetImageFormat (SCE_CImage*);
int SCE_CGetImageFormat_ (void);
int SCE_CGetImagePixelSize (SCE_CImage*);
int SCE_CGetImagePixelSize_ (void);
int SCE_CGetImageDataType (SCE_CImage*);
int SCE_CGetImageDataType_ (void);

int SCE_CGetImageType (SCE_CImage*);
int SCE_CGetImageType_ (void);
int SCE_CGetImagePixelFormat (SCE_CImage*);
int SCE_CGetImagePixelFormat_ (void);
size_t SCE_CGetImageDataSize (SCE_CImage*);
size_t SCE_CGetImageDataSize_ (void);

/* indique si le niveau de mipmap courant est dans un format compresse */
int SCE_CGetImageIsCompressed (SCE_CImage*);
int SCE_CGetImageIsCompressed_ (void);

/* retourne les donnees pixels de l'image,
   apres les avoir mises a jour si necessaire */
void* SCE_CGetImageData (SCE_CImage*);
void* SCE_CGetImageData_ (void);

/* constante acceptable pour les trois derniers parametres de ResizeImage() */
#define SCE_IMAGE_DO_NOT_CHANGE 0
void SCE_CResizeImage (SCE_CImage*, int, int, int);
void SCE_CResizeImage_ (int, int, int);
void SCE_CRescaleImage (SCE_CImage*, float, float, float);
void SCE_CRescaleImage_ (float, float, float);

/* inverse l'image en la faisait "pivoter" sur l'axe x */
void SCE_CFlipImage (SCE_CImage*);
void SCE_CFlipImage_ (void);

void SCE_CSetImagePixelFormat (SCE_CImage*, SCEenum);
void SCE_CSetImagePixelFormat_ (SCEenum);
void SCE_CSetImageAllPixelFormat (SCE_CImage*, SCEenum);
void SCE_CSetImageAllPixelFormat_ (SCEenum);

int SCE_CBuildImageMipmaps (SCE_CImage*);
int SCE_CBuildImageMipmaps_ (void);

void* SCE_CLoadImage (FILE*, const char*, void*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
