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

/* created: 13/02/2009
   updated: 13/02/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEUtils.h>

static int initialized = SCE_FALSE;

/**
 * \brief Initializes the sub-module 'utils' of the SCEngine
 * \param outlog stream where write the error messages
 * \return SCE_ERROR on error, SCE_OK otherwise
 * \note Logger_LogSrc() don't have to be used if this function fails.
 */
int SCE_Init_Utils (FILE *outlog)
{
    if (initialized)
        return SCE_OK;
    if (SCE_Init_Mem () < 0)
    {
        fputs ("error: can't initialize memory manager", stderr);
        SCE_btend ();
        return SCE_ERROR;
    }
    if (outlog)
        Logger_Init (outlog);
    else
    {
        fputs ("error: invalid output stream of errors", stderr);
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_Init_Matrix () < 0)
    {
        Logger_LogFinish ("can't initialize matrices manager");
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_Init_FastList () < 0)
    {
        Logger_LogFinish ("can't initialize fast lists manager");
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_Init_Media () < 0)
    {
        Logger_LogFinish ("can't initialize medias manager");
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_Init_Resource () < 0)
    {
        Logger_LogFinish ("can't initialize resources manager");
        SCE_btend ();
        return SCE_ERROR;
    }
    initialized = SCE_TRUE;
    return SCE_OK;
}

/**
 * \brief Quit the sub-module 'utils' of the SCEngine
 */
void SCE_Quit_Utils (void)
{
    if (!initialized)
        return;
    SCE_Quit_Resource ();
    SCE_Quit_Media ();
    SCE_Quit_FastList ();
    /*SCE_Quit_Matrix ();*/
    /*SCE_Quit_Error ();*/
    /*SCE_Quit_Mem ();*/
}
