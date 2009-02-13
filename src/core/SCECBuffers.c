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
 
/* Cree le : 10 janvier 2007
   derniere modification le 23/08/2008 */

#include <string.h> /* pas la peine de le prendre de utils... a moins que ? */

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECTexture.h>
#include <SCE/core/SCECBuffers.h>

/**
 * \file SCECBuffers.c
 * \copydoc corebuffers
 * 
 * \file SCECBuffers.h
 * \copydoc corebuffers
 */

/**
 * \defgroup corebuffers VBOs and IBOs managment
 * \ingroup core
 * \internal
 * \brief Manage and draw buffers which contains mesh informations
 */

/** @{ */


/* constantes internes */
#define SCE_BUFFER_OFFSET(p) ((char*)NULL + (p))


/* ajoute le 09/10/2007 */
static SCE_CVertexBuffer *vb_binded = NULL;

/* ajoute le 09/10/2007 */
static SCE_CIndexBuffer *ib_binded = NULL;

/* ajoute le 09/10/2007 */
/* utilise dans UseVertexBuffer */
static SCE_CVertexDeclaration *dec_ = NULL;

/* ajoute le 01/03/2008 */
/* types de buffers actives (tous par defaut) */
static int enabled[SCE_TEXCOORD7 + 32]; /* TODO: on ajoute un peu au pif § */


/* ajoute le 01/03/2008 */
/**
 * \brief Initializes the buffers manager
 * \returns the success of the initialisation (always SCE_OK for now)
 */
int SCE_CBufferInit (void)
{
    int i;
    for (i=0; i<SCE_TEXCOORD7 + 32; i++)
        enabled[i] = SCE_TRUE;
    return SCE_OK;
}

/* ajoute le 09/10/2007 */
void SCE_CBindVertexBuffer (SCE_CVertexBuffer *vb)
{
    vb_binded = vb;
}

/* ajoute le 09/10/2007 */
void SCE_CBindIndexBuffer (SCE_CIndexBuffer *ib)
{
    ib_binded = ib;
}


/* ajoute le 09/10/2007 */
/* revise le 28/10/2007 */
/**
 * \brief Initializes a vertex declaration structure
 */
void SCE_CInitVertexDeclaration (SCE_CVertexDeclaration *d)
{
    d->set = NULL;
    d->active = SCE_TRUE;
    d->attrib = SCE_POSITION;
    d->type = SCE_UNSIGNED_BYTE;
    d->size = 1;
    d->first = 0;
    d->stride = 0;
    d->user = SCE_TRUE;
}
/* ajoute le 14/10/2007 */
/**
 * \brief Creates a new vertex declaration structure
 * \returns a pointer to the newly allocated structure, or NULL on error
 */
SCE_CVertexDeclaration* SCE_CCreateVertexDeclaration (void)
{
    SCE_CVertexDeclaration *dec = SCE_malloc (sizeof *dec);
    if (dec)
        SCE_CInitVertexDeclaration (dec);
    else
        Logger_LogSrc ();
    return dec;
}
/* ajoute le 16/10/2007 */
/**
 * \brief Deletes a vertex declaration structure
 */
void SCE_CDeleteVertexDeclaration (void *d)
{
    if (d)
        SCE_free (d);
}

/* ajoute le 09/10/2007 */
/* revise le 16/10/207 */
/**
 * \brief Initializes a buffer data structure
 */
void SCE_CInitBufferData (SCE_CBufferData *d)
{
    d->data = NULL;
    d->data_size = 0;
    d->first = 0;
    d->user = SCE_TRUE;
}
/* ajoute le 16/10/2007 */
/**
 * \brief Creates a new buffer data structure
 * \returns a pointer to the newly allocated structure, or NULL on error
 */
SCE_CBufferData* SCE_CCreateBufferData (void)
{
    SCE_CBufferData *d = SCE_malloc (sizeof *d);
    if (!d)
        Logger_LogSrc ();
    else
        SCE_CInitBufferData (d);
    return d;
}
/* revise le 16/10/2007 */
/**
 * \brief Deletes a buffer data structure
 */
void SCE_CDeleteBufferData (void *d)
{
    SCE_btstart ();
    if (d)
    {
        SCE_CBufferData *data = d;
        if (!data->user)
            SCE_free (data->data);
        SCE_free (data);
    }
    SCE_btend ();
}

/* ajoute le 09/10/2007 */
/* revise le 16/10/2007 */
/**
 * \brief Initializes a vertex buffer structure
 * \param vb structure to initialize
 */
void SCE_CInitVertexBuffer (SCE_CVertexBuffer *vb)
{
    vb->id = 0;
    vb->data = NULL;
    vb->size = 0;
    vb->decs = NULL;
}

/* ajoute le 09/10/2007 */
/* revise le 16/10/2007 */
/**
 * \brief Initializes an index buffer structure
 * \param ib structure to initialize
 */
void SCE_CInitIndexBuffer (SCE_CIndexBuffer *ib)
{
    ib->id = 0;
    ib->size = 0;
    ib->data = NULL;
}


/* revise le 16/10/2007 */
/**
 * \brief Creates a new vertex buffer
 * \returns the pointer to the new vertex buffer
 */
SCE_CVertexBuffer* SCE_CCreateVertexBuffer (void)
{
    SCE_CVertexBuffer *vb = NULL;
    SCE_btstart ();
    vb = SCE_malloc (sizeof *vb);
    if (!vb)
        Logger_LogSrc ();
    else
    {
        SCE_CInitVertexBuffer (vb);
        vb->data = SCE_List_Create (SCE_CDeleteBufferData);
        if (!vb->data)
        {
            SCE_CDeleteVertexBuffer (vb);
            Logger_LogSrc ();
            SCE_btend ();
            return NULL;
        }
        vb->decs = SCE_List_Create (NULL);
        if (!vb->decs)
        {
            SCE_CDeleteVertexBuffer (vb);
            vb = NULL;
            Logger_LogSrc ();
        }
        glGenBuffersARB (1, &vb->id);
    }
    SCE_btend ();
    return vb;
}

/* revise le 16/10/2007 */
/**
 * \brief Creates an index buffer
 * \returns the pointer to the new index buffer
 */
SCE_CIndexBuffer* SCE_CCreateIndexBuffer (void)
{
    SCE_CIndexBuffer *ib = NULL;
    SCE_btstart ();
    ib = SCE_malloc (sizeof *ib);
    if (!ib)
        Logger_LogSrc ();
    else
    {
        SCE_CInitIndexBuffer (ib);
        glGenBuffersARB (1, &ib->id);
        ib->data = SCE_List_Create (SCE_CDeleteBufferData);
        if (!ib->data)
        {
            SCE_CDeleteIndexBuffer (ib);
            ib = NULL;
            Logger_LogSrc ();
        }
    }
    SCE_btend ();
    return ib;
}


#define SCE_CDEFAULTVBFUNC(action)\
SCE_CVertexBuffer *back = vb_binded;\
SCE_CBindVertexBuffer (vb);\
action;\
SCE_CBindVertexBuffer (back);

#define SCE_CDEFAULTVBFUNCR(t, action)\
t r;\
SCE_CVertexBuffer *back = vb_binded;\
SCE_CBindVertexBuffer (vb);\
r = action;\
SCE_CBindVertexBuffer (back);\
return r;

#define SCE_CDEFAULTIBFUNC(action)\
SCE_CIndexBuffer *back = ib_binded;\
SCE_CBindIndexBuffer (ib);\
action;\
SCE_CBindIndexBuffer (back);

#define SCE_CDEFAULTIBFUNCR(t, action)\
t r;\
SCE_CIndexBuffer *back = ib_binded;\
SCE_CBindIndexBuffer (ib);\
r = action;\
SCE_CBindIndexBuffer (back);\
return r;


/**
 * \brief Deletes a vertex buffer
 * \param vb structure to delete
 */
void SCE_CDeleteVertexBuffer (SCE_CVertexBuffer *vb)
{
    SCE_CDEFAULTVBFUNC (SCE_CDeleteVertexBuffer_ ())
}
void SCE_CDeleteVertexBuffer_ (void)
{
    SCE_btstart ();
    if (vb_binded)
    {
        glDeleteBuffersARB (1, &vb_binded->id);

        SCE_CClearVertexBuffer_ ();
        SCE_List_Delete (vb_binded->decs);
        SCE_List_Delete (vb_binded->data);

        SCE_free (vb_binded);
        vb_binded = NULL;
    }
    SCE_btend ();
}

/**
 * \brief Deletes an index buffer
 * \param ib index buffer to delete
 */
void SCE_CDeleteIndexBuffer (SCE_CIndexBuffer *ib)
{
    SCE_CDEFAULTIBFUNC (SCE_CDeleteIndexBuffer_ ())
}
void SCE_CDeleteIndexBuffer_ (void)
{
    SCE_btstart ();
    if (ib_binded)
    {
        glDeleteBuffersARB (1, &ib_binded->id);
        SCE_List_Delete (ib_binded->data);
        SCE_free (ib_binded);
        ib_binded = NULL;
    }
    SCE_btend ();
}


/* ajoute le 06/01/2008 */
/**
 * \brief Clears a vertex buffer
 * \param vb vertex buffer to clear
 *
 * Clears the content of the given vertex buffer, clears the data
 * pointers' list and all its vertex declarations. The user's vertex
 * declarations are not freeds. After call, \p vb is like a new vertex buffer
 * created by SCE_CCreateVertexBuffer().
 */
void SCE_CClearVertexBuffer (SCE_CVertexBuffer *vb)
{
    SCE_CDEFAULTVBFUNC (SCE_CClearVertexBuffer_ ())
}
/* ajoute le 06/01/2008 */
void SCE_CClearVertexBuffer_ (void)
{
    SCE_CVertexDeclaration *dec = NULL;
    SCE_SListIterator *i = NULL;

    SCE_btstart ();
    vb_binded->size = 0;
    SCE_List_ForEach (i, vb_binded->decs)
    {
        dec = SCE_List_GetData (i);
        /* si cette declaration n'est pas a l'utilisateur */
        if (!dec->user)
            SCE_CDeleteVertexDeclaration (dec); /* on peut la supprimer */
    }
    SCE_List_Clear (vb_binded->decs);
    SCE_List_Clear (vb_binded->data);
    SCE_btend ();
}

/* ajoute le 06/01/2008 */
/**
 * \brief Clears an index buffer
 * \param ib index buffer to clear
 * 
 * Clears the data pointers' list. After call, \p ib is like a new index buffer
 * created by SCE_CCreateIndexBuffer().
 */
void SCE_CClearIndexBuffer (SCE_CIndexBuffer *ib)
{
    SCE_btstart ();
    SCE_List_Clear (ib->data);
    ib->size = 0;
    SCE_btend ();
}
/* ajoute le 06/01/2008 */
void SCE_CClearIndexBuffer_ (void)
{
    SCE_btstart ();
    SCE_List_Clear (ib_binded->data);
    ib_binded->size = 0;
    SCE_btend ();
}


/* ajoute le 09/10/2007 */
/**
 * \brief Adds new data for the given vertex buffer
 * \param data_size the size of \p data in bytes
 * \param data pointer to the data
 * \returns SCE_OK on success or SCE_ERROR otherwise
 *
 * The given pointer \p data will not be freed at structure deletion
 */
int SCE_CAddVertexBufferData (SCE_CVertexBuffer *vb,
                              size_t data_size, void *data)
{
    SCE_CDEFAULTVBFUNCR (int, SCE_CAddVertexBufferData_ (data_size, data))
}
/* revise le 16/10/2007 */
int SCE_CAddVertexBufferData_ (size_t data_size, void *data)
{
    SCE_btstart ();
    if (data)
    {
        SCE_CBufferData *d = SCE_CCreateBufferData ();
        if (!d)
        {
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        if (SCE_List_PrependNewl (vb_binded->data, d) < 0)
        {
            SCE_CDeleteBufferData (d);
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        
        /* affectation */
        d->data_size = data_size;
        d->data = data;
        d->first = vb_binded->size;

        /* par addition cumulative, pour empiler les donnees dans le buffer */
        vb_binded->size += data_size;
    }

    SCE_btend ();
    return SCE_OK;
}
/* ajoute le 09/10/2007 */
/**
 * \brief Duplicates and adds new data to a vertex buffer
 * \returns SCE_OK on success or SCE_ERROR otherwise
 * \sa SCE_CAddVertexBufferData()
 */
int SCE_CAddVertexBufferDataDup (SCE_CVertexBuffer *vb,
                                 size_t data_size, void *data)
{
    SCE_CDEFAULTVBFUNCR (int, SCE_CAddVertexBufferDataDup_ (data_size, data))
}
/* ajoute le 09/10/2007 */
/* revise le 16/10/2007 */
int SCE_CAddVertexBufferDataDup_ (size_t data_size, void *data)
{
    SCE_btstart ();
    if (data)
    {
        SCE_CBufferData *d = NULL;
        void *new = SCE_Mem_Dup (data, data_size);
        if (!new)
        {
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        if (SCE_CAddVertexBufferData_ (data_size, new) < 0)
        {
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        /* les donnees ne sont _pas_ a l'utilisateur */
        d = SCE_List_GetData (SCE_List_GetLast (vb_binded->data));
        d->user = SCE_FALSE;
    }
    SCE_btend ();
    return SCE_OK;
}

/* ajoute le 09/10/2007 */
/**
 * \brief Adds new data for the given index buffer
 * \param data_size the size of \p data in bytes
 * \param data pointer to the data
 * \returns SCE_OK on success or SCE_ERROR otherwise
 *
 * The given pointer \p data will not be freed at structure deletion
 */
int SCE_CAddIndexBufferData (SCE_CIndexBuffer *ib, size_t data_size, void *data)
{
    SCE_CDEFAULTIBFUNCR (int, SCE_CAddIndexBufferData_ (data_size, data))
}
/* revise le 16/10/2007 */
int SCE_CAddIndexBufferData_ (size_t data_size, void *data)
{
    SCE_btstart ();
    if (data)
    {
        SCE_CBufferData *d = SCE_CCreateBufferData ();
        if (!d)
        {
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        if (SCE_List_PrependNewl (ib_binded->data, d) < 0)
        {
            SCE_CDeleteBufferData (d);
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }

        /* affectation */
        d->data_size = data_size;
        d->data = data;
        d->first = ib_binded->size;

        /* par addition cumulative, pour empiler les donnes dans le buffer */
        ib_binded->size += data_size;
    }

    SCE_btend ();
    return SCE_OK;
}
/* ajoute le 09/10/2007 */
/**
 * \brief Duplicates and adds new data to an index buffer
 * \returns SCE_OK on success or SCE_ERROR otherwise
 * \sa SCE_CAddIndexBufferData()
 */
int SCE_CAddIndexBufferDataDup (SCE_CIndexBuffer *ib,
                                size_t data_size, void *data)
{
    SCE_CDEFAULTIBFUNCR (int, SCE_CAddIndexBufferDataDup_ (data_size, data))
}
/* ajoute le 09/10/2007 */
/* revise le 16/10/2007 */
int SCE_CAddIndexBufferDataDup_ (size_t data_size, void *data)
{
    SCE_CBufferData *d = NULL;
    void *new = NULL;

    SCE_btstart ();
    new = SCE_Mem_Dup (data, data_size);
    if (!new)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_CAddIndexBufferData_ (data_size, new) < 0)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    /* les donnees ne sont _pas_ a l'utilisateur */
    d = SCE_List_GetData (SCE_List_GetLast (ib_binded->data));
    d->user = SCE_FALSE;
    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief Get the first data's position available
 * \returns the index of the position
 * \todo da shit doc
 */
size_t SCE_CGetVertexBufferFirst (SCE_CVertexBuffer *vb)
{
    if (vb)
        return vb->size;
    return 0;
}

/* ajoute le 01/03/2008 */
void SCE_CActivateBufferType (SCEenum type, int active)
{
    enabled[type-SCE_COLOR] = active;
}
/**
 * \brief Enable a data type for render
 */
void SCE_CEnableBufferType (SCEenum type)
{
    enabled[type-SCE_COLOR] = SCE_TRUE;
}
/**
 * \brief Disable a data type for render
 */
void SCE_CDisableBufferType (SCEenum type)
{
    enabled[type-SCE_COLOR] = SCE_FALSE;
}

/* ajoute le 09/10/2007 */
static void SCE_CPositionBufferFunc (void)
{
    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (dec_->size, dec_->type, dec_->stride,
                     SCE_BUFFER_OFFSET (dec_->first));
}
/* ajoute le 09/10/2007 */
static void SCE_CNormalBufferFunc (void)
{
    if (enabled[SCE_NORMAL-SCE_COLOR])
    {
        glEnableClientState (GL_NORMAL_ARRAY);
        glNormalPointer (dec_->type, dec_->stride,
                         SCE_BUFFER_OFFSET (dec_->first));
    }
}
/* ajoute le 09/10/2007 */
static void SCE_CColorBufferFunc (void)
{
    if (enabled[0/*SCE_COLOR-SCE_COLOR*/])
    {
        glEnableClientState (GL_COLOR_ARRAY);
        glColorPointer (dec_->size, dec_->type, dec_->stride,
                        SCE_BUFFER_OFFSET (dec_->first));
    }
}
/* ajoute le 09/10/2007 */
static void SCE_CTexcoordBufferFunc (void)
{
    if (enabled[SCE_TEXCOORD0+dec_->attrib-SCE_COLOR])
    {
        glClientActiveTexture (GL_TEXTURE0 + dec_->attrib - SCE_TEXCOORD0);
        glEnableClientState (GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer (dec_->size, dec_->type, dec_->stride,
                           SCE_BUFFER_OFFSET(dec_->first));
    }
}
/* ajoute le 09/10/2007 */
static void SCE_CAttribBufferFunc (void)
{
    int attrib = dec_->attrib - SCE_ATTRIB0;

/**
 * \todo a deplacer
 */
#ifdef SCE_DEBUG
    if (attrib >= /*max_attribs*/16)
    {
        Logger_Log (SCE_INVALID_SIZE);
        Logger_LogMsg ("you can't set an vertex attribute "
                       "if his index is greater than %d",
                       /*max_attribs*/ 16);
    }
#endif

    glEnableVertexAttribArray (attrib);
    glVertexAttribPointer (attrib, dec_->size, dec_->type, 0, dec_->stride,
                           SCE_BUFFER_OFFSET (dec_->first));
}

/* ajoute le 16/10/2007 */
/**
 * \brief Builds a vertex declaration
 * \param dec the vertex declaration
 */
void SCE_CBuildVertexDeclaration (SCE_CVertexDeclaration *dec)
{
    SCE_btstart ();
    /* attribution de la bonne fonction d'envoi des informations */
    switch (dec->attrib)
    {
    case SCE_POSITION: dec->set = SCE_CPositionBufferFunc; break;
    case SCE_NORMAL: dec->set = SCE_CNormalBufferFunc; break;
    case SCE_COLOR: dec->set = SCE_CColorBufferFunc; break;
    default:
        if (dec->attrib >= SCE_TEXCOORD0 &&
            dec->attrib <= SCE_CGetMaxTextureUnits () + SCE_TEXCOORD0)
            dec->set = SCE_CTexcoordBufferFunc;
        else if (dec->attrib >= SCE_ATTRIB0)
            dec->set = SCE_CAttribBufferFunc;
    }
    SCE_btend ();
}

/* ajoute le 10/10/2007 */
/**
 * \brief Adds a vertex declaration to a vertex buffer
 * \param vb the vertex buffer
 * \param dec the vertex declaration to add to \p vb
 * \note the vertex declaration will not be freed at \p vb's deletion
 */
int SCE_CAddVertexDeclaration (SCE_CVertexBuffer *vb,
                               SCE_CVertexDeclaration *dec)
{
    SCE_CDEFAULTVBFUNCR (int, SCE_CAddVertexDeclaration_ (dec))
}
/* revise le 16/10/2007 */
int SCE_CAddVertexDeclaration_ (SCE_CVertexDeclaration *dec)
{
    SCE_btstart ();
    if (dec)
    {
        if (SCE_List_PrependNewl (vb_binded->decs, dec) < 0)
        {
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        SCE_CBuildVertexDeclaration (dec);
    }
    SCE_btend ();
    return SCE_OK;
}

/* ajoute le 10/10/2007 */
/**
 * \brief Duplicates and adds a vertex declaration to a vertex buffer
 * \param vb the vertex buffer
 * \param dec the vertex declaration to add to \p vb
 * \sa SCE_CAddVertexDeclaration()
 */
int SCE_CAddVertexDeclarationDup (SCE_CVertexBuffer *vb,
                                  SCE_CVertexDeclaration *dec)
{
    SCE_CDEFAULTVBFUNCR (int, SCE_CAddVertexDeclarationDup_ (dec))
}
/* ajoute le 10/10/2007 */
int SCE_CAddVertexDeclarationDup_ (SCE_CVertexDeclaration *dec)
{
    SCE_CVertexDeclaration *new = NULL;
    SCE_btstart ();
    new = SCE_Mem_Dup (dec, sizeof *dec);
    if (!new)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    new->user = SCE_FALSE;
    SCE_btend ();
    return SCE_CAddVertexDeclaration_ (new);
}


/* ajoute le 09/10/2007 */
/**
 * \brief Builds a vertex buffer
 * \param vb the vertex buffer to build
 * \param usage the GL usage of the buffer, you can set... ahem, what U can set ? :-'
 */
void SCE_CBuildVertexBuffer (SCE_CVertexBuffer *vb, SCEenum usage)
{
    SCE_CDEFAULTVBFUNC (SCE_CBuildVertexBuffer_ (usage))
}
/* revise le 16/10/2007 */
void SCE_CBuildVertexBuffer_ (SCEenum usage)
{
    SCE_SListIterator *i = NULL;
    SCE_CBufferData *d = NULL;

    SCE_btstart ();
    glBindBufferARB (GL_ARRAY_BUFFER, vb_binded->id);
    glBufferDataARB (GL_ARRAY_BUFFER, vb_binded->size, NULL, usage);

    SCE_List_ForEach (i, vb_binded->data)
    {
        d = SCE_List_GetData (i);
        glBufferSubDataARB (GL_ARRAY_BUFFER, d->first, d->data_size, d->data);
    }
    SCE_btend ();
}


/* ajoute le 09/10/2007 */
/**
 * \brief Builds an index buffer
 * \param vb the index buffer to build
 * \param usage the GL usage of the buffer, you can set... ahem, what U can set ? :-'
 */
void SCE_CBuildIndexBuffer (SCE_CIndexBuffer *ib, SCEenum usage)
{
    SCE_CDEFAULTIBFUNC (SCE_CBuildIndexBuffer_ (usage))
}
/* revise le 09/10/2007 */
void SCE_CBuildIndexBuffer_ (SCEenum usage)
{
    SCE_SListIterator *i = NULL;
    SCE_CBufferData *d = NULL;

    SCE_btstart ();
    glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER, ib_binded->id);
    glBufferDataARB (GL_ELEMENT_ARRAY_BUFFER, ib_binded->size, NULL, usage);

    SCE_List_ForEach (i, ib_binded->data)
    {
        d = SCE_List_GetData (i);
        glBufferSubDataARB (GL_ELEMENT_ARRAY_BUFFER,
                            d->first, d->data_size, d->data);
    }
    SCE_btend ();
}


/* ajoute le 09/10/2007 */
/* revise le 05/05/2008 */
/**
 * \brief Locks a vertex buffer
 * \param vb the vertex buffer to lock
 * \param mode the lock mode, you can set GL_READ_ONLY, GL_WRITE_ONLY or
 * GL_READ_WRITE
 * \sa SCE_CUnlockVertexBuffer()
 * 
 * This function calls glMapBuffer and save a pointer to a modifiable
 * memory.
 */
void SCE_CLockVertexBuffer (SCE_CVertexBuffer *vb, SCEenum mode)
{
    glBindBufferARB (GL_ARRAY_BUFFER, vb->id);
    vb->mapptr = glMapBufferARB (GL_ARRAY_BUFFER, mode);
}
/* revise le 05/05/2008 */
/**
 * \brief Unlocks a vertex buffer
 * \param vb the vertex buffer to unlock
 * \sa SCE_CLockVertexBuffer()
 * 
 * This function calls glUnmapBuffer.
 */
void SCE_CUnlockVertexBuffer (SCE_CVertexBuffer *vb)
{
    glBindBufferARB (GL_ARRAY_BUFFER, vb->id);
    glUnmapBufferARB (GL_ARRAY_BUFFER);
    vb->mapptr = NULL;
}

/* ajoute le 10/10/2007 */
/* revise le 05/05/2008 */
/**
 * \brief Locks an index buffer
 * \param ib the index buffer to lock
 * \param mode the lock mode, you can set GL_READ_ONLY, GL_WRITE_ONLY or
 * GL_READ_WRITE
 * \sa SCE_CUnlockIndexBuffer()
 * 
 * This function calls glMapBuffer and save a pointer to a modifiable
 * memory.
 */
void SCE_CLockIndexBuffer (SCE_CIndexBuffer *ib, SCEenum mode)
{
    glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER, ib->id);
    ib->mapptr = glMapBufferARB (GL_ELEMENT_ARRAY_BUFFER, mode);
}
/* revise le 05/05/2008 */
/**
 * \brief Unlocks a index buffer
 * \param ib the index buffer to unlock
 * \sa SCE_CLockIndexBuffer()
 * 
 * This function calls glUnmapBuffer.
 */
void SCE_CUnlockIndexBuffer (SCE_CIndexBuffer *ib)
{
    glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER, ib->id);
    glUnmapBufferARB (GL_ELEMENT_ARRAY_BUFFER);
    ib->mapptr = NULL;
}


/* ajoute le 10/10/2007 */
/* revise le 05/05/2008 */
/**
 * \brief Updates a vertex buffer
 * \param vb the vertex buffer to update
 * \sa SCE_CLockVertexBuffer() SCE_CUnlockVertexBuffer()
 * 
 * Copy all the data set with SCE_CAddVertexBufferData() into the vertex
 * buffer. It is necessary only if you changed yours data. This function
 * requires you called SCE_CLockVertexBuffer() with GL_WRITE_ONLY or
 * GL_READ_WRITE before.
 */
void SCE_CUpdateVertexBuffer (SCE_CVertexBuffer *vb)
{
    SCE_CBufferData *d = NULL;
    SCE_SListIterator *i = NULL;
    SCE_List_ForEach (i, vb->data)
    {
        d = SCE_List_GetData (i);
        memcpy (((char*)vb->mapptr) + d->first, d->data, d->data_size);
    }
}
/* ajoute le 10/10/2007 */
/* revise le 05/05/2008 */
/**
 * \brief Updates an index buffer
 * \param vb the index buffer to update
 * \sa SCE_CLockIndexBuffer() SCE_CUnlockIndexBuffer()
 * 
 * Copy all the data set with SCE_CAddIndexBufferData() into the index
 * buffer. It is necessary only if you changed yours data. This function
 * requires you called SCE_CLockIndexBuffer() with GL_WRITE_ONLY or
 * GL_READ_WRITE before.
 */
void SCE_CUpdateIndexBuffer (SCE_CIndexBuffer *ib)
{
    SCE_CBufferData *d = NULL;
    SCE_SListIterator *i = NULL;
    SCE_List_ForEach (i, ib->data)
    {
        d = SCE_List_GetData (i);
        memcpy (((char*)ib->mapptr) + d->first, d->data, d->data_size);
    }
}


/* revise le 28/10/2007 */
/**
 * \brief Defines the vertex buffer used for the render
 */
void SCE_CUseVertexBuffer (SCE_CVertexBuffer *vb)
{
    const SCEuint max_attribs = SCE_CGetInteger (GL_MAX_VERTEX_ATTRIBS);
    SCE_SListIterator *it = NULL;

    if (!vb)
    {
        unsigned int i = 0;
        /* par precaution, on desactive tout */
        glDisableClientState (GL_VERTEX_ARRAY);
        glDisableClientState (GL_NORMAL_ARRAY);
        glDisableClientState (GL_COLOR_ARRAY);
        glDisableClientState (GL_TEXTURE_COORD_ARRAY);
        for (; i < max_attribs; i++)
            glDisableVertexAttribArray (i);
        return;
    }

    glBindBufferARB (GL_ARRAY_BUFFER, vb->id);
    SCE_List_ForEach (it, vb->decs)
    {
        dec_ = SCE_List_GetData (it);
        if (dec_->active)
            dec_->set ();
    }
}

/**
 * \brief Defines the index buffer used for the render
 */
void SCE_CUseIndexBuffer (SCE_CIndexBuffer *ib)
{
    if (ib)
        glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER, ib->id);
}


/**
 * \brief Draws the used vertex buffer ignoring the index buffer
 * \param p_type type of the polygons, this can be set at SCE_POINTS, SCE_LINES,
 * SCE_TRIANGLES, SCE_QUADS or SCE_POLYGON
 * \param first the first vertex to begin to read for the render
 * \param count the number of vertices to read for the render
 * \sa SCE_CDrawIndexedBuffer()
 */
void SCE_CDrawBuffer (SCEenum p_type, GLint first, GLsizei count)
{
    glDrawArrays (p_type, first, count);
}

/* revise le 15/10/2007 */
/**
 * \brief Draws the used vertex buffer with the used index buffer
 * \param p_type type of the polygons, this can be set at SCE_POINTS, SCE_LINES,
 * SCE_TRIANGLES, SCE_QUADS or SCE_POLYGON
 * \param i_type the data type of the indices
 * \param first the first vertex to begin to read for the render
 * \param count the number of vertices to read for the render
 */
void SCE_CDrawIndexedBuffer (SCEenum p_type, SCEenum i_type,
                             size_t first, GLsizei count)
{
    glDrawElements (p_type, count, i_type, SCE_BUFFER_OFFSET (first));
}

/** @} */
