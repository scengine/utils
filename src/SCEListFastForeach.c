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

/* created: 26/01/2009
   updated: 10/04/2010 */

#include <stdlib.h>

#include "SCE/utils/SCEMacros.h"
#include "SCE/utils/SCEListFastForeach.h"

#define SCE_NUM_SIZES 9

static unsigned int sizes[SCE_NUM_SIZES] = {10000, 1000, 500, 200, 100, 50, 20, 10, 1};

#define SCE_List_FastFunc(n)\
static SCE_SListIterator* SCE_List_Fast_##n (SCE_SListIterator *it,\
                                             SCE_FListFastForeach f, void *p)\
{\
    unsigned int i;\
    for (i = 0; i < n; i++)\
    {\
        f (it, p);\
        it = it->next;\
    }\
    return it;\
}
#define SCE_List_FastFunc2(n)\
static SCE_SListIterator* SCE_List_Fast2_##n (SCE_SListIterator *it,\
                                              SCE_FListFastForeach2 f, void *p)\
{\
    unsigned int i;\
    for (i = 0; i < n; i++)\
    {\
        f (it->data, p);\
        it = it->next;\
    }\
    return it;\
}
#define SCE_List_FastFunc3(n)\
static SCE_SListIterator* SCE_List_Fast3_##n (SCE_SListIterator *it,\
                                              SCE_FListFastForeach3 f)\
{\
    unsigned int i;\
    for (i = 0; i < n; i++)\
    {\
        f (it);\
        it = it->next;\
    }\
    return it;\
}
#define SCE_List_FastFunc4(n)\
static SCE_SListIterator* SCE_List_Fast4_##n (SCE_SListIterator *it,\
                                              SCE_FListFastForeach4 f)\
{\
    unsigned int i;\
    for (i = 0; i < n; i++)\
    {\
        f (it->data);\
        it = it->next;\
    }\
    return it;\
}

SCE_List_FastFunc (10)
SCE_List_FastFunc (20)
SCE_List_FastFunc (50)
SCE_List_FastFunc (100)
SCE_List_FastFunc (200)
SCE_List_FastFunc (500)
SCE_List_FastFunc (1000)
SCE_List_FastFunc (10000)

SCE_List_FastFunc2 (10)
SCE_List_FastFunc2 (20)
SCE_List_FastFunc2 (50)
SCE_List_FastFunc2 (100)
SCE_List_FastFunc2 (200)
SCE_List_FastFunc2 (500)
SCE_List_FastFunc2 (1000)
SCE_List_FastFunc2 (10000)

SCE_List_FastFunc3 (10)
SCE_List_FastFunc3 (20)
SCE_List_FastFunc3 (50)
SCE_List_FastFunc3 (100)
SCE_List_FastFunc3 (200)
SCE_List_FastFunc3 (500)
SCE_List_FastFunc3 (1000)
SCE_List_FastFunc3 (10000)

SCE_List_FastFunc4 (10)
SCE_List_FastFunc4 (20)
SCE_List_FastFunc4 (50)
SCE_List_FastFunc4 (100)
SCE_List_FastFunc4 (200)
SCE_List_FastFunc4 (500)
SCE_List_FastFunc4 (1000)
SCE_List_FastFunc4 (10000)

typedef SCE_SListIterator* (*SCE_fastfunc)
(SCE_SListIterator*, SCE_FListFastForeach, void*);
typedef SCE_SListIterator* (*SCE_fastfunc2)
(SCE_SListIterator*, SCE_FListFastForeach2, void*);
typedef SCE_SListIterator* (*SCE_fastfunc3)
(SCE_SListIterator*, SCE_FListFastForeach3);
typedef SCE_SListIterator* (*SCE_fastfunc4)
(SCE_SListIterator*, SCE_FListFastForeach4);

static SCE_fastfunc fastfuncs[SCE_NUM_SIZES];
static SCE_fastfunc2 fastfuncs2[SCE_NUM_SIZES];
static SCE_fastfunc3 fastfuncs3[SCE_NUM_SIZES];
static SCE_fastfunc4 fastfuncs4[SCE_NUM_SIZES];

static SCE_SListIterator* SCE_List_Fast_1 (SCE_SListIterator *it,
                                           SCE_FListFastForeach f, void *p)
{
    f (it, p);
    return it->next;
}
static SCE_SListIterator* SCE_List_Fast2_1 (SCE_SListIterator *it,
                                            SCE_FListFastForeach2 f, void *p)
{
    f (it->data, p);
    return it->next;
}
static SCE_SListIterator* SCE_List_Fast3_1 (SCE_SListIterator *it,
                                            SCE_FListFastForeach3 f)
{
    f (it);
    return it->next;
}
static SCE_SListIterator* SCE_List_Fast4_1 (SCE_SListIterator *it,
                                            SCE_FListFastForeach4 f)
{
    f (it->data);
    return it->next;
}

int SCE_Init_FastList (void)
{
    fastfuncs[0] = SCE_List_Fast_10000;
    fastfuncs[1] = SCE_List_Fast_1000;
    fastfuncs[2] = SCE_List_Fast_500;
    fastfuncs[3] = SCE_List_Fast_200;
    fastfuncs[4] = SCE_List_Fast_100;
    fastfuncs[5] = SCE_List_Fast_50;
    fastfuncs[6] = SCE_List_Fast_20;
    fastfuncs[7] = SCE_List_Fast_10;
    fastfuncs[8] = SCE_List_Fast_1;

    fastfuncs2[0] = SCE_List_Fast2_10000;
    fastfuncs2[1] = SCE_List_Fast2_1000;
    fastfuncs2[2] = SCE_List_Fast2_500;
    fastfuncs2[3] = SCE_List_Fast2_200;
    fastfuncs2[4] = SCE_List_Fast2_100;
    fastfuncs2[5] = SCE_List_Fast2_50;
    fastfuncs2[6] = SCE_List_Fast2_20;
    fastfuncs2[7] = SCE_List_Fast2_10;
    fastfuncs2[8] = SCE_List_Fast2_1;

    fastfuncs3[0] = SCE_List_Fast3_10000;
    fastfuncs3[1] = SCE_List_Fast3_1000;
    fastfuncs3[2] = SCE_List_Fast3_500;
    fastfuncs3[3] = SCE_List_Fast3_200;
    fastfuncs3[4] = SCE_List_Fast3_100;
    fastfuncs3[5] = SCE_List_Fast3_50;
    fastfuncs3[6] = SCE_List_Fast3_20;
    fastfuncs3[7] = SCE_List_Fast3_10;
    fastfuncs3[8] = SCE_List_Fast3_1;

    fastfuncs4[0] = SCE_List_Fast4_10000;
    fastfuncs4[1] = SCE_List_Fast4_1000;
    fastfuncs4[2] = SCE_List_Fast4_500;
    fastfuncs4[3] = SCE_List_Fast4_200;
    fastfuncs4[4] = SCE_List_Fast4_100;
    fastfuncs4[5] = SCE_List_Fast4_50;
    fastfuncs4[6] = SCE_List_Fast4_20;
    fastfuncs4[7] = SCE_List_Fast4_10;
    fastfuncs4[8] = SCE_List_Fast4_1;

    return SCE_OK;
}

void SCE_Quit_FastList (void)
{
}

static void SCE_List_MakeNumCalls (unsigned int rest, unsigned int *n)
{
    unsigned int i;
    for (i = 0; i < SCE_NUM_SIZES; i++)
    {
        n[i] = rest / sizes[i];
        rest = rest % sizes[i];
    }
}

void SCE_List_FastForEach (SCE_SList *l, unsigned int size,
                           SCE_FListFastForeach f, void *param)
{
    SCE_SListIterator *it = NULL;
    unsigned int n[SCE_NUM_SIZES] = {0};
    unsigned int i, j;

    SCE_List_MakeNumCalls (size, n);

    it = SCE_List_GetFirst (l);
    for (i = 0; i < SCE_NUM_SIZES; i++)
    {
        for (j = 0; j < n[i]; j++)
            it = fastfuncs[i] (it, f, param);
    }
}
void SCE_List_FastForEach2 (SCE_SList *l, unsigned int size,
                            SCE_FListFastForeach2 f, void *param)
{
    SCE_SListIterator *it = NULL;
    unsigned int n[SCE_NUM_SIZES] = {0};
    unsigned int i, j;

    SCE_List_MakeNumCalls (size, n);

    it = SCE_List_GetFirst (l);
    for (i = 0; i < SCE_NUM_SIZES; i++)
    {
        for (j = 0; j < n[i]; j++)
            it = fastfuncs2[i] (it, f, param);
    }
}
void SCE_List_FastForEach3 (SCE_SList *l, unsigned int size,
                            SCE_FListFastForeach3 f)
{
    SCE_SListIterator *it = NULL;
    unsigned int n[SCE_NUM_SIZES] = {0};
    unsigned int i, j;

    SCE_List_MakeNumCalls (size, n);

    it = SCE_List_GetFirst (l);
    for (i = 0; i < SCE_NUM_SIZES; i++)
    {
        for (j = 0; j < n[i]; j++)
            it = fastfuncs3[i] (it, f);
    }
}
void SCE_List_FastForEach4 (SCE_SList *l, unsigned int size,
                            SCE_FListFastForeach4 f)
{
    SCE_SListIterator *it = NULL;
    unsigned int n[SCE_NUM_SIZES] = {0};
    unsigned int i, j;

    SCE_List_MakeNumCalls (size, n);

    it = SCE_List_GetFirst (l);
    for (i = 0; i < SCE_NUM_SIZES; i++)
    {
        for (j = 0; j < n[i]; j++)
            it = fastfuncs4[i] (it, f);
    }
}
