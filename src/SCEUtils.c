/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2010  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
   updated: 10/04/2010 */

#include <stdio.h>
#include <pthread.h>

#include "SCE/utils/SCEUtils.h"

/**
 * \defgroup utils Useful functions
 * \ingroup SCEngine
 * \internal
 * \brief Useful functions used in the other parts of SCE
 */

static pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned int init_n = 0;

/**
 * \brief Initializes the sub-module 'utils' of the SCEngine
 * \param outlog stream where write the error messages
 * \return SCE_ERROR on error, SCE_OK otherwise
 * \note SCEE_LogSrc() don't have to be used if this function fails.
 * 
 * Initialization is thread-safe. Initialization is stacked, so it can be done
 * more than once, but each initialization call must have a corresponding
 * uninitialization call (SCE_Quit_Utils ()).
 */
int SCE_Init_Utils (FILE *outlog)
{
    int ret = SCE_OK;
    if (pthread_mutex_lock (&init_mutex) != 0) {
        ret = SCE_ERROR;
        SCE_Init_Error (outlog);
        SCEE_Log (42);
        SCEE_LogMsg ("failed to lock initialization mutex");
    } else {
        init_n++;
        if (init_n == 1) {
            SCE_Init_Error (outlog);
            ret = SCE_ERROR;
            if (SCE_Init_Mem () < 0) {
                SCEE_LogSrc ();
                SCEE_LogSrcMsg ("can't initialize memory manager");
            } else if (SCE_Init_Matrix () < 0) {
                SCEE_LogSrc ();
                SCEE_LogSrcMsg ("can't initialize matrices manager");
            } else if (SCE_Init_FastList () < 0) {
                SCEE_LogSrc ();
                SCEE_LogSrcMsg ("can't initialize fast lists manager");
            } else if (SCE_Init_Media () < 0) {
                SCEE_LogSrc ();
                SCEE_LogSrcMsg ("can't initialize medias manager");
            } else if (SCE_Init_Resource () < 0) {
                SCEE_LogSrc ();
                SCEE_LogSrcMsg ("can't initialize resources manager");
            } else {
                ret = SCE_OK;
            }
        }
        pthread_mutex_unlock (&init_mutex);
        if (ret == SCE_ERROR) {
            SCE_Quit_Utils ();
        }
    }
    return ret;
}

/**
 * \brief Quit the sub-module 'utils' of the SCEngine
 */
void SCE_Quit_Utils (void)
{
    if (pthread_mutex_lock (&init_mutex) != 0) {
        SCEE_Log (42);
        SCEE_LogMsg ("failed to lock initialization mutex");
    } else {
        init_n--;
        if (init_n < 1) {
            SCE_Quit_Resource ();
            SCE_Quit_Media ();
            SCE_Quit_FastList ();
            /*SCE_Quit_Matrix ();*/
            /*SCE_Quit_Error ();*/
            SCE_Quit_Mem ();
        }
        pthread_mutex_unlock (&init_mutex);
    }
}
