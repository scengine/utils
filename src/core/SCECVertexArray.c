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
 
/* created: 26/07/2009
   updated: 02/08/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECVertexArray.h>

/**
 * \file SCECVertexArray.c
 * \copydoc
 * 
 * \file SCECVertexArray.h
 * \copydoc
 */

/**
 * \defgroup vertexarray GL vertex arrays
 * \ingroup core
 * \internal
 * \brief OpenGL vertex arrays
 * @{
 */

static SCE_SList vaused;
static int vao_used = SCE_FALSE;

int SCE_CVertexArrayInit (void)
{
    SCE_List_Init (&vaused);
    return SCE_OK;
}
void SCE_CVertexArrayQuit (void)
{
    SCE_List_Flush (&vaused);
}

void SCE_CInitVertexArrayData (SCE_CVertexArrayData *data)
{
    data->attrib = SCE_POSITION;
    data->type = SCE_FLOAT;
    data->stride = 0;
    data->size = 3;
    data->data = NULL;
}
SCE_CVertexArrayData* SCE_CCreateVertexArrayData (void)
{
    SCE_CVertexArrayData *data = NULL;
    if (!(data = SCE_malloc (sizeof *data)))
        SCEE_LogSrc ();
    else
        SCE_CInitVertexArrayData (data);
    return data;
}
void SCE_CDeleteVertexArrayData (SCE_CVertexArrayData *data)
{
    if (data) {
        SCE_free (data);
    }
}

/* TODO: These functions are not in GL 3.1 */
static void SCE_CSetVAPos (SCE_CVertexArrayData *data)
{
    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (data->size, data->type, data->stride, data->data);
}
static void SCE_CUnsetVAPos (SCE_CVertexArrayData *data)
{
    glDisableClientState (GL_VERTEX_ARRAY);
}
static void SCE_CSetVANor (SCE_CVertexArrayData *data)
{
    glEnableClientState (GL_NORMAL_ARRAY);
    glNormalPointer (data->type, data->stride, data->data);
}
static void SCE_CUnsetVANor (SCE_CVertexArrayData *data)
{
    glDisableClientState (GL_NORMAL_ARRAY);
}
static void SCE_CSetVACol (SCE_CVertexArrayData *data)
{
    glEnableClientState (GL_COLOR_ARRAY);
    glColorPointer (data->size, data->type, data->stride, data->data);
}
static void SCE_CUnsetVACol (SCE_CVertexArrayData *data)
{
    glDisableClientState (GL_COLOR_ARRAY);
}
static void SCE_CSetVATex (SCE_CVertexArrayData *data)
{
    glClientActiveTexture (GL_TEXTURE0 + data->attrib - SCE_TEXCOORD0);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer (data->size, data->type, data->stride, data->data);
}
static void SCE_CUnsetVATex (SCE_CVertexArrayData *data)
{
    glClientActiveTexture (GL_TEXTURE0 + data->attrib - SCE_TEXCOORD0);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}
/****/
static void SCE_CSetVAAtt (SCE_CVertexArrayData *data)
{
    /* hope that data->attrib isn't too large */
    int attrib = data->attrib - SCE_ATTRIB0;
    glEnableVertexAttribArray (attrib);
    glVertexAttribPointer (attrib, data->size, data->type, 0,
                           data->stride, data->data);
}
static void SCE_CUnsetVAAtt (SCE_CVertexArrayData *data)
{
    glDisableVertexAttribArray (data->attrib - SCE_ATTRIB0);
}

/**
 * \brief
 */
void SCE_CInitVertexArray (SCE_CVertexArray *va)
{
    va->id = 0;
    va->set = SCE_CSetVAPos;
    va->unset = SCE_CUnsetVAPos;
    SCE_CInitVertexArrayData (&va->data);
    SCE_List_InitIt (&va->it);
    SCE_List_SetData (&va->it, va);
}
/**
 * \brief
 */
SCE_CVertexArray* SCE_CCreateVertexArray (void)
{
    SCE_CVertexArray *va = NULL;
    if (!(va = SCE_malloc (sizeof *va)))
        SCEE_LogSrc ();
    else
        SCE_CInitVertexArray (va);
    return va;
}
void SCE_CClearVertexArray (SCE_CVertexArray *va)
{
    if (va->id != 0)
        glDeleteArrays (1, &va->id);
    /* NOTE: cannot remove the iterator safely, so NEVER clear a vertex array
       on rendering stage */
}
/**
 * \brief
 */
void SCE_CDeleteVertexArray (SCE_CVertexArray *va)
{
    if (va) {
        SCE_CClearVertexArray (va);
        SCE_free (va);
    }
}

/**
 * \brief
 */
void SCE_CInitIndexArray (SCE_CIndexArray *ia)
{
    ia->type = SCE_UNSIGNED_BYTE;
    ia->data = NULL;
}
/**
 * \brief
 */
SCE_CIndexArray* SCE_CCreateIndexArray (void)
{
    SCE_CIndexArray *ia = NULL;
    if (!(ia = SCE_malloc (sizeof *ia)))
        SCEE_LogSrc ();
    else
        SCE_CInitIndexArray (ia);
    return ia;
}
/**
 * \brief
 */
void SCE_CDeleteIndexArray (SCE_CIndexArray *ia)
{
    if (ia) {
        SCE_free (ia);
    }
}


/**
 * \brief Gets \p &va->data
 * \sa SCE_CSetVertexArrayData(), SCE_CSetVertexArrayNewData()
 */
SCE_CVertexArrayData* SCE_CGetVertexArrayData (SCE_CVertexArray *va)
{
    return &va->data;
}
/**
 * \brief Defines data for a vertex array
 *
 * Copies \p data into \p va->data, so \p data can be a static structure.
 * \sa SCE_CSetVertexArrayNewData(), SCE_CGetVertexArrayData()
 */
void SCE_CSetVertexArrayData (SCE_CVertexArray *va, SCE_CVertexArrayData *data)
{
    va->data = *data;
    switch (data->attrib) {
    case SCE_POSITION:
        va->set = SCE_CSetVAPos;
        va->unset = SCE_CUnsetVAPos;
        break;
    case SCE_NORMAL:
        va->set = SCE_CSetVANor;
        va->unset = SCE_CUnsetVANor;
        break;
    case SCE_COLOR:
        va->set = SCE_CSetVACol;
        va->unset = SCE_CUnsetVACol;
        break;
    default:
        if (data->attrib >= SCE_TEXCOORD0 &&
            data->attrib <= SCE_CGetMaxTextureUnits () + SCE_TEXCOORD0) {
            va->set = SCE_CSetVATex;
            va->unset = SCE_CUnsetVATex;
        } else if (data->attrib >= SCE_ATTRIB0) {
            va->set = SCE_CSetVAAtt;
            va->unset = SCE_CUnsetVAAtt;
        }
#ifdef SCE_DEBUG
        else {
            SCEE_SendMsg ("Unknown attrib type %d", data->attrib);
        }
#endif
    }
}
/**
 * \brief Like SCE_CSetVertexArrayData() but creates a static
 * SCE_CVertexArrayData structure based on given parameters
 * \sa SCE_CSetVertexArrayData(), SCE_CGetVertexArrayData()
 */
void SCE_CSetVertexArrayNewData (SCE_CVertexArray *va,
                                 SCE_CVertexAttributeType attrib, SCEenum type,
                                 SCEsizei stride, SCEint size, void *p)
{
    SCE_CVertexArrayData data;
    data.attrib = attrib;
    data.type = type;
    data.stride = stride;
    data.size = size;
    data.data = p;
    SCE_CSetVertexArrayData (va, &data);
}


/**
 * \brief Mark the beginning of a vertex array setup sequence using the
 * OpenGL Vertex Array Objects
 * \sa SCE_CEndVertexArraySequence(), SCE_CCallVertexArraySequence()
 */
void SCE_CBeginVertexArraySequence (SCE_CVertexArray *va)
{
    if (va->id != 0)
        glDeleteArrays (1, &va->id); /* reset and create new */
    glGenVertexArrays (1, &va->id);
    glBindVertexArray (va->id);
}
/**
 * \brief Calls the registered sequence of the given vertex array
 * \sa SCE_CBeginVertexArraySequence(), SCE_CEndVertexArraySequence()
 */
void SCE_CCallVertexArraySequence (SCE_CVertexArray *va)
{
    glBindVertexArray (va->id);
    vao_used = SCE_TRUE;
}
/**
 * \brief Ends a setup sequence or a call
 * \sa SCE_CBeginVertexArraySequence(), SCE_CCallVertexArraySequence()
 */
void SCE_CEndVertexArraySequence (void)
{
    glBindVertexArray (0);
}

/**
 * \brief GL calls to make the given vertex array active for the render
 */
void SCE_CUseVertexArray (SCE_CVertexArray *va)
{
    va->set (&va->data);
    SCE_List_Appendl (&vaused, &va->it);
}
void SCE_CRender (SCE_CPrimitiveType prim, SCEuint n_vertices)
{
    glDrawArrays (prim, 0, n_vertices);
}
void SCE_CRenderInstanced (SCE_CPrimitiveType prim, SCEuint n_vertices,
                           SCEuint n_inst)
{
    glDrawArraysInstanced (prim, 0, n_vertices, n_inst);
}
void SCE_CRenderIndexed (SCE_CPrimitiveType prim, SCE_CIndexArray *ia,
                         SCEuint n_indices)
{
    glDrawElements (prim, n_indices, ia->type, ia->data);
}
void SCE_CRenderIndexedInstanced (SCE_CPrimitiveType prim, SCE_CIndexArray *ia,
                                  SCEuint n_indices, SCEuint n_instances)
{
    glDrawElementsInstanced (prim, n_indices, ia->type, ia->data, n_instances);
}

/**
 * \brief Call this function when the render of a group of vertex arrays is done
 * \sa SCE_CCallVertexArraySequence(), SCE_CEndVertexArraySequence()
 */
void SCE_CFinishVertexArrayRender (void)
{
    SCE_SListIterator *it = NULL;
    if (vao_used) {
        vao_used = SCE_FALSE;
        glBindVertexArray (0);
    }
    /*else*/
    SCE_List_ForEach (it, &vaused) {
        SCE_CVertexArray *va = SCE_List_GetData (it);
        va->unset (&va->data);
    }
    SCE_List_Flush (&vaused);
}

/** @} */
