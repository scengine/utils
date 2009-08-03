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

#ifndef SCECTEXTURE_H
#define SCECTEXTURE_H

#include <stdarg.h>
#include <SCE/utils/SCEList.h>
#include <SCE/core/SCECImage.h>
#include <SCE/SCECommon.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup coretexture
 * @{
 */

/**
 * \copydoc sce_ctexdata
 */
typedef struct sce_ctexdata SCE_CTexData;
/**
 * \brief A SCE texture data
 * 
 * Contains all the data of a texture
 */
struct sce_ctexdata
{
    SCE_CImage *img;  /**< The image of this texture data */
    int canfree;      /**< Do we have rights to delete \c img ? */
    int user;         /**< Is user the owner of this structure ? */
    SCEenum target;   /**< Target of this texture data */
    int level;        /**< Mipmap level of this texture data */
    int w, h, d;      /**< Image's dimensions */
    SCEenum pxf;      /**< Desired internal (in VRAM) pixel format */
    SCEenum fmt;      /**< Format of \c data */
    SCEenum type;     /**< Type of \c data (SCE_UNSIGNED_BYTE, ...) */
    size_t data_size; /**< Size of \c data in bytes */
    int data_user;    /**< Is user the owner of \c data ? */
    void *data;       /**< Images's raw data */
    int comp;         /**< Is \c pxf a compressed pixel format ? */
};

/**
 * \copydoc sce_ctexture
 */
typedef struct sce_ctexture SCE_CTexture;
/**
 * \brief A SCE core texture
 * \todo incomplete documentation
 */
struct sce_ctexture
{
    SCEuint id;         /**< OpenGL identifier */
    SCEenum target;     /**< Target (type) of the texture */
    SCE_SList *data[6]; /**< Data of the texture, one list represent mipmap
                         *   levels, all the 6 lists are used for cubemaps */
    int have_data;      /**< Boolean, true if have data */

    int use_mipmap;     /**< Do we use mipmapping ? */
    int hw_mipmap;      /**< Do we use hardware for mipmaps generation ? */

    enum SCE_ETexType
    {
        SCE_TEXTYPE_1D, SCE_TEXTYPE_2D, SCE_TEXTYPE_3D, SCE_TEXTYPE_CUBE
    } type;             /**< Dunno olol */
};

/** @} */

int SCE_CTextureInit (void);
void SCE_CTextureQuit (void);

int SCE_CGetTextureResourceType (void);

/* defini la texture active */
void SCE_CBindTexture (SCE_CTexture*) SCE_GNUC_DEPRECATED;

/* initialise une structure de donnees d'une texture */
void SCE_CInitTexData (SCE_CTexData*);
/* cree une donnee de texture */
SCE_CTexData* SCE_CCreateTexData (void);
/* cree et initialise une donnee de texture a partir d'une image (bindee) */
SCE_CTexData* SCE_CCreateTexDataFromImage (void);
/* supprime une donnee de texture */
void SCE_CDeleteTexData (void*);
/* duplique dans son integralite une donnee de texture */
SCE_CTexData* SCE_CDupTexData (SCE_CTexData*);

/* cree une texture */
SCE_CTexture* SCE_CCreateTexture (SCEenum);
/* supprime une texture */
void SCE_CDeleteTexture (SCE_CTexture*);
void SCE_CDeleteTexture_ (void);

/* retourne le nombre maximum d'unites de texture */
int SCE_CGetMaxTextureUnits (void);
/* retourne la taille maximale d'une texture */
int SCE_CGetMaxTextureSize (void);
int SCE_CGetMaxTextureCubeSize (void);
int SCE_CGetMaxTexture3DSize (void);

/* installe un filtre a une texture */
void SCE_CSetTextureFilter (SCE_CTexture*, SCEint);
void SCE_CSetTextureFilter_ (SCEint);

/* defini si une texture est pixelisee vue de pres */
void SCE_CPixelizeTexture (SCE_CTexture*, int);
void SCE_CPixelizeTexture_ (int);

/* envoie un parametre specifie par l'utilisateur */
void SCE_CSetTextureParam (SCE_CTexture*, SCEenum, int);
void SCE_CSetTextureParam_ (SCEenum, int);
void SCE_CSetTextureParamf (SCE_CTexture*, SCEenum, float);
void SCE_CSetTextureParamf_ (SCEenum, float);

void SCE_CSetTextureGen (SCE_CTexture*, SCEenum, SCEenum, int);
void SCE_CSetTextureGen_ (SCEenum, SCEenum, int);
void SCE_CSetTextureGenf (SCE_CTexture*, SCEenum, SCEenum, float);
void SCE_CSetTextureGenf_ (SCEenum, SCEenum, float);
void SCE_CSetTextureGenfv (SCE_CTexture*, SCEenum, SCEenum, float*);
void SCE_CSetTextureGenfv_ (SCEenum, SCEenum, float*);

void SCE_CForceTexturePixelFormat (int, int);
void SCE_CForceTextureType (int, int);
void SCE_CForceTextureFormat (int, int);

SCEenum SCE_CGetTextureTarget (SCE_CTexture*);
SCEenum SCE_CGetTextureTarget_ (void);

SCE_CTexData* SCE_CGetTextureTexData (SCE_CTexture*, int, int);
SCE_CTexData* SCE_CGetTextureTexData_ (int, int);

int SCE_CHasTextureData (SCE_CTexture*);

int SCE_CIsTextureUsingMipmaps (SCE_CTexture*);
int SCE_CIsTextureUsingMipmaps_ (void);
int SCE_CGetTextureNumMipmaps (SCE_CTexture*, int);
int SCE_CGetTextureNumMipmaps_ (int);
int SCE_CGetTextureWidth (SCE_CTexture*, int, int);
int SCE_CGetTextureHeight (SCE_CTexture*, int, int);

int SCE_CGetTextureValidSize (int, int);

void SCE_CResizeTextureImage (SCE_CImage*, int, int, int);
void SCE_CResizeTextureImage_ (int, int, int);

int SCE_CAddTextureImage (SCE_CTexture*, int, SCE_CImage*, int);
int SCE_CAddTextureImage_ (int, SCE_CImage*, int);

int SCE_CAddTextureTexData (SCE_CTexture*, int, SCE_CTexData*, int);
int SCE_CAddTextureTexData_ (int, SCE_CTexData*, int);
int SCE_CAddTextureTexDataDup (SCE_CTexture*, int, SCE_CTexData*);
int SCE_CAddTextureTexDataDup_ (int, SCE_CTexData*);

SCE_CImage* SCE_CRemoveTextureImage (SCE_CTexture*, int, int);
SCE_CImage* SCE_CRemoveTextureImage_ (int, int);
void SCE_CEraseTextureImage (SCE_CTexture*, int, int);
void SCE_CEraseTextureImage_ (int, int);

SCE_CTexData* SCE_CRemoveTextureTexData (SCE_CTexture*, int, int);
SCE_CTexData* SCE_CRemoveTextureTexData_ (int, int);
void SCE_CEraseTextureTexData (SCE_CTexture*, int, int);
void SCE_CEraseTextureTexData_ (int, int);

SCE_CTexture* SCE_CLoadTexturev (int, int, int, int, int, const char**);
SCE_CTexture* SCE_CLoadTexture (int, int, int, int, int, ...);

int SCE_CBuildTexture (SCE_CTexture*, int, int);
int SCE_CBuildTexture_ (int, int);

int SCE_CUpdateTexture (SCE_CTexture*, int, int);
int SCE_CUpdateTexture_ (int, int);

void SCE_CSetActiveTextureUnit (unsigned int);

void SCE_CUseTexture (SCE_CTexture*, int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
