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

/* created: 15/08/2012
   updated: 21/08/2012 */

#ifndef SCEFILECACHE_H
#define SCEFILECACHE_H

#include <pthread.h>
#include "SCE/utils/SCEList.h"
#include "SCE/utils/SCEFile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sce_sfilecache SCE_SFileCache;
struct sce_sfilecache {
    unsigned int max_cached;
    unsigned int n_cached;
    SCE_SList cached;
    pthread_mutex_t cached_mutex;
};

extern SCE_SFileSystem sce_cachefs;

int SCE_Init_FileCache (void);
void SCE_Quit_FileCache (void);

void* SCE_FileCache_GetRaw (SCE_SFile*);

void SCE_FileCache_InitCache (SCE_SFileCache*);
void SCE_FileCache_ClearCache (SCE_SFileCache*);

void SCE_FileCache_SetMaxCachedFiles (SCE_SFileCache*, unsigned int);
unsigned int SCE_FileCache_GetNumCachedFiles (SCE_SFileCache*);

void SCE_FileCache_CacheFile (SCE_SFileCache*, SCE_SFile*);
void SCE_FileCache_UncacheFile (SCE_SFile*);

void SCE_FileCache_Update (SCE_SFileCache*);
int SCE_FileCache_Sync (SCE_SFileCache*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
