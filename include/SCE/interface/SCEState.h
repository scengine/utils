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
 
/* Cree le : 07/03/2008
   derniere modification : 31/03/2008 */

#ifndef SCESTATE_H
#define SCESTATE_H

#include <SCE/utils/SCEList.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SCE_STATE_MAXPUSHS 16

/* structure de gestion d'un environnement d'etats */
typedef struct sce_sstatemanager SCE_SStateManager;
struct sce_sstatemanager
{
    SCE_SList *sa;                   /* tous les applicateurs d'etats */
    int pstates[SCE_STATE_MAXPUSHS]; /* pushed states */
    unsigned int n_pushed;           /* nombre d'etats "pushes" */
    int id;                        /* id du dernier etat cree pour ce manager */
};

#define SCE_STATE_SET 0
#define SCE_STATE_UNSET 1

typedef void (*SCE_FStateFunc)(void*, void*);

/* structure caracterisant un etat et ses fonctions d'application */
typedef struct sce_sstate SCE_SState;
struct sce_sstate
{
    SCE_FStateFunc set[2]; /* fonction de mise en application de l'etat &
                              fonction d'annulation des modifications de set */
    int use_unset;         /* defini si on utilisera la fonction set[1]() */
    int state;             /* ID de l'etat que mettra en place la fonction */
    void *param;           /* second parametre de set */
    int canfree;           /* indique si cette structure peut etre detruite */
};

/* applicateur d'etat, possede un pointeur sur tous les objets
   qu'il doit mettre en etat, ainsi que plusieurs etats */
typedef struct sce_sstateapplicator SCE_SStateApplicator;
struct sce_sstateapplicator
{
    SCE_SList *objs;   /* objets auxquels appliquer les etats */
    SCE_SList *states; /* etats a attribuer aux objets */
    int canfree;       /* indique si cette structure peut etre detruite */
};

int SCE_Init_State (void);
void SCE_Quit_State (void);

/* initialise une structure */
void SCE_State_InitManager (SCE_SStateManager*);
void SCE_State_Init (SCE_SState*);
void SCE_State_InitApplicator (SCE_SStateApplicator*);

/* cree une structure */
SCE_SStateManager* SCE_State_CreateManager (void);
SCE_SState* SCE_State_Create (void);
SCE_SStateApplicator* SCE_State_CreateApplicator (void);

/* detruit une structure */
void SCE_State_DeleteManager (SCE_SStateManager*);
void SCE_State_Delete (SCE_SState*);
void SCE_State_DeleteApplicator (SCE_SStateApplicator*);

/* genere un identifiant encore non utilise pour ce manager */
int SCE_State_GenID (SCE_SStateManager*);

/* ajoute un applicateur a un manager */
int SCE_State_AddApplicator (SCE_SStateManager*, SCE_SStateApplicator*);
/* enleve un applicateur a un manager */
void SCE_State_RemoveApplicator (SCE_SStateManager*, SCE_SStateApplicator*);
/* defini l'etat courant pour tous les applicateurs */
void SCE_State_Set (SCE_SStateManager*, int, int);
/* idem en systeme de push/pop */
void SCE_State_Push (SCE_SStateManager*, int);
void SCE_State_Pop (SCE_SStateManager*);

/* ajoute des objets a un appliqueur d'etat */
int SCE_State_AddObjects (SCE_SStateApplicator*, void**, unsigned int);

/* ajoute un etat a un appliqueur */
int SCE_State_AddState (SCE_SStateApplicator*, SCE_SState*);
int SCE_State_AddNewState (SCE_SStateApplicator*, int,
                           SCE_FStateFunc, SCE_FStateFunc, void*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
