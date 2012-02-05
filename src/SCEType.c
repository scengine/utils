/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2012  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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

/* created: 17/04/2010
   updated: 23/01/2012 */

#include <string.h>
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEMemory.h"
#include "SCE/utils/SCEType.h"

size_t SCE_Type_Sizeof (SCE_EType type)
{
    switch (type) {
    case SCE_NONE_TYPE:
        return 0;               /* :trollface: */
    case SCE_DOUBLE:
        return sizeof (SCEdouble);
    case SCE_FLOAT:
        return sizeof (SCEfloat);
    case SCE_INT:
    case SCE_UNSIGNED_INT:
        return sizeof (SCEint);
    case SCE_SHORT:
    case SCE_UNSIGNED_SHORT:
        return sizeof (SCEshort);
    case SCE_BYTE:
    case SCE_UNSIGNED_BYTE:
        return sizeof (SCEbyte);
    case SCE_SIZE_T:
        return sizeof (size_t);
#ifdef SCE_DEBUG
    default:
        SCEE_Log (SCE_INVALID_ARG);
        SCEE_LogMsg ("unknown data type %d", type);
#endif
    }
    return 0;
}


/**
 * \brief Converts data from one to another type
 * \param tdest destination data type
 * \param dest destination data pointer (must be already allocated)
 * \param tsrc source data type
 * \param src source data pointer
 * \param n number of elements in \p src (not bytes, just the number of typed
 * values)
 * \sa SCE_Type_ConvertDup()
 */
void SCE_Type_Convert (int tdest, void *dest, int tsrc,
                       const void *src, size_t n)
{
    union SCE_UType {
        SCEubyte *ub;
        SCEbyte *b;
        SCEushort *us;
        SCEshort *s;
        SCEuint *ui;
        SCEint *i;
        SCEfloat *f;
        SCEdouble *d;
        size_t *st;
    };

    size_t i;
    union SCE_UType tin, tout;

    if (tdest == tsrc) {
        memcpy (dest, src, n * SCE_Type_Sizeof (tdest));
        return;
    }

#define SCE_TYPE_FOR(type, namedest, namesrc)\
    case type:\
        tout.namedest = dest;\
        for (i = 0; i < n; i++)\
            tout.namedest[i] = tin.namesrc[i];\
        break;

#define SCE_TYPE_SWITCH(type, namesrc)\
    case type:\
        tin.namesrc = (void*)src;\
        switch (tdest) {\
        SCE_TYPE_FOR (SCE_UNSIGNED_BYTE,  ub, namesrc)\
        SCE_TYPE_FOR (SCE_BYTE,           b,  namesrc)\
        SCE_TYPE_FOR (SCE_UNSIGNED_SHORT, us, namesrc)\
        SCE_TYPE_FOR (SCE_SHORT,          s,  namesrc)\
        SCE_TYPE_FOR (SCE_UNSIGNED_INT,   ui, namesrc)\
        SCE_TYPE_FOR (SCE_INT,            i,  namesrc)\
        SCE_TYPE_FOR (SCE_FLOAT,          f,  namesrc)\
        SCE_TYPE_FOR (SCE_DOUBLE,         d,  namesrc)\
        SCE_TYPE_FOR (SCE_SIZE_T,         st, namesrc)\
        }\
        break;

    switch (tsrc) {
    SCE_TYPE_SWITCH (SCE_UNSIGNED_BYTE,  ub)
    SCE_TYPE_SWITCH (SCE_BYTE,           b)
    SCE_TYPE_SWITCH (SCE_UNSIGNED_SHORT, us)
    SCE_TYPE_SWITCH (SCE_SHORT,          s)
    SCE_TYPE_SWITCH (SCE_UNSIGNED_INT,   ui)
    SCE_TYPE_SWITCH (SCE_INT,            i)
    SCE_TYPE_SWITCH (SCE_FLOAT,          f)
    SCE_TYPE_SWITCH (SCE_DOUBLE,         d)
    SCE_TYPE_SWITCH (SCE_SIZE_T,         st)
    }
}

/**
 * \brief Converts data and allocates memory for them
 * \param tdest destination type
 * \param tsrc source type
 * \param src data to convert
 * \param n number of variables into \p src
 *
 * This function is a combination of SCE_Mem_Dup() and SCE_Type_Convert(). It
 * allocates memory for the further converted data, and calls SCE_Type_Convert().
 * Total size of \p src is \p n * SCE_Type_Sizeof (\p tsrc).
 * \sa SCE_Mem_Dup(), SCE_Type_Convert(), SCE_Type_Sizeof()
 */
void* SCE_Type_ConvertDup (int tdest, int tsrc, const void *src, size_t n)
{
    size_t size;
    void *dest = NULL;

    if (!(size = SCE_Type_Sizeof (tdest))) {
        SCEE_LogSrc ();
        return NULL;
    }
    if (tdest == tsrc)
        return SCE_Mem_Dup (src, size * n);

    dest = SCE_malloc (size * n);
    if (!dest) {
        SCEE_LogSrc ();
        return NULL;
    }

    SCE_Type_Convert (tdest, dest, tsrc, src, n);
    return dest;
}
