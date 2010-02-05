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
   updated: 20/01/2010 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <SCE/utils/SCETime.h>

#include <SCE/utils/SCEError.h>

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
 * \brief Backtracer depth
 */
#define SCE_MAX_ERRORS 16

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
struct sce_serror
{
    time_t date;       /**< Date when error has occured */
    int code;          /**< Code of the error */
    unsigned int line; /**< Line of the file where the error occured */
    char func[SCE_MAX_ERROR_INFO_LEN];/**< Function where the error occured */
    char file[SCE_MAX_ERROR_INFO_LEN];/**< Filename where the error occred */
    char msg[SCE_MAX_ERROR_MSG_LEN];  /**< Error message */
};

/**
 * \internal
 * \brief The current error
 *
 * Contains all the data of the current error.
 */
static SCE_SError errors[SCE_MAX_ERRORS];

static unsigned int current_error = 0;
static SCE_SError *error = &errors[0];

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

/**
 * \brief Initializes the error manager
 * \param outlog File used for logging
 * \returns 0 if no error occured, else a negative integer
 *
 * This function initializes \c error, with null data and set the stream used for logging.
 */
int SCE_Init_Error (FILE *outlog)
{
    stream = (outlog ? outlog : stderr);
    SCE_Error_Clear ();
    return 0;
}

/**
 * \brief Clear error manager
 *
 * This function just erases current error, if any.
 */
void SCE_Error_Clear (void)
{
    unsigned int i;
    for (i = 0; i < SCE_MAX_ERRORS; i++)
        SCE_Error_Init (&errors[i]);
    current_error = 0;
    error = &errors[0];
}

/**
 * \internal
 * \brief Increments current error in the backtracer list
 */
static void SCE_Error_Incr (void)
{
    current_error++;
    error = &errors[current_error];
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
    error->date = time (NULL);
    error->line = line;
    error->code = code;
    strncpy (error->file, file, SCE_MAX_ERROR_INFO_LEN);
    if (func)
        strncpy (error->func, func, SCE_MAX_ERROR_INFO_LEN);
    SCE_Error_GetCodeMsg (errors[0].code, error->msg, SCE_MAX_ERROR_MSG_LEN);
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
    SCE_Error_Incr ();
    error->line = line;
    strncpy (error->file, file, SCE_MAX_ERROR_INFO_LEN);
    if (func)
        strncpy (error->func, func, SCE_MAX_ERROR_INFO_LEN);
}

void SCE_Error_LogSrcMsg (const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    vsnprintf (error->msg, SCE_MAX_ERROR_MSG_LEN, fmt, args);
    va_end (args);
}


/**
 * \brief Gets the error code
 */
int SCE_Error_GetCode (void)
{
    return error[0].code;
}

/**
 * \brief Gets the string associated with an error code
 */
void SCE_Error_GetCodeMsg (int code, char *str, size_t size)
{
    /* according to SCE_EError */
    static const char messages[SCE_NUM_ERRORS][18] =
    {
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
    return (errors[0].code != SCE_NO_ERROR);
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

    time_info = gmtime (&errors[0].date);
    SCE_Time_MakeString (date, time_info);

    fprintf (stream, "\n[log %s]\n", date);
    for (i = current_error; i >= 0; i--)
    {
        fprintf (stream, "%s%s:%s (%d): %s%c\n",
                 (i == current_error ? "error: " : " from: "),
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
    int i = 0;

    fprintf (stream, "error:\n");
    for (i = current_error; i >= 0; i--)
    {
        /* only if there is a message available */
        if (errors[i].msg[0])
            fprintf (stream, "  %s%c\n", errors[i].msg, (i == 0 ? '.' : ':'));
    }
}

/** @} */
