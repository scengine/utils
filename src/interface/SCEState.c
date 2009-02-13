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

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCEState.h>

/* ajoute le 07/03/2008 */
int SCE_Init_State (void)
{
    return SCE_OK;
}
/* ajoute le 07/03/2008 */
void SCE_Quit_State (void)
{
}


/* ajoute le 07/03/2008 */
void SCE_State_InitManager (SCE_SStateManager *sm)
{
    unsigned int i;
    sm->sa = NULL;
    for (i=0; i<SCE_STATE_MAXPUSHS; i++)
        sm->pstates[i] = 0;
    sm->n_pushed = 0;
}
/* ajoute le 07/03/2008 */
/* revise le 29/03/2008 */
void SCE_State_Init (SCE_SState *s)
{
    s->set[0] = s->set[1] = NULL;
    s->use_unset = SCE_FALSE;
    s->state = 0;
    s->param = NULL;
    s->canfree = SCE_FALSE;
}
/* ajoute le 07/03/2008 */
void SCE_State_InitApplicator (SCE_SStateApplicator *sa)
{
    sa->objs = NULL;
    sa->states = NULL;
    sa->canfree = SCE_FALSE;
}

/* ajoute le 07/03/2008 */
SCE_SStateManager* SCE_State_CreateManager (void)
{
    SCE_SStateManager *sm = NULL;

    SCE_btstart ();
    if (!(sm = SCE_malloc (sizeof *sm)))
        Logger_LogSrc ();
    else
    {
        SCE_State_InitManager (sm);
        sm->sa = SCE_List_Create((SCE_FListFreeFunc)SCE_State_DeleteApplicator);
        if (!sm->sa)
        {
            SCE_State_DeleteManager (sm), sm = NULL;
            Logger_LogSrc ();
        }
    }
    SCE_btend ();
    return sm;
}
/* ajoute le 07/03/2008 */
SCE_SState* SCE_State_Create (void)
{
    SCE_SState *s = NULL;

    SCE_btstart ();
    if (!(s = SCE_malloc (sizeof *s)))
        Logger_LogSrc ();
    else
        SCE_State_Init (s);
    SCE_btend ();
    return s;
}
/* ajoute le 07/03/2008 */
SCE_SStateApplicator* SCE_State_CreateApplicator (void)
{
    SCE_SStateApplicator *sa = NULL;

    SCE_btstart ();
    if (!(sa = SCE_malloc (sizeof *sa)))
        Logger_LogSrc ();
    else
    {
        SCE_State_InitApplicator (sa);
        if (!(sa->objs = SCE_List_Create (NULL)))
        {
            SCE_State_DeleteApplicator (sa);
            Logger_LogSrc ();
            SCE_btend ();
            return NULL;
        }
        sa->states = SCE_List_Create ((SCE_FListFreeFunc)SCE_State_Delete);
        if (!sa)
        {
            SCE_State_DeleteApplicator (sa), sa = NULL;
            Logger_LogSrc ();
        }
    }
    SCE_btend ();
    return sa;
}

/* ajoute le 07/03/2008 */
void SCE_State_DeleteManager (SCE_SStateManager *sm)
{
    if (sm)
    {
        SCE_List_Delete (sm->sa);
        SCE_free (sm);
    }
}
/* ajoute le 07/03/2008 */
void SCE_State_Delete (SCE_SState *s)
{
    if (s && s->canfree)
        SCE_free (s);
}
/* ajoute le 07/03/2008 */
void SCE_State_DeleteApplicator (SCE_SStateApplicator *sa)
{
    if (sa && sa->canfree)
    {
        SCE_List_Delete (sa->objs);
        SCE_List_Delete (sa->states);
        SCE_free (sa);
    }
}

/* ajoute le 07/03/2008 */
int SCE_State_GenID (SCE_SStateManager *sm)
{
    sm->id++;
    return sm->id;
}

/* ajoute le 07/03/2008 */
int SCE_State_AddApplicator (SCE_SStateManager *sm, SCE_SStateApplicator *sa)
{
    SCE_btstart ();
    if (SCE_List_AppendNewl (sm->sa, sa) < 0)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_btend ();
    return SCE_OK;
}
/* ajoute le 07/03/2008 */
void SCE_State_RemoveApplicator(SCE_SStateManager *sm, SCE_SStateApplicator *sa)
{
    SCE_SListIterator *it = NULL;
    SCE_btstart ();
    if ((it = SCE_List_LocateIterator (sm->sa, sa, NULL)))
    {
        SCE_List_Remove (sm->sa, it);
        SCE_List_DeleteIt (it);
    }
    SCE_btend ();
}

/* ajoute le 07/03/2008 */
/* application d'un etat a tous les objets d'un applicateur (garder privee ?) */
static void SCE_State_Apply (SCE_SStateApplicator *sa, int state, int mode)
{
    SCE_SState *s = NULL;
    SCE_SListIterator *i = NULL, *j = NULL;

    SCE_btstart ();
    SCE_List_ForEach (i, sa->states)
    {
        s = SCE_List_GetData (i);
        if (s->state == state && !(mode == SCE_STATE_UNSET && !s->use_unset))
        {
            SCE_List_ForEach (j, sa->objs)
                s->set[mode] (SCE_List_GetData (j), s->param);
        }
    }
    SCE_btend ();
}

/* ajoute le 07/03/2008 */
void SCE_State_Set (SCE_SStateManager *sm, int state, int mode)
{
    SCE_SListIterator *i = NULL;
    SCE_List_ForEach (i, sm->sa)
        SCE_State_Apply (SCE_List_GetData (i), state, mode);
}

/* ajoute le 07/03/2008 */
void SCE_State_Push (SCE_SStateManager *sm, int state)
{
    if (sm->n_pushed < SCE_STATE_MAXPUSHS)
    {
        SCE_State_Set (sm, state, SCE_STATE_SET);
        sm->pstates[sm->n_pushed] = state;
        sm->n_pushed++;
    }
}
/* ajoute le 07/03/2008 */
void SCE_State_Pop (SCE_SStateManager *sm)
{
    if (sm->n_pushed > 0)
    {
        sm->n_pushed--;
        SCE_State_Set (sm, sm->pstates[sm->n_pushed], SCE_STATE_UNSET);
    }
}


/* ajoute le 07/03/2008 */
int SCE_State_AddObjects (SCE_SStateApplicator *sa, void **objs, unsigned int n)
{
    unsigned int i;

    SCE_btstart ();
    for (i=0; i<n; i++)
    {
        if (SCE_List_AppendNewl (sa->objs, objs[i]) < 0)
        {
            Logger_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
    }
    SCE_btend ();
    return SCE_OK;
}

/* ajoute le 07/03/2008 */
int SCE_State_AddState (SCE_SStateApplicator *sa, SCE_SState *s)
{
    SCE_btstart ();
    if (SCE_List_PrependNewl (sa->states, s) < 0)
    {
        Logger_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_btend ();
    return SCE_OK;
}

/* ajoute le 07/03/2008 */
int SCE_State_AddNewState (SCE_SStateApplicator *sa, int state,
                           SCE_FStateFunc set, SCE_FStateFunc unset, void *p)
{
    SCE_SState *s = NULL;

#define SCE_ASSERT(c)\
    if (c)\
    {\
        SCE_State_Delete (s);\
        Logger_LogSrc ();\
        SCE_btend ();\
        return SCE_ERROR;\
    }

    SCE_btstart ();
    SCE_ASSERT (!(s = SCE_State_Create ()))
    s->canfree = SCE_TRUE;
    SCE_ASSERT (SCE_State_AddState (sa, s) < 0)

    s->set[SCE_STATE_SET] = set;
    s->set[SCE_STATE_UNSET] = unset;
    s->state = state;
    s->param = p;

    SCE_btend ();
    return SCE_OK;
}
