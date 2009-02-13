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
 
/* created: 12/10/2006
   updated: 13/02/2009 */

#include <SCE/SCEngine.h>

/**
 * \defgroup SCEngine
 */

/** @{ */

/**
 * \defgroup interface Main interface for using the SCEngine
 * \ingroup SCEngine
 * \internal
 * \brief
 */

/**
 * \brief Initializes the SCEngine
 * \param outlog stream used to log informations and errors
 * \returns SCE_ERROR if any error has occured, SCE_OK otherwise
 *
 * This function initializes all sub modules of SCEngine.
 */
int SCE_Init (FILE *outlog, SCEflags flags)
{
    SCE_btstart ();
    if (SCE_Init_Utils (outlog) < 0)
    {
        SCE_btend ();
        return SCE_ERROR;
    }

    if (SCE_CInit (flags) < 0)
    {
        Logger_LogFinish ("can't initialize core!");
        SCE_btend ();
        return SCE_ERROR;
    }

    if (SCE_Init_Texture () < 0)
    {
        SCE_Quit ();
        Logger_LogFinish ("can't initialize textures manager");
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_Init_Shader () < 0)
    {
        SCE_Quit ();
        Logger_LogFinish ("can't initialize shaders manager");
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_Init_Mesh () < 0)
    {
        SCE_Quit ();
        Logger_LogFinish ("can't initialize meshs manager");
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_Init_State () < 0)
    {
        SCE_Quit ();
        Logger_LogFinish ("can't initialize states manager");
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_Init_Quad () < 0)
    {
        SCE_Quit ();
        Logger_LogFinish ("can't initialize quads manager");
        SCE_btend ();
        return SCE_ERROR;
    }
    if (SCE_Init_Scene () < 0)
    {
        SCE_Quit ();
        Logger_LogFinish ("can't initialize scenes manager");
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_btend ();
    return SCE_OK;
}

/**
 * \brief Quit SCEngine and free memory used
 * This function frees every modules initialised by SCE_Init()
 * \returns SCE_ERROR if any error has occured, SCE_OK otherwise
 */
void SCE_Quit (void)
{
    SCE_btstart ();
    SCE_Quit_Scene ();
    SCE_Quit_Quad ();
    SCE_Quit_State ();
    SCE_Quit_Mesh ();
    SCE_Quit_Shader ();
    SCE_Quit_Texture ();
    SCE_CQuit ();
    SCE_Quit_Utils ();
    SCE_btend ();
}


const char* SCE_GetVersionString (void)
{
    return SCE_VERSION_STRING;
}

/** @} */
