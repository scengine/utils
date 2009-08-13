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
   updated: 13/08/2009 */

#include <string.h>             /* memcpy */
#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEMath.h>  /* MIN/MAX */
#include <SCE/core/SCECSupport.h>
#include <SCE/core/SCECBuffer.h>

/**
 * \file SCECBuffer.c
 * \copydoc
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

static SCE_SList modified;      /* all modified buffers */

void (*SCE_CUpdateBuffer) (SCE_CBuffer*);
static void SCE_CUpdateBufferMapClassic (SCE_CBuffer*);
static void SCE_CUpdateBufferMapRange (SCE_CBuffer*);

int SCE_CBufferInit (void)
{
    if (SCE_CIsSupported ("GL_ARB_map_buffer_range"))
        SCE_CUpdateBuffer = SCE_CUpdateBufferMapRange;
    else
        SCE_CUpdateBuffer = SCE_CUpdateBufferMapClassic;
    SCE_List_Init (&modified);
}
void SCE_CQuitBuffer (void)
{
    SCE_List_Flush (&modified);
}

void SCE_CInitBufferData (SCE_CBufferData *data)
{
    data->first = 0;
    data->size = 0;
    data->data = NULL;
    data->range[0] = data->range[1] = 0;
    SCE_List_InitIt (&data->it);
    SCE_List_SetData (&data->it, data);
    data->modified = SCE_FALSE;
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
    SCE_List_Remove (&buf->it);
}
void SCE_CDeleteBuffer (SCE_CBuffer *buf)
{
    if (buf) {
        SCE_CClearBuffer (buf);
        SCE_free (buf);
    }
}


/* prout */
static void SCE_CResetBufferRange (SCE_CBuffer *buf)
{
    buf->range[0] = buf->size;
    buf->range[1] = 0;
}
/* only used into CModifiedBuffer() */
static void SCE_CUpdateBufferRange (SCE_CBuffer *buf, const size_t *range)
{
    /* offset from the main buffer */
    size_t offset = range[0];
    buf->range[0] = MIN (buf->range[0], offset);
    buf->range[1] = MAX (buf->range[1], offset + range[1]);
}
/**
 * \brief Sets the modified range of an entire buffer, useful for index buffers
 */
void SCE_CModifiedBuffer (SCE_CBuffer *buf, const size_t *range)
{
    size_t r[2];
    if (!range) {
        r[0] = 0;
        r[1] = buf->size;
        range = r;
    }
    SCE_CUpdateBufferRange (buf, range);
}
/**
 * \brief Mark a buffer as unmodified: will not be updated by
 * SCE_CUpdateModifiedBuffers()
 * \sa SCE_CModifiedBuffer(), SCE_CModifiedBufferData()
 */
void SCE_CUnmodifiedBuffer (SCE_CBuffer *buf)
{
    SCE_CResetBufferRange (buf);
    SCE_List_Remove (&buf->it);
}
/**
 * \brief Defines a buffer data as modified
 * \param range modified range in bytes, NULL means 'all'
 * \sa SCE_CUpdateModifiedBuffers(), SCE_CUpdateBuffer(),
 * SCE_CUnmodifiedBufferData()
 */
void SCE_CModifiedBufferData (SCE_CBufferData *data, const size_t *range)
{
    SCE_List_Removel (&data->it);
    SCE_List_Appendl (&data->buf->modified, &data->it);
    if (range) {
        /* if already modified, get the largest range */
        if (data->modified) {
            data->range[0] = MIN (data->range[0], range[0]);
            data->range[1] = MAX (data->range[1], range[1]);
        } else {
            data->range[0] = range[0];
            data->range[1] = range[1];
        }
    } else {
        data->range[0] = 0;
        data->range[1] = data->size;
    }
    {
        size_t r[2];
        r[0] = data->range[0] + data->first;
        r[1] = data->range[1];
        SCE_CModifiedBuffer (data->buf, r);
    }
    data->modified = SCE_TRUE;
    /* add the buffer to the modified buffers list */
    SCE_List_Remove (&data->buf->it);
    SCE_List_Appendl (&modified, &data->buf->it);
}
/**
 * \brief Defines a buffer data as unmodified (will not be updated)
 * \sa SCE_CModifiedBufferData()
 */
void SCE_CUnmodifiedBufferData (SCE_CBufferData *data)
{
    SCE_List_Removel (&data->it);
    SCE_List_Appendl (&data->buf->data, &data->it);
    data->modified = SCE_FALSE;
}

/**
 * \brief Adds a buffer data to a buffer
 * \sa SCE_CAddBufferNewData(), SCE_CRemoveBufferData()
 */
void SCE_CAddBufferData (SCE_CBuffer *buf, SCE_CBufferData *data)
{
    SCE_List_Appendl (&buf->data, &data->it);
    data->buf = buf;
    data->first = buf->size;
    buf->size += data->size;
}
/**
 * \brief Create and adds a new buffer data
 * \param s size of the new buffer data in bytes
 * \param p pointer to the data of the buffer data
 * \sa SCE_CAddBufferData(), SCE_CRemoveBufferData()
 */
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
/**
 * \brief Removes a buffer data from its buffer
 * \sa SCE_CAddBufferData(), SCE_CAddBufferNewData()
 */
void SCE_CRemoveBufferData (SCE_CBufferData *data)
{
    if (data->buf) {
        SCE_List_Removel (&data->it);
        data->buf = NULL;
    }
}

/**
 * \brief Builds a buffer
 * \param target type of the buffer (todo: use an enum)
 * \param usage buffer usage
 */
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

#if 0
void* SCE_CLockBufferClassic (SCE_CBuffer *buf, size_t *range,
                              SCE_CLockBufferFlags flags)
{
    SCEenum mode;
    if (flags & SCE_BUFFER_WRITE_LOCK && flags & SCE_BUFFER_READ_LOCK)
        mode = GL_READ_WRITE;
    else if (flags & SCE_BUFFER_WRITE_LOCK)
        mode = GL_WRITE_ONLY;
    else if (flags & SCE_BUFFER_READ_LOCK)
        mode = GL_READ_ONLY;
    glBindBuffer (buf->target, buf->id);
    glMapBuffer (buf->target, mode);
}
void* SCE_CLockBufferRange (SCE_CBuffer *buf, size_t *range,
                            SCE_CLockBufferFlags flags)
{
    SCEbitfield mode = 0;
    if (flags & SCE_BUFFER_WRITE_LOCK)
        mode |= GL_MAP_WRITE_BIT;
    if (flags & SCE_BUFFER_READ_LOCK)
        mode |= GL_MAP_READ_BIT;
    glBindBuffer (buf->target, buf->id);
    glMapBuffer (buf->target, mode);
}
void SCE_CUnlockBuffer (SCE_CBuffer *buf)
{
    glBindBuffer (buf->target, buf->id);
    glUnmapBuffer (buf->target);
}
#endif

static void SCE_CUpdateBufferMapClassic (SCE_CBuffer *buf)
{
    void *ptr = NULL;
    SCE_SListIterator *pro = NULL, *it = NULL;
    SCEenum target = buf->target;
    /* TODO: do it all in one? */
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
        SCE_CUnmodifiedBufferData (bd);
    }
    glUnmapBuffer (target);
    glBindBuffer (target, 0);
    SCE_CResetBufferRange (buf);
}
static void SCE_CUpdateBufferMapRange (SCE_CBuffer *buf)
{
    void *ptr = NULL;
    SCE_SListIterator *pro = NULL, *it = NULL;
    SCEenum target = buf->target;
    /* TODO: do it all in one? */
    glBindBuffer (target, buf->id);
    ptr = glMapBufferRange (target, buf->range[0], buf->range[1],
                            GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
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
        size_t offset, length;
        SCE_CBufferData *bd = SCE_List_GetData (it);
        offset = bd->range[0] + bd->first - buf->range[0];
        length = bd->range[1];
        memcpy (&((char*)ptr)[offset],
                &((char*)bd->data)[bd->range[0]],
                length);
        SCE_CUnmodifiedBufferData (bd);
        /* give to the GL the modified subrange */
        glFlushMappedBufferRange (target, offset, length);
    }
    glUnmapBuffer (target);
    glBindBuffer (target, 0);
    SCE_CResetBufferRange (buf);
}

/**
 * \brief Updates all buffers containing modified buffer data
 * \sa SCE_CModifiedBufferData(), SCE_CUpdateBuffer()
 */
void SCE_CUpdateModifiedBuffers (void)
{
    SCE_SListIterator *it;
    SCE_List_ForEach (it, &modified)
        SCE_CUpdateBuffer (SCE_List_GetData (it));
    SCE_List_Flush (&modified);
}

/**
 * \brief Sets up a buffer as activated
 */
void SCE_CUseBuffer (SCE_CBuffer *buf)
{
    glBindBuffer (buf->target, buf->id);
}

/** @} */
