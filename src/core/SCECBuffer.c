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
 
/* created: 10/01/2007
   updated: 01/08/2009 */

#include <string.h>             /* memcpy */
#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECBuffer.h>

/**
 * \file SCECBuffer.c
 * \copydoc
 * 
 * \file SCECBuffer.h
 * \copydoc
 */

/**
 * \defgroup corebuffers GL buffers
 * \ingroup core
 * \internal
 * \brief OpenGL buffers generic management
 * @{
 */

void (*SCE_CUpdateBuffer) (SCE_CBuffer*);
static void SCE_CUpdateBufferClassic (SCE_CBuffer*);
static void SCE_CUpdateBufferRange (SCE_CBuffer*);

int SCE_CBufferInit (void)
{
    if (SCE_CIsSupported ("GL_ARB_map_buffer_range"))
        SCE_CUpdateBuffer = SCE_CUpdateBufferRange;
    else
        SCE_CUpdateBuffer = SCE_CUpdateBufferClassic;
}
void SCE_CQuitBuffer (void)
{
}

void SCE_CInitBufferData (SCE_CBufferData *data)
{
    data->first = 0;
    data->size = 0;
    data->data = NULL;
    data->range[0] = data->range[1] = 0;
    SCE_List_InitIt (&data->it);
    SCE_List_SetData (&data->it, data);
    data->buf = NULL;
    data->user = SCE_TRUE;
}
SCE_CBufferData* SCE_CCreateBufferData (void)
{
    SCE_CBufferData *data = NULL;
    if (!(data = SCE_malloc (sizeof *data)))
        SCEE_LogSrc ();
    else
        SCE_CInitBufferData (data);
    return data;
}
void SCE_CClearBufferData (SCE_CBufferData *data)
{
    SCE_CRemoveBufferData (data);
}
void SCE_CDeleteBufferData (SCE_CBufferData *data)
{
    if (data) {
        SCE_CClearBufferData (data);
        SCE_free (data);
    }
}

static void SCE_CFreeBufferBufferData (void *d)
{
    SCE_CBufferData *data = d;
    if (data->user)
        SCE_CRemoveBufferData (data);
    else
        SCE_CDeleteBufferData (data);
}
void SCE_CInitBuffer (SCE_CBuffer *buf)
{
    glGenBuffers (1, &buf->id);
    SCE_List_Init (&buf->data);
    SCE_List_SetFreeFunc (&buf->data, SCE_CFreeBufferBufferData);
    SCE_List_Init (&buf->modified);
    SCE_List_SetFreeFunc (&buf->modified, SCE_CFreeBufferBufferData);
    buf->mapptr = NULL;
}
SCE_CBuffer* SCE_CCreateBuffer (void)
{
    SCE_CBuffer *buf = NULL;
    if (!(buf = SCE_malloc (sizeof *buf)))
        SCEE_LogSrc ();
    else
        SCE_CInitBuffer (buf);
    return buf;
}
void SCE_CClearBuffer (SCE_CBuffer *buf)
{
    glDeleteBuffers (1, &buf->id);
    buf->id = 0;
    SCE_List_Clear (&buf->modified);
    SCE_List_Clear (&buf->data);
}
void SCE_CDeleteBuffer (SCE_CBuffer *buf)
{
    if (buf) {
        SCE_CClearBuffer (buf);
        SCE_free (buf);
    }
}

static void SCE_CResetBufferRange (SCE_CBuffer *buf)
{
    buf->range[0] = buf->size;
    buf->range[1] = 0;
}
static void SCE_CUpdateRange (SCE_CBuffer *buf, SCE_CBufferData *data)
{
    /* offset from the main buffer */
    size_t offset = data->range[0] + data->first;
    buf->range[0] = min (buf->range[0], offset);
    buf->range[1] = max (buf->range[1], offset + data->range[1]);
}

void SCE_CModifiedBufferData (SCE_CBufferData *data, size_t *range)
{
    SCE_List_Removel (&data->it);
    SCE_List_Appendl (&data->buf->modified, &data->it);
    if (range) {
        data->range[0] = range[0];
        data->range[1] = range[1];
    } else {
        data->range[0] = 0;
        data->range[1] = data->size;
    }
    SCE_CUpdateBufferRange (data->buf, data);
}

void SCE_CAddBufferData (SCE_CBuffer *buf, SCE_CBufferData *data)
{
    SCE_List_Appendl (&buf->data, &data->it);
    data->buf = buf;
    data->first = buf->size;
    buf->size += data->size;
}
SCE_CBufferData* SCE_CAddBufferNewData (SCE_CBuffer *buf, size_t s, void *p)
{
    SCE_CBufferData *data = NULL;
    if (!(data = SCE_CCreateBufferData ()))
        SCEE_LogSrc ();
    else {
        data->user = SCE_FALSE;
        data->size = s;
        data->data = p;
        SCE_CAddBufferData (buf, data);
    }
    return data;
}
void SCE_CRemoveBufferData (SCE_CBufferData *data)
{
    if (data->buf) {
        SCE_List_Removel (&data->it);
        data->buf = NULL;
    }
}

void SCE_CBuildBuffer (SCE_CBuffer *buf, SCEenum target, SCE_CBufferUsage usage)
{
    SCE_CBufferData *data = NULL;
    SCE_SListIterator *it = NULL;
    glBindBuffer (target, buf->id);
    glBufferData (target, buf->size, NULL, usage);
    SCE_List_ForEach (it, &buf->data) {
        data = SCE_List_GetData (it);
        glBufferSubData (target, data->first, data->size, data->data);
    }
    glBindBuffer (target, 0);
    buf->target = target;
    SCE_CResetBufferRange (buf);
}

static void SCE_CUpdateBufferClassic (SCE_CBuffer *buf)
{
    void *ptr = NULL;
    SCE_SListIterator *pro = NULL, *it = NULL;
    SCEenum target = buf->target;
    glBindBuffer (target, buf->id);
    ptr = glMapBuffer (target, GL_WRITE_ONLY);
#ifdef SCE_DEBUG
    if (!ptr) {
        /* TODO: make shortcut for this code */
        SCEE_Log (SCE_GL_ERROR);
        SCEE_LogMsg ("GL error on glMapBuffer(): %s",
                     gluErrorString (glGetError ()));
        return;                 /* lonlz */
    }
#endif
    SCE_List_ForEachProtected (pro, it, &buf->modified) {
        SCE_CBufferData *bd = SCE_List_GetData (it);
        memcpy (&((char*)ptr)[bd->range[0] + bd->first],
                &((char*)bd->data)[bd->range[0]],
                bd->range[1]);
        SCE_List_Removel (it);
        SCE_List_Appendl (&buf->data, it);
    }
    glUnmapBuffer (target);
    glBindBuffer (target, 0);
    SCE_CResetBufferRange (buf);
}
static void SCE_CUpdateBufferRange (SCE_CBuffer *buf)
{
    void *ptr = NULL;
    SCE_SListIterator *pro = NULL, *it = NULL;
    SCEenum target = buf->target;
    glBindBuffer (target, buf->id);
    ptr = glMapBufferRange (target, buf->range[0], buf->range[1],
                            GL_MAP_WRITE_BIT);
    /* errors generated by glMapBufferRange() are user-errors, except
       GL_OUT_OF_MEMORY, which can occur in this function */
#ifdef SCE_DEBUG
    if (!ptr) {
        /* TODO: make shortcut for this code */
        SCEE_Log (SCE_GL_ERROR);
        SCEE_LogMsg ("GL error on glMapBufferRange(): %s",
                     gluErrorString (glGetError ()));
        /* TODO: kick use of GLU */
        return;                 /* lonlz */
    }
#endif
    SCE_List_ForEachProtected (pro, it, &buf->modified) {
        SCE_CBufferData *bd = SCE_List_GetData (it);
        memcpy (&((char*)ptr)[bd->range[0] + bd->first - buf->range[0]],
                &((char*)bd->data)[bd->range[0]],
                bd->range[1]);
        SCE_List_Removel (it);
        SCE_List_Appendl (&buf->data, it);
    }
    glUnmapBuffer (target);
    glBindBuffer (target, 0);
    SCE_CResetBufferRange (buf);
}

void SCE_CUseBuffer (SCE_CBuffer *buf)
{
    glBindBuffer (buf->target, buf->id);
}

/** @} */
