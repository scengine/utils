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
   updated: 27/09/2008 */

#ifndef SCEERROR_H
#define SCEERROR_H

#include <errno.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define Logger_Init(out) SCE_Error_Init(out)
#define Logger_Clear() SCE_Error_Clear()
#define Logger_LogCode(c)\
 SCE_Error_SetErrorCode_(__FILE__, __FUNCTION__, __LINE__, c)
#define Logger_Log_(a,b,c) SCE_Error_SetErrorCode_(a,b,c)
/**
 * \brief Add current position to the backtrace
 * \ingroup error
 * \sa SCE_Error_SetSrc
 */
#define Logger_LogSrc() SCE_Error_SetSrc(__FILE__, __FUNCTION__, __LINE__)
#define Logger_GetMsg() SCE_Error_GetErrorMsg()
#define Logger_GetCode() SCE_Error_GetErrorCode()
#define Logger_GetCodeMsg(code) SCE_Error_GetCodeMsg(code)
#define Logger_GetFile() SCE_Error_GetFile()
#define Logger_GetFunc() SCE_Error_GetFunc()
#define Logger_GetLine() SCE_Error_GetLine()
#define Logger_GetTime() SCE_Error_GetTime()
#define Logger_HaveError() SCE_Error_HaveError()
#define Logger_HaveMsg() SCE_Error_HaveMsg()
#define Logger_Out() SCE_Error_OutStream()

#define Logger_LogFinish(msg)\
do {\
    Logger_LogSrc();\
    if(!Logger_HaveMsg())\
    {\
        if(msg)\
            Logger_LogMsg(msg);\
        else\
            Logger_LogMsg(Logger_GetCodeMsg(Logger_GetCode()));\
    }\
} while (0)


#define Logger_Log(code)\
do {\
    if(Logger_HaveError())\
    {\
        fprintf(stderr, "%s (%d): an error is already logged in file '%s' at line %d\n",\
                __FILE__, __LINE__, Logger_GetFile(), Logger_GetLine());\
    }\
    else\
        Logger_LogCode(code);\
} while (0)

/**
 * \brief Logs the error of the current errno
 * \param prefix prefix for the error message or NULL
 * \returns see Logger_LogFromErrno()
 * \see Logger_LogFromErrno()
 */
#define Logger_LogErrno(prefix) (Logger_LogFromErrno (errno, prefix))

/*
#define Logger_Log(code)\
    #ifdef __DEBUG__\
    if(Logger_HaveError())\
    {\
        fprintf(stderr, "%s (%d): have already a error logged\n", __FILE__, __LINE__);\
    }\
    else\
    {\
    #endif\
        if(Logger_HaveMsg())\
        {\
        Logger_Log(code);\
        Logger_LogMsg(Logger_GetCodeMsg());
    #ifdef __DEBUG__\
    }\
    #endif
*/

enum sce_enum_error
{
    SCE_OUT_OF_MEMORY,       
    SCE_INVALID_OPERATION,   
    SCE_INVALID_POINTER,     
    SCE_INVALID_ARG,         
    SCE_INVALID_SIZE,        
    SCE_INVALID_ENUM,
    SCE_FILE_NOT_FOUND,
    SCE_BAD_FORMAT
};

typedef enum sce_enum_error SCE_EError;

#define SCE_NUM_ERRORS 8


int SCE_Error_Init(FILE*);
void SCE_Error_Clear(void);
void SCE_Error_SetErrorCode_(const char*, const char*, const int, int);
int Logger_LogMsg(const char*, ...);
int Logger_LogFromErrno(int, const char*);
void Logger_PrintMsg(const char*, ...);
void SCE_Error_SetSrc(const char*, const char*, const int);
const char* SCE_Error_GetErrorMsg(void);
int SCE_Error_GetErrorCode(void);
const char* SCE_Error_GetCodeMsg(int);
const char* SCE_Error_GetFile(void);
const char* SCE_Error_GetFunc(void);
int SCE_Error_GetLine(void);
time_t SCE_Error_GetTime(void);
int SCE_Error_HaveError(void);
int SCE_Error_HaveMsg(void);
int SCE_Error_OutStream(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
