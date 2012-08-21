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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEFile.h"
#include "SCE/utils/polarssl-sha1.h"
#include "SCE/utils/SCESha1.h"



void SCE_Sha1_Init (SCE_TSha1 sum)
{
    memset (sum, 0, sizeof sum);
}

void SCE_Sha1_Sum (SCE_TSha1 sum, const unsigned char *data, size_t len)
{
    sha1 (data, len, sum);
}

int SCE_Sha1_StreamSum (SCE_TSha1 sum, SCE_SFile *fp)
{
    sha1_context  ctx;
    unsigned char buf[BUFSIZ];
    size_t        n;
    int           save_errno;
    
    SCE_File_Rewind (fp);
    sha1_starts (&ctx);
    while ((n = SCE_File_Read (buf, sizeof *buf, sizeof buf, fp)) > 0)
        sha1_update (&ctx, buf, n);

    save_errno = errno;
    sha1_finish (&ctx, sum);
    if (SCEE_HaveError ()) {    /* tkt. */
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    
    return SCE_OK;
}

int SCE_Sha1_FileSum (SCE_TSha1 sum, const char *filename)
{
    SCE_SFile fp;
    int rv = SCE_ERROR;

    SCE_File_Init (&fp);
    if (SCE_File_Open (&fp, NULL, filename, SCE_FILE_READ) < 0)
        SCEE_LogSrc ();
    else {
        rv = SCE_Sha1_StreamSum (sum, &fp);
        if (SCE_File_Close (&fp) != 0 || rv != SCE_OK) {
            SCEE_LogSrc ();
            rv = SCE_ERROR;
        }
    }
    
    return rv;
}

static unsigned char SCE_Sha1_Tonumber (const char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    else
        return 0xff;        /* impossible hexadecimal value */
}

/**
 * @brief Converts a string to a SHA1 sum
 * @param sum A SCE_TSha1 object where write the sum
 * @param str A string containing the string representation of a SHA1 sum
 * @returns SCE_OK on success, SCE_ERROR otheriwse
 * 
 * Tries to convert a string to a SHA1 sum. Only the first 40 characters are
 * read, which allows both extra data at the end and non-0-terminated strings.
 */
int SCE_Sha1_FromString (SCE_TSha1 sum, const char *str)
{
    int i;
    for (i = 0; i < 20; i++) {
        unsigned char a, b;
        if (!str[i * 2] || !str[i * 2 + 1])
            break;
        a = SCE_Sha1_Tonumber (tolower (str[i * 2]));
        b = SCE_Sha1_Tonumber (tolower (str[i * 2 + 1]));
        if (a > 0xf || b > 0xf) {
            SCEE_Log (9875);
            SCEE_LogMsg ("wrong SHA-1 string: unexpected character %c or %c",
                         str[i * 2], str[i * 2 + 1]);
            return SCE_ERROR;
        }
        sum[i] = a << 4;
        sum[i] |= b;
    }
    if (i != 20) {
        SCEE_Log (43);
        SCEE_LogMsg ("SHA-1 string is too short: expected 40 characters, "
                     "got %d", i);
        return SCE_ERROR;
    }
    return SCE_OK;
}

/**
 * @brief Converts a SHA1 sum to its string representation
 * @param str A string at least SCE_SHA1_STRING_SIZE long where write the sum
 * @param sum A valid SHA sum
 * 
 * Converts a SHA1 sum to its string representation. A \0 is appened at the end
 * of the string.
 */
void SCE_Sha1_ToString (char *str, SCE_TSha1 sum)
{
    char b[] = {"0123456789abcdef"};
    size_t i;
    for (i = 0; i < 20; i++) {
        str[i * 2]     = b[(sum[i] >> 4) & 0x0f];
        str[i * 2 + 1] = b[sum[i] & 0x0f];
    }
    str[40] = 0;
}


/**
 * \brief Compares two sums and tells if they are the same
 */
int SCE_Sha1_Equal (SCE_TSha1 s1, SCE_TSha1 s2)
{
    size_t i;
    for (i = 0; i < sizeof (SCE_TSha1); i++) {
        if (s1[i] != s2[i])
            return SCE_FALSE;
    }
    return SCE_TRUE;
}
