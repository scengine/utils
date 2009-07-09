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
 
/* created: 23/03/2008
   updated: 07/04/2008 */

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECSupport.h>
#include <SCE/core/SCECOcclusionQuery.h>

/**
 * \file SCECOcclusionQuery.c
 * \copydoc occlusionquery
 * \brief Occlusion queries managment
 * 
 * \file SCECOcclusionQuery.h
 * \copydoc occlusionquery
 * \brief Occlusion queries managment
 */

/**
 * \defgroup occlusionquery Occlusion queries managment
 * \ingroup core
 * \brief Occlusion queries managment
 */

/** @{ */


static SCEuint queryid = 0;
static int drawpixels = 0;
static int funcid = 1;

typedef void (*SCE_FOcclusionQueryFunc)(int);
static void SCE_COcclusionQueryCallback (int);
static void SCE_COcclusionQueryNull (int);

static SCE_FOcclusionQueryFunc funcs[] =
{
    SCE_COcclusionQueryCallback, SCE_COcclusionQueryNull
};

/* ajoute le 23/03/2008 */
/**
 * \internal
 */
int SCE_COcclusionQueryInit (void)
{
    if (SCE_CHasCap (SCE_OCCLUSION_QUERY))
    {
        glGenQueries (1, &queryid);
        funcid = 0;
    }
    else
        funcid = 1;
    return SCE_OK;
}
/**
 * \internal
 */
void SCE_COcclusionQueryQuit (void)
{
    if (SCE_CHasCap (SCE_OCCLUSION_QUERY))
        glDeleteQueries (1, &queryid);
    queryid = 0; drawpixels = 0;
}

static void SCE_COcclusionQueryCallback (int begin)
{
    if (begin)
        glBeginQuery (GL_SAMPLES_PASSED, queryid);
    else
    {
        glEndQuery (GL_SAMPLES_PASSED);
        glGetQueryObjectiv (queryid, GL_QUERY_RESULT, &drawpixels);
    }
}
/**
 * \internal
 * \brief Dummy callback to use if occlusion queries aren't supported by the
 *        hardware
 */
static void SCE_COcclusionQueryNull (int begin)
{
}

/* ajoute le 23/03/2008 */
/**
 * \brief Starts or ends an occulsion query
 * \param begin SCE_TRUE to begin an occulsion query or SCE_FALSE to end the
 *        current one
 * 
 * \note This function doesn't do anything if occlusion queries aren't supported
 *       in hardware.
 * \note You can check if the hardware supports occlusion queries by calling
 *       SCE_CHasCap(SCE_OCCLUSION_QUERY)
 * 
 * \see SCE_CHasCap()
 */
void SCE_COcclusionQuery (int begin)
{
    funcs[funcid] (begin);
}

/* ajoute le 23/03/2008 */
/**
 * \brief Gets result of an occlusion query
 * \returns the number of drawn pixels
 * 
 * \note This function always returns 0 if occlusion queries aren't supported by
 *       in hardware.
 * \note You can check if the hardware supports occlusion queries by calling
 *       SCE_CHasCap(SCE_OCCLUSION_QUERY)
 * 
 * \see SCE_COcclusionQuery()
 * \see SCE_CHasCap()
 */
int SCE_CGetQueryResult (void)
{
    return drawpixels;
}

/** @} */
