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
 
/* created: 16/09/2006
   updated: 28/06/2010 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>

#include "SCE/utils/SCETime.h"
#include "SCE/utils/SCEError.h"

/**
 * \file SCEError.c
 * \copydoc error
 * \file SCEError.h
 * \copydoc error
 */

/**
 * \defgroup error Error manager
 * \ingroup utils
 * \internal
 * \brief Handling and reporting errors
 *
 * This modules provides functions to deal with errors.
 * It makes possible to get a backtrace to the currently error.
 * It is also used for logging informations to the user.
 * From the function where occured the error, to the user, each function
 * call has to be added to the backtrace.
 */

/** @{ */

/**
 * \brief Number of threads that can raise an error simultaneously
 */
#define SCE_MAX_ERROR_THREADS 4

/**
 * \brief Backtracer depth
 */
#define SCE_BACKTRACE_DEPTH 24

/**
 * \brief Maximum size for error messages
 */
#define SCE_MAX_ERROR_MSG_LEN 8192

/**
 * \brief Maximum size for information strings
 */
#define SCE_MAX_ERROR_INFO_LEN 512

/**
 * \copydoc sce_serror
 */
typedef struct sce_serror SCE_SError;
/**
 * \brief A SCE Error
 */
struct sce_serror {
    time_t date;       /**< Date when error has occured */
    int code;          /**< Code of the error */
    unsigned int line; /**< Line of the file where the error occured */
    char func[SCE_MAX_ERROR_INFO_LEN];/**< Function where the error occured */
    char file[SCE_MAX_ERROR_INFO_LEN];/**< Filename where the error occured */
    char msg[SCE_MAX_ERROR_MSG_LEN];  /**< Error message */
};

/**
 * \copydoc sce_serror
 */
typedef struct sce_serrorlog SCE_SErrorLog;
/**
 * \brief A complete log of an error
 */
struct sce_serrorlog {
    pthread_t owner;            /**< Thread that raised this error */
    unsigned int current;
    SCE_SError errors[SCE_BACKTRACE_DEPTH];
};

/**
 * \internal
 * \brief The current error
 *
 * Contains all the data of the current error.
 */
static SCE_SErrorLog scelogs[SCE_MAX_ERROR_THREADS];
/**
 * \internal
 * \brief Mutex that protects \c scelogs
 */
static pthread_mutex_t logsmutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * \internal
 * \brief Stream used for loggin
 */
static FILE *stream = NULL;

static void SCE_Error_Init (SCE_SError *err)
{
    err->date = 0;
    err->code = 0;
    err->line = 0;
    memset (err->msg, '\0', SCE_MAX_ERROR_MSG_LEN);
    memset (err->file, '\0', SCE_MAX_ERROR_INFO_LEN);
    memset (err->func, '\0', SCE_MAX_ERROR_INFO_LEN);
}

static void SCE_Error_InitLog (SCE_SErrorLog *l)
{
    size_t i;
    l->owner;                   /* wat do */
    l->current = 0;
    for (i = 0; i < SCE_BACKTRACE_DEPTH; i++)
        SCE_Error_Init (&l->errors[i]);
}

static SCE_SErrorLog* SCE_Error_GetLog (void)
{
    SCE_SErrorLog *l = NULL;
    size_t i;
    pthread_t th;
    th = pthread_self ();
    if (!pthread_mutex_lock (&logsmutex)) {
        for (i = 0; i < SCE_MAX_ERROR_THREADS; i++) {
            if (scelogs[i].current > 0 && pthread_equal (th, scelogs[i].owner)) {
                l = &scelogs[i];
                break;
            }
        }
        /* search for the first available log */
        for (i = 0; i < SCE_MAX_ERROR_THREADS; i++) {
            if (scelogs[i].current == 0) {
                scelogs[i].owner = th;
                l = &scelogs[i];
                break;
            }
        }
        pthread_mutex_unlock (&logsmutex);
    }
    return l;                   /* NOTE: may return NULL */
}

/**
 * \brief Initializes the error manager
 * \param outlog File used for logging
 * \returns 0 if no error occured, a negative integer otherwise
 *
 * This function initializes \c error, with null data and set the stream used for logging.
 */
int SCE_Init_Error (FILE *outlog)
{
    size_t i;
    stream = (outlog ? outlog : stderr);
    for (i = 0; i < SCE_MAX_ERROR_THREADS; i++)
        SCE_Error_InitLog (&scelogs[i]);
    return 0;                   /* NOTE: return SCE_OK ? */
}

/**
 * \brief Clear error manager
 *
 * This function just erases current error, if any.
 */
void SCE_Error_Clear (void)
{
    SCE_SErrorLog *l = SCE_Error_GetLog ();
    SCE_Error_InitLog (l);
}


/**
 * \brief Set error data into \c error
 * \param file File where the error occured
 * \param func Function where the error occured
 * \param line Line where the error occured
 * \param code Error code
 */
void SCE_Error_Log (const char *file, const char *func, unsigned int line,
                    int code)
{
    SCE_SError *error = NULL;
    SCE_SErrorLog *l = SCE_Error_GetLog ();
    error = &l->errors[l->current];
    error->date = time (NULL);
    error->line = line;
    error->code = code;
    strncpy (error->file, file, SCE_MAX_ERROR_INFO_LEN);
    if (func)
        strncpy (error->func, func, SCE_MAX_ERROR_INFO_LEN);
    SCE_Error_GetCodeMsg (error->code, error->msg, SCE_MAX_ERROR_MSG_LEN);
}


/**
 * \brief Logs a message
 * \param fmt format printf-like
 *
 * This function logs a message formatted by sprintf.
 * \note The difference between this function and SCEE_SendMsg is that this
 * function register the log into error and SCEE_SendMsg doesn't.
 */
void SCE_Error_LogMsg (const char *fmt, ...)
{
    va_list args;
    SCE_SError *error = NULL;
    SCE_SErrorLog *l = SCE_Error_GetLog ();
    error = &l->errors[l->current];
    memset (error->msg, '\0', SCE_MAX_ERROR_MSG_LEN);
    va_start (args, fmt);
    vsnprintf (error->msg, SCE_MAX_ERROR_MSG_LEN, fmt, args);
    va_end (args);
}

/**
 * \brief Logs an error message from the errno value
 * \param prefix prefix for the error message (e.g. name of function that
 *               propagated errno) or NULL
 * \param errnum the errno value
 */
void SCE_Error_LogFromErrno (const char *file, const char *func,
                             unsigned int line, int errnum, const char* prefix)
{
    if (!prefix)
        prefix = "errno";
    SCE_Error_Log (file, func, line, SCE_ERRNO_ERROR);
    SCE_Error_LogMsg ("%s: %s", prefix, strerror (errnum));
}

/**
 * \brief Prints a formatted message to the log stream
 * \param fmt The format printf-like
 * \note This function doesn't register the log into error
 */
void SCE_Error_SendMsg (const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    vfprintf (stream, fmt, args);
    va_end (args);
}

/**
 * \brief Adds an error source in the current error backtrace.
 * \param file file where the error occured
 * \param func func where the error occured
 * \param line line where the error occured
 */
void SCE_Error_LogSrc (const char *file, const char *func, unsigned int line)
{
    SCE_SError *error = NULL;
    SCE_SErrorLog *l = SCE_Error_GetLog ();
    l->current++;
    if (l->current < SCE_BACKTRACE_DEPTH) {
        error = &l->errors[l->current];
        error->line = line;
        strncpy (error->file, file, SCE_MAX_ERROR_INFO_LEN);
        if (func)
            strncpy (error->func, func, SCE_MAX_ERROR_INFO_LEN);
    }
}

void SCE_Error_LogSrcMsg (const char *fmt, ...)
{
    va_list args;
    SCE_SError *error = NULL;
    SCE_SErrorLog *l = SCE_Error_GetLog ();
    error = &l->errors[l->current];
    va_start (args, fmt);
    vsnprintf (error->msg, SCE_MAX_ERROR_MSG_LEN, fmt, args);
    va_end (args);
}


/**
 * \brief Gets the error code
 */
int SCE_Error_GetCode (void)
{
    SCE_SError *error = NULL;
    SCE_SErrorLog *l = SCE_Error_GetLog ();
    error = &l->errors[l->current];
    return error->code;
}

/**
 * \brief Gets the string associated with an error code
 */
void SCE_Error_GetCodeMsg (int code, char *str, size_t size)
{
    /* according to SCE_EError */
    static const char messages[SCE_NUM_ERRORS][18] = {
        "no error",
        "out of memory",
        "invalid operation",
        "invalid pointer",
        "invalid argument",
        "invalid size",
        "invalid enumerant",
        "file not found",
        "bad format"
    };
    if (code >= 0 && code < SCE_NUM_ERRORS)
        strncpy (str, messages[code], size);
}

/**
 * \brief Is any error logged in?
 */
int SCE_Error_HaveError (void)
{
    SCE_SErrorLog *l = SCE_Error_GetLog ();
    return (l->errors[0].code != SCE_NO_ERROR);
}

/**
 * \brief Prints the error to the log stream.
 *
 * Prints the error in the log stream. This function
 * prints the backtrace of the error.
 */
void SCE_Error_Out (void)
{
    const struct tm *time_info = NULL;
    char date[32] = {0};
    int i = 0;
    SCE_SError *errors = NULL;
    SCE_SErrorLog *l = SCE_Error_GetLog ();

    errors = l->errors;
    time_info = gmtime (&errors[0].date);
    SCE_Time_MakeString (date, time_info);

    fprintf (stream, "\n[log %s]\n", date);
    for (i = l->current; i >= 0; i--) {
        fprintf (stream, "%s%s:%s (%d): %s%c\n",
                 (i == l->current ? "error: " : " from: "),
                 errors[i].file,
                 errors[i].func, errors[i].line, errors[i].msg,
                 (i == 0 ? '.' : ':'));
    }
    fprintf (stream, "[end log]\n");
}

/**
 * \brief Soft version of SCE_Error_Out()
 * \sa SCE_Error_Out()
 */
void SCE_Error_SoftOut (void)
{
    size_t i = 0;
    SCE_SError *errors = NULL;
    SCE_SErrorLog *l = SCE_Error_GetLog ();

    errors = l->errors;
    fprintf (stream, "error:\n");
    for (i = l->current; i >= 0; i--) {
        /* only if there is a message available */
        if (errors[i].msg[0])
            fprintf (stream, "  %s%c\n", errors[i].msg, (i == 0 ? '.' : ':'));
    }
}

/** @} */
