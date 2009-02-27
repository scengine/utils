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
 
/* created: 19/01/2007
   updated: 18/02/2009 */

#include <SCE/SCEMinimal.h>
#include <SCE/interface/lib4fm.h>
#include <SCE/interface/libwar.h>

#include <SCE/utils/SCEVector.h>
#include <SCE/utils/SCEString.h>
#include <SCE/utils/SCEResources.h>
#include <SCE/utils/SCEMedia.h>

#include <SCE/core/SCECTexture.h>
#include <SCE/core/SCECBuffers.h>

#include <SCE/interface/SCEMesh.h>

#include <SCE/interface/SCE4FMLoader.h>
#include <SCE/interface/SCEOBJLoader.h>

/**
 * \defgroup mesh Meshs managment
 * \ingroup interface
 * \internal
 * \brief 
 */

/** @{ */

static int is_init = SCE_FALSE;

/* mesh en cache pour le dessin */
static SCE_SMesh *m = NULL;

static int sce_4fm_media_type;
static int sce_obj_media_type;

/* liste des mises a jour a effectuer */
static SCE_SList *updates = NULL;
/* pointeur sur le dernier tampon (SCE_SMeshVertexBuffer)
   dont on a demande la mise a jour */
static SCE_SListIterator *upit = NULL;
/* nombre de mises a jour demandees */
static unsigned int num_updates = 0;

int SCE_Init_Mesh (void)
{
    SCE_btstart ();
    if (!is_init)
    {
        if (!(updates = SCE_List_Create (NULL)))
        {
            Logger_LogSrc ();
            return SCE_ERROR;
        }

        /* enregistrement des loaders par defaut */
        sce_4fm_media_type = SCE_Media_GenTypeID ();
        SCE_Media_RegisterLoader (sce_4fm_media_type, FFM_MAGIC,
                                  "."FFM_FILE_EXTENSION, SCE_4FM_Load);
        sce_obj_media_type = SCE_Media_GenTypeID ();
        SCE_Media_RegisterLoader (sce_obj_media_type, 0,
                                  "."WAR_FILE_EXTENSION, SCE_OBJ_Load);

        is_init = SCE_TRUE;
    }
    SCE_btend ();
    return SCE_OK;
}

void SCE_Quit_Mesh (void)
{
    is_init = SCE_FALSE;
    SCE_List_Delete (updates), updates = NULL;
    num_updates = 0;
    upit = NULL;
}


static void SCE_Mesh_InitVertexData (SCE_SMeshVertexData *d)
{
    d->dec = NULL;
    d->data = NULL;
    d->canfree = SCE_FALSE;
    d->size = 0;
}
static SCE_SMeshVertexData* SCE_Mesh_CreateVertexData (void)
{
    SCE_SMeshVertexData *d = NULL;

    SCE_btstart ();
    d = SCE_malloc (sizeof *d);
    if (!d)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    SCE_Mesh_InitVertexData (d);
    d->dec = SCE_CCreateVertexDeclaration ();
    if (!d->dec)
    {
        SCE_free (d);
        Logger_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    SCE_btend ();
    return d;
}
static void SCE_Mesh_DeleteVertexData (void *data)
{
    SCE_btstart ();
    if (data)
    {
        SCE_SMeshVertexData *d = data;
        SCE_CDeleteVertexDeclaration (d->dec);
        if (d->canfree)
            SCE_free (d->data);
        SCE_free (d);
    }
    SCE_btend ();
}

/**
 * \brief Initializes a SCE_SMeshVertexBuffer structure
 * \param vb the vertex buffer to initialize
 */
void SCE_Mesh_InitVB (SCE_SMeshVertexBuffer *vb)
{
    vb->usage = GL_STREAM_DRAW;
    vb->buffer = NULL;
    vb->data = NULL;
    vb->active = SCE_FALSE;
    vb->builded = SCE_FALSE;
}

/**
 * \brief Initializes a SCE_SMeshIndexBuffer structure
 * \param ib the index buffer to initialize
 */
void SCE_Mesh_InitIB (SCE_SMeshIndexBuffer *ib)
{
    ib->usage = GL_STATIC_DRAW;
    ib->buffer = NULL;
    ib->type = SCE_UNSIGNED_BYTE;
    ib->data = NULL;
    ib->canfree = SCE_FALSE;
    ib->size = 0;
    ib->builded = SCE_FALSE;
}

/**
 * \brief Creates a new vertex buffer by allocating memory and calling
 * SCE_Mesh_InitVB(), this allocate too memory for internals structures
 * \returns the new vertex buffer
 */
SCE_SMeshVertexBuffer* SCE_Mesh_CreateVB (void)
{
    SCE_SMeshVertexBuffer *vb = NULL;

    SCE_btstart ();
    vb = SCE_malloc (sizeof *vb);
    if (!vb)
        Logger_LogSrc ();
    else
    {
#define SCE_MESHASSERT(c)\
        if (c)\
        {\
            SCE_Mesh_DeleteVB (vb);\
            Logger_LogSrc ();\
            SCE_btend ();\
            return NULL;\
        }
        /* pour chaque buffer cree, on lui reserve une place
           dans la liste des buffers a mettre a jour */
        SCE_MESHASSERT (SCE_List_AppendNewl (updates, NULL) < 0)
        SCE_Mesh_InitVB (vb);
        SCE_MESHASSERT (!(vb->buffer = SCE_CCreateVertexBuffer ()))
        vb->data = SCE_List_Create (SCE_Mesh_DeleteVertexData);
        if (!vb->data)
        {
            SCE_Mesh_DeleteVB (vb), vb = NULL;
            Logger_LogSrc ();
        }
#undef SCE_MESHASSERT
    }
    SCE_btend ();
    return vb;
}
/**
 * \brief Deletes an existing vertex buffer created by SCE_Mesh_CreateVB()
 * \param b the vertex buffer to delete
 * \note The prototype of this function is adapted for SCE_List_Create().
 */
void SCE_Mesh_DeleteVB (void *b)
{
    SCE_btstart ();
    if (b)
    {
        SCE_SMeshVertexBuffer *vb = b;
        SCE_CDeleteVertexBuffer (vb->buffer);
        SCE_List_Delete (vb->data);
        SCE_free (vb);
    }
    SCE_btend ();
}

/**
 * \brief Builds an existing vertex buffer
 * \param vb the vertex buffer to build
 *
 * Builds an existing vertex buffer created by SCE_Mesh_CreateVB(), by
 * calling SCE_CAddVertexBufferData() for each data in vb->data, then
 * SCE_CBuildVertexBuffer() is called.
 */
int SCE_Mesh_BuildVB (SCE_SMeshVertexBuffer *vb)
{
    SCE_SListIterator *i = NULL;
    SCE_SMeshVertexData *d = NULL;

    SCE_btstart ();
    /* si le buffer est deja construit ou si il ne possede aucune donnee */
    if (vb->builded || SCE_List_GetSize (vb->data) < 1)
    {
        SCE_btend ();
        return SCE_OK;
    }

    SCE_CBindVertexBuffer (vb->buffer);
    SCE_List_ForEach (i, vb->data)
    {
        d = SCE_List_GetData (i);
        d->dec->first = SCE_CGetVertexBufferFirst (vb->buffer);
        if (SCE_CAddVertexDeclaration_ (d->dec) < 0)
        {
            SCE_CBindVertexBuffer (NULL);
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        if (SCE_CAddVertexBufferData_ (d->size, d->data) < 0)
        {
            SCE_CBindVertexBuffer (NULL);
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
    }
    SCE_CBuildVertexBuffer_ (vb->usage);
    SCE_CBindVertexBuffer (NULL);

    SCE_btend ();
    return SCE_OK;
}

/**
 * \brief Builds an index buffer
 * \param ib the index buffer to build
 * \warning It is not recommanded to call this function, is reserved for
 * an internal use.
 */
int SCE_Mesh_BuildIB (SCE_SMeshIndexBuffer *ib)
{
    SCE_btstart ();
    if (!ib->builded && ib->data)
    {
        SCE_CBindIndexBuffer (ib->buffer);
        if (SCE_CAddIndexBufferData_ (ib->size, ib->data) < 0)
        {
            SCE_CBindIndexBuffer (NULL);
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        SCE_CBuildIndexBuffer_ (ib->usage);
        SCE_CBindIndexBuffer (NULL);
    }
    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief Initializes a SCE_SMesh structure
 * \param mesh the structure to initialize
 */
void SCE_Mesh_Init (SCE_SMesh *mesh)
{
    mesh->vertices = NULL;

    SCE_Mesh_InitIB (&mesh->ib);

    mesh->vcount = 0;
    mesh->icount = 0;
    mesh->use_indices = SCE_FALSE;
    mesh->polygon_type = GL_POINTS; /* evitons tout probleme */
    mesh->builded = SCE_FALSE;

    mesh->pos = mesh->nor = NULL;

    mesh->sortedfaces = NULL;
}

/**
 * \brief Creates a new SCE_SMesh by allocating memory and calling
 * SCE_Mesh_Init(), this allocate too memory for internals structures
 * \returns the new mesh
 */
SCE_SMesh* SCE_Mesh_Create (void)
{
    SCE_SMesh *mesh = NULL;

    SCE_btstart ();
    mesh = SCE_malloc (sizeof *mesh);
    if (!mesh)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    SCE_Mesh_Init (mesh);
    mesh->ib.buffer = SCE_CCreateIndexBuffer ();
    if (!mesh->ib.buffer)
    {
        SCE_free (mesh);
        Logger_LogSrc ();
        SCE_btend ();
        return NULL;
    }
    mesh->vertices = SCE_List_Create (SCE_Mesh_DeleteVB);
    if (!mesh->vertices)
    {
        SCE_Mesh_Delete (mesh), mesh = NULL;
        Logger_LogSrc ();
    }

    SCE_btend ();
    return mesh;
}

/**
 * \brief Deletes an existing mesh created by SCE_Mesh_Create()
 * \param mesh the mesh to delete
 */
void SCE_Mesh_Delete (SCE_SMesh *mesh)
{
    SCE_btstart ();
    if (mesh && SCE_Resource_Free (mesh))
    {
        if (mesh->ib.canfree)
            SCE_free (mesh->ib.data);
        SCE_CDeleteIndexBuffer (mesh->ib.buffer);
        SCE_List_Delete (mesh->vertices);
        SCE_List_Delete (mesh->sortedfaces);
        SCE_free (mesh);
    }
    SCE_btend ();
}
/**
 * \brief Deletes a list of SCE_SMesh structures stored in an array, usually
 * this arrays are created by the meshs loader and used for geometry LOD
 * \param meshs an array contains SCE_SMesh pointers to delete by calling
 * SCE_Mesh_Delete()
 */
void SCE_Mesh_DeleteList (SCE_SMesh **meshs)
{
    if (meshs && SCE_Resource_Free (meshs))
    {
        unsigned int i = 0;
        while (meshs[i])
            SCE_Mesh_Delete (meshs[i++]);
        SCE_free (meshs);
    }
}


/**
 * \brief Adds an existing vertex buffer to a mesh
 * \param mesh the mesh on which add the vertex buffer
 * \param b the vertex buffer to add
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_Mesh_AddVB (SCE_SMesh *mesh, SCE_SMeshVertexBuffer *b)
{
    SCE_btstart ();
    if (SCE_List_AppendNewl (mesh->vertices, b) < 0)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_btend ();
    return SCE_List_GetSize (mesh->vertices) - 1;
}
/**
 * \brief Creates and adds a new vertex buffer to a mesh
 * \param mesh the mesh on which add the new vertex buffer
 * \param mode the usage mode of the new vertex buffer, can be 0 to keep
 * the default value: GL_STREAM_DRAW
 * \sa SCE_SMeshVertexBuffer.usage
 */
int SCE_Mesh_AddNewVB (SCE_SMesh *mesh, int mode)
{
    int ret = SCE_ERROR;
    SCE_SMeshVertexBuffer *b = NULL;

    SCE_btstart ();
    b = SCE_Mesh_CreateVB ();
    if (!b)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    /* mode peut etre egal a zero pour laisser la valeur par defaut */
    if (mode)
        b->usage = mode;
    ret = SCE_Mesh_AddVB (mesh, b);
    if (ret < 0)
    {
        SCE_Mesh_DeleteVB (b);
        Logger_LogSrc ();
    }
    SCE_btend ();
    return ret;
}
/**
 * \brief Remove a vertex buffer from a mesh and return it
 * \param mesh the mesh on which remove the vertex buffer
 * \param id the vertex buffer's identifier, usually the buffer id is its
 * position in the buffer list (SCE_SMesh.vertices) that start at 0
 * (who for improve this documentation ??) (ki pr ameliorer cette doc ??)
 */
SCE_SMeshVertexBuffer* SCE_Mesh_RemoveVB (SCE_SMesh *mesh, unsigned int id)
{
    void *data = NULL;
    SCE_SListIterator *it = NULL;

    SCE_btstart ();
    it = SCE_List_GetIterator (mesh->vertices, id);
    if (!it)
        Logger_LogSrc ();
    else
    {
        data = SCE_List_GetData (it);
        SCE_List_Remove (mesh->vertices, it);
        SCE_List_DeleteIt (it);
    }
    SCE_btend ();
    return data;
}
/**
 * \brief Erases a vertex buffer of a mesh
 *
 * Erases a vertex buffer of a mesh by calling SCE_Mesh_DeleteVB() and
 * SCE_Mesh_RemoveVB() like this:
 * SCE_Mesh_DeleteVB (SCE_Mesh_RemoveVB (\p mesh, \p id))
 */
void SCE_Mesh_EraseVB (SCE_SMesh *mesh, unsigned int id)
{
    SCE_btstart ();
    SCE_Mesh_DeleteVB (SCE_Mesh_RemoveVB (mesh, id));
    SCE_btend ();
}


/**
 * \brief Activates a vertex buffer of a mesh
 * \param id I suggest you to see SCE_Mesh_RemoveVB() (good luck)
 * \param activate is vertex buffer activated ?
 * \note Is \p mesh is NULL, this function acts on the mesh binded by
 * SCE_Mesh_Use(), if no mesh is binded, this function maybe segfault
 */
void SCE_Mesh_ActivateVB (SCE_SMesh *mesh, unsigned int id, int activate)
{
    SCE_SMeshVertexBuffer *vb = NULL;
    SCE_SListIterator *i = NULL;

    SCE_btstart ();
    if (!mesh)
        mesh = m;

    i = SCE_List_GetIterator (mesh->vertices, id);
    if (i)
    {
        vb = SCE_List_GetData (i);
        vb->active = activate;
    }
    SCE_btend ();
}

/**
 * \brief Activates an index buffer of a mesh
 * \param activate is index buffer actived ?
 * \note Is \p mesh is NULL, this function acts on the mesh binded by
 * SCE_Mesh_Use(), if no mesh is binded, this function maybe segfault
 */
void SCE_Mesh_ActivateIB (SCE_SMesh *mesh, int activate)
{
    if (mesh)
        mesh->use_indices = activate;
    else
        m->use_indices = activate;
}


/**
 * \brief Finds and returns the vertices data of \p mesh having the attribute
 * \p attrib
 * \param attrib the vertex attribute of the researching data
 * \param vb if is not NULL, this pointer is used to store the vertex buffer
 * corresponding to the founded data
 */
SCE_SMeshVertexData* SCE_Mesh_LocateData (SCE_SMesh *mesh, int attrib,
                                          SCE_SMeshVertexBuffer **vb)
{
    SCE_SListIterator *i = NULL, *j = NULL;
    SCE_SMeshVertexData *data = NULL;
    SCE_SMeshVertexBuffer *tmp = NULL;

    SCE_btstart ();
    if (!vb)
        vb = &tmp;

    SCE_List_ForEach (i, mesh->vertices)
    {
        *vb = SCE_List_GetData (i);
        SCE_List_ForEach (j, (*vb)->data)
        {
            data = SCE_List_GetData (j);
            if (data->dec->attrib == attrib)
            {
                SCE_btend ();
                return data;
            }
        }
    }

    *vb = NULL;
    SCE_btend ();
    return NULL;
}

/**
 * \brief Returns SCE_SMesh::pos
 */
void* SCE_Mesh_GetVerticesPositions (SCE_SMesh *mesh)
{
    return mesh->pos;
}
/**
 * \brief Returns SCE_SMesh::nor
 */
void* SCE_Mesh_GetVerticesNormals (SCE_SMesh *mesh)
{
    return mesh->nor;
}

/**
 * \brief Adds vertex data to the vertex buffer \p id of \p mesh
 * \param mesh the mesh on which add the data
 * \param id see the description of \p id in SCE_Mesh_RemoveVB()
 * \param attrib vertex attribute of the data
 * \param type data type (SCE_FLOAT, SCE_INT, ...)
 * \param size number of components of the vectors' data
 * \param count number of vertices in \p data
 * \param data pointer to the data
 * \param canfree defines if \p mesh has right to delete \p data
 * \returns the new vertex buffer id, SCE_ERROR on error
 *
 * If \p data is NULL, the function will stop and return SCE_OK.
 * If \p mesh already contains vertices data that have for attribute \p attrib,
 * they will be removed from its vertex buffer. If the previous operation
 * cleared the vertex buffer, it will be removed too. If \p canfree is SCE_TRUE,
 * \p data will be freed when \p mesh is deleted. If \p count is greater than
 * the previous count(s) specified then the vertices in excess are ignored.
 * It is recommanded to use SCE_VERTICES_TYPE for \p type, that gives a full
 * compatibility with functions acts on the mesh's geometry. \p data is a
 * pointer to an array of contigous vectors that  the data to store,
 * each vector represent a vertex. \p size defines the vectors length, usually
 * 3 for the vertex position and vertex normals, 2 for the texture coordinates.
 * To get the maximal and minimal values for \p size for each attribute type,
 * see the documentation of glVertexPointer() and its friends.
 *
 * \warning SCE_OK is 0, and a vertex buffer id can be 0 too.
 */
int SCE_Mesh_AddVertices (SCE_SMesh *mesh, unsigned int id, int attrib,
                          SCEenum type, unsigned int size, unsigned int count,
                          void *data, int canfree)
{
    SCE_SMeshVertexData *d = NULL;
    SCE_SListIterator *it = NULL;
    SCE_SMeshVertexBuffer *vb = NULL;

#define SCE_MESH_ERROR()\
    {\
        Logger_LogSrc ();\
        SCE_btend ();\
        return SCE_ERROR;\
    }

    SCE_btstart ();
    if (!data)
    {
        SCE_btend ();
        return SCE_OK;
    }

    /* verification du count (si un count est deja present) */
    if (!mesh->vcount)
        mesh->vcount = count;
    else if (count < mesh->vcount)
    {
        Logger_Log (SCE_INVALID_ARG);
        Logger_LogMsg ("too small count, the minimum requested for"
                       " this mesh is %u", mesh->vcount);
        SCE_btend ();
        return SCE_ERROR;
    }

    /* on verifie si ce type de donnee a deja ete specifie */
    if ((d = SCE_Mesh_LocateData (mesh, attrib, &vb)))
    {
        /* si ces donnees existent, on les supprime dans leur buffer */
        it = SCE_List_LocateIterator (vb->data, d, NULL);
        SCE_List_Remove (vb->data, it);
        SCE_List_DeleteIt (it);
        it = NULL;
        if (d->canfree)
            SCE_free (d->data);
        d->data = NULL;
        /* et on demande la reconstruction du buffer */
        SCE_CClearVertexBuffer (vb->buffer);
        vb->builded = SCE_FALSE;
        /* si le buffer est vide et qu'il ne s'agit pas
           du buffer de destination des donnees */
        if (SCE_List_GetSize (vb->data) < 1 &&
            SCE_List_LocateIndex (mesh->vertices, vb, NULL) != id)
        {
            /* on peut le supprimer */
            it = SCE_List_LocateIterator (mesh->vertices, vb, NULL);
            SCE_List_Erase (mesh->vertices, it);
        }
    }
    else
    {
        /* creation d'une donnee de vertex buffer */
        d = SCE_Mesh_CreateVertexData ();
        if (!d)
            SCE_MESH_ERROR ()
    }

    /* recuperation du vb... */
    it = SCE_List_GetIterator (mesh->vertices, id);
    if (!it)
    {
        /* ...creation si inexistant */
        Logger_Clear (); /* suppression de l'erreur loggee par GetIterator */
        if ((id = SCE_Mesh_AddNewVB (mesh, 0)) < 0)
            SCE_MESH_ERROR ()
        /* ce get ne peut pas echouer */
        it = SCE_List_GetIterator (mesh->vertices, id);
    }
    vb = SCE_List_GetData (it);

    if (vb->builded)
    {
        /* la reconstruction du buffer sera requise */
        SCE_CClearVertexBuffer (vb->buffer);
        vb->builded = SCE_FALSE;
    }

    /* ajout des donnees au buffer */
    if (SCE_List_AppendNewl (vb->data, d) < 0)
    {
        SCE_Mesh_DeleteVertexData (d);
        SCE_MESH_ERROR ()
    }

    /* specification des donnes */
    d->dec->attrib = attrib;
    d->dec->type = type;
    d->dec->size = size;

    d->size = count * size * SCE_CSizeofType (type);
    /* aucune duplication des donnees */
    d->data = data;
    d->canfree = canfree;

    if (attrib == SCE_POSITION)
        mesh->pos = data; /* hopla */
    else if (attrib == SCE_NORMAL)
        mesh->nor = data; /* hopli */

    mesh->builded = SCE_FALSE;

    SCE_btend ();
#undef SCE_MESH_ERROR
    /* par defaut on retourne l'id du tampon,
       au cas ou on en aurait cree un nouveau avec un id different */
    return id;
}

/**
 * \brief Adds vertex data to the vertex buffer \p id
 *
 * This function works like SCE_Mesh_AddVertices(), but \p data is duplicated
 * in memory and its new type is SCE_VERTICES_TYPE.
 */
int SCE_Mesh_AddVerticesDup (SCE_SMesh *mesh, unsigned int id, int attrib,
                             SCEenum type, unsigned int size,
                             unsigned int count, const void *data)
{
    /* conversion des donnees */
    SCEvertices *new = NULL;

    SCE_btstart ();
    new = SCE_Mem_ConvertDup (SCE_VERTICES_TYPE, type, data, count * size);
    if (!new)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_btend ();
    return SCE_Mesh_AddVertices (mesh, id, attrib, SCE_VERTICES_TYPE,
                                 size, count, new, SCE_TRUE);
}


/**
 * \brief Defines the indices of a mesh
 * \param usage the index buffer's usage, see SCE_SMeshIndexBuffer::usage,
 * can be 0
 * \param type data type (SCE_UNSIGNED_INT, SCE_UNSIGNED_SHORT, ...)
 * \param count number of vertices that the indices dereference
 * \param data the index data
 * \param canfree defines if \p mesh has right to delete \p data
 * \returns always SCE_OK for now
 *
 * Set the indices to use for render \p mesh.
 * If \p data is NULL, does nothing and return. If usage is 0, then the
 * previous value specified is used, or the default value (GL_STATIC_DRAW) if
 * none previously value has been specified. Calling this function will activate
 * the indices for rendering. \p type must be an integer and unsigned type,
 * it is recommanded to use SCE_INDICES_TYPE for \p type, that gives a full
 * compatibility with functions acts on the mesh's geometry.
 */
int SCE_Mesh_SetIndices (SCE_SMesh *mesh, SCEenum usage, SCEenum type,
                         unsigned int count, void *data, int canfree)
{
    SCE_btstart ();
    if (!data)
    {
        SCE_btend ();
        return SCE_OK;
    }

    if (mesh->ib.builded)
    {
        /* suppression du buffer, car une reconstruction sera necessaire */
        SCE_CClearIndexBuffer (mesh->ib.buffer);
    }

    mesh->ib.size = count * SCE_CSizeofType (type);
    if (mesh->ib.canfree)
        SCE_free (mesh->ib.data); /* suppression des donnees precedentes */
    mesh->ib.data = data;
    mesh->ib.canfree = canfree;
    mesh->ib.type = type;
    if (usage)
        mesh->ib.usage = usage;

    mesh->use_indices = SCE_TRUE;
    mesh->icount = count;

    mesh->ib.builded = mesh->builded = SCE_FALSE;

    SCE_btend ();
    return SCE_OK;
}

/**
 * \brief Defines the indices of a mesh
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * This function works like SCE_Mesh_SetIndices(), but \p data is duplicated
 * in memory and its new type is SCE_INDICES_TYPE.
 */
int SCE_Mesh_SetIndicesDup (SCE_SMesh *mesh, SCEenum mode, SCEenum type,
                            unsigned int count, const void *data)
{
    SCEenum t;
    void *data_dup = NULL;

    SCE_btstart ();
#if 0
    /* conversion des donnees en le type le plus leger */
    t = ((count >= 256*256) ? SCE_UNSIGNED_INT : SCE_UNSIGNED_SHORT);
#endif
    t = SCE_INDICES_TYPE;
    if (!(data_dup = SCE_Mem_ConvertDup (t, type, data, count)))
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_btend ();
    return SCE_Mesh_SetIndices (mesh, mode, t, count, data_dup, SCE_TRUE);
}


/**
 * \brief Gets the number of vertices per face
 *
 * If SCE_SMesh.polygon_type is SCE_TRIANGLES, this function returns 3,
 * for SCE_QUADS it returns 4, etc.
 */
int SCE_Mesh_GetNumVerticesPerFace (SCE_SMesh *mesh)
{
    int vpf = 0;
    switch (mesh->polygon_type)
    {
    case SCE_POINTS:         vpf = 1; break;
    case SCE_LINES:          vpf = 2; break;/*
    case SCE_TRIANGLE_STRIP:
    case SCE_TRIANGLE_FAN:*/
    case SCE_TRIANGLES:      vpf = 3; break;
    case SCE_QUADS:          vpf = 4;
#ifdef SCE_DEBUG
        break;
    default:
        Logger_Log (-1);
        Logger_LogMsg ("unsupported face type to compute the "
                       "number of vertices per face");
        vpf = SCE_ERROR;
#endif
    }
    return vpf;
}
/* ajoute le 27/03/2008 */
/**
 * \brief Gets the number of faces of a mesh
 */
int SCE_Mesh_GetNumFaces (SCE_SMesh *mesh)
{
    if (mesh->use_indices)
        return mesh->icount/SCE_Mesh_GetNumVerticesPerFace (mesh);
    else
        return mesh->vcount/SCE_Mesh_GetNumVerticesPerFace (mesh);
}

/**
 * \brief Calls an user-defined function for each face of a mesh
 * \param func the callback function to call for each face
 * \param param an user-defined parameter that is sent to func when is called
 * \returns SCE_ERROR on error, SCE_OK otherwise, if the engine has not been
 * compiled in debug mode, SCE_OK is always returned.
 */
int SCE_Mesh_ForEachFace (SCE_SMesh *mesh, SCE_FMeshFaceFunc func, void *param)
{
    SCE_SMeshFace face;
    int i, j, num_faces, vpf;

    num_faces = SCE_Mesh_GetNumFaces (mesh);
    vpf = SCE_Mesh_GetNumVerticesPerFace (mesh);
    face.num_vertices = vpf;
    face.pos = mesh->pos;
    face.nor = mesh->nor;

    if (mesh->use_indices)
    {
        unsigned int *indices = mesh->ib.data;
#ifdef SCE_DEBUG
        if (mesh->ib.type != SCE_UNSIGNED_INT)
        {
            Logger_Log (SCE_INVALID_OPERATION);
            Logger_LogMsg ("unsigned int indices expected on "
                           "this mesh for calling %s", __FUNCTION__);
            return SCE_ERROR;
        }
#endif
        for (i=0; i<num_faces; i++)
        {
            for (j=0; j<vpf; j++)
                face.index[j] = indices[i*vpf+j]*3;
            if (!func (mesh, &face, i, param))
                return SCE_OK;
        }
    }
    else
    {
        for (i=0; i<num_faces; i++)
        {
            for (j=0; j<vpf; j++)
                face.index[j] = (i*vpf+j)*3;
            if (!func (mesh, &face, i, param))
                return SCE_OK;
        }
    }
    return SCE_OK;
}


/* calcul la position moyenne d'un polygone */
static void SCE_Mesh_GetFaceCenter (SCE_TVector3 v[4], int vpf, SCE_TVector3 c)
{
    int i;
    SCE_Vector3_Copy (c, v[0]);
    for (i=1; i<vpf; i++)
        SCE_Vector3_Operator1v (c, +=, v[i]);
    SCE_Vector3_Operator1 (c, /=, vpf);
}

/* structure d'une face vue par la liste 'sortedfaces' des meshs */
typedef struct
{
    float dist;
    SCEindices index[4];
} SCE_SMeshFaceInfo;
/* callback pour 'sortedfaces' pour supprimer une face (cf au ci-dessus) */
static void SCE_Mesh_DeleteFaceInfo (void *i)
{
    SCE_free (i);
}

/* ajoute une nouvelle face a la liste en copiant ses donnees */
static int SCE_Mesh_CopyFaceToSortedList (SCE_SMesh *mesh, SCE_SMeshFace *face,
                                          int facenum, void *param)
{
    SCE_SMeshFaceInfo *fi;
    fi = SCE_List_GetData (SCE_List_GetIterator (mesh->sortedfaces, facenum));
    fi->index[0] = face->index[0];
    fi->index[1] = face->index[1];
    fi->index[2] = face->index[2];
    fi->index[3] = face->index[3];
    return SCE_TRUE;
}

/* construit la liste des faces triees et la met a jour.
   procede uniquement a sa mise a jour si elle est deja cree */
static int SCE_Mesh_MakeSortedList (SCE_SMesh *mesh)
{
    int diff;
    if (!mesh->sortedfaces)
    {
        mesh->sortedfaces = SCE_List_Create (SCE_Mesh_DeleteFaceInfo);
        if (!mesh->sortedfaces)
        {
            Logger_LogSrc ();
            return SCE_ERROR;
        }
    }
    /* calcul de la difference entre le nombre reel de faces
       et la taille de la sorted list */
    diff = SCE_Mesh_GetNumFaces (mesh) - SCE_List_GetSize (mesh->sortedfaces);
    /* s'il y a plus de faces que ne peut en contenir la sorted list, on l'agrandie */
    if (diff > 0)
    {
        unsigned int i;
        for (i=0; i<diff; i++)
        {
            SCE_SMeshFaceInfo *fi = SCE_malloc (sizeof *fi);
            /* TODO: to test (Append used before) */
            if (!fi || SCE_List_PrependNewl (mesh->sortedfaces, fi) < 0)
            {
                SCE_free (fi);
                Logger_LogSrc ();
                return SCE_ERROR;
            }
        }
    }
    /* copie brut des donnees */
    if (SCE_Mesh_ForEachFace (mesh, SCE_Mesh_CopyFaceToSortedList, NULL) < 0)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    return SCE_OK;
}

/* callbacks */
static int SCE_Mesh_OrderNearToFar (SCE_SMeshFaceInfo *first,
                                    SCE_SMeshFaceInfo *next)
{
    return (first->dist > next->dist);
}
static int SCE_Mesh_OrderFarToNear (SCE_SMeshFaceInfo *first,
                                    SCE_SMeshFaceInfo *next)
{
    return (first->dist < next->dist);
}

static void SCE_Mesh_SortSortedList (SCE_SMesh *mesh, int order)
{
    SCE_SListIterator *it = NULL, *jt = NULL;
    SCE_SMeshFaceInfo *info = NULL, *tmp = NULL;
    int (*needChange)(SCE_SMeshFaceInfo*, SCE_SMeshFaceInfo*);

    if (order == SCE_MESH_SORT_NEAR_TO_FAR)
        needChange = SCE_Mesh_OrderNearToFar;
    else
        needChange = SCE_Mesh_OrderFarToNear;

    /* pour chaque face */
    SCE_List_ForEach (it, mesh->sortedfaces)
    {
        info = SCE_List_GetData (it);
        jt = it;
        /* on verifie dans chacune des faces suivantes */
        SCE_List_ForEachNext (jt)
        {
            tmp = SCE_List_GetData (jt);
            /* s'il faut les inverser */
            if (needChange (info, tmp))
            {
                /* si oui, alors on echange les donnees des iterateurs */
                SCE_List_SetData (jt, info);
                SCE_List_SetData (it, tmp);
                /* et on remplace la face que l'on compare par la nouvelle */
                info = tmp;
            }
        }
    }
}

/**
 * \brief Sorts the faces of a mesh
 * \param order defines the order to sort the faces, can be
 * SCE_MESH_SORT_NEAR_TO_FAR or SCE_MESH_SORT_FAR_TO_NEAR
 * \param p the position from which the faces will be sorted
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * \warning \p mesh MUST have indices
 */
int SCE_Mesh_SortFaces (SCE_SMesh *mesh, int order, SCE_TVector3 p)
{
    unsigned int i, j = 0;
    SCE_SListIterator *it = NULL;
    SCE_SMeshFaceInfo *info = NULL;
    SCE_TVector3 center, vertex[4];
    int vpf;
    SCEvertices *pos = mesh->pos;
    SCEindices *indices = mesh->ib.data;

#ifdef SCE_DEBUG
    if (!indices)
    {
        Logger_Log (SCE_INVALID_OPERATION);
        Logger_LogMsg ("the mesh of which you want to sort the "
                       "faces hasn't indices");
        return SCE_ERROR;
    }
#endif

    /* TODO: condition merdique */
    if (!mesh->sortedfaces)
        if (SCE_Mesh_MakeSortedList (mesh) < 0)
        {
            Logger_LogSrc ();
            return SCE_ERROR;
        }

    vpf = SCE_Mesh_GetNumVerticesPerFace (mesh);

    /* calcul de la distance de chaque triangle par rapport a la position */
    SCE_List_ForEach (it, mesh->sortedfaces)
    {
        info = SCE_List_GetData (it);
        for (i=0; i<vpf; i++)
            SCE_Vector3_Copy (vertex[i], &pos[info->index[i]]);
        SCE_Mesh_GetFaceCenter (vertex, vpf, center);
        info->dist = SCE_Vector3_Distance (center, p);
    }

    /* tri des faces */
    SCE_Mesh_SortSortedList (mesh, order);

    i = 0;
    /* mise a jour des donnees dans mesh->ib.data */
    SCE_List_ForEach (it, mesh->sortedfaces)
    {
        info = SCE_List_GetData (it);
        for (j=0; j<vpf; j++)
            indices[i+j] = info->index[j]/3;
        i += vpf;
        /* TODO: d'apres l'effet de MakeSortedList, il peut y avoir un
           debordement de 'data' ici (sortedfaces n'est jamais raccourcie) */
    }
    /* updating IBO */
    /* TODO: proceder a un log update... hqn ah non pas possible :D */
    SCE_CLockIndexBuffer (mesh->ib.buffer, GL_WRITE_ONLY);
    SCE_CUpdateIndexBuffer (mesh->ib.buffer);
    SCE_CUnlockIndexBuffer (mesh->ib.buffer);

    return SCE_OK;
}


/**
 * \brief Compute the tangent, binormal and normal for a triangle
 * \param vertex the vertices' positions of the triangle
 * \param texcoord the texture coordinates of the triangle, can be NULL
 * \param index face's indices to use, can be NULL
 * \param tangents here is stored the computed tangents, can be NULL
 * \param binormals here is stored the computed binormals, can be NULL
 * \param normals here is written the computed normals, can be NULL
 * \warning If \p texcoord is NULL, only normals will be computed
 */
void SCE_Mesh_ComputeTriangleTBN (SCEvertices *vertex, SCEvertices *texcoord,
                                  SCEindices *index, SCEvertices *tangents,
                                  SCEvertices *binormals, SCEvertices *normals)
{
    /* vecteurs des 'cotes' de la face */
    SCE_TVector3 side0, side1;
    /* vecteurs temporaires */
    SCE_TVector3 tmpTangente, tmpNormal, tmpBinormal;
    SCEvertices deltaT0=0., deltaT1=0., deltaB0=0., deltaB1=0., scale=0.;
    SCEindices default_indices[3] = {0, 1, 2};

    SCE_btstart ();
    if (!index)
        index = default_indices;

    /* on calcule 2 vecteurs formant les bords du triangle
       le vecteur side0 est celui allant du vertex 0 au vertex 1 */
    SCE_Vector3_Operator2v (side0, =, &vertex[index[1]*3],
                                   -, &vertex[index[0]*3]);
    /* le vecteur side1 est celui allant du vertex 0 au vertex 2 */
    SCE_Vector3_Operator2v (side1, =, &vertex[index[2]*3],
                                   -, &vertex[index[0]*3]);

    if (texcoord)
    {
        /* calculs douteux mais qui parait-il marchent...
           (et c'est verifie, ils marchent) */
        /* ici, +1 pour prendre la composante v (uv) */
        deltaT0 = texcoord[index[1]*2+1] - texcoord[index[0]*2+1];
        deltaT1 = texcoord[index[2]*2+1] - texcoord[index[0]*2+1];
        /* composante u */
        deltaB0 = texcoord[index[1]*2] - texcoord[index[0]*2];
        deltaB1 = texcoord[index[2]*2] - texcoord[index[0]*2];
        scale = 1.0f / ((deltaB0 * deltaT1) - (deltaB1 * deltaT0));
    }

    if (normals)
    {
        SCE_Vector3_Cross (tmpNormal, side0, side1);
        SCE_Vector3_Normalize (tmpNormal);
        /* affectation */
        SCE_Vector3_Operator1v (&normals[index[0]*3], +=, tmpNormal);
        SCE_Vector3_Operator1v (&normals[index[1]*3], +=, tmpNormal);
        SCE_Vector3_Operator1v (&normals[index[2]*3], +=, tmpNormal);
    }

    if (texcoord && tangents)
    {
        /* on calcule la tangente temporaire */
        SCE_Vector3_Operator1v (tmpTangente, = deltaT1*, side0);
        SCE_Vector3_Operator1v (tmpTangente, -= deltaT0*, side1);
        SCE_Vector3_Operator1 (tmpTangente, *=, scale);
        SCE_Vector3_Normalize (tmpTangente);
        /* affectation (addition) */
        SCE_Vector3_Operator1v (&tangents[index[0]*3], +=, tmpTangente);
        SCE_Vector3_Operator1v (&tangents[index[1]*3], +=, tmpTangente);
        SCE_Vector3_Operator1v (&tangents[index[2]*3], +=, tmpTangente);
    }
    if (texcoord && binormals)
    {
        /* on calcule la binormale temporaire */
        SCE_Vector3_Operator1v (tmpBinormal, = -deltaB1*, side0);
        SCE_Vector3_Operator1v (tmpBinormal, += deltaB0*, side1);
        SCE_Vector3_Operator1 (tmpBinormal, *=, scale);
        SCE_Vector3_Normalize (tmpBinormal);
        /* affectation (addition) */
        SCE_Vector3_Operator1v (&binormals[index[0]*3], +=, tmpBinormal);
        SCE_Vector3_Operator1v (&binormals[index[1]*3], +=, tmpBinormal);
        SCE_Vector3_Operator1v (&binormals[index[2]*3], +=, tmpBinormal);
    }
    SCE_btend ();
}


/**
 * \brief Compute the tangent, binormal and normal for a lot of polygons
 * \param polygon_type the type of the polygons (SCE_TRIANGLES, ...)
 * \param icount number of vertices that \indices dereference
 * \param vcount number of vertices that are availables in
 * \p vertex and \p texcoord
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * This function works like SCE_Mesh_ComputeTriangleTBN(), but on a lot of
 * polygons.
 */
int SCE_Mesh_ComputeTBN (SCEenum polygon_type, SCEvertices *vertex,
                         SCEvertices *texcoord, SCEindices *indices,
                         unsigned int icount, unsigned int vcount,
                         SCEvertices *tangents, SCEvertices *binormals,
                         SCEvertices *normals)
{
    unsigned int i;
    SCEindices *index = NULL; /* indices finaux (index si != NULL sinon i) */
    SCEindices t_indices[3];  /* indices temporaires pour ComputeTriangleTBN */
    unsigned int count;       /* count reel */

    SCE_btstart ();
    /* on suppose que les tableaux tangents, binormals et normals
       sont initialises a des valeurs (vecteurs) nulles (0.0;0.0;0.0) */

    /* creation d'un tableau d'indices temporaire
       si aucun tableau d'indices n'a ete donne */
    if (!indices)
    {
        /* aucun index fourni, creation */
        count = vcount;
        index = SCE_malloc (sizeof *index * count);
        if (!index)
        {
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        /* tableau croissant */
        for (i=0; i<count; i++)
            index[i] = i;
    }
    else
    {
        /* des indices ont ete donnes, on les utilise */
        index = indices;
        count = icount;
    }

    /* suivant le type de polygone, on construit un triangle
       en suivant des regles bien precises, puis en donne
       notre triangle a manger a notre fonction cherie ComputeTriangleTBN */
    switch (polygon_type)
    {
    /* NOTE: penser a modifier SCECTypes.h en consequence */
    case SCE_TRIANGLES:
        /* i+=3: pour chaque triangle */
        for (i=0; i<count; i+=3)
            SCE_Mesh_ComputeTriangleTBN (vertex, texcoord, &index[i],
                                         tangents, binormals, normals);
        break;

    case SCE_TRIANGLE_STRIP:
        for (i=0; i<count-2; i++)
            SCE_Mesh_ComputeTriangleTBN (vertex, texcoord, &index[i],
                                         tangents, binormals, normals);
        break;

    case SCE_TRIANGLE_FAN:
        /* tous les triangles ont un sommet en commun (le premier) donc : */
        t_indices[0] = index[0];

        /* en commencant par le premier sommet apres le sommet commun,
           jusqu'au dernier sommet -1 */
        for (i=1; i<count-1; i++)
        {
            t_indices[1] = index[i];
            t_indices[2] = index[i+1];
            SCE_Mesh_ComputeTriangleTBN (vertex, texcoord, t_indices,
                                         tangents, binormals, normals);
        }
        break;

    case SCE_QUADS:
        /* i+=4: pour chaque quad */
        for (i=0; i<count; i+=4)
        {
            /* 1 quads = 2 triangles, donc : */
            t_indices[0] = index[i];
            t_indices[1] = index[i+1];
            t_indices[2] = index[i+2];
            SCE_Mesh_ComputeTriangleTBN (vertex, texcoord, t_indices,
                                         tangents, binormals, normals);

            t_indices[0] = index[i+2];
            t_indices[1] = index[i+3];
            t_indices[2] = index[i];
            SCE_Mesh_ComputeTriangleTBN (vertex, texcoord, t_indices,
                                         tangents, binormals, normals);
        }
        break;

    default:
        /* type de primitive non supporte, dommage pour l'utilisateur :D */
        if (!indices)
            SCE_free (index);
        Logger_Log (SCE_INVALID_OPERATION);
        Logger_LogMsg ("primitive type unsupported, you must choose one"
                       " of the following types: SCE_TRIANGLES, "
                       "SCE_TRIANGLE_STRIP, SCE_TRIANGLE_FAN or SCE_QUADS");
        SCE_btend ();
        return SCE_ERROR;
    }

    /* on libere ce qu'on a (eventuellement) alloue */
    if (!indices)
        SCE_free (index);

    /* renormalisation des vecteurs */
    if (normals)
        for (i=0; i<vcount; i++)
            SCE_Vector3_Normalize (&normals[i*3]);
    if (binormals)
        for (i=0; i<vcount; i++)
            SCE_Vector3_Normalize (&binormals[i*3]);
    if (tangents)
        for (i=0; i<vcount; i++)
            SCE_Vector3_Normalize (&tangents[i*3]);

    SCE_btend ();
    return SCE_OK;
}

/* renvoie les donnees demandees si elles sont conformes aux demandes
   des fonctions de gestion des donnees */
static SCE_SMeshVertexData* SCE_Mesh_GetVertices (SCE_SMesh *mesh, SCEenum type,
                                                  const char *name)
{
    SCE_SMeshVertexData *v = NULL;
    SCE_btstart ();
    /* on recupere les donnees */
    v = SCE_Mesh_LocateData (mesh, type, NULL);
    if (!v)
    {
        Logger_Log (SCE_INVALID_OPERATION);
        Logger_LogMsg ("any buffer contains %s in this mesh", name);
        SCE_btend ();
        return NULL;
    }
    /* on verifie si les donnees sont du bon type */
    if (v->dec->type != SCE_VERTICES_TYPE)
    {
        Logger_Log (SCE_INVALID_OPERATION);
        Logger_LogMsg ("data type of the %s is not SCE_VERTICES_TYPE", name);
        SCE_btend ();
        return NULL;
    }
    SCE_btend ();
    return v;
}
/* idem que GetVertices, mais pour les indices */
static SCEindices* SCE_Mesh_GetIndices (SCE_SMesh *mesh, int *del)
{
    SCEindices *indices = NULL;
    if (mesh->use_indices)
    {
        if (mesh->ib.type != SCE_INDICES_TYPE)
        {
            indices = SCE_Mem_ConvertDup (SCE_INDICES_TYPE, mesh->ib.type,
                                          mesh->ib.data, mesh->icount);
            *del = SCE_TRUE;
        }
        else
        {
            indices = mesh->ib.data;
            *del = SCE_FALSE;
        }
    }
    return indices;
}

/**
 * \brief Generates tangents, binormals and normals from a mesh
 * \param mesh the mesh that gives the data for the generation
 * \param tangents here is stored the computed tangents, can be NULL
 * \param binormals here is stored the computed binormals, can be NULL
 * \param normals here is written the computed normals, can be NULL
 * \param texunit the texture unit where get the texture coordinates
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_Mesh_GenerateTBN (SCE_SMesh *mesh, SCEvertices *tangents,
                          SCEvertices *binormals, SCEvertices *normals,
                          SCEuint texunit)
{
    SCE_SMeshVertexData *vertex = NULL, *texcoord = NULL, tmp;
    SCEindices *indices = NULL;
    int del = 0; /* kick the warning "used uninitialized" */

    SCE_btstart ();
    if (texunit >= SCE_CGetMaxTextureUnits ())
    {
        /* TODO: message d'erreur frequent, prevoir qq'chose... */
        Logger_Log (SCE_INVALID_ARG);
        Logger_LogMsg ("the texture unit number %d are not available on your "
                       "hardware", texunit);
        SCE_btend ();
        return SCE_ERROR;
    }

    /* on recupere les donnees */
    vertex = SCE_Mesh_GetVertices (mesh, SCE_POSITION, "vertices positions");
    if (tangents || binormals)
        texcoord = SCE_Mesh_GetVertices (mesh, SCE_TEXCOORD0 + texunit,
                                         "texture coordinates");
    else
    {
        SCE_Mesh_InitVertexData (&tmp);
        texcoord = &tmp;
    }

    if (!vertex || !texcoord)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    /* on recupere les indices */
    indices = SCE_Mesh_GetIndices (mesh, &del);

    SCE_Mesh_ComputeTBN (mesh->polygon_type, vertex->data, texcoord->data,
                         indices, mesh->icount, mesh->vcount,
                         tangents, binormals, normals);

    if (del)
        SCE_free (indices);

    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief Adds generated vertex data from a mesh into this
 * \param buf the vertex buffer identifier where store the new data, see
 * SCE_Mesh_RemoveVB() for more informations about this parameter
 * \param gen_type can be set the following flags: SCE_GEN_TANGENTS,
 * SCE_GEN_BINORMALS and SCE_GEN_NORMALS
 * \param attrib_type vertex attribute for the first generated data, is
 * incremented by 1 for each next data generated
 * \param texunit the texture unit where get the texture coordinates
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_Mesh_AddGenVertices (SCE_SMesh *mesh, SCEuint buf, SCEenum gen_type,
                             SCEuint attrib_type, SCEuint texunit)
{
    SCEvertices *t = NULL, *b = NULL, *n = NULL;
    unsigned int i;
    size_t nvars;

    SCE_btstart ();
#define SCE_MESH_DEL()\
    {\
        SCE_free (n);\
        SCE_free (b);\
        SCE_free (t);\
        Logger_LogSrc ();\
        SCE_btend ();\
        return SCE_ERROR;\
    }
    nvars = mesh->vcount * 3;
    if (gen_type & SCE_GEN_TANGENTS)
    {
        t = SCE_malloc (nvars * sizeof *t);
        if (!t)
            SCE_MESH_DEL ()
        for (i=0; i<nvars; i++)
            t[i] = 0.0f;
    }
    if (gen_type & SCE_GEN_BINORMALS)
    {
        b = SCE_malloc (nvars * sizeof *b);
        if (!b)
            SCE_MESH_DEL ()
        for (i=0; i<nvars; i++)
            b[i] = 0.0f;
    }
    if (gen_type & SCE_GEN_NORMALS)
    {
        n = SCE_malloc (nvars * sizeof *n);
        if (!n)
            SCE_MESH_DEL ()
        for (i=0; i<nvars; i++)
            n[i] = 0.0f;
    }

    if (SCE_Mesh_GenerateTBN (mesh, t, b, n, texunit) < 0)
        SCE_MESH_DEL ()

    /** TODO: attrib_type peut devenir trop grand */
    if (gen_type & SCE_GEN_TANGENTS)
    {
        if (SCE_Mesh_AddVertices (mesh, buf, attrib_type, SCE_VERTICES_TYPE,
                                  3, mesh->vcount, t, SCE_TRUE) < 0)
            SCE_MESH_DEL ()
        attrib_type++;
    }
    if (gen_type & SCE_GEN_BINORMALS)
    {
        if (SCE_Mesh_AddVertices (mesh, buf, attrib_type, SCE_VERTICES_TYPE,
                                  3, mesh->vcount, b, SCE_TRUE) < 0)
            SCE_MESH_DEL ()
        attrib_type++;
    }
    if (gen_type & SCE_GEN_NORMALS)
    {
        if (SCE_Mesh_AddVertices (mesh, buf, attrib_type, SCE_VERTICES_TYPE,
                                  3, mesh->vcount, n, SCE_TRUE) < 0)
            SCE_MESH_DEL ()
    }

#undef SCE_MESH_DEL

    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief Compute a bounding box from a lot of vertices' positions
 * \param v the vertices' positions (the vectors must have 3 components)
 * \param vcount the number of vertices to read into \p v
 * \param o the position of the bounding box's origin (point bottom, left, near)
 * \param w bounding box's width
 * \param h bounding box's height
 * \param d bounding box's depth
 * \sa SCE_Mesh_GenerateBoundingBox()
 */
void SCE_Mesh_ComputeBoundingBox (SCEvertices *v, unsigned int vcount,
                                  SCE_TVector3 o, float *w, float *h, float *d)
{
    SCE_TVector3 max = {0., 0., 0.}, min = {0., 0., 0.};
    unsigned int i, p, count;

    /* TODO: utiliser un "Rectangle3D" ? */
    SCE_btstart ();
    count = vcount * 3;
    for (i=0; i<count; i++)
    {
        p = i % 3;
        max[p] = (v[i] > max[p] ? v[i] : max[p]);
        min[p] = (v[i] < min[p] ? v[i] : min[p]);
    }
    SCE_Vector3_Operator1v (o, =, min);
    *w = max[0] - min[0];
    *h = max[1] - min[1];
    *d = max[2] - min[2];
    SCE_btend ();
}
/**
 * \brief Vectorial version of SCE_Mesh_ComputeBoundingBox()
 *
 * Calls SCE_Mesh_ComputeBoundingBox (\p v, \p vcount, \p o, \p &vec[0],
 * \p &vec[1], \p &vec[2])
 */
void SCE_Mesh_ComputeBoundingBoxv (SCEvertices *v, unsigned int vcount,
                                   SCE_TVector3 o, SCE_TVector3 vec)
{
    SCE_Mesh_ComputeBoundingBox (v, vcount, o, &vec[0], &vec[1], &vec[2]);
}
/**
 * \brief Generate the bounding box of a mesh
 * \param box where store the bounding box of \p mesh
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * \sa SCE_Mesh_ComputeBoundingBox()
 */
int SCE_Mesh_GenerateBoundingBox (SCE_SMesh *mesh, SCE_SBoundingBox *box)
{
    SCE_SMeshVertexData *v = NULL;
    SCE_TVector3 o, d;

    SCE_btstart ();
    v = SCE_Mesh_GetVertices (mesh, SCE_POSITION, "vertices positions");
    if (!v)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_Mesh_ComputeBoundingBoxv (v->data, mesh->vcount, o, d);
    SCE_BoundingBox_Setv (box, o, d);
    SCE_btend ();
    return SCE_OK;
}

/**
 * \brief Computes a bounding sphere from a lot of vertices
 * \param v the vertices
 * \param vcount the number of vertices that \p v contains
 * \param center the center of the sphere
 * \param r the radius of the sphere
 *
 * Computes a bounding sphere from a lot of vertices and writes the sphere's
 * properties in \p center and \p r.
 * \sa SCE_Mesh_ComputeBoundingBox(), SCE_Mesh_GenerateBoundingSphere()
 */
void SCE_Mesh_ComputeBoundingSphere (SCEvertices *v, unsigned int vcount,
                                     SCE_TVector3 center, float *r)
{
    SCE_TVector3 tmp;
    float d;
    unsigned int i;

    SCE_btstart ();
    *r = 0.0f;
    /* calcul du centre */
    SCE_Mesh_ComputeBoundingBoxv (v, vcount, center, tmp);
    /* tmp n'est pas un vecteur complet, il faut l'additionner a 'center'
       pour obtenir sa veritable position */
    SCE_Vector3_Operator2v (center, +=, center, +, tmp);
    SCE_Vector3_Operator1 (center, /=, 2.);

    /* determination du point le plus eloigne du centre (donnera le rayon) */
    for (i=0; i<vcount; i++)
    {
        d = SCE_Vector3_Distance (center, &v[i*3]);
        *r = (*r < d ? d : *r);
    }
    SCE_btend ();
}
/**
 * \brief Generates a bounding sphere from a mesh
 * \param mesh the mesh from which to compute the bounding sphere
 * \param s the bounding sphere to set
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * Computes a bounding sphere from a mesh and writes the sphere's
 * properties in \p center and \p r.
 * \sa SCE_Mesh_GenerateBoundingBox(), SCE_Mesh_ComputeBoundingSphere() 
 */
int SCE_Mesh_GenerateBoundingSphere (SCE_SMesh *mesh, SCE_SBoundingSphere *s)
{
    SCE_TVector3 center;
    float radius;
    SCE_SMeshVertexData *v = NULL;

    SCE_btstart ();
    v = SCE_Mesh_GetVertices (mesh, SCE_POSITION, "vertices positions");
    if (!v)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_Mesh_ComputeBoundingSphere (v->data, mesh->vcount, center, &radius);
    SCE_BoundingSphere_Setv (s, center, radius);
    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief Generates vertices positions for a non-indexed cube
 * \param v where to store the vertices
 * \param o the origin of the cube
 * \param w,h,d the dimensions of the cube
 * \todo see bounding box module for generate vertices
 */
void SCE_Mesh_GenerateCubeVertices (SCEvertices v[72], SCE_TVector3 o,
                                    float w, float h, float d)
{
#if 1
    /* far */
    v[0]  = o[0];     v[1]  = o[1];     v[2]  = o[2];
    v[3]  = o[0] + w; v[4]  = o[1];     v[5]  = o[2];
    v[6]  = o[0] + w; v[7]  = o[1] + h; v[8]  = o[2];
    v[9]  = o[0];     v[10] = o[1] + h; v[11] = o[2];
    /* bottom */
    v[12] = o[0];     v[13] = o[1];     v[14] = o[2];
    v[15] = o[0] + w; v[16] = o[1];     v[17] = o[2];
    v[18] = o[0] + w; v[19] = o[1];     v[20] = o[2] + d;
    v[21] = o[0];     v[22] = o[1];     v[23] = o[2] + d;
    /* left */
    v[24] = o[0];     v[25] = o[1];     v[26] = o[2];
    v[27] = o[0];     v[28] = o[1] + h; v[29] = o[2];
    v[30] = o[0];     v[31] = o[1] + h; v[32] = o[2] + d;
    v[33] = o[0];     v[34] = o[1];     v[35] = o[2] + d;
    /* right */
    v[36] = o[0] + w; v[37] = o[1];     v[38] = o[2];
    v[39] = o[0] + w; v[40] = o[1] + h; v[41] = o[2];
    v[42] = o[0] + w; v[43] = o[1] + h; v[44] = o[2] + d;
    v[45] = o[0] + w; v[46] = o[1];     v[47] = o[2] + d;
    /* top */
    v[48] = o[0] + w; v[49] = o[1] + h; v[50] = o[2];
    v[51] = o[0] + w; v[52] = o[1] + h; v[53] = o[2] + d;
    v[54] = o[0];     v[55] = o[1] + h; v[56] = o[2] + d;
    v[57] = o[0];     v[58] = o[1] + h; v[59] = o[2];
    /* near */
    v[60] = o[0] + w; v[61] = o[1] + h; v[62] = o[2] + d;
    v[63] = o[0] + w; v[64] = o[1];     v[65] = o[2] + d;
    v[66] = o[0];     v[67] = o[1];     v[68] = o[2] + d;
    v[69] = o[0];     v[70] = o[1] + h; v[71] = o[2] + d;
#else
    unsigned int i = 0;
    /* near */
    v[i]    = o[0] + w; v[i+1]  = o[1] + h; v[i+2]  = o[2] + d;
    v[i+3]  = o[0] + w; v[i+4]  = o[1];     v[i+5]  = o[2] + d;
    v[i+6]  = o[0];     v[i+7]  = o[1];     v[i+8]  = o[2] + d;
    v[i+9]  = o[0];     v[i+10] = o[1] + h; v[i+11] = o[2] + d;
    i += 12;
    /* bottom */
    v[i]    = o[0];     v[i+1]  = o[1]; v[i+2]  = o[2];
    v[i+3]  = o[0] + w; v[i+4]  = o[1]; v[i+5]  = o[2];
    v[i+6]  = o[0] + w; v[i+7]  = o[1]; v[i+8]  = o[2] + d;
    v[i+9]  = o[0];     v[i+10] = o[1]; v[i+11] = o[2] + d;
    i += 12;
    /* right */
    v[i]    = o[0] + w; v[i+1]  = o[1];     v[i+2]  = o[2];
    v[i+3]  = o[0] + w; v[i+4]  = o[1] + h; v[i+5]  = o[2];
    v[i+6]  = o[0] + w; v[i+7]  = o[1] + h; v[i+8]  = o[2] + d;
    v[i+9]  = o[0] + w; v[i+10] = o[1];     v[i+11] = o[2] + d;
    i += 12;
    /* far */
    v[i]    = o[0];     v[i+1]  = o[1];     v[i+2]  = o[2];
    v[i+3]  = o[0] + w; v[i+4]  = o[1];     v[i+5]  = o[2];
    v[i+6]  = o[0] + w; v[i+7]  = o[1] + h; v[i+8]  = o[2];
    v[i+9]  = o[0];     v[i+10] = o[1] + h; v[i+11] = o[2];
    i += 12;
    /* left */
    v[i]    = o[0]; v[i+1]  = o[1];     v[i+2]  = o[2];
    v[i+3]  = o[0]; v[i+4]  = o[1] + h; v[i+5]  = o[2];
    v[i+6]  = o[0]; v[i+7]  = o[1] + h; v[i+8]  = o[2] + d;
    v[i+9]  = o[0]; v[i+10] = o[1];     v[i+11] = o[2] + d;
    i += 12;
    /* top */
    v[i]    = o[0] + w; v[i+1]  = o[1] + h; v[i+2]  = o[2];
    v[i+3]  = o[0] + w; v[i+4]  = o[1] + h; v[i+5]  = o[2] + d;
    v[i+6]  = o[0];     v[i+7]  = o[1] + h; v[i+8]  = o[2] + d;
    v[i+9]  = o[0];     v[i+10] = o[1] + h; v[i+11] = o[2];
    i += 12;
#endif
}


/**
 * \brief Creates a cube mesh
 * \param o the origin of the cube
 * \param w,h,d dimensions of the cube
 * \returns a new mesh
 * \sa SCE_Mesh_CreateCubev(), SCE_Mesh_CreateIndexedCube()
 */
SCE_SMesh* SCE_Mesh_CreateCube (SCE_TVector3 o, float w, float h, float d)
{
    SCEvertices v[72];
    SCE_SMesh *mesh = NULL;

    SCE_btstart ();
    SCE_Mesh_GenerateCubeVertices (v, o, w, h, d);

#define SCE_ASSERT(c)\
    if (c)\
    {\
        SCE_Mesh_Delete (mesh);\
        Logger_LogSrc ();\
        SCE_btend ();\
        return NULL;\
    }
    SCE_ASSERT (!(mesh = SCE_Mesh_Create ()))
    SCE_ASSERT (SCE_Mesh_AddVerticesDup (mesh, 0, SCE_POSITION,
                                         SCE_VERTICES_TYPE, 3, 24, v) < 0)
    SCE_Mesh_ActivateVB (mesh, 0, SCE_TRUE);
    SCE_ASSERT (SCE_Mesh_Build (mesh) < 0)
    SCE_Mesh_SetRenderMode (mesh, GL_QUADS);
    SCE_btend ();
#undef SCE_ASSERT
    return mesh;
}
/**
 * \brief Creates a cube mesh
 * \param d dimensions of the cube
 * \sa SCE_Mesh_CreateCube(), SCE_Mesh_CreateIndexedCube()
 */
SCE_SMesh* SCE_Mesh_CreateCubev (SCE_TVector3 o, SCE_TVector3 d)
{
    return SCE_Mesh_CreateCube (o, d[0], d[1], d[2]);
}

/**
 * \brief Gets an internal array of indices
 *
 * Gets an internal array of indices, to use with the vertices given by
 * SCE_Mesh_GenerateIndexedCubeVertices(). Do not modify the content of the
 * returned pointer, or get fucked.
 */
const SCEindices* SCE_Mesh_GetIndexedCubeIndices (void)
{
    static SCEindices i[] =
    {
        0, 1, 2, 6,
        0, 1, 4, 7,
        0, 6, 5, 7,
        1, 2, 3, 4,
        2, 3, 5, 6,
        3, 4, 7, 5
    };
    return i;
}

/**
 * \brief Generates vertices positions for an indexed cube
 * \param v where to store the vertices positions
 * \param o the origin of the cube
 * \param w,h,d the dimensions of the cube
 */
void SCE_Mesh_GenerateIndexedCubeVertices (SCEvertices v[24], SCE_TVector3 o,
                                           float w, float h, float d)
{
    v[0]  = o[0];     v[1]  = o[1];     v[2]  = o[2];
    v[3]  = o[0] + w; v[4]  = o[1];     v[5]  = o[2];
    v[6]  = o[0] + w; v[7]  = o[1] + h; v[8]  = o[2];
    v[9]  = o[0] + w; v[10] = o[1] + h; v[11] = o[2] + d;
        
    v[12] = o[0] + w; v[13] = o[1];     v[14] = o[2] + d;
    v[15] = o[0];     v[16] = o[1] + h; v[17] = o[2] + d;
    v[18] = o[0];     v[19] = o[1] + h; v[20] = o[2];
    v[21] = o[0];     v[22] = o[1];     v[23] = o[2] + d;
}

/**
 * \brief Creates a mesh of an indexed cube
 * \param o the origin of the cube
 * \param w,h,d the dimensions of the cube
 * \returns a new mesh
 * \sa SCE_Mesh_CreateIndexedCubev(), SCE_Mesh_CreateCube(),
 * SCE_Mesh_GenerateIndexedCubeVertices(), SCE_Mesh_GetIndexedCubeIndices()
 */
SCE_SMesh* SCE_Mesh_CreateIndexedCube(SCE_TVector3 o, float w, float h, float d)
{
    SCEvertices v[24];
    SCE_SMesh *mesh = NULL;
    const SCEindices *i = SCE_Mesh_GetIndexedCubeIndices ();

    SCE_btstart ();
    SCE_Mesh_GenerateIndexedCubeVertices (v, o, w, h, d);

#define SCE_ASSERT(c)\
    if (c)\
    {\
        SCE_Mesh_Delete (mesh);\
        Logger_LogSrc ();\
        SCE_btend ();\
        return NULL;\
    }
    SCE_ASSERT (!(mesh = SCE_Mesh_Create ()))
    SCE_ASSERT (SCE_Mesh_AddVerticesDup (mesh, 0, SCE_POSITION,
                                         SCE_VERTICES_TYPE, 3, 8, v) < 0)
    SCE_ASSERT (SCE_Mesh_SetIndicesDup (mesh, 0, SCE_INDICES_TYPE, 24, i) < 0)
    SCE_Mesh_ActivateVB (mesh, 0, SCE_TRUE);
    SCE_Mesh_ActivateIB (mesh, SCE_TRUE);
    SCE_ASSERT (SCE_Mesh_Build (mesh) < 0)
    SCE_Mesh_SetRenderMode (mesh, GL_QUADS);
    SCE_btend ();
#undef SCE_ASSERT
    return mesh;
}
/**
 * \brief Creates a mesh of an indexed cube
 * \param d dimensions of the cube
 * \sa SCE_Mesh_CreateIndexedCube(), SCE_Mesh_CreateCube()
 */
SCE_SMesh* SCE_Mesh_CreateIndexedCubev (SCE_TVector3 o, SCE_TVector3 d)
{
    return SCE_Mesh_CreateIndexedCube (o, d[0], d[1], d[2]);
}


#if 0
SCE_SMesh* SCE_Mesh_CreateFromBoundingBox (SCE_SBoundingBox *box)
{
    SCE_SMesh *cube = NULL;
    SCE_TVector3 d;

#define SCE_ASSERT(c)\
    if (c)\
    {\
        Logger_LogSrc ();\
        SCE_btend ();\
        return NULL;\
    }
    SCE_btstart ();
    SCE_BoundingBox_GetDimensionsv (box, d);
    SCE_ASSERT (!(cube=SCE_Mesh_CreateIndexedCubev(
                        SCE_BoundingBox_GetOrigin(box), d)))
    SCE_btend ();
#undef SCE_ASSERT
    return cube;
}
#endif


/**
 * \brief Builds a mesh
 * \param mesh the mesh to build
 *
 * Builds a mesh by calling SCE_Mesh_BuildVB() for each vertex buffer of \p mesh
 * and SCE_Mesh_BuildIB().
 */
int SCE_Mesh_Build (SCE_SMesh *mesh)
{
    SCE_SListIterator *i = NULL;

    SCE_btstart ();
    if (mesh->builded)
    {
        SCE_btend ();
        return SCE_OK;
    }

    SCE_List_ForEach (i, mesh->vertices)
    {
        if (SCE_Mesh_BuildVB (SCE_List_GetData (i)) < 0)
        {
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
    }

    if (SCE_Mesh_BuildIB (&mesh->ib) < 0)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }

    mesh->builded = SCE_TRUE;

    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief Defines the polygon type to use for a mesh
 * \param mode can be SCE_POINTS, SCE_LINES, SCE_LINE_LOOP, SCE_TRIANGLES,
 * SCE_TRIANGLE_STRIP or SCE_TRIANGLE_FAN, SCE_QUADS and SCE_QUAD_STRIP are
 * deprecated
 */
void SCE_Mesh_SetRenderMode (SCE_SMesh *mesh, SCEenum mode)
{
    mesh->polygon_type = mode;
}


/**
 * \brief Updates a vertex buffer of a mesh
 * \param bufid the vertex buffer identifier to update, see SCE_Mesh_RemoveVB()
 * for more details
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * This function updates the vertex buffer specified by \p bufid by calling
 * SCE_CUpdateVertexBuffer().
 */
int SCE_Mesh_Update (SCE_SMesh *mesh, unsigned int bufid)
{
    SCE_SListIterator *i = NULL;
    SCE_SMeshVertexBuffer *b = NULL;

    i = SCE_List_GetIterator (mesh->vertices, bufid);
    if (!i)
    {
        Logger_LogMsg ("any buffer number %u in this mesh", bufid);
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }

    b = SCE_List_GetData (i);
    SCE_CLockVertexBuffer (b->buffer, GL_WRITE_ONLY);
    SCE_CUpdateVertexBuffer (b->buffer);
    SCE_CUnlockVertexBuffer (b->buffer);
    return SCE_OK;
}
/**
 * \brief Register an update to do later in a register list
 * \sa SCE_Mesh_Update() SCE_Mesh_ApplyUpdates()
 */
int SCE_Mesh_LogUpdate (SCE_SMesh *mesh, unsigned int bufid)
{
    SCE_SListIterator *i = SCE_List_GetIterator (mesh->vertices, bufid);
    if (!i)
    {
        Logger_LogMsg ("none buffer number %u in this mesh", bufid);
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }

    upit = SCE_List_GetIterator (updates, num_updates);
    /* pas la peine de tester 'upit', on considere que si une mise a jour est
       demandee c'est que l'utilisateur sait ce qu'il fait :D */
    SCE_List_SetData (upit, SCE_List_GetData (i));
    num_updates++;
    return SCE_OK;
}

/**
 * \brief Apply all the registers updates
 *
 * All the updates register by SCE_Mesh_LogUpdate() are done by calling
 * SCE_Mesh_Update(), then the register's list is cleared
 *
 * \sa SCE_Mesh_LogUpdate()
 */
void SCE_Mesh_ApplyUpdates (void)
{
    SCE_SMeshVertexBuffer *buf = NULL;
    SCE_CVertexBuffer *vb = NULL;

    /* pour chaque buffer a mettre a jour */
    SCE_List_ForEachPrev (upit)
    {
        /* on le met a jour */
        buf = SCE_List_GetData (upit);
        vb = buf->buffer;
        SCE_CLockVertexBuffer (vb, GL_WRITE_ONLY);
        SCE_CUpdateVertexBuffer (vb);
    }
    /* pour chaque buffer a mettre a jour */
    SCE_List_ForEachPrev (upit)
    {
        /* on rend invalide le pointeur */
        buf = SCE_List_GetData (upit);
        vb = buf->buffer;
        SCE_CUnlockVertexBuffer (vb);
    }
    upit = NULL;
    num_updates = 0;
}


/**
 * \brief Loads a lot of meshs from a file
 * \param fname the file name
 * \param n_meshs the number of meshs read
 * \returns an array that contains the read meshs
 *
 * This function uses the resources manager to load the meshs.
 */
SCE_SMesh** SCE_Mesh_Load (const char *fname, int *n_meshs)
{
    SCE_SMesh **meshs = NULL;
    int i, unused = 0;
    size_t size;
    char *subname = NULL;

    SCE_btstart ();
    if (!n_meshs)
        n_meshs = &unused;
    size = strlen (fname) + 8;
    subname = SCE_malloc (size);
    if (!subname)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    meshs = SCE_Resource_Load (fname, n_meshs, NULL);
    if (!meshs)
    {
        SCE_free (subname);
        Logger_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    for (i=0; i<*n_meshs; i++)
    {
        memset (subname, '\0', size);
        strcpy (subname, fname);
        strcat (subname, "_lod");
        strcat (subname, SCE_String_Strof (i, 0));
        /* TODO: et la valeur de retour n'est pas analysee ? */
        SCE_Resource_Add (subname, meshs[i]);
    }
    SCE_free (subname);

    SCE_btend ();
    return meshs;
}


/**
 * \brief Defines the binded mesh
 * \sa SCE_Mesh_Draw
 */
void SCE_Mesh_Use (SCE_SMesh *mesh)
{
    SCE_SListIterator *i = NULL;
    SCE_SMeshVertexBuffer *b = NULL;

    m = mesh;
    if (m)
    {
        SCE_List_ForEach (i, m->vertices)
        {
            b = SCE_List_GetData (i);
            if (b->active)
                SCE_CUseVertexBuffer (b->buffer);
        }
        if (m->use_indices)
            SCE_CUseIndexBuffer (m->ib.buffer);
    }
    else
    {
        SCE_CUseIndexBuffer (NULL);
        SCE_CUseVertexBuffer (NULL);
    }
}

/**
 * \brief Render the binded mesh
 */
void SCE_Mesh_Draw (void)
{
    if (m->use_indices)
        SCE_CDrawIndexedBuffer (m->polygon_type, m->ib.type, 0, m->icount);
    else
        SCE_CDrawBuffer (m->polygon_type, 0, m->vcount);
}

/**
 * \brief Render a mesh
 * \param mesh the mesh to render
 */
void SCE_Mesh_Render (SCE_SMesh *mesh)
{
    SCE_SListIterator *i = NULL;
    SCE_SMeshVertexBuffer *b = NULL;

    /* TODO: prevoir des pointeurs de fonction (+ rapide) */
    SCE_List_ForEach (i, mesh->vertices)
    {
        b = SCE_List_GetData (i);
        if (b->active)
            SCE_CUseVertexBuffer (b->buffer);
    }

    if (mesh->use_indices)
    {
        SCE_CUseIndexBuffer (mesh->ib.buffer);
        SCE_CDrawIndexedBuffer (mesh->polygon_type, mesh->ib.type, 0,
                                mesh->icount);
    }
    else
        SCE_CDrawBuffer (mesh->polygon_type, 0, mesh->vcount);

    /* on re-utilise le mesh par defaut */
    if (m)
        SCE_Mesh_Use (m);
}

/**
 * \deprecated
 * \brief Render an array of meshs
 * \param meshs the meshs' array
 * \param start where begin to read in \p meshs
 * \param n the number of meshs to render
 * \warning \p meshs length must be greater or equal to start+n
 */
void SCE_Mesh_RenderList (SCE_SMesh **meshs, unsigned int start, unsigned int n)
{
    while (start < n)
        SCE_Mesh_Render (meshs[start++]);
}

/** @} */
