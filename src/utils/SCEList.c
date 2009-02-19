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
 
/* created: 21/09/2007
   updated: 14/02/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEList.h>

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
 */

/** @{ */

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
        Logger_LogSrc ();
    else
        SCE_List_InitIt (it);
    return it;
}

/**
 * \brief Deletes an iterator
 */
void SCE_List_DeleteIt (SCE_SListIterator *it)
{
    SCE_free (it);
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
    l->first = NULL;
    l->f = NULL;
    l->canfree = SCE_TRUE;
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
        Logger_LogSrc ();
    else
    {
        SCE_List_Init (l);
        l->f = f;
    }
    return l;
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
    SCE_SListIterator *it = l->first;
    SCE_SListIterator *tmp = NULL;

    while (it)
    {
        tmp = it;
        it = it->next;
        if (l->f)
            l->f (tmp->data);
        if (l->canfree)
            SCE_List_DeleteIt (tmp);
    }
    l->first = NULL;
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
    if (l)
    {
        SCE_List_Clear (l);
        SCE_free (l);
    }
}

/**
 * \brief Defines if the iterators of \p l can be deleted by callin
 * SCE_List_DeleteIt() on \p l deletion
 * \param canfree can be SCE_TRUE or SCE_FALSE
 */
void SCE_List_CanDeleteIterators (SCE_SList *l, int canfree)
{
    l->canfree = canfree;
}


/**
 * \brief
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
 * \brief
 */
void SCE_List_Append (SCE_SListIterator *it, SCE_SListIterator *new)
{
    if (it->next)
        it->next->prev = new;
    new->next = it->next;
    new->prev = it;
    it->next = new;
}

/**
 * \brief
 */
void SCE_List_Prependl (SCE_SList *l, SCE_SListIterator *it)
{
    if (!l->first)
    {
        l->first = it;
        it->next = it->prev = NULL;
    }
    else
    {
        SCE_List_Prepend (l->first, it);
        l->first = it;
    }
}
/**
 * \brief
 */
void SCE_List_Appendl (SCE_SList *l, SCE_SListIterator *it)
{
    if (!l->first)
    {
        l->first = it;
        it->next = it->prev = NULL;
    }
    else
        SCE_List_Append (SCE_List_GetLast (l), it);
}

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
    if (!new)
    {
        Logger_LogSrc ();
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
    if (!new)
    {
        Logger_LogSrc ();
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
    if (!it)
    {
        Logger_LogSrc ();
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
    if (!it)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    it->data = d;
    SCE_List_Appendl (l, it);
    return SCE_OK;
}

/**
 * \brief Removes an element of a list
 * \param l the SCE_SList from where detach the iterator (can be NULL)
 * \param it the iterator to detach
 * 
 * This function removes the element represented by the iterator from the given
 * list and returns its data. The iterator \p it isn't deleted.
 * If you just want to remove an element and its data, use SCE_List_Erase().
 * \sa SCE_List_Erase(), SCE_List_RemoveFirst(), SCE_List_RemoveLast()
 */
void SCE_List_Remove (SCE_SList *l, SCE_SListIterator *it)
{    
    if (it->prev)
        it->prev->next = it->next;
    else if (l)                 /* l can be NULL */
        l->first = it->next;

    if (it->next)
        it->next->prev = it->prev;

    it->next = NULL;
    it->prev = NULL;
}
/**
 * \brief Removes the first element of a list
 * \param l the SCE_SList from where detach data
 * \returns the removed iterator
 * 
 * This function calls SCE_List_Remove (\p l, SCE_List_GetFirst (\p l)).
 * \sa SCE_List_Remove(), SCE_List_RemoveLast(), SCE_List_EraseFirst()
 */
SCE_SListIterator* SCE_List_RemoveFirst (SCE_SList *l)
{
    SCE_SListIterator *it = l->first;
    SCE_List_Remove (l, it);
    return it;
}
/**
 * \brief Removes the last element of a list
 * \param l the SCE_SList from where detach data
 * \returns the removed iterator
 * 
 * This function calls SCE_List_Remove (\p l, SCE_List_GetLast (\p l)).
 * \sa SCE_List_Remove(), SCE_List_RemoveFirst(), SCE_List_EraseLast()
 */
SCE_SListIterator* SCE_List_RemoveLast (SCE_SList *l)
{
    SCE_SListIterator *it = SCE_List_GetLast (l);
    SCE_List_Remove (l, it);
    return it;
}

/**
 * \brief Fully remove an element of a list
 * \param l the SCE_SList from where remove element
 * \param it the iterator of the element to be removed
 * \sa SCE_List_Remove(), SCE_List_EraseFirst(), SCE_List_EraseLast()
 */
void SCE_List_Erase (SCE_SList *l, SCE_SListIterator *it)
{
    SCE_List_Remove (l, it);
    if (l->f)
        l->f (it->data);
    if (l->canfree)
        SCE_List_DeleteIt (it);
}
/**
 * \brief Fully remove the first element of a list
 * \param l the SCE_SList from where detach data
 * \sa SCE_List_Erase(), SCE_List_EraseLast(), SCE_List_RemoveFirst()
 */
void SCE_List_EraseFirst (SCE_SList *l)
{
    SCE_SListIterator *it = SCE_List_RemoveFirst (l);
    if (l->f)
        l->f (it->data);
    if (l->canfree)
        SCE_List_DeleteIt (it);
}
/**
 * \brief Fully remove the last element of a list
 * \param l the SCE_SList from where detach data
 * \sa SCE_List_Erase(), SCE_List_EraseFirst(), SCE_List_RemoveLast()
 */
void SCE_List_EraseLast (SCE_SList *l)
{
    SCE_SListIterator *it = SCE_List_RemoveLast (l);
    if (l->f)
        l->f (it->data);
    if (l->canfree)
        SCE_List_DeleteIt (it);
}

/**
 * \brief Removes an element of a list
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
        SCE_List_Remove (l, it);
}

/**
 * \brief Fully remove an element of a list
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
unsigned int SCE_List_GetSize (SCE_SList *l)
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
SCE_SListIterator* SCE_List_GetFirst (SCE_SList *l)
{
    return l->first;
}
/**
 * \brief Gets the last iterator of a list
 * \param l the list from where get the iterator
 * \returns the last iterator of the list, if any
 */
SCE_SListIterator* SCE_List_GetLast (SCE_SList *l)
{
    SCE_SListIterator *it = l->first;
    if (it)
    {
        while (it->next)
            it = it->next;
    }
    return it;
}

/**
 * \brief Gets the next iterator of an interator
 * \param it the base iterator
 * \returns the next iterator, if any
 */
SCE_SListIterator* SCE_List_GetNext (SCE_SListIterator *it)
{
    return it->next;
}
/**
 * \brief Gets the previous iterator of an interator
 * \param it the base iterator
 * \returns the previous iterator, if any
 */
SCE_SListIterator* SCE_List_GetPrev (SCE_SListIterator *it)
{
    return it->prev;
}

/**
 * \brief Gets the index position of an iterator
 * \param it the iterator from which you would know the index
 * \returns the index position of the iterator
 */
unsigned int SCE_List_GetIndex (SCE_SListIterator *it)
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
SCE_SListIterator* SCE_List_GetIterator (SCE_SList *l, unsigned int n)
{
    SCE_SListIterator *it = NULL;
    unsigned int i = 0;
    
    SCE_List_ForEach (it, l)
    {
        if (i == n)
            return it;
        i++;
    }
    Logger_Log (/*SCE_STACK_OVERFLOW*/SCE_INVALID_ARG);
    return NULL;
}

/**
 * \brief Gets an iterator of a list by its data
 * \param l an SCE_SList list
 * \param data the data to compare to each list's data
 * \param f a function to compares \p data to each list's data, or NULL for a
 * pointer comparison
 * \returns the founded iterator, if any
 * 
 * This function searches for an iterator containing \p data.
 * The comparison is done by calling \p f and/or comparing the pointers.
 */
SCE_SListIterator* SCE_List_LocateIterator (SCE_SList *l, void *data,
                                            SCE_FListCompareData f)
{
    SCE_SListIterator *i = NULL;
    SCE_List_ForEach (i, l)
    {
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
unsigned int SCE_List_LocateIndex (SCE_SList *l, void *data,
                                   SCE_FListCompareData f)
{
    unsigned int j = 0;
    SCE_SListIterator *i = NULL;
    SCE_List_ForEach (i, l)
    {
        if ((f && f (data, i->data)) || data == i->data)
            return j;
        j++;
    }
    return 0;
}


#if 0
int SCE_List_Switch (SCE_SListIterator *it1, SCE_SListIterator *it2)
{
    SCE_SListIterator tmp;
    tmp = *it1;
    it1->prev = it2->prev;
    it1->next = it2->next;
    it2->prev = tmp.prev;
    it2->next = tmp.next;

    if (it1->prev)
        it1->prev->next = it1;
    if (it1->next)
        it1->next->prev = it1;

    if (it2->prev)
        it2->prev->next = it2;
    if (it2->next)
        it2->next->prev = it2;



    SCE_SListIterator *whereadd = NULL;
    void (*func1)(SCE_SListIterator*, SCE_SListIterator*);
    void (*func2)(SCE_SListIterator*, SCE_SListIterator*);

    if (it1->next)
    {
        whereadd = it1->next;
        func1 = SCE_List_Prepend;
    }
    else if (it1->prev)
    {
        whereadd = it1->prev;
        func1 = SCE_List_Append;
    }
    else
        return SCE_ERROR;

    if (it2->next)
        func2 = SCE_List_Append;
    else if (it2->prev)
        func2 = SCE_List_Prepend;
    else
        return SCE_ERROR;

    SCE_List_Remove (NULL, it1);
    func2 (it2, it1);
    if (whereadd != it2)
    {
        SCE_List_Remove (NULL, it2);
        func1 (whereadd, it2);
    }
}

void SCE_List_Switchl (SCE_SList *l1, SCE_SListIterator *it1,
                       SCE_SList *l2, SCE_SListIterator *it2)
{
    SCE_SListIterator tmp1, tmp2;
    int init1 = SCE_FALSE, init2 = SCE_FALSE;

    if (!it1->prev)
    {
        SCE_List_InitIt (&tmp1);
        it1->prev = &tmp1;
        init1 = SCE_TRUE;
    }
    if (!it2->prev)
    {
        SCE_List_InitIt (&tmp2);
        it2->prev = &tmp2;
        init2 = SCE_TRUE;
    }

    SCE_List_Switch (it1, it2); /* can't fail */

    if (init1)
    {
        l1->first = tmp1.next;
        tmp1.next->prev = NULL;
    }
    if (init2)
    {
        l2->first = tmp2.next;
        tmp2.next->prev = NULL;
    }
}

void SCE_List_SortNext (SCE_SListIterator *it, SCE_FListCompareData change)
{
    SCE_SListIterator *it2, *pro, *pro2;
    SCE_List_ForEachNextProtected (pro, it)
    {
        it2 = it;
        SCE_List_ForEachPrevProtected (pro2, it2)
        {
            if (change (it2, it))
                SCE_List_Switch (it, it2);
        }
    }
}

void SCE_List_SortPrev (SCE_SListIterator *it, SCE_FListCompareData change)
{
    SCE_SListIterator *it2, *pro, *pro2;
    SCE_List_ForEachPrevProtected (pro, it)
    {
        it2 = it;
        SCE_List_ForEachNextProtected (pro2, it2)
        {
            if (change (it, it2))
                SCE_List_Switch (it2, it);
        }
    }
}

void SCE_List_Sortl (SCE_SList *l, SCE_FListCompareData change)
{
    SCE_List_SortNext (l->first, change);
    /* find the new 'first' */
    while (l->first->prev)
        l->first = l->first->prev;
}
#endif

/** @} */
