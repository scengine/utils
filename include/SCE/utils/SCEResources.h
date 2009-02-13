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
 
/* Cree le : 2 janvier 2007
   derniere modification le 13/02/2008 */

#ifndef SCERESOURCES_H
#define SCERESOURCES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* initialise le gestionnaire de ressources */
int SCE_Init_Resource (void);
/* quitte le gestionnaire de ressources */
void SCE_Quit_Resource (void);

/* change l'etat du gestionnaire de ressources (active/desactive) */
void SCE_Resource_Active (int);

/* ajoute une nouvelle ressource */
int SCE_Resource_Add (const char*, void*);
/* charge une ressource */
void* SCE_Resource_Load (const char*, void*, void*);
/* charge une nouvelle ressource */
void* SCE_Resource_LoadNew (const char*, void*, void*);
/* supprime une ressource, et renvoie 'true' si la ressource n'est plus
   utilisee, et peut donc etre supprimee */
int SCE_Resource_Free (void*);

/* indique combien de fois une ressource est utilisee,
   en se basant soit sur son nom, soit sur le pointeur vers ses donnees */
int SCE_Resource_NumUsed (const char*, void*);

/* retourne le nombre de ressources chargees */
int SCE_Resource_NumLoaded (void);

/* renvoie le nom d'une ressource */
char* SCE_Resource_GetName (void*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
