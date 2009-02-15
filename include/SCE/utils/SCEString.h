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
 
/* Cree le : 28 fevrier 2007
   derniere modification le 13/02/2008 */

#ifndef SCESTRING_H
#define SCESTRING_H


/* on fait de cet en-tete le seul et unique
 * en-tete de gestion des chaines de caracteres
 */
#include <string.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* retourne le chiffre auquel correspond la chaine str */
double SCE_String_Valof (const char*);

/* retourne la chaine correspondante au chiffre passe en argument
 * nd represente la precision decimale souhaitee.
 */
const char* SCE_String_Strof (double, unsigned int);

/* renvoie de nombre d'occurrences au caractere '2nd param' dans la chaine */
unsigned int SCE_String_NChrInStr (const char*, char);

/* renvoie l'extension d'un nom de fichier passe en parametre */
char* SCE_String_GetExt (char*);

/* compare deux chaines en comptant les majuscules ou non */
int SCE_String_Cmp (const char*, const char*, int);

char* SCE_String_Dup (const char*);

char* SCE_String_CatDup (const char*, const char*);

char* SCE_String_CatDupMulti (const char* str, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
