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
 
/* created: 16/09/2006
   updated: 12/07/2009 */

#ifndef SCEERROR_H
#define SCEERROR_H

#include <errno.h>
#include <stdio.h>
#include <time.h>

#include <SCE/SCECommon.h>

#ifdef __cplusplus
extern "C"
{
#endif


#define SCEE_Log(c) SCE_Error_Log (__FILE__, SCE_FUNCTION, __LINE__, c)
#define SCEE_LogFromErrno(a,c)\
    SCE_Error_LogFromErrno(__FILE__, SCE_FUNCTION, __LINE__, a, c)
#define SCEE_LogSrc() SCE_Error_LogSrc(__FILE__, SCE_FUNCTION, __LINE__)
#define SCEE_LogMsg SCE_Error_LogMsg
#define SCEE_LogSrcMsg SCE_Error_LogSrcMsg
#define SCEE_SendMsg SCE_Error_SendMsg
#define SCEE_GetCodeMsg(code) SCE_Error_GetCodeMsg(code)
#define SCEE_HaveError() SCE_Error_HaveError()
#define SCEE_Out() SCE_Error_Out()
#define SCEE_SoftOut() SCE_Error_SoftOut()

/**
 * \brief Logs the error of the current errno
 * \param prefix prefix for the error message or NULL
 * \see SCEE_LogFromErrno()
 */
#define SCEE_LogErrno(prefix) (SCEE_LogFromErrno (errno, prefix))

enum sce_enum_error
{
    SCE_NO_ERROR = 0,      /* 0 is 'no error' */
    SCE_OUT_OF_MEMORY,
    SCE_INVALID_OPERATION,
    SCE_INVALID_POINTER,
    SCE_INVALID_ARG,
    SCE_INVALID_SIZE,
    SCE_INVALID_ENUM,
    SCE_FILE_NOT_FOUND,
    SCE_BAD_FORMAT,
    SCE_NUM_ERRORS
};
typedef enum sce_enum_error SCE_EError;

int SCE_Init_Error (FILE*);

void SCE_Error_Clear (void);

void SCE_Error_Log (const char*, const char*, const int, int);
void SCE_Error_LogMsg (const char*, ...) SCE_GNUC_PRINTF (1, 2);
void SCE_Error_LogFromErrno (const char*, const char*, const int, int,
                             const char*);

void SCE_Error_SendMsg (const char*, ...) SCE_GNUC_PRINTF (1, 2);
void SCE_Error_LogSrc (const char*, const char*, const int);
void SCE_Error_LogSrcMsg (const char*, ...) SCE_GNUC_PRINTF (1, 2);

int SCE_Error_GetCode (void);
void SCE_Error_GetCodeMsg (int, char*, size_t);

int SCE_Error_HaveError (void);

void SCE_Error_Out (void);
void SCE_Error_SoftOut (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
