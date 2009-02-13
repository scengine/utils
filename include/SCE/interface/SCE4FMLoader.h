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
 
/* Cree le : 3 novembre 2006
   derniere modification le 03/02/2008 */

#ifndef SCE4FMLOADER_H
#define SCE4FMLOADER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* fonction de chargement */
void* SCE_4FM_Load (FILE*, const char*, void*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
