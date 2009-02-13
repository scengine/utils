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
 
/* Cree le : 26 fevrier 2007
   derniere modification le 28/02/2007 */

#ifndef SCETIME_H
#define SCETIME_H

#include <time.h> /* NOTE: a mettre dans le extern "C" ? */

#ifdef __cplusplus
extern "C"
{
#endif

/* ecrit la date dans la chaine pointee en argument.
 * SCE_Time_MakeString ecrit au maximum 24 caracteres dans str.
 */
void SCE_Time_MakeString (char*, const struct tm* const);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
