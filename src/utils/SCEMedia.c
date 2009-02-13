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
 
/* Cree le : 5 janvier 2007
   derniere modification le 06/11/2008 */

#include <errno.h>

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEString.h>
#include <SCE/utils/SCEList.h>
#include <SCE/utils/SCEMedia.h>


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



/* structure representant la fonction a utiliser
   pour le chargement d'un media definie */
typedef struct sce_sfuncload SCE_SFuncLoad;
struct sce_sfuncload
{
    /* type du media que la fonction charge
       identifiant utilise en interne */
    int type_id;

    /* TODO: prevoir de regrouper tout ceci dans une liste, car en effet
     * il peut n'y avoir qu'un seul type global, mais plusieurs magics
     * et extensions differentes (ex. type = image, exts = .png, .gif, ...
     * {
     */
    /* nombre magic du fichier NOTE: a remplacer par un char[4] */
    int magic;

    /* extension(s) du fichier */
    char *exts;

    /* fonction de chargement */
    SCE_FMediaLoadFunc load;
    /*
     * }
     */
};

/* idem pour la sauvegarde */
typedef struct sce_sfuncsave SCE_SFuncSave;
struct sce_sfuncsave
{
    /* type du media que la fonction charge
       identifiant utilise en interne */
    int type_id;

    /* fonction de sauvegarde */
    SCE_FMediaSaveFunc save;
};


/* revise le 03/10/2007 */
/* fonctions de chargement de medias */
static SCE_SList *loadfuncs = NULL;

/* revise le 03/10/2007 */
/* fonctions de sauvegarde de medias */
static SCE_SList *savefuncs = NULL;


/* ajoute le 10/01/2008 */
static void SCE_Media_DeleteLFunc (void *f)
{
    if (f)
    {
        SCE_SFuncLoad *l = f;
        SCE_free (l->exts);
        SCE_free (f);
    }
}

static void SCE_Media_DeleteSFunc (void *f)
{
    SCE_free (f);
}

/* revise le 03/10/2007 */
/** 
 * \brief 
 * \param 
 * \returns 
 */
int SCE_Init_Media (void)
{
    SCE_btstart ();
    loadfuncs = SCE_List_Create (SCE_Media_DeleteLFunc);
    if (!loadfuncs)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    savefuncs = SCE_List_Create (SCE_Media_DeleteSFunc);
    if (!savefuncs)
    {
        SCE_List_Delete (loadfuncs);
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_btend ();
    return SCE_OK;
}

/* revise le 03/10/2007 */
void SCE_Quit_Media (void)
{
    SCE_btstart ();
    SCE_List_Delete (savefuncs);
    savefuncs = NULL;
    SCE_List_Delete (loadfuncs);
    loadfuncs = NULL;
    SCE_btend ();
}


/* revise le 20/10/2007 */
static SCE_SFuncLoad* SCE_Media_LocateLoadFuncByExt (const char *ext)
{
    SCE_SFuncLoad *f = NULL;
    SCE_SListIterator *i = NULL;
    size_t extlen;
    char *p = NULL;
    char *s = NULL;

    if (!ext)
        return NULL;

    extlen = strlen (ext);

    SCE_List_ForEach (i, loadfuncs)
    {
        f = SCE_List_GetData (i);
        s = f->exts;
        while (*s != '\0' && (p = strstr (s, ext)))
        {
            if ( (p == f->exts || p[-1] == ' ') &&
                 (p[extlen] == '\0' || p[extlen] == ' ') )
                return f;
            s = p+1;
        }
    }

    return NULL;
}

/* revise le 03/10/2007 */
static SCE_SFuncLoad* SCE_Media_LocateLoadFuncByMagic (const int magic)
{
    SCE_SFuncLoad *f = NULL;
    SCE_SListIterator *i = NULL;

    /* 0 n'est pas un nombre magique valide */
    if (magic == 0)
        return NULL;

    SCE_List_ForEach (i, loadfuncs)
    {
        f = SCE_List_GetData (i);
        if (f->magic == magic)
            return f;
    }

    return NULL;
}

/* ajoute le 21/04/2008 */
static SCE_SFuncLoad* SCE_Media_LocateLoadFuncByType (int type_id)
{
    SCE_SFuncLoad *f = NULL;
    SCE_SListIterator *i = NULL;

    SCE_List_ForEach (i, loadfuncs)
    {
        f = SCE_List_GetData (i);
        if (f->type_id == type_id)
            return f;
    }

    return NULL;
}

/* revise le 03/10/2007 */
static SCE_SFuncSave* SCE_Media_LocateSaveFuncByType (int type_id)
{
    SCE_SFuncSave *f = NULL;
    SCE_SListIterator *i = NULL;

    if (type_id <= 0)
        return NULL;

    SCE_List_ForEach (i, savefuncs)
    {
        f = SCE_List_GetData (i);
        if (f->type_id == type_id)
            return f;
    }

    return NULL;
}


int SCE_Media_GenTypeID (void)
{
    static int id = 0;
    id++;
    return id;
}


/* revise le 03/10/2007 */
void* SCE_Media_LoadFromFile (const char *fname, void *param, int *type_id)
{
    SCE_SFuncLoad *f = NULL;
    FILE *file = NULL;
    void *media = NULL;
    int magic;
    int tmp_id = 0; /* type_id pointera dessus s'il est egal a NULL */

    SCE_btstart ();
    if (!type_id)
        type_id = &tmp_id;

    /* on considere toujours que le fichier est un fichier binaire */
    file = fopen (fname, "rb");
    if (!file)
    {
        int errval = errno;
        Logger_Log (SCE_FILE_NOT_FOUND);
        Logger_LogMsg ("can't open '%s': %s", fname, strerror (errval));
        SCE_btend ();
        return NULL;
    }

    fread (&magic, sizeof (magic), 1, file);
    rewind (file);

    /* si un ID a ete specifie, on force le chargement pour ce type de media */
    if (*type_id > 0)
        f = SCE_Media_LocateLoadFuncByType (*type_id);
    else
    {
        f = SCE_Media_LocateLoadFuncByMagic (magic);
        if (!f)
            f = SCE_Media_LocateLoadFuncByExt (SCE_String_GetExt ((char*)fname));
    }

    if (f)
    {
        media = f->load (file, fname, param);
        *type_id = f->type_id;
    }
    else
    {
        /* la fonction de chargement du type du fichier
           n'a pas ete trouvee... */
        Logger_Log (SCE_INVALID_ARG);
        Logger_LogMsg ("load request of an unknown file type");
        media = NULL;
        *type_id = SCE_UNKNOWN_TYPE;
    }

    fclose (file);

    SCE_btend ();
    return media;
}

int SCE_Media_RegisterLoader (
  int type_id,              /* type de la ressource chargee par la fonction */
  int magic,                /* nombre magique du type de fichier charge */
  const char *ext,          /* extention du type de fichier charge */
  SCE_FMediaLoadFunc fct)   /* fonction de chargement du media */
{
    SCE_SFuncLoad *f = NULL;

    if (fct == NULL)
    {
        Logger_Log (SCE_INVALID_ARG);
        return SCE_ERROR;
    }

    f = SCE_malloc (sizeof *f);
    if (!f)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }

    f->type_id = type_id;
    f->magic = magic;
    f->exts = NULL;
    f->load = fct;

    if (ext)
    {
        f->exts = SCE_malloc (strlen (ext) + 1);
        if (!f->exts)
        {
            SCE_free (f);
            Logger_LogSrc ();
            return SCE_ERROR;
        }
        strcpy (f->exts, ext);
    }

    if (SCE_List_PrependNewl (loadfuncs, f) < 0)
    {
        SCE_free (f);
        Logger_LogSrc ();
        return SCE_ERROR;
    }

    return SCE_OK;
}


/* revise le 03/10/2007 */
int SCE_Media_SaveToFile (void* param, const char *fname, int type_id)
{
    SCE_SFuncSave *f = SCE_Media_LocateSaveFuncByType (type_id);
    if (!f)
    {
        Logger_Log (SCE_INVALID_ARG);
        return SCE_ERROR;
    }
    return f->save(fname, param);
}

/* revise le 03/10/2007 */
int SCE_Media_RegisterSaver (int type_id, SCE_FMediaSaveFunc fct)
{
    SCE_SFuncSave *f = SCE_malloc (sizeof *f);
    if (!f)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    
    f->type_id = type_id;
    f->save = fct;
    
    return SCE_OK;
}


/** @} */
