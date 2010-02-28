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

#ifndef SCEUTILS_H
#define SCEUTILS_H

#include <SCE/utils/SCEBacktracer.h>
#include <SCE/utils/SCEError.h>
#include <SCE/utils/SCEMemory.h>
#include <SCE/utils/SCETime.h>

#include <SCE/utils/SCEMath.h>
#include <SCE/utils/SCEVector.h>
#include <SCE/utils/SCEQuaternion.h>
#include <SCE/utils/SCEMatrix.h>
#include <SCE/utils/SCELine.h>
#include <SCE/utils/SCERectangle.h>
#include <SCE/utils/SCEPlane.h>

#include <SCE/utils/SCEInert.h>
#include <SCE/utils/SCEMedia.h>
#include <SCE/utils/SCEResource.h>
#include <SCE/utils/SCEList.h>
#include <SCE/utils/SCEListFastForeach.h>
#include <SCE/utils/SCEString.h>

#ifdef __cplusplus
extern "C"
{
#endif

int SCE_Init_Utils (FILE*);
void SCE_Quit_Utils (void);

/**
 * \addtogroup utils
 * \def SCE_FLAG_ADD
 * \brief Adds a flag to a flag variable
 * \param var the variable
 * \param the flag to add
 * 
 * \note it is the same to do var |= flag
 */
#define SCE_FLAG_ADD(var, flag)     (var |= flag)
/**
 * \addtogroup utils
 * \def SCE_FLAG_REMOVE
 * \brief Removes a flag from a flag variable
 * \param var the variable
 * \param the flag to remove
 * 
 * This removes a flag from a set of flags. This macro works if \p var does
 * not already have the flag to remove, in contrast to naïve var ^= flag.
 */
#define SCE_FLAG_REMOVE(var, flag)  (var = (var | flag) ^ flag)
/**
 * \addtogroup utils
 * \def SCE_FLAG_TEST
 * \brief Checks whether a flag is in a variable
 * \param var the variable
 * \param the flag to test for
 * 
 * \note it is the same to do var & flag
 */
#define SCE_FLAG_TEST(var, flag)    (var & flag)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
