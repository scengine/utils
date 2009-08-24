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
   updated: 03/08/2009 */

#include <config.h>
#include <SCE/SCEngine.h>

/**
 * \defgroup SCEngine
 *
 * SCEngine is a free and open-source 3D real time rendering engine written
 * in the C language
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
    if (SCE_Init_Utils (outlog) < 0)
        goto fail;

    if (SCE_CInit (flags) < 0)
        goto fail;
    if (SCE_Init_Texture () < 0)
        goto fail;
    if (SCE_Init_Shader () < 0)
        goto fail;
    if (SCE_Init_Geometry () < 0)
        goto fail;
    if (SCE_Init_BoxGeom () < 0)
        goto fail;
    if (SCE_Init_Mesh () < 0)
        goto fail;
    {
        if (SCE_Init_OBJ () < 0)
            goto fail;
    }
    if (SCE_Init_AnimGeom () < 0)
        goto fail;
    if (SCE_Init_Anim () < 0)
        goto fail;
    {
        if (SCE_Init_idTechMD5 () < 0)
            goto fail;
    }
    if (SCE_Init_Quad () < 0)
        goto fail;
    if (SCE_Init_Scene () < 0)
        goto fail;

    return SCE_OK;
fail:
    SCE_Quit ();
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("failed to initialize SCEngine");
    return SCE_ERROR;
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
    SCE_Quit_idTechMD5 ();
    SCE_Quit_Anim ();
    SCE_Quit_AnimGeom ();
    SCE_Quit_OBJ ();
    SCE_Quit_Mesh ();
    SCE_Quit_BoxGeom ();
    SCE_Quit_Geometry ();
    SCE_Quit_Shader ();
    SCE_Quit_Texture ();
    SCE_CQuit ();
    SCE_Quit_Utils ();
    SCE_btend ();
}

/**
 * \brief Gets the version string of the SCEngine
 */
const char* SCE_GetVersionString (void)
{
    return SCE_VERSION_STRING;
}

/** @} */
