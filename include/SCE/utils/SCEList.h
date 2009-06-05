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
   updated: 07/03/2009 */

#ifndef SCELIST_H
#define SCELIST_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup memory
 * @{
 */

/** \copydoc sce_slist */
typedef struct sce_slist SCE_SList;
typedef struct sce_slistiterator SCE_SListIterator;

struct sce_slistiterator
{
    SCE_SListIterator *next, *prev;
    void *data;
};

#define SCE_LIST_ITERATOR_NO_MALLOC 1
#define SCE_LIST_ABUSIVE_MACRO 0

typedef void (*SCE_FListFreeFunc)(void*);
typedef int (*SCE_FListCompareData)(void*, void*);

/**
 * \brief A list structure
 */
struct sce_slist
{
    SCE_SListIterator first;  /**< First iterator of the list */
    SCE_SListIterator last;   /**< Last iterator of the list */
    SCE_FListFreeFunc f;      /**< Free function */
    int canfree;              /**< Does the list can delete iterators? */
};

/** @} */

void SCE_List_InitIt (SCE_SListIterator*);

SCE_SListIterator* SCE_List_CreateIt (void);
void SCE_List_DeleteIt (SCE_SListIterator*);

void SCE_List_Init (SCE_SList*);

SCE_SList* SCE_List_Create (SCE_FListFreeFunc);
void SCE_List_Flush (SCE_SList*);
void SCE_List_Clear (SCE_SList*);
void SCE_List_Delete (SCE_SList*);

void SCE_List_CanDeleteIterators (SCE_SList*, int);

void SCE_List_Prepend (SCE_SListIterator*, SCE_SListIterator*);
void SCE_List_Append (SCE_SListIterator*, SCE_SListIterator*);

void SCE_List_Prependl (SCE_SList*, SCE_SListIterator*);
void SCE_List_Appendl (SCE_SList*, SCE_SListIterator*);
int SCE_List_PrependNew (SCE_SListIterator*, void*);
int SCE_List_AppendNew (SCE_SListIterator*, void*);
int SCE_List_PrependNewl (SCE_SList*, void*);
int SCE_List_AppendNewl (SCE_SList*, void*);

void SCE_List_Removel (SCE_SListIterator*);
SCE_SListIterator* SCE_List_RemoveFirst (SCE_SList*);
SCE_SListIterator* SCE_List_RemoveLast (SCE_SList*);

void SCE_List_Erase (SCE_SList*, SCE_SListIterator*);
void SCE_List_EraseFirst (SCE_SList*);
void SCE_List_EraseLast (SCE_SList*);

void SCE_List_RemoveFromData (SCE_SList*, void*);
void SCE_List_EraseFromData (SCE_SList*, void*);

void SCE_List_Join (SCE_SList*, SCE_SList*);
void SCE_List_Insert (SCE_SList*, SCE_SList*);
void SCE_List_BreakStart (SCE_SList*);
void SCE_List_BreakEnd (SCE_SList*);
void SCE_List_Break (SCE_SList*);
void SCE_List_BreakAll (SCE_SList*);
void SCE_List_Extract (SCE_SList*);

void* SCE_List_SetData (SCE_SListIterator*, void*);
#if 0
void* SCE_List_GetData (SCE_SListIterator*);
#endif

unsigned int SCE_List_GetSize (SCE_SList*);
SCE_SListIterator* SCE_List_GetFirst (SCE_SList*);
SCE_SListIterator* SCE_List_GetLast (SCE_SList*);
int SCE_List_HasElements (SCE_SList*);

SCE_SListIterator* SCE_List_GetNext (SCE_SListIterator*);
SCE_SListIterator* SCE_List_GetPrev (SCE_SListIterator*);

unsigned int SCE_List_GetIndex (SCE_SListIterator*);
SCE_SListIterator* SCE_List_GetIterator (SCE_SList*, unsigned int);
SCE_SListIterator* SCE_List_LocateIterator (SCE_SList*, void*,
                                            SCE_FListCompareData);
unsigned int SCE_List_LocateIndex (SCE_SList*, void*,
                                   SCE_FListCompareData);

/**
 * \brief Gets data of an iterator
 * \param it the SCE_SListIterator from where get data
 * \returns the data, if any
 */
#define SCE_List_GetData(it) (((SCE_SListIterator*)(it))->data)


#if SCE_LIST_ABUSIVE_MACRO
#define SCE_List_Prependl(l, it)\
do {\
    ((SCE_SListIterator*)(it))->prev = ((SCE_SList*)(l))->first.next->prev;\
    ((SCE_SListIterator*)(it))->next = ((SCE_SList*)(l))->first.next;   \
    ((SCE_SList*)(l))->first.next->prev = ((SCE_SListIterator*)(it));   \
    ((SCE_SList*)(l))->first.next = ((SCE_SListIterator*)(it));         \
} while (0)

#define SCE_List_Appendl(l, it)\
do {\
    ((SCE_SListIterator*)(it))->next = ((SCE_SList*)(l))->last.prev->next;\
    ((SCE_SListIterator*)(it))->prev = ((SCE_SList*)(l))->last.prev;    \
    ((SCE_SList*)(l))->last.prev->next = ((SCE_SListIterator*)(it));    \
    ((SCE_SList*)(l))->last.prev = ((SCE_SListIterator*)(it));          \
} while (0)

#define SCE_List_Removel(it)\
do {\
    ((SCE_SListIterator*)(it))->next->prev = ((SCE_SListIterator*)(it))->prev; \
    ((SCE_SListIterator*)(it))->prev->next = ((SCE_SListIterator*)(it))->next; \
    /*((SCE_SListIterator*)(it))->next = NULL;                          \
      ((SCE_SListIterator*)(it))->prev = NULL;                            \*/\
} while (0)
#endif


#define SCE_List_ForEach(it, l)\
    for ((it) = (l)->first.next; (it)->next; (it) = (it)->next)
#define SCE_List_ForEachProtected(pro, it, l)\
    for ((it) = (l)->first.next, (pro) = (it)->next;\
         (pro);\
         (it) = (pro), (pro) = (pro)->next)
#if 0
#define SCE_List_ForEachNextList(l)\
    for (; (l)->last.next; (l) = (l)->last.next->data)
#define SCE_List_ForEachPrevList(l)\
    for (; (l)->first.prev; (l) = (l)->first.prev->data)
#else
#define SCE_List_ForEachNextList(l)\
    for (; (l); (l) = ((l)->last.next ? (l)->last.next->data : NULL))
#define SCE_List_ForEachPrevList(l)\
    for (; (l); (l) = ((l)->first.prev ? (l)->first.prev->data : NULL))
#endif

#if 0
#define SCE_List_ForEachNextListProtected(pro, l)\
    for ((pro) = (((l) && (l)->last.next) ? (l)->last.next->data : (l)); \
         (l);\
         (l) = (pro), (pro) = ((pro) && (pro)->last.next ?\
                                (pro)->last.next->data : NULL))
#define SCE_List_ForEachPrevListProtected(pro, l)\
    for ((pro) = (((l) && (l)->first.prev) ? (l)->first.prev->data : (l));\
         (l);\
         (l) = (pro), (pro) = (((pro) && (pro)->first.prev) ?\
                                (pro)->first.prev->data : NULL))
#else
#define SCE_List_ForEachNextListProtected(pro, l)\
    for ((pro) = ((l)->last.next ? (l)->last.next->data : NULL); \
         (l);\
         (l) = (pro), (pro) = ((pro) && (pro)->last.next ?\
                                (pro)->last.next->data : NULL))
#define SCE_List_ForEachPrevListProtected(pro, l)\
    for ((pro) = ((l)->first.prev ? (l)->first.prev->data : NULL);    \
         (l);\
         (l) = (pro), (pro) = (((pro) && (pro)->first.prev) ?\
                                (pro)->first.prev->data : NULL))
#endif

/** \deprecated */
#define SCE_List_ForEachNext(it)\
    for (; (it); (it) = (it)->next)
/** \deprecated */
#define SCE_List_ForEachNextProtected(pro, it)\
    for ((pro) = (it)->next;\
         (it);\
         (it) = (pro), (pro) = ((pro) ? (pro)->next : NULL))

/** \deprecated */
#define SCE_List_ForEachPrev(it)\
    for (; (it); (it) = (it)->prev)
/** \deprecated */
#define SCE_List_ForEachPrevProtected(pro, it)\
    for ((pro) = (it)->prev;\
         (it);\
         (it) = (pro), (pro) = ((pro) ? (pro)->prev : NULL))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
