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
 
/* Cree le : 21/10/2007
   derniere modification : 16/10/2008 */

#ifndef SCEBACKTRACER_H
#define SCEBACKTRACER_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef SCE_USE_BACKTRACER
/* enregistre l'entree dans une fonction */
void SCE_BT_Start (const char*, unsigned int);

/* enregistre la sortie de la fonction courante */
void SCE_BT_End (void);

#define SCE_btstart() SCE_BT_Start (__FUNCTION__, __LINE__)
#define SCE_btend() SCE_BT_End ()
#else
#define SCE_btstart()
#define SCE_btend()
#endif /* SCE_USE_BACKTRACER */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
