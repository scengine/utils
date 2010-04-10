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
 
/* created: 05/01/2007
   updated: 10/04/2010 */

#include <stdio.h>
#include <errno.h>

#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEMemory.h"
#include "SCE/utils/SCEString.h"
#include "SCE/utils/SCEList.h"
#include "SCE/utils/SCEMedia.h"


/**
 * \file SCEMedia.c
 * \copydoc media
 * \brief Media managment
 * \file SCEMedia.h
 * \copydoc media
 * \brief Media managment
 */

/**
 * \defgroup media Media managment
 * \ingroup utils
 * \brief Managing medias
 * 
 * This module provides an easy way to manage medias.
 * It make easy to load, save and manage medias as in a library.
 */

/** @{ */

/* a media type */
typedef struct sce_smediatype SCE_SMediaType;
struct sce_smediatype
{
    int type;                   /* abstract type (comes from SCEResource) */
    char *exts;
    SCE_FMediaLoadFunc load;
    SCE_FMediaSaveFunc save;
    SCE_SListIterator it;
};

static SCE_SList funs;


static void SCE_Media_InitType (SCE_SMediaType *type)
{
    type->type = 0;
    type->exts = NULL;
    type->load = NULL;
    type->save = NULL;
    SCE_List_InitIt (&type->it);
    SCE_List_SetData (&type->it, type);
}
static SCE_SMediaType* SCE_Media_CreateType (void)
{
    SCE_SMediaType *type = NULL;
    if (!(type = SCE_malloc (sizeof *type)))
    {
        SCEE_LogSrc ();
        return NULL;
    }
    SCE_Media_InitType (type);
    return type;
}
static void SCE_Media_DeleteType (void *t)
{
    if (t)
    {
        SCE_SMediaType *type = t;
        SCE_free (type->exts);
        SCE_free (type);
    }
}

/**
 * \brief Initializes media manager
 * \returns always SCE_OK for now
 */
int SCE_Init_Media (void)
{
    SCE_List_Init (&funs);
    SCE_List_SetFreeFunc (&funs, SCE_Media_DeleteType);
    return SCE_OK;
}
void SCE_Quit_Media (void)
{
    SCE_List_Clear (&funs);
}


static int SCE_Media_ExtFound (const char *lot, const char *ext)
{
    size_t extlen;
    const char *p = NULL;
    const char *s = lot;
    /* very important for types without extension, which have a NULL 'exts' */
    if (!lot)
    {
        if (!ext)
            return SCE_TRUE;
        else
            return SCE_FALSE;
    }
    if (!ext)
        return SCE_FALSE;
    extlen = strlen (ext);
    while (*s != '\0' && (p = strstr (s, ext)))
    {
        if ( (p == lot || p[-1] == ' ') &&
             (p[extlen] == '\0' || p[extlen] == ' ') )
            return SCE_TRUE;
        s = p+1;
    }
    return SCE_FALSE;
}
static SCE_SMediaType* SCE_Media_LocateFromExt (const char *ext)
{
    SCE_SMediaType *t = NULL;
    SCE_SListIterator *it = NULL;

    if (!ext)
        return NULL;

    SCE_List_ForEach (it, &funs)
    {
        t = SCE_List_GetData (it);
        if (SCE_Media_ExtFound (t->exts, ext))
            return t;
    }
    return NULL;
}
static SCE_SMediaType* SCE_Media_LocateFromType (int type)
{
    SCE_SMediaType *t = NULL;
    SCE_SListIterator *it = NULL;

    SCE_List_ForEach (it, &funs)
    {
        t = SCE_List_GetData (it);
        if (t->type == type)
            return t;
    }
    return NULL;
}
static SCE_SMediaType* SCE_Media_LocateFromTypeAndExt (int type,
                                                       const char *ext)
{
    SCE_SMediaType *t = NULL;
    SCE_SListIterator *it = NULL;

    SCE_List_ForEach (it, &funs)
    {
        t = SCE_List_GetData (it);
        if (t->type == type && SCE_Media_ExtFound (t->exts, ext))
            return t;
    }
    return NULL;
}


int SCE_Media_Register (int type, const char *ext, SCE_FMediaLoadFunc load,
                        SCE_FMediaSaveFunc save)
{
    SCE_SMediaType *t = NULL;

    if (!(t = SCE_Media_CreateType ()))
    {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }

    t->type = type;
    t->load = load;
    t->save = save;

    if (ext)
    {
        if (!(t->exts = SCE_String_Dup (ext)))
        {
            SCE_Media_DeleteType (t);
            SCEE_LogSrc ();
            return SCE_ERROR;
        }
    }

    SCE_List_Appendl (&funs, &t->it);

    return SCE_OK;
}

void* SCE_Media_Load (int type, const char *fname, void *param)
{
    SCE_SMediaType *t = NULL;
    FILE *file = NULL;
    void *media = NULL;

    /* always opened as binary */
    file = fopen (fname, "rb");
    if (!file)
    {
        int errval = errno;
        SCEE_Log (SCE_FILE_NOT_FOUND);
        SCEE_LogMsg ("can't open '%s': %s", fname, strerror (errval));
        return NULL;
    }

    t = SCE_Media_LocateFromTypeAndExt (type, SCE_String_GetExt (fname));
    if (t)
        media = t->load (file, fname, param);
    else
    {
        /* la fonction de chargement du type du fichier
           n'a pas ete trouvee... */
        SCEE_Log (SCE_INVALID_ARG);
        SCEE_LogMsg ("load request of an unknown file type");
        media = NULL;
    }
    fclose (file);

    if (!media)
    {
        SCEE_LogSrc ();
        SCEE_LogSrcMsg ("failed to load '%s'", fname);
    }

    return media;
}


int SCE_Media_Save (int type, void *data, const char *fname)
{
    SCE_SMediaType *t = NULL;
    t = SCE_Media_LocateFromTypeAndExt (type, SCE_String_GetExt (fname));
    if (!t)
    {
        SCEE_Log (SCE_INVALID_ARG);
        return SCE_ERROR;
    }
    return t->save (data, fname);
}

/** @} */
