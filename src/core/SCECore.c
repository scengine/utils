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
 
/* created: 15/12/2006
   updated: 19/01/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECore.h>

/**
 * \file SCECore.c
 * \copydoc core
 * 
 * \file SCECore.h
 * \copydoc core
 */

/**
 * \defgroup core Core functions, make OpenGL API calls
 * \ingroup SCEngine
 * @{
 */

int SCE_CInit (SCEflags flags)
{
    SCE_btstart ();
#define SCE_COREASSERT(c)\
    if (c)\
    {\
        SCE_CQuit ();\
        Logger_LogSrc ();\
        SCE_btend ();\
        return SCE_ERROR;\
    }

    SCE_COREASSERT (SCE_CSupportInit () < 0)
    if (!SCE_CHasCap (SCE_VBO))
    {
        Logger_Log (-1);
        Logger_LogMsg ("your hardware doesn't support vertex buffer object "
                       "extension: initialization aborted.");
        return SCE_ERROR;
    }

    SCE_COREASSERT (SCE_CBufferInit () < 0)
    SCE_COREASSERT (SCE_CImageInit () < 0)
    SCE_COREASSERT (SCE_CTextureInit () < 0)
    SCE_COREASSERT (SCE_CFramebufferInit () < 0)
    SCE_COREASSERT (SCE_CShaderInit (flags & SCE_CINIT_CG_SHADERS) < 0)
    SCE_COREASSERT (SCE_COcclusionQueryInit () < 0)

    SCE_CUseMaterial (NULL);

    /* enabling some default states */
    glEnable (GL_NORMALIZE);
    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    /*glEnable (GL_ALPHA_TEST);*/

    SCE_btend ();
    return SCE_OK;
}
/* revise le 24/03/2008 */
void SCE_CQuit (void)
{
    SCE_btstart ();
    SCE_COcclusionQueryQuit ();
    SCE_CShaderQuit ();
    SCE_CTextureQuit ();
    SCE_CImageQuit ();
    SCE_btend ();
}


/**
 * \brief Defines the color used for cleared the color buffer
 */
void SCE_CClearColor (float r, float g, float b, float a)
{
    glClearColor (r, g, b, a);
}
/**
 * \brief Defines the value used for cleared the depth buffer
 */
void SCE_CClearDepth (float d)
{
    glClearDepth (d);
}

/**
 * \brief Clears the specified buffers
 *
 * This function calls glClear(\p mask).
 */
void SCE_CClear (const GLbitfield mask)
{
    glClear (mask);
}

void SCE_CFlush (void)
{
    glFlush ();
}


void SCE_CSetState (SCEenum state, int active)
{
    if (active)
        glEnable (state);
    else
        glDisable (state);
}

void SCE_CSetState2 (SCEenum state, SCEenum state2, int active)
{
    if (active)
    {
        glEnable (state);
        glEnable (state2);
    }
    else
    {
        glDisable (state);
        glDisable (state2);
    }
}

void SCE_CSetState3 (SCEenum state, SCEenum state2, SCEenum state3, int active)
{
    if (active)
    {
        glEnable (state);
        glEnable (state2);
        glEnable (state3);
    }
    else
    {
        glDisable (state);
        glDisable (state2);
        glDisable (state3);
    }
}

void SCE_CSetState4 (SCEenum state, SCEenum state2,
                     SCEenum state3, SCEenum state4, int active)
{
    if (active)
    {
        glEnable (state);
        glEnable (state2);
        glEnable (state3);
        glEnable (state4);
    }
    else
    {
        glDisable (state);
        glDisable (state2);
        glDisable (state3);
        glDisable (state4);
    }
}

void SCE_CSetState5 (SCEenum state, SCEenum state2, SCEenum state3,
                     SCEenum state4, SCEenum state5, int active)
{
    if (active)
    {
        glEnable (state);
        glEnable (state2);
        glEnable (state3);
        glEnable (state4);
        glEnable (state5);
    }
    else
    {
        glDisable (state);
        glDisable (state2);
        glDisable (state3);
        glDisable (state4);
        glDisable (state5);
    }
}

void SCE_CSetState6 (SCEenum state, SCEenum state2, SCEenum state3,
                     SCEenum state4, SCEenum state5, SCEenum state6, int active)
{
    if (active)
    {
        glEnable (state);
        glEnable (state2);
        glEnable (state3);
        glEnable (state4);
        glEnable (state5);
        glEnable (state6);
    }
    else
    {
        glDisable (state);
        glDisable (state2);
        glDisable (state3);
        glDisable (state4);
        glDisable (state5);
        glDisable (state6);
    }
}

void SCE_CSetBlending (SCEenum src, SCEenum dst)
{
    glBlendFunc (src, dst);
}

/**
 * Enables/Disables color buffer writing
 * \param a can be SCE_TRUE or SCE_FALSE
 */
void SCE_CActivateColorBuffer (int a)
{
    glColorMask (a, a, a, a);
}
/**
 * Enables/Disables depth buffer writing
 * \param a can be SCE_TRUE or SCE_FALSE
 */
void SCE_CActivateDepthBuffer (int a)
{
    glDepthMask (a);
}

/**
 * \brief Defines which faces will be culled by the backface culling
 * \param mode can be SCE_FRONT or SCE_BACK
 *
 * This function calls glCullFace(\p mode).
 */
void SCE_CSetCulledFaces (SCEenum mode)
{
    glCullFace (mode);
}

/**
 * \brief Defines which faces will be culled by the backface culling
 * \param mode can be SCE_LESS, SCE_LEQUAL, SCE_GREATER or SCE_GEQUAL
 *
 * This function calls glDepthFunc(\p mode).
 */
void SCE_CSetValidPixels (SCEenum mode)
{
    glDepthFunc (mode);
}

/** @} */
