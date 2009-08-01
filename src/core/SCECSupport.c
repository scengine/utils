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
 
/* created: 31/01/2006
   updated: 01/08/2009 */

#include <string.h>
#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECSupport.h>

/**
 * \file SCECSupport.c
 * \copydoc extenstionsupport
 * \brief OpenGL extensions support managment
 * \file SCECSupport.h
 * \copydoc extenstionsupport
 * \brief OpenGL extensions support managment
 */

/**
 * \defgroup extenstionsupport OpenGL extensions support managment
 * \ingroup core
 * \brief OpenGL extensions support managment
 * 
 * This module is usefull to determine if an OpenGL extension is supported by
 * the running implementation.
 */

/** @{ */

static unsigned char caps[SCE_NUM_CAPS];

static void SCE_CCheckCaps (void)
{
    caps[SCE_TEX_MULTI] =
    SCE_CIsSupported ("GL_ARB_multitexture");

    caps[SCE_TEX_NON_POWER_OF_TWO] =
    SCE_CIsSupported ("GL_ARB_texture_non_power_of_two");

    caps[SCE_TEX_HW_GEN_MIPMAP] =
    SCE_CIsSupported ("GL_SGIS_generate_mipmap");

    caps[SCE_TEX_DXT1] = caps[SCE_TEX_DXT3] = caps[SCE_TEX_DXT5] =
    SCE_CIsSupported ("GL_ARB_texture_compression") &&
    SCE_CIsSupported ("GL_EXT_texture_compression_s3tc");

    caps[SCE_TEX_S3TC] =
    SCE_CIsSupported ("GL_EXT_texture_compression_s3tc");

    caps[SCE_TEX_3DC] = SCE_FALSE; /* TODO: onlz */

    caps[SCE_VBO] =
    SCE_CIsSupported ("GL_ARB_vertex_buffer_object");

    caps[SCE_VAO] =
    SCE_CIsSupported ("GL_ARB_vertex_array_object") ||
    SCE_CIsSupported ("GL_EXT_vertex_array_object") || /* lol? */
    SCE_CIsSupported ("GL_ATI_vertex_array_object");

    caps[SCE_FBO] =
    SCE_CIsSupported ("GL_ARB_framebuffer_object") ||
    SCE_CIsSupported ("GL_EXT_framebuffer_object");

    caps[SCE_PBO] =
    SCE_CIsSupported ("GL_EXT_pixel_buffer_object") ||
    SCE_CIsSupported ("GL_ARB_pixel_buffer_object");

    caps[SCE_VERTEX_SHADER_GLSL] = /* TODO: obsolete */
    SCE_CIsSupported ("GL_ARB_vertex_shader") &&
    SCE_CIsSupported ("GL_ARB_shader_objects") &&
    SCE_CIsSupported ("GL_ARB_shading_language_100");

    caps[SCE_FRAGMENT_SHADER_GLSL] = /* TODO: obsolete */
    SCE_CIsSupported ("GL_ARB_fragment_shader") &&
    SCE_CIsSupported ("GL_ARB_shader_objects") &&
    SCE_CIsSupported ("GL_ARB_shading_language_100");

#if 0
    caps[SCE_CG_SHADERS] =
    SCE_CIsSupported ("..."); /* NOTE: a completer... ou pas ? */
#endif
    caps[SCE_CG_SHADERS] = 0;

    caps[SCE_OCCLUSION_QUERY] =
    SCE_CIsSupported ("GL_ARB_occlusion_query") ||
    SCE_CIsSupported ("GL_NV_occlusion_query");

    caps[SCE_MRT] =
    SCE_CIsSupported ("GL_ARB_draw_buffers") ||
    SCE_CIsSupported ("GL_ATI_draw_buffers");

    caps[SCE_HW_INSTANCING] =
    SCE_CIsSupported ("GL_ARB_draw_instanced") ||
    SCE_CIsSupported ("GL_EXT_draw_instanced");
}

/**
 * \internal
 */
int SCE_CSupportInit (void)
{
    /* recherche des extensions supportees */
    SCE_CCheckCaps ();
    return SCE_OK;
}

/**
 * \brief Search an occurrence of \p ext in glGetString(GL_EXTENSIONS)
 * \param ext an OpenGL extension name
 * \returns SCE_TRUE if the OpenGL implementation supports the extension,
 *          SCE_FALSE otherwise
 * 
 * \see SCE_CHasCap()
 */
int SCE_CFindExtension (const char *ext)
{
    return (strstr ((const char*)glGetString (GL_EXTENSIONS), ext) ?
            SCE_TRUE : SCE_FALSE);
}

#if 0
/**
 * \brief Wrapper for the GLEW's SCE_CIsSupported() function
 * \param ext an OpenGL extension name
 * \returns SCE_TRUE if both GLEW and the running OpenGL implementation supports
 *          the extension, SCE_FALSE otherwise
 * 
 * \see SCE_CSupport()
 */
int SCE_CGlewSupport (const char *ext)
{
    return glewIsSupported (ext);
}
#endif

/**
 * \brief Checks if an OpenGL extension is supported by the running
 *        implementation
 * \param ext an OpenGL extension name
 * \returns SCE_TRUE if the OpenGL implementation supports the extension,
 *          SCE_FALSE otherwise
 * \see SCE_CHasCap()
 */
int SCE_CIsSupported (const char *ext)
{
    return SCE_CFindExtension (ext);
}

/**
 * \brief Fastly checks if a OpenGL's extension supported by the core engine is
 *        supported by the implementation
 * 
 * This function is designed to fastly checks if one of the SCE_CCap extensions
 * is supported by the running implementation. This is mostly used internally 
 * and does not support all OpenGL extensions. For a full test, use the
 * SCE_CSupport() function instead of this one.
 * 
 * \see SCE_CCap
 * \see SCE_CSupport()
 */
int SCE_CHasCap (SCE_CCap cap)
{
    return caps[cap];
}

/** @} */
