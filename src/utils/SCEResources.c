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
 
/* created: 02/01/2007
   updated: 06/11/2008 */

#include <stdio.h>
#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEString.h>
#include <SCE/utils/SCEList.h>
#include <SCE/utils/SCEMedia.h>
#include <SCE/utils/SCEResources.h>

/**
 * \file SCEResources.c
 * \copydoc resources
 * \file SCEResources.h
 * \copydoc resources
 */

/**
 * \defgroup resources Resources managment
 * \ingroup utils
 * \brief Resources managment
 */

/** @{ */

/** \internal */
typedef struct sce_sresource SCE_SResource;
struct sce_sresource
{
    void *data;  /* donnees */
    char *name;  /* nom */
    /* fonction de suppression de la ressource */
    /*void (*delete)(void*);*/
    unsigned int nb_used; /* nombre de fois utilisee */
};


static SCE_SList *resources = NULL;
static int sce_res_enabled = SCE_TRUE;


static void SCE_Resource_Init (SCE_SResource *r)
{
    r->data = NULL;
    r->name = NULL;
    r->nb_used = 0;
}

static SCE_SResource* SCE_Resource_Create (void)
{
    SCE_SResource *res = NULL;
    SCE_btstart ();
    res = SCE_malloc (sizeof *res);
    if (!res)
        Logger_LogSrc ();
    else
        SCE_Resource_Init (res);
    SCE_btend ();
    return res;
}

/* ajoute le 09/01/2008 */
static void SCE_Resource_Delete (void *r)
{
    SCE_btstart ();
    if (r)
    {
        SCE_SResource *res = r;
        SCE_free (res->name);
        SCE_free (res);
    }
    SCE_btend ();
}


/* revise le 09/01/2008 */
/**
 * \brief Initialize the resources manager
 * \returns SCE_OK on succes or SCE_ERROR on failure. This function actually
 * returns always SCE_OK
 * 
 * This function initialize the resource manager.
 */
int SCE_Init_Resource (void)
{
    SCE_btstart ();
    resources = SCE_List_Create (SCE_Resource_Delete);
    SCE_btend ();
    return SCE_OK;
}
/* revise le 09/01/2008 */
/**
 * \brief Exits the resource manager
 * 
 * This function exits the resource manager.
 */
void SCE_Quit_Resource (void)
{
    SCE_btstart ();
    SCE_List_Delete (resources);
    resources = NULL;
    SCE_btend ();
}


/* TODO: ca pux, parce que quand on charge deux
         ressources du meme nom ca chie :D */
/* ajoute le 27/06/2007 */
/**
 * \brief Activates or disactivate the resource manager
 * \param active 1 to activate or 0 to disactivate the resource manager
 */
void SCE_Resource_Activate (int active)
{
    sce_res_enabled = active;
}


/* revise le 09/01/2008 */
/* recherche une ressource par son nom */
static SCE_SResource* SCE_Resource_LocateFromName (const char *name)
{
    SCE_SListIterator *i = NULL;
    SCE_SResource *res = NULL;

    SCE_List_ForEach (i, resources)
    {
        res = SCE_List_GetData (i);
        if (SCE_String_Cmp (name, res->name, 1))
            return res;
    }

    return NULL;
}

/* revise le 09/01/2008 */
/* recherche une ressource par ses donnees */
static SCE_SResource* SCE_Resource_LocateFromData (void *data)
{
    SCE_SListIterator *i = NULL;
    SCE_SResource *res = NULL;

    SCE_List_ForEach (i, resources)
    {
        res = SCE_List_GetData (i);
        if (res->data == data)
            return res;
    }

    return NULL;
}


/* ajoute le 12/02/2008 */
/* revise le 06/11/2008 */
/**
 * \brief Add a resource to the resource manager
 * \param name the name of the resource
 * \param data the data of the resource
 * \returns SCE_OK on success or SCE_ERROR on failure
 * 
 * This function add a new resource to the resource manager.
 */
int SCE_Resource_Add (const char *name, void *data)
{
    SCE_SResource *res = NULL;
    size_t slen;

    SCE_btstart ();
    res = SCE_Resource_LocateFromName (name);
    if (res)
    {
        /* une ressource portant ce nom existe deja */
        if (res->data == data)
        {
            /* pas de probleme il s'agit de la meme ressource,
               il s'agit donc de l'ajout d'une utilisation supplementaire */
            res->nb_used++;
            SCE_btend ();
            return SCE_OK;
        }
        else
        {
            /* nom invalide car deja utilise, refus de l'ajout */
            Logger_Log (SCE_INVALID_OPERATION);
            Logger_LogMsg ("a resource named '%s' already exists!", name);
            SCE_btend ();
            return SCE_ERROR;
        }
    }
    res = SCE_Resource_Create ();
    if (!res)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_List_PrependNewl (resources, res) < 0)
    {
        SCE_Resource_Delete (res);
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }

    slen = strlen (name) + 1;
    res->name = SCE_malloc (slen);
    if (!res->name)
    {
        SCE_Resource_Delete (res);
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    memset (res->name, '\0', slen);
    strcpy (res->name, name);
    res->nb_used = 1;
    res->data = data;

    SCE_btend ();
    return SCE_OK;
}

/* revise le 09/01/2008 */
/**
 * \brief Load a resource from a file
 * \param fname the filename from where load the resource
 * \param p1 parameter
 * \param p2 parameter
 * \returns a pointer to a resource data
 *
 * This function loads a resource from a file in a resource manager compliant
 * format.
 *
 * \sa SCE_Resource_LoadNew()
 */
void* SCE_Resource_Load (const char *fname, void *p1, void *p2)
{
    SCE_SResource *res = NULL; /* identifiant de la ressource a charger */
    void *data = NULL;         /* donnees a renvoyer */

    SCE_btstart ();
    /* on commence par rechercher si la ressource a deja ete chargee */
    /* si le gestionnaire est desactive, res_id vaudra -1, donc le
       chargement d'une nouvelle ressource sera force */
    if (sce_res_enabled)
        res = SCE_Resource_LocateFromName (fname);

    if (res)
    {
        /* ressource trouvee */
        data = res->data;  /* recuperation des donnees */
        res->nb_used++;    /* incrementation du compteur */
    }
    else
    {
        /* ressource non trouvee ou nouveau chargement force,
           creation d'une nouvelle ressource et chargement */
        data = SCE_Resource_LoadNew (fname, p1, p2);
        if (!data)
            Logger_LogSrc ();
    }

    SCE_btend ();
    return data;
}

/* revise le 03/02/2008 */
/**
 * \brief Loads a new resource from a file
 * \param fname the filename from where load the resource
 * \param p1 parameter
 * \param p2 parameter
 * \returns a pointer to a resource data (see SCE_Media_LoadFromFile())
 * 
 * This function loads a new resource from a file in a resource manager
 * compliant format.
 * If you simply want to load a resource to the resource manager, prefer use
 * SCE_Resource_Load().
 *
 * \sa SCE_Media_LoadFromFile() SCE_Resource_Load()
 */
void* SCE_Resource_LoadNew (const char *fname, void *p1, void *p2)
{
    SCE_SListIterator *i = NULL;
    SCE_SResource *res = NULL;
    void *data = NULL;

    SCE_btstart ();
    if (SCE_Resource_Add (fname, NULL) < 0)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return NULL;
    }
    /* recuperation de la ressource */
    i = SCE_List_GetLast (resources);
    res = SCE_List_GetData (i);
    /* chargement de la ressource via le MediaManager */
    data = SCE_Media_LoadFromFile (fname, p1, p2);
    if (!data)
    {
        SCE_Resource_Delete (res);
        Logger_LogSrc ();
        SCE_btend ();
        return NULL;
    }
    res->data = data;

    SCE_btend ();
    return data;
}

/* TODO: review doc */
/**
 * \brief Decrements the number of uses of a resource and returns if it can be
 * freed or not
 * \param data a pointer to the resource (from a function like
 * SCE_Resource_Load())
 * \returns 1 if the resource can be freed and 0 if it's always used
 * 
 * This function decrements the number of uses of a resource and returns a
 * boolean that indicates if the resource can be freed or not.
 */
int SCE_Resource_Free (void *data)
{
    int ret = SCE_FALSE;
    SCE_SResource *res = NULL;

    SCE_btstart ();
    if (data)
        res = SCE_Resource_LocateFromData (data);
    if (res)
    {
        res->nb_used--;

        if (res->nb_used == 0)
        {
            /* la ressource n'est plus utilisee du tout, destruction */
            SCE_SListIterator *i=SCE_List_LocateIterator (resources, res, NULL);
            SCE_List_Erase (resources, i);
            /* on autorise la destruction du media par l'utilisateur */
            ret = SCE_TRUE;
        }
    }

    SCE_btend ();
    return ret;
}


/* revise le 09/01/2008 */
/**
 * \brief Get the number of uses of a resource
 * \param name the resource's name, or NULL
 * \param data the resource's pointer, or NULL
 * \returns the number of uses of the resource
 * 
 * This function get the number of uses of a resource from its name or its
 * pointer. \p name or \p data can be NULL. It both \p name and \p data are
 * NULL, this function will return 0.
 */
int SCE_Resource_NumUsed (const char *name, void *data)
{
    SCE_SResource *res = NULL;

    SCE_btstart ();
    if (name)
        res = SCE_Resource_LocateFromName (name);
    if (!res)
        res = SCE_Resource_LocateFromData (data);
    SCE_btend ();
    return (res ? res->nb_used : 0);
}

/* revise le 09/01/2008 */
/**
 * \brief Get the number of loaded resources
 * \returns the number of loaded resources
 * 
 * Returns the number of loaded reources in the resource manager
 */
int SCE_Resource_NumLoaded (void)
{
    return SCE_List_GetSize (resources);
}

/* revise le 09/01/2008 */
/**
 * \brief Get a resource name from its pointer
 * \param data a resource pointer
 * \returns the name of the resource that contains \p data or NULL if no
 * resource contains \p data
 * 
 * This function get a resource name from a resource pointer.
 */
char* SCE_Resource_GetName (void *data)
{
    SCE_SResource *res = NULL;
    res = SCE_Resource_LocateFromData (data);
    return (res ? res->name : NULL);
}
