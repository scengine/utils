/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2012  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
 
/* created: 21/09/2007
   updated: 14/03/2012 */

#include <stdlib.h>

#include "SCE/utils/SCEMacros.h"
#include "SCE/utils/SCEError.h"
#include "SCE/utils/SCEMemory.h"
#include "SCE/utils/SCEList.h"

/**
 * \file SCEList.c
 * \copydoc list
 * \brief Linked lists managment
 * 
 * \file SCEList.h
 * \copydoc list
 * \brief Linked lists managment
 */

/**
 * \defgroup list Linked lists managment
 * \ingroup utils
 * @{
 */

/**
 * \brief Initializes an iterator
 */
void SCE_List_InitIt (SCE_SListIterator *it)
{
    it->next = it->prev = NULL;
    it->data = NULL;
}
/**
 * \brief Creates an iterator
 */
SCE_SListIterator* SCE_List_CreateIt (void)
{
    SCE_SListIterator *it = SCE_malloc (sizeof *it);
    if (!it)
        SCEE_LogSrc ();
    else
        SCE_List_InitIt (it);
    return it;
}
/**
 * \brief Deletes an iterator
 * \param it The iterator to delete
 */
void SCE_List_DeleteIt (SCE_SListIterator *it)
{
    SCE_free (it);
}

static void SCE_List_JoinFirstLast (SCE_SList *l)
{
    l->first.next = &l->last;
    l->last.prev = &l->first;
}
/**
 * \brief Initializes a list
 * \param l the list to initialize
 * 
 * This function initializes a SCE_SList structure.
 * \sa SCE_List_Create(), SCE_SList
 */
void SCE_List_Init (SCE_SList *l)
{
    SCE_List_InitIt (&l->first);
    SCE_List_InitIt (&l->last);
    SCE_List_JoinFirstLast (l);
    l->first.data = l->last.data = l;
    l->f = NULL;
    l->f2 = NULL;
    l->f2arg = NULL;
    /* TODO: kick useless calls of CanDeleteIterators() in the engine */
    l->canfree = SCE_FALSE;     /* by default, CANT free iterators */
}
/**
 * \brief Creates a new list
 * \param f a SCE_FListFreeFunc function to free the list's data at deletion
 * \returns a newly allocated SCE_SList, or NULL on error
 * 
 * This function creates a new SCE_SList structure and initialize it; then you
 * haven't to init it yourself.
 * The SCE_FListFreeFunc is used to free the list's data when calling a free
 * function like SCE_List_Clear() or SCE_List_Delete(). If it is NULL, the
 * list's data will not be freed.
 */
SCE_SList* SCE_List_Create (SCE_FListFreeFunc f)
{
    SCE_SList *l = NULL;
    if (!(l = SCE_malloc (sizeof *l)))
        SCEE_LogSrc ();
    else {
        SCE_List_Init (l);
        l->f = f;
    }
    return l;
}
/**
 * \brief Creates a new list like SCE_List_Create(), but with a different free
 * function
 * \param arg user data for \p f
 * \param f a SCE_FListFreeFunc function to free the list's data at deletion
 * \returns a newly allocated SCE_SList, or NULL on error
 *
 * When the list is deleted, call that for each iterator:
 * \p f (\p arg, SCE_List_GetData (iterator))
 * \sa SCE_List_Create()
 */
SCE_SList* SCE_List_Create2 (void *arg, SCE_FListFreeFunc2 f)
{
    SCE_SList *l = NULL;
    if (!(l = SCE_List_Create (NULL)))
        SCEE_LogSrc ();
    else {
        l->f2 = f;
        l->f2arg = arg;
    }
    return l;
 }
/**
 * \brief Flushs a list by ignoring all its elements
 * \note do not call this function if \p l is joined to other list(s)
 */
void SCE_List_Flush (SCE_SList *l)
{
    if (SCE_List_HasElements (l)) {
        l->first.next->prev = NULL;
        l->last.prev->next = NULL;
        SCE_List_JoinFirstLast (l);
    }
}
/**
 * \brief Clears a list
 * \param l the SCE_SList to clear
 * 
 * This function clears any data in a SCE_SList by using the registered free
 * function (see SCE_List_Create()); but leave the list itself reusable without
 * any reallocation.
 * Use this function if you would change all data in the list.
 * If you just want to free the list, use SCE_List_Delete() instead.
 * \sa SCE_List_Delete(), SCE_List_Create()
 */
void SCE_List_Clear (SCE_SList *l)
{
    SCE_SListIterator *pro = NULL;
    SCE_SListIterator *it = NULL;
    SCE_List_ForEachProtected (pro, it, l)
        SCE_List_Erase (l, it);
}
/**
 * \brief Deletes a list
 * \param l the list to delete
 * 
 * This function deletes a SCE_SList and all it's data by calling
 * SCE_List_Clear().
 * After a call of this function, the list is fully deleted and can't be reused.
 * \sa SCE_List_Clear()
 */
void SCE_List_Delete (SCE_SList *l)
{
    if (l) {
        if (l->f || l->f2 || l->canfree)
            SCE_List_Clear (l);
        SCE_free (l);
    }
}

/**
 * \brief Defines if the iterators of \p l can be deleted by calling
 * SCE_List_DeleteIt() on \p l deletion
 * \param l A list
 * \param canfree can be SCE_TRUE or SCE_FALSE
 */
void SCE_List_CanDeleteIterators (SCE_SList *l, int canfree)
{
    l->canfree = canfree;
}

/**
 * \brief Defines the free function to use for the given list
 * \param l A list
 * \param f The new list's free function
 * \sa SCE_List_Create()
 */
void SCE_List_SetFreeFunc (SCE_SList *l, SCE_FListFreeFunc f)
{
    l->f = f;
    l->f2 = NULL;
    l->f2arg = NULL;
}
/**
 * \brief Defines the free function to use for the given list
 * \param l A list
 * \param f The new list's second free function
 * \param a User-defined argument for \p f
 * \sa SCE_List_Create2()
 */
void SCE_List_SetFreeFunc2 (SCE_SList *l, SCE_FListFreeFunc2 f, void *a)
{
    l->f = NULL;
    l->f2 = f;
    l->f2arg = a;
}

/**
 * \brief Check whether an iterator it attached to a list
 * \param it an iterator
 * \return SCE_TRUE if \p it is attached to a list, SCE_FALSE otherwise
 */
int SCE_List_IsAttached (const SCE_SListIterator *it)
{
    return it->prev && it->next;
}

/**
 * \brief Attaches \p new at the end of \p it
 * \param it A list iterator
 * \param new Another list iterator to attach right after \p it
 */
void SCE_List_Attach (SCE_SListIterator *it, SCE_SListIterator *new)
{
    it->next = new;
    new->prev = it;
}
/**
 * \brief Prepends a list iterator to another one
 * \param it A list iterator
 * \param new Another list iterator to prepend right before \p it
 */
void SCE_List_Prepend (SCE_SListIterator *it, SCE_SListIterator *new)
{
    if (it->prev)
        it->prev->next = new;
    new->prev = it->prev;
    new->next = it;
    it->prev = new;
}
/**
 * \brief Appends a list iterator to another one
 * \param it A list iterator
 * \param new Another list iterator to append right after \p it
 */
void SCE_List_Append (SCE_SListIterator *it, SCE_SListIterator *new)
{
    if (it->next)
        it->next->prev = new;
    new->next = it->next;
    new->prev = it;
    it->next = new;
}

#if !SCE_LIST_ABUSIVE_MACRO
/**
 * \brief
 */
void SCE_List_Prependl (SCE_SList *l, SCE_SListIterator *it)
{
    it->prev = l->first.next->prev;
    it->next = l->first.next;
    l->first.next->prev = it;
    l->first.next = it;
}
/**
 * \brief
 */
void SCE_List_Appendl (SCE_SList *l, SCE_SListIterator *it)
{
    it->next = l->last.prev->next;
    it->prev = l->last.prev;
    l->last.prev->next = it;
    l->last.prev = it;
}
#endif

/**
 * \brief Prepends data to a list iterator
 * \param i the SCE_SListIterator where data has to be prepended
 * \param d the data to prepend to the iterator
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * 
 * This function prepends data to a SCE_SListIterator.
 * \sa SCE_List_Prepend(), SCE_List_Prependl()
 */
int SCE_List_PrependNew (SCE_SListIterator *i, void *d)
{
    SCE_SListIterator *new = SCE_List_CreateIt ();
    if (!new) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    new->data = d;
    SCE_List_Prepend (i, new);
    return SCE_OK;
}
/**
 * \brief Appends data to a list iterator
 * \param i the SCE_SListIterator where data has to be appended
 * \param d the data to append to the iterator
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * 
 * This function appends data to a SCE_SListIterator.
 * \sa SCE_List_Append(), SCE_List_Appendl()
 */
int SCE_List_AppendNew (SCE_SListIterator *i, void *d)
{
    SCE_SListIterator *new = SCE_List_CreateIt ();
    if (!new) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    new->data = d;
    SCE_List_Append (i, new);
    return SCE_OK;
}

/**
 * \brief Prepends data to a SCE_SList
 * \param l the SCE_SList where data has to be prepended
 * \param d the data to prepend to the list
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * 
 * This function prepends data to a SCE_SList.
 */
int SCE_List_PrependNewl (SCE_SList *l, void *d)
{
    SCE_SListIterator *it = SCE_List_CreateIt ();
    if (!it) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    it->data = d;
    SCE_List_Prependl (l, it);
    return SCE_OK;
}
/**
 * \brief Appends data to a SCE_SList
 * \param l the SCE_SList where data has to be appended
 * \param d the data to append to the list
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * 
 * This function appends data to a SCE_SList.
 */
int SCE_List_AppendNewl (SCE_SList *l, void *d)
{
    SCE_SListIterator *it = SCE_List_CreateIt ();
    if (!it) {
        SCEE_LogSrc ();
        return SCE_ERROR;
    }
    it->data = d;
    SCE_List_Appendl (l, it);
    return SCE_OK;
}
/**
 * \brief Prepends all the iterators of \p l2 into \p l1
 * \sa SCE_List_AppendAll()
 */
void SCE_List_PrependAll (SCE_SList *l1, SCE_SList *l2)
{
    if (SCE_List_HasElements (l2)) {
        SCE_List_Attach (l2->last.prev, l1->first.next);
        SCE_List_Attach (&l1->first, l2->first.next);
        SCE_List_JoinFirstLast (l2); /* flush */
    }
}
/**
 * \brief Appends all the iterators of \p l2 into \p l1
 * \sa SCE_List_PrependAll()
 */
void SCE_List_AppendAll (SCE_SList *l1, SCE_SList *l2)
{
    if (SCE_List_HasElements (l2)) {
        SCE_List_Attach (l1->last.prev, l2->first.next);
        SCE_List_Attach (l2->last.prev, &l1->last);
        SCE_List_JoinFirstLast (l2); /* flush */
    }
}


/**
 * \brief Removes an element
 * \param it the iterator to detach
 *
 * Removes the given element \p it if it is inserted. This function checks
 * if the iterator is inserted before remove it.
 * \sa SCE_List_Removel()
 */
void SCE_List_Remove (SCE_SListIterator *it)
{
    if (it->next)
        it->next->prev = it->prev;
    if (it->prev) {
        it->prev->next = it->next;
        it->prev = NULL;
    }
    it->next = NULL;
}
#if !SCE_LIST_ABUSIVE_MACRO
/**
 * \brief Removes an element of a list
 * \param it the iterator to detach
 * 
 * This function removes the element represented by the iterator from the given
 * list and returns its data. The iterator \p it isn't deleted.
 * If you just want to remove an element and its data, use SCE_List_Erase().
 * \note This function will not work if the list if \p it is combined to other
 * lists and \p it is the first or the last iterator of its list.
 * \sa SCE_List_Remove(), SCE_List_Erase(), SCE_List_RemoveFirst(),
 * SCE_List_RemoveLast()
 */
void SCE_List_Removel (SCE_SListIterator *it)
{
    it->next->prev = it->prev;
    it->prev->next = it->next;
    it->next = NULL;
    it->prev = NULL;
}
#endif
/**
 * \brief Removes the first element of a list
 * \param l the SCE_SList from where detach data
 * \returns the removed iterator
 * 
 * This function calls SCE_List_Removel (\p l, SCE_List_GetFirst (\p l)).
 * \sa SCE_List_Removel(), SCE_List_RemoveLast(), SCE_List_EraseFirst()
 */
SCE_SListIterator* SCE_List_RemoveFirst (SCE_SList *l)
{
    SCE_SListIterator *it = l->first.next;
    SCE_List_Removel (it);
    return it;
}
/**
 * \brief Removes the last element of a list
 * \param l the SCE_SList from where detach data
 * \returns the removed iterator
 * 
 * This function calls SCE_List_Remove (\p l, SCE_List_GetLast (\p l)).
 * \sa SCE_List_Removel(), SCE_List_RemoveFirst(), SCE_List_EraseLast()
 */
SCE_SListIterator* SCE_List_RemoveLast (SCE_SList *l)
{
    SCE_SListIterator *it = l->last.prev;
    SCE_List_Removel (it);
    return it;
}

/**
 * \brief Fully remove an element of a list
 * \param l the SCE_SList from where remove element
 * \param it the iterator of the element to be removed
 * \sa SCE_List_Removel(), SCE_List_EraseFirst(), SCE_List_EraseLast()
 */
void SCE_List_Erase (SCE_SList *l, SCE_SListIterator *it)
{
    SCE_List_Remove (it);
    if (l->f)
        l->f (it->data);
    else if (l->f2)
        l->f2 (l->f2arg, it->data);
    if (l->canfree)
        SCE_List_DeleteIt (it);
}
/**
 * \brief Fully remove the first element of a list
 * \param l the SCE_SList from where detach data
 * \warning Does not check if there is any iterator to delete, if
 * the list is empty the behavior is undefined.
 * \sa SCE_List_Erase(), SCE_List_EraseLast(), SCE_List_RemoveFirst()
 */
void SCE_List_EraseFirst (SCE_SList *l)
{
#if 1
    SCE_List_Erase (l, SCE_List_GetFirst (l));
#else
    SCE_SListIterator *it = SCE_List_RemoveFirst (l);
    if (l->f)
        l->f (it->data);
    else if (l->f2)
        l->f2 (l->f2arg, it->data);
    if (l->canfree)
        SCE_List_DeleteIt (it);
#endif
}
/**
 * \brief Fully remove the last element of a list
 * \param l the SCE_SList from where detach data
 * \warning Does not check if there is any iterator to delete, if
 * the list is empty the behavior is undefined.
 * \sa SCE_List_Erase(), SCE_List_EraseFirst(), SCE_List_RemoveLast()
 */
void SCE_List_EraseLast (SCE_SList *l)
{
#if 1
    SCE_List_Erase (l, SCE_List_GetLast (l));
#else
    SCE_SListIterator *it = SCE_List_RemoveLast (l);
    if (l->f)
        l->f (it->data);
    else if (l->f2)
        l->f2 (l->f2arg, it->data);
    if (l->canfree)
        SCE_List_DeleteIt (it);
#endif
}

/**
 * \brief Removes an element of a list
 * \param l A list
 * \param data the data assigned to the iterator to detach
 * 
 * This function calls SCE_List_Remove() on the iterator returned by
 * SCE_List_LocateIterator() if it is not NULL.
 * \sa SCE_List_EraseFromData(), SCE_List_Erase(), SCE_List_Remove(),
 * SCE_List_LocateIterator()
 */
void SCE_List_RemoveFromData (SCE_SList *l, void *data)
{
    SCE_SListIterator *it = SCE_List_LocateIterator (l, data, NULL);
    if (it)
        SCE_List_Removel (it);
}

/**
 * \brief Fully remove an element of a list
 * \param l A list
 * \param data the data assigned to the iterator to remove
 *
 * This function calls SCE_List_Erase() on the iterator returned by
 * SCE_List_LocateIterator() if it is not NULL.
 * \sa SCE_List_RemoveFromData(), SCE_List_Remove(), SCE_List_Erase(),
 * SCE_List_LocateIterator()
 */
void SCE_List_EraseFromData (SCE_SList *l, void *data)
{
    SCE_SListIterator *it = SCE_List_LocateIterator (l, data, NULL);
    if (it)
        SCE_List_Erase (l, it);
}


/**
 * \brief Attachs \p l2 next to \p l1
 * \sa SCE_List_Insert(), SCE_List_Extract(), SCE_SList::first
 */
void SCE_List_Join (SCE_SList *l1, SCE_SList *l2)
{
    l2->first.prev = &l1->last;
    l2->first.next->prev = l1->last.prev;

    l1->last.next = &l2->first;
    l1->last.prev->next = l2->first.next;
}

/**
 * \brief Inserts \p l2 next to \p l1.
 *
 * Inserts \p l2 next to \p l1, the list attached to the end of \p l1 is then
 * attached to \p l2.
 * \sa SCE_List_Join(), SCE_List_Extract()
 */
void SCE_List_Insert (SCE_SList *l1, SCE_SList *l2)
{
    if (!l1->last.next)
        SCE_List_Join (l1, l2);
    else {
        SCE_SList *l3 = l1->last.next->data;
        SCE_List_BreakEnd (l1);
        SCE_List_Join (l1, l2);
        SCE_List_Join (l2, l3);
    }
}

static void SCE_List_InitFirst (SCE_SListIterator *it)
{
    it->prev = NULL;
    it->next->prev = it;
}
static void SCE_List_InitLast (SCE_SListIterator *it)
{
    it->next = NULL;
    it->prev->next = it;
}

/**
 * \brief Breaks a join created by SCE_List_Join()
 * \sa SCE_List_BreakEnd()
 */
void SCE_List_BreakStart (SCE_SList *l)
{
    if (l->first.prev)
        SCE_List_InitLast (l->first.prev);
    SCE_List_InitFirst (&l->first);
}
/**
 * \brief Breaks a join created by SCE_List_Join()
 * \sa SCE_List_BreakStart()
 */
void SCE_List_BreakEnd (SCE_SList *l)
{
    if (l->last.next)
        SCE_List_InitFirst (l->last.next);
    SCE_List_InitLast (&l->last);
}

/**
 * \brief Breaks a list entirely, removes it from its continuation (if any)
 * \sa SCE_List_BreakAll(), SCE_List_BreakStart(), SCE_List_BreakEnd()
 */
void SCE_List_Break (SCE_SList *l)
{
    SCE_List_BreakStart (l);
    SCE_List_BreakEnd (l);
}
/**
 * \brief Breaks a list continuation
 * \sa SCE_List_Break(), SCE_List_BreakStart(), SCE_List_BreakEnd()
 */
void SCE_List_BreakAll (SCE_SList *l)
{
    SCE_SList *lpro = NULL, *list = l;
    if (l->first.prev)
        l = l->first.prev->data;
    SCE_List_ForEachNextListProtected (lpro, list)
        SCE_List_Break (list);
    SCE_List_ForEachPrevListProtected (lpro, l)
        SCE_List_Break (l);
}

/**
 * \brief Extracts a list from a continuation of lists
 * \sa SCE_List_Break(), SCE_List_BreakAll(), SCE_List_Insert()
 */
void SCE_List_Extract (SCE_SList *l)
{
    SCE_SListIterator *prv, *nxt;
    prv = l->first.prev;
    nxt = l->last.next;
    if (prv) {
        if (!nxt)
            SCE_List_InitLast (prv);
        else {
            prv->next = nxt;
            prv->prev->next = nxt->next;
            nxt->prev = prv;
            nxt->next->prev = prv->prev;
        }
    }
    else if (nxt)
        SCE_List_InitFirst (nxt);

    SCE_List_InitFirst (&l->first);
    SCE_List_InitLast (&l->last);
}


/**
 * \brief Sets data of an iterator
 * \param it the SCE_SListIterator where set the data
 * \param data the data to set to the iterator
 * \returns the old data, if any
 */
void* SCE_List_SetData (SCE_SListIterator *it, void *data)
{
    void *old = it->data;
    it->data = data;
    return old;
}
#if 0
/**
 * \brief Gets data of an iterator
 * \param it the SCE_SListIterator from where get data
 * \returns the data, if any
 */
void* SCE_List_GetData (SCE_SListIterator *it)
{
    return it->data;
}
#endif

/**
 * \brief Gets the length of a SCE_SList
 * \param l a SCE_SList
 * \returns the length of the list pointed by \p l
 */
unsigned int SCE_List_GetLength (const SCE_SList *l)
{
    unsigned int n = 0;
    SCE_SListIterator *it;
    SCE_List_ForEach (it, l)
        n++;
    return n;
}

/**
 * \brief Gets the first iterator of a list
 * \param l the list from where get the iterator
 * \returns the first iterator of the list, if any
 */
SCE_SListIterator* SCE_List_GetFirst (const SCE_SList *l)
{
    return l->first.next;
}
/**
 * \brief Gets the last iterator of a list
 * \param l the list from where get the iterator
 * \returns the last iterator of the list, if any
 */
SCE_SListIterator* SCE_List_GetLast (const SCE_SList *l)
{
    return l->last.prev;
}

/**
 * \brief Has \p l any element?
 */
int SCE_List_HasElements (const SCE_SList *l)
{
    return (l->first.next != &l->last);
}

/**
 * \brief Gets the next iterator of an interator
 * \param it the base iterator
 * \returns the next iterator, if any
 */
SCE_SListIterator* SCE_List_GetNext (const SCE_SListIterator *it)
{
    return it->next;
}
/**
 * \brief Gets the previous iterator of an interator
 * \param it the base iterator
 * \returns the previous iterator, if any
 */
SCE_SListIterator* SCE_List_GetPrev (const SCE_SListIterator *it)
{
    return it->prev;
}

/**
 * \brief Is \p it the first iterator of the list \p l ?
 * \sa SCE_List_IsLast(), SCE_List_GetFirst()
 */
int SCE_List_IsFirst (const SCE_SList *l, const SCE_SListIterator *it)
{
    return (it == l->first.next);
}
/**
 * \brief Is \p it the last iterator of the list \p l ?
 * \sa SCE_List_IsFirst(), SCE_List_GetLast()
 */
int SCE_List_IsLast (const SCE_SList *l, const SCE_SListIterator *it)
{
    return (it == l->last.prev);
}


/**
 * \brief Gets the index position of an iterator
 * \param it the iterator from which you would know the index
 * \returns the index position of the iterator
 */
unsigned int SCE_List_GetIndex (const SCE_SListIterator *it)
{
    unsigned int n = 0;
    SCE_List_ForEachPrev (it)
        n++;
    return n - 1;
}

/**
 * \brief Gets an iterator of a list from its index position
 * \param l the SCE_SList where search for iterator
 * \param n the index of the iterator you're searching for
 * \returns the iterator at \p n position, if any
 */
SCE_SListIterator* SCE_List_GetIterator (const SCE_SList *l, unsigned int n)
{
    SCE_SListIterator *it = NULL;
    unsigned int i = 0;
    
    SCE_List_ForEach (it, l) {
        if (i == n)
            return it;
        i++;
    }
    return NULL;
}

/**
 * \brief Gets an iterator of a list by its data
 * \param l an SCE_SList list
 * \param data the data to compare to each list's data
 * \param f a function to compares \p data to each list's data, or NULL for a
 * pointer comparison
 * \returns the iterator found, if any
 * 
 * This function searches for an iterator containing \p data.
 * The comparison is done by calling \p f and/or comparing the pointers.
 */
SCE_SListIterator* SCE_List_LocateIterator (const SCE_SList *l, void *data,
                                            SCE_FListCompareData f)
{
    SCE_SListIterator *i = NULL;
    SCE_List_ForEach (i, l) {
        if ((f && f (data, i->data)) || data == i->data)
            return i;
    }
    return NULL;
}

/**
 * \brief Gets an iterator's index of a list by its data
 * \param l an SCE_SList list
 * \param data the data to compare to each list's data
 * \param f a function to compares \p data to each list's data, or NULL for a
 * pointer comparison
 * \returns the founded iterator's index, or 0 if index can't be found
 * \sa SCE_List_LocateIterator()
 */
unsigned int SCE_List_LocateIndex (const SCE_SList *l, void *data,
                                   SCE_FListCompareData f)
{
    unsigned int j = 0;
    SCE_SListIterator *i = NULL;
    SCE_List_ForEach (i, l) {
        if ((f && f (data, i->data)) || data == i->data)
            return j;
        j++;
    }
    return 0;
}

#if 0

void SCE_List_Sort (SCE_SList *l, SCE_FListCompareData comesafter)
{
    SCE_SListIterator *first = l->first.next;
    SCE_List_Flush (l);
}
#endif

/* part of the quicksort implementation */
static unsigned int SCE_List_QuickSortPartition (SCE_SList *l,
                                                 unsigned int start,
                                                 unsigned int end,
                                                 SCE_FListCompareData func)
{
    while (start < end) {
        SCE_SListIterator *it_s;
        SCE_SListIterator *it_e;

        it_s = SCE_List_GetIterator (l, start);
        it_e = SCE_List_GetIterator (l, end);
        while (start < end) {
            if (func (it_s->data, it_e->data) > 0) {
                SCE_List_Swapl (it_s, it_e);
                break;
            }
            end --;
            it_e = it_e->prev;
        }
        it_s = SCE_List_GetIterator (l, start);
        it_e = SCE_List_GetIterator (l, end);
        while (start < end) {
            if (func (it_s->data, it_e->data) > 0) {
                SCE_List_Swapl (it_s, it_e);
                break;
            }
            start ++;
            it_s = it_s->next;
        }
    }
    return start;
}

/**
 * \brief Sorts a specified range in a list
 * \param l a list
 * \param start the start of the range
 * \param end the end of the range, plus one (e.g. SCE_List_GetLength())
 * \param func a function used to compare two elements of the list
 * 
 * \warning \p start and \p end must be valid for the given list
 * 
 * \see SCE_List_QuickSort()
 */
void SCE_List_QuickSortRange (SCE_SList *l, unsigned int start,
                              unsigned int end, SCE_FListCompareData func)
{
    if (start < end) {
        unsigned int p;
        
        p = SCE_List_QuickSortPartition (l, start, end-1, func);
        SCE_List_QuickSortRange (l, start, p, func);
        SCE_List_QuickSortRange (l, p+1, end, func);
    }
}

/**
 * \brief Sorts a list
 * \param l a list
 * \param func a function used to compare two elements of the list
 * 
 * This function simply calls
 * SCE_List_QuickSortRange(l, 0, SCE_List_GetLength (l), func).
 * 
 * \see SCE_List_QuickSortRange()
 */
void SCE_List_QuickSort (SCE_SList *l, SCE_FListCompareData func)
{
    SCE_List_QuickSortRange (l, 0, SCE_List_GetLength (l), func);
}

/**
 * @brief Swaps two elements in a list
 * @param a The element to swap with @b
 * @param b The element to swap with @a
 */
void SCE_List_Swapl (SCE_SListIterator *a, SCE_SListIterator *b)
{
    if (a->next == b) {
        a->prev->next = b;
        b->next->prev = a;
        a->next = b->next;
        b->prev = a->prev;
        a->prev = b;
        b->next = a;
    } else if (b->next == a) {
        b->prev->next = a;
        a->next->prev = b;
        b->next = a->next;
        a->prev = b->prev;
        b->prev = a;
        a->next = b;
    } else {
        SCE_SListIterator *prev_a = a->prev;
        SCE_SListIterator *prev_b = b->prev;

        SCE_List_Removel (a);
        SCE_List_Removel (b);
        SCE_List_Append (prev_a, b);
        SCE_List_Append (prev_b, a);
    }
}

/**
 * @brief Swaps two elements
 * @param a The element to swap with @b
 * @param b The element to swap with @a
 */
void SCE_List_Swap (SCE_SListIterator *a, SCE_SListIterator *b)
{
    if (a->next == b) {
        if (a->prev) a->prev->next = b;
        if (b->next) b->next->prev = a;
        a->next = b->next;
        b->prev = a->prev;
        a->prev = b;
        b->next = a;
    } else if (b->next == a) {
        if (b->prev) b->prev->next = a;
        if (a->next) a->next->prev = b;
        b->next = a->next;
        a->prev = b->prev;
        b->prev = a;
        a->next = b;
    } else {
        SCE_SListIterator *prev_a = a->prev;
        SCE_SListIterator *next_a = a->next;
        SCE_SListIterator *prev_b = b->prev;
        SCE_SListIterator *next_b = b->next;

        SCE_List_Remove (a);
        SCE_List_Remove (b);
        if (prev_a) {
            SCE_List_Append (prev_a, b);
        } else {
            SCE_List_Prepend (b, next_a);
        }
        if (prev_b) {
            SCE_List_Append (prev_b, a);
        } else {
            SCE_List_Prepend (a, next_b);
        }
    }
}

/**
 * @brief Sorts a list
 * @param l a list
 * @param func a function used to compare two elements of the list
 * 
 * This function sorts a list using the GnomeSort algorithm.
 */
void SCE_List_GnomeSort (SCE_SList *l, SCE_FListCompareData func)
{
    SCE_SListIterator *it = SCE_List_GetFirst (l);
    SCE_SListIterator *last_it = it;
    unsigned int cur = 0, last = 0;

    while (&l->last != it) {
        if (SCE_List_IsFirst (l, it) || func (it->data, it->prev->data) >= 0) {
            if (cur < last) {
                it = last_it->next;
                cur = last + 1;
            } else {
                it = it->next;
                cur ++;
            }
        } else {
            SCE_List_Swapl (it, it->prev);
            if (cur > last) {
                last_it = it->next;
                last = cur;
            } else {
                cur --;
            }
        }
    }
}

/** @} */
