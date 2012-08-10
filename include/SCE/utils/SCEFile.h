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

/* created: 09/08/2012
   updated: 09/08/2012 */

#ifndef SCEFILE_H
#define SCEFILE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sce_sfile SCE_SFile;
typedef struct sce_sfilesystem SCE_SFileSystem;

typedef void* (*SCE_FOpenFunc)(const char*, const char*);
typedef int (*SCE_FCloseFunc)(void*);
typedef size_t (*SCE_FReadFunc)(void*, size_t, size_t, void*);
typedef size_t (*SCE_FWriteFunc)(void*, size_t, size_t, void*);
typedef int (*SCE_FSeekFunc)(void*, long, int);
typedef long (*SCE_FTellFunc)(void*);
typedef void (*SCE_FRewindFunc)(void*);

struct sce_sfilesystem {
    void *udata;
    SCE_FOpenFunc xopen;
    SCE_FCloseFunc xclose;
    SCE_FReadFunc xread;
    SCE_FWriteFunc xwrite;
    SCE_FSeekFunc xseek;
    SCE_FTellFunc xtell;
    SCE_FRewindFunc xrewind;
};

struct sce_sfile {
    void *file;
    SCE_SFileSystem *fs;
};

extern SCE_SFileSystem sce_cfs;

int SCE_Init_File (void);
void SCE_Quit_File (void);

void SCE_File_Init (SCE_SFile*);

int SCE_File_Open (SCE_SFile*, SCE_SFileSystem*, const char*, const char*);
int SCE_File_Close (SCE_SFile*);

size_t SCE_File_Read (void*, size_t, size_t, SCE_SFile*);
size_t SCE_File_Write (void*, size_t, size_t, SCE_SFile*);

int SCE_File_Seek (SCE_SFile*, long, int);
long SCE_File_Tell (SCE_SFile*);
void SCE_File_Rewind (SCE_SFile*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */