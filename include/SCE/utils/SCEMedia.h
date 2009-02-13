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
 
/* Cree le : 5 janvier 2007
   derniere modification le 03/10/2007 */

#ifndef SCEMEDIA_H
#define SCEMEDIA_H

#ifdef __cplusplus
extern "C"
{
#endif

#define SCE_UNKNOWN_TYPE -1


typedef void* (*SCE_FMediaLoadFunc) (FILE*, const char*, void*);
typedef int (*SCE_FMediaSaveFunc) (const char*, void*);


/* fonction d'initialisation */
int SCE_Init_Media (void);
/* libere la memoire allouee par le MediaManager */
void SCE_Quit_Media (void);

/* fonction de generation d'un identifiant de type */
int SCE_Media_GenTypeID (void);

/* fonction de chargement d'un media */
void* SCE_Media_LoadFromFile (const char*, void*, int*);

/* fonction d'enregistrement d'une fonction de chargement d'un media
   pour un nombre magic et une extension specifies */
int SCE_Media_RegisterLoader (int, int, const char*, SCE_FMediaLoadFunc);

/* fonction de sauvegarde d'un media */
int SCE_Media_SaveToFile (void*, const char*, int);

/* fonction d'enregistrement d'une fonction de sauvegarde d'un media */
int SCE_Media_RegisterSaver (int, SCE_FMediaSaveFunc);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
