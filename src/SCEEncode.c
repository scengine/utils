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

/* created: 06/05/2012
   updated: 22/08/2012 */

#include <stdlib.h>
#include "SCE/utils/SCEType.h"
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEEncode.h"

#define SCE_ENCODE_DEBUG 0

/**
 * \brief extracts sign, exponent and mantissa from a real number
 * \param a A real
 * \param ex Return location for the exponent
 * \param m Return location for the mantissa
 * \returns The sign
 */
static unsigned int SCE_Encode_GetFloatData (float a, int *ex, float *m)
{
#if SCE_ENCODE_DEBUG
    float input = a;
#endif
    unsigned int sign = 0;      /* positive */
    int e = 0;
    
    if (a < 0.0f) {
        a *= -1.0f;
        sign = 1;               /* negative */
    }
    if (a == 0.0f /* lol */) {
        a = 0.0;
        e = 0;
        /* skip */
    } else if (a < 1.0f) {
        for (; a < 1.0f; e--) {
            a *= 2.0f;
        }
    } else {
        for (; a >= 2.0f; e++) {
            a /= 2.0f;
        }
    }
    *ex = e;
    *m = a - (1.0f * (e != 0));

#if SCE_ENCODE_DEBUG
    printf ("FGD: %1$f (%1$g):\n"
            "  s: %2$u\n"
            "  e: %3$d\n"
            "  m: %4$f (%4$g)\n", input, sign, e, *m);
#endif

    return sign;
}

/**
 * \brief Creates a real from its sign, exponent and mantissa
 * \param sign Sign (0 = positive, 1 = negative)
 * \param ex The exponent
 * \param m The mantissa
 * \returns The created real
 */
static float SCE_Encode_SetFloatData (unsigned int sign, int ex, float m)
{
#if SCE_ENCODE_DEBUG
    int input_ex = ex;
#endif
    float a;
    
    a = m + (1.0f * (ex != 0));
    if (ex > 0) {
        for (; ex > 0; ex --) {
            a *= 2.0f;
        }
    } else {
        for (; ex < 0; ex ++) {
            a /= 2.0f;
        }
    }
    if (sign) {
        a *= -1.0f;
    }

#if SCE_ENCODE_DEBUG
    printf ("FSD: %1$f (%1$g):\n"
            "  s: %2$u\n"
            "  e: %3$d\n"
            "  m: %4$f (%4$g)\n", a, sign, input_ex, m);
#endif

    return a;
}

/**
 * \param m The number to write
 * \param off offset in bits from the start of \p out
 * \param n the number of bits to write
 * \param out buffer where write
 *
 * Writes the \p n first bits of \p m into \p out at offset \p offset.
 */
static unsigned char* SCE_Encode_Write (unsigned long m, int *off, int n,
                                        unsigned char *out)
{
    unsigned char mask;
    int offset = *off;
    
    while (offset > 8) {
        offset -= 8;
        out ++;
    }
    /* mask where write in the first byte */
    mask = (unsigned char)((1 << (8 - offset)) - 1);
    if (offset + n < 8) {
        /* if we don't use the full first byte, remove unused bits (at the
         * right) from our mask */
        mask ^= (unsigned char)((1 << (8 - n - offset)) - 1);
        *out = (unsigned char)((*out & ~mask) |
                               (mask & (unsigned char)(m << (8 - n - offset))));
        *off = offset + n;
    } else {
        /* remove data where we will write; then add portion of m for the first
         * byte */
        *out = (unsigned char)((~mask & *out) |
                               (mask & (unsigned char)(m >> (n - 8 + offset))));
        /* adjust n to the remaining bits */
        n -= (8 - offset);
        /* and then add the remaining bits, if any */
        while (n > 0) {
            out++;
            if (n < 8) {
                /* add the last chunk of m */
                *out = (unsigned char)(m << (8 - n)) |
                                       (*out & (unsigned char)((1 << (8 - n))
                                                               - 1));
            } else {
                /* add the next chunk of m */
                *out = (unsigned char)(m >> (n - 8)) /* & 0xff */;
            }
            n -= 8;
        }
        if (n == 0) {
            *off = 0;
            out++;
        } else
            *off = n + 8;
    }
    return out;
}

/**
 * \param field buffer from where read
 * \param off offset in bits from the start of \p field ([0 - 7])
 * \param n the number of bits to read
 * \returns the read number
 *
 * Reads \p n bits from \p field at offset \p offset and return the read value
 * as a unsigned long.
 * It returns the position of the end of reading into \p field and \p offset
 */
static SCEulong SCE_Encode_Read (const unsigned char **field, int *off, int n)
{
    SCEulong m = 0;
    unsigned char mask;
    int offset = *off;

    while (offset > 8) {
        offset -= 8;
        (*field) ++;
    }
    /* mask where read in the first byte */
    mask = (unsigned char)((1 << (8 - offset)) - 1);
    if (offset + n < 8) {
        /* if we don't use the full first byte, remove unused bits (at the
         * right) from our mask */
        mask ^= (unsigned char)((1 << (8 - n - offset)) - 1);
        m = (SCEulong)((*(*field) & mask) >> (8 - n - offset));
        *off = offset + n;
    } else {
        /* read masked input and move extracted bits to their final position */
        m = (SCEulong)((*(*field) & mask) << (n - 8 + offset));
        /* adjust n to the remaining bits */
        n -= (8 - offset);
        /* and then read the remaining bits, if any */
        while (n > 0) {
            (*field)++;
            if (n < 8) {
                /* read the last chunk */
                m |= (SCEulong)((*(*field) >> (8 - n)) & ((1 << n) - 1));
            } else {
                /* read the next chunk */
                m |= (SCEulong)(*(*field) << (n - 8));
            }
            n -= 8;
        }
        if (n == 0) {
            *off = 0;
            (*field)++;
        } else
            *off = n + 8;
    }

    return m;
}

/**
 * \brief Writes a float to a buffer
 * \param a A floating-point value
 * \param se whether the exponent field is signed or not (mouahaha)
 * \param ne number of bits for the exponent part
 * \param nm number of bits for the mantissa part
 * \param offset offset (in bits) where start writing
 * \param buf buffer where write value
 * \returns SCE_ERROR on error, SCE_OK otherwise
 *
 * The number of bits written in \p out at \p offset is 1+ne+nm
 */
/* you're limited to 256 bits for each field! harhar */
int SCE_Encode_Float (float a, int se, unsigned char ne, unsigned char nm,
                      int *offset, unsigned char **out)
{
    unsigned int s;
    int e;
    float fm;
    unsigned long m;            /* mantissa */
    unsigned int max_e;         /* maximum exponent value */

    s = SCE_Encode_GetFloatData (a, &e, &fm);
    max_e = (unsigned int)(1 << (ne - 1));
    if (se) e += (int)(max_e / 2u);
    if (e < 0) {                /* assume 0 */
        e = 0;
        s = 0;
        m = 0;
    } else {
#if SCE_DEBUG
        if (e >= max_e) {
            SCEE_Log (SCE_INVALID_ARG);
            SCEE_LogMsg ("too large number %g (ex = %d): not enough space"
                         " in exponent field (%d)", a, e, ne);
            return SCE_ERROR;
        }
#endif
        m = (unsigned long)(fm * (float)(1 << (nm - 1)));
    }
    *out = SCE_Encode_Write (s, offset, 1, *out);
    *out = SCE_Encode_Write (e, offset, ne, *out);
    *out = SCE_Encode_Write (m, offset, nm, *out);
    
    return SCE_OK;
}

/**
 * \brief reads a float from a buffer
 * \param field buffer where read value
 * \param se whether the exponent field is signed or not (mouahaha)
 * \param ne number of bits of the exponent part
 * \param nm number of bits of the mantissa part
 * \param offset offset (in bits) where start reading
 * \returns the read value
 */
float SCE_Decode_Float (const unsigned char **field, int se, unsigned char ne,
                        unsigned char nm, int *offset)
{
    unsigned int s;
    int e;
    unsigned long m;
    float fm;
    
    s = SCE_Encode_Read (field, offset, 1);
    e = SCE_Encode_Read (field, offset, ne);
    m = SCE_Encode_Read (field, offset, nm);
    fm = (float)m / (float)(1 << (nm - 1));
    if (se) e -= (1 << (ne - 1)) / 2;
    
    return SCE_Encode_SetFloatData (s, e, fm);
}


size_t SCE_Encode_Floats (const float *floats, size_t n_floats, int se,
                          unsigned char ne, unsigned char nm,
                          unsigned char *out)
{
    size_t i, n_bits;
    int off = 0;

    for (i = 0; i < n_floats; i++)
        SCE_Encode_Float (floats[i], se, ne, nm, &off, &out);

    n_bits = se ? 1 : 0;
    n_bits += ne + nm;
    n_bits *= n_floats;

    return n_bits / 8 + (n_bits % 8 ? 1 : 0);
}

void SCE_Decode_Floats (float *floats, size_t n_floats, int se,
                        unsigned char ne, unsigned char nm,
                        const unsigned char *in)
{
    size_t i;
    int off = 0;

    for (i = 0; i < n_floats; i++)
        floats[i] = SCE_Decode_Float (&in, se, ne, nm, &off);
}

#if 0
size_t SCE_Encode_StreamFloats (const float *f, size_t n, int se,
                                unsigned char ne,
                                unsigned char nm, SCE_SFile *fp)
{
    unsigned char buffer[256] = {0};
}
void SCE_Decode_StreamFloats (float*, size_t, int, unsigned char,
                              unsigned char, SCE_SFile*);
#endif


void SCE_Encode_Long (long id, unsigned char *data)
{
    data[0] = 0xFF & (unsigned char)id;
    data[1] = 0xFF & (unsigned char)(id >> 8);
    data[2] = 0xFF & (unsigned char)(id >> 16);
    data[3] = 0xFF & (unsigned char)(id >> 24);
}

long SCE_Decode_Long (const unsigned char *data)
{
    long id;
    id = (unsigned char)data[0];
    id |= ((unsigned char)data[1] << 8);
    id |= ((unsigned char)data[2] << 16);
    id |= ((unsigned char)data[3] << 24);
    return id;
}

void SCE_Encode_StreamLong (long l, SCE_SFile *fp)
{
    unsigned char buffer[4] = {0};
    SCE_Encode_Long (l, buffer);
    SCE_File_Write (buffer, 1, 4, fp);
    /* we should check write() return value */
}
long SCE_Decode_StreamLong (SCE_SFile *fp)
{
    unsigned char buffer[4] = {0};
    SCE_File_Read (buffer, 1, 4, fp);
    /* we should check read() return value */
    return SCE_Decode_Long (buffer);
}
