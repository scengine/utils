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
   updated: 15/11/2008 */

#ifndef SCELIST_H
#define SCELIST_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sce_slist SCE_SList;
typedef struct sce_slistiterator SCE_SListIterator;

struct sce_slistiterator
{
    SCE_SListIterator *next, *prev;
    void *data;
};

#define SCE_LIST_ITERATOR_NO_MALLOC 1

typedef void (*SCE_FListFreeFunc)(void*);
typedef int (*SCE_FListCompareData)(void*, void*);

struct sce_slist
{
    SCE_SListIterator *first;
    SCE_FListFreeFunc f;
    int canfree;                /* can delete iterators */
};


void SCE_List_InitIt (SCE_SListIterator*);

SCE_SListIterator* SCE_List_CreateIt (void);
void SCE_List_DeleteIt (SCE_SListIterator*);

void SCE_List_Init (SCE_SList*);

SCE_SList* SCE_List_Create (SCE_FListFreeFunc);
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

void SCE_List_Remove (SCE_SList*, SCE_SListIterator*);
SCE_SListIterator* SCE_List_RemoveFirst (SCE_SList*);
SCE_SListIterator* SCE_List_RemoveLast (SCE_SList*);

void SCE_List_Erase (SCE_SList*, SCE_SListIterator*);
void SCE_List_EraseFirst (SCE_SList*);
void SCE_List_EraseLast (SCE_SList*);

void* SCE_List_SetData (SCE_SListIterator*, void*);
#if 0
void* SCE_List_GetData (SCE_SListIterator*);
#endif

unsigned int SCE_List_GetSize (SCE_SList*);
SCE_SListIterator* SCE_List_GetFirst (SCE_SList*);
SCE_SListIterator* SCE_List_GetLast (SCE_SList*);

SCE_SListIterator* SCE_List_GetNext (SCE_SListIterator*);
SCE_SListIterator* SCE_List_GetPrev (SCE_SListIterator*);

unsigned int SCE_List_GetIndex (SCE_SListIterator*);
SCE_SListIterator* SCE_List_GetIterator (SCE_SList*, unsigned int);
SCE_SListIterator* SCE_List_LocateIterator (SCE_SList*, void*,
                                            SCE_FListCompareData);
unsigned int SCE_List_LocateIndex (SCE_SList*, void*,
                                   SCE_FListCompareData);

int SCE_List_Switch (SCE_SListIterator*, SCE_SListIterator*);
void SCE_List_Switchl (SCE_SList*, SCE_SListIterator*,
                       SCE_SList*, SCE_SListIterator*);

#define SCE_List_GetData(it) (((SCE_SListIterator*)(it))->data)

#define SCE_List_ForEach(it, l)\
    for ((it) = (l)->first; (it); (it) = (it)->next)
#define SCE_List_ForEachProtected(pro, it, l)\
    for ((it) = (l)->first, (pro) = ((it) ? (it)->next : NULL);\
         (it);\
         (it) = (pro), (pro) = ((pro) ? (pro)->next : NULL))

#define SCE_List_ForEachNext(it)\
    for (; (it); (it) = (it)->next)
#define SCE_List_ForEachNextProtected(pro, it)\
    for ((pro) = (it)->next;\
         (it);\
         (it) = (pro), (pro) = ((pro) ? (pro)->next : NULL))

#define SCE_List_ForEachPrev(it)\
    for (; (it); (it) = (it)->prev)
#define SCE_List_ForEachPrevProtected(pro, it)\
    for ((pro) = (it)->prev;\
         (it);\
         (it) = (pro), (pro) = ((pro) ? (pro)->prev : NULL))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
