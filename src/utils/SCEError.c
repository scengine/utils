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
   updated: 26/07/2007 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <SCE/utils/SCETime.h>

#include <SCE/utils/SCEError.h>

/**
 * \defgroup utils Useful functions
 * \ingroup SCEngine
 * \internal
 * \brief Useful functions used in the other parts of SCE
 */

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
 * \brief Maximal size for error messages
 */
#define SCE_MAX_MSG_LEN 4096

/**
 * \brief Maximal size for informations strings
 */
#define SCE_MAX_INFO_LEN 512

/**
 * \brief Coefficient of reallocation for the errors sources
 */
#define SCE_SRC_ALLOC_SIZE 8

/**
 * \copydoc sce_errorsrc
 */
typedef struct sce_errorsrc SCE_ErrorSrc;
/**
 * \brief A Source of SCE Error
 *
 * Used to store data on each function in the error backtrace.
 * \sa SCE_Error_SetSrc
 */
struct sce_errorsrc
{
    unsigned int line;
    char func[SCE_MAX_INFO_LEN];
    char file[SCE_MAX_INFO_LEN];
};

/**
 * \copydoc sce_error
 */
typedef struct sce_error SCE_Error;
/**
 * \brief A SCE Error
 */
struct sce_error
{
    time_t date;  /**< Date when error has occured */
    
    int have_error;    /**< Does we have an error already set ? */
    int have_msg;      /**< Does we have an error message already set ?*/
    int code;          /**< Code of the error */
    
    unsigned int line;          /**< Line of the file where the error occured */
    char func[SCE_MAX_INFO_LEN];/**< Function where the error occured */
    char file[SCE_MAX_INFO_LEN];/**< Filename where the error occred */
    
    char msg[SCE_MAX_MSG_LEN];  /**< Error message */
    
    SCE_ErrorSrc *src;          /**< Source of the error */
    size_t n_src;               /**< Number of src in current backtrace */
    size_t src_size;            /**< Total size allocated for src  */
};

/**
 * \brief The current error.
 * \internal
 *
 * Contains all the data of the current error
 */
static SCE_Error error;
/**
 * \brief Stream used for loggin.
 * \internal
 */
static FILE* stream;


/**
 * \brief Initializes the error manager
 * \param outlog File used for logging
 * \returns 0 if no error occured, else a negative integer
 *
 * This function initializes \c error, with null data and set the stream used for logging.
 */
int SCE_Error_Init (FILE *outlog)
{
    /* on s'assure que stream pointe vers un bon flux */
    stream = (outlog) ? outlog : stderr;

    /* initialisation des tampons */
    memset (error.msg, '\0', SCE_MAX_MSG_LEN);
    memset (error.file, '\0', SCE_MAX_INFO_LEN);
    memset (error.func, '\0', SCE_MAX_INFO_LEN);

    error.date = 0;
    error.have_error = 0;
    error.have_msg = 0;
    error.code = 0;
    error.line = 0;
    error.n_src = 0;
    error.src_size = 0;
    error.src = NULL;

    return 0;
}

/**
 * \brief Clear error manager
 *
 * This function just erases current error, if any.
 */
void SCE_Error_Clear (void)
{
    free (error.src);
    SCE_Error_Init (stream);
}

/**
 * \brief Set error data into \c error
 * \param file File where the error occured
 * \param func Function where the error occured
 * \param line Line where the error occured
 * \param code Error code
 */
void SCE_Error_SetErrorCode_ (const char *file, const char *func,
                              const int line, int code)     
{
    error.date = time (NULL);
    error.have_error = 1;
    error.line = line;
    error.code = code;

    /* pas de verification de la taille de file et func, en principe
       aucun nom de fichier/fonction ne depasse SCE_MAXLEN caracteres... */
    strncpy (error.file, file, SCE_MAX_INFO_LEN);
    strncpy (error.func, func, SCE_MAX_INFO_LEN);
}


/**
 * \brief Log a message
 * \param fmt format printf-like
 * \sa Logger_PrintMsg
 * \returns If an error has already been register,
 * this function returns its error code, 0 otherwise
 *
 * This function logs a message formatted by sprintf.
 * \note The difference between this function and Logger_PrintMsg is that this
 * function register the log into error and Logger_PrintMsg doesn't.
 */
int Logger_LogMsg (const char *fmt, ...)
{
    va_list args;

    /* NOTE: spas un peu pourri non ? faudrait peut-etre afficher l'erreur
             precedente en cas de "double log" */
    if (error.have_msg)
        return error.code;

    if (!error.have_error)
        error.date = time (NULL);

    error.have_error = 1;
    error.have_msg = 1;

    va_start (args, fmt);
    vsnprintf (error.msg, SCE_MAX_MSG_LEN, fmt, args);
    va_end (args);

    return 0;
}

/**
 * \brief Logs a error (code and message) from an errno value
 * \param prefix prefix for the error message (e.g. name of function that
 *               propagated errno) or NULL
 * \param errnum the errno value
 * \returns see Logger_LogMsg()
 * \see Logger_Log() Logger_LogMsg() Logger_LogErrno()
 */
int Logger_LogFromErrno (int errnum, const char* prefix)
{
    if (! prefix)
      prefix = "errno";
    Logger_Log (errnum);
    return Logger_LogMsg ("%s: %s", prefix, strerror (errnum));
}

/**
 * \brief Print a formatted message to the log stream
 * \param fmt The format printf-like
 * \note This function doesn't register the log into error
 */
void Logger_PrintMsg (const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    vfprintf (stream, fmt, args);
    va_end (args);
}

/**
 * \brief Add an error source in the current error backtrace.
 * \param file file where the error occured
 * \param func func where the error occured
 * \param line line where the error occured
 */
void SCE_Error_SetSrc (const char *file, const char *func, const int line)
{
    if (error.n_src + 1 > error.src_size)
    {
        error.src_size += SCE_SRC_ALLOC_SIZE;
        error.src = realloc (error.src, error.src_size * sizeof *error.src);
        if (!error.src)
        {
            fputs ("error: cannot allocate memory in error manager!\n", stderr);
            return; /* hoho */
        }
    }

    error.src[error.n_src].line = line;
    strncpy (error.src[error.n_src].file, file, SCE_MAX_INFO_LEN);
    strncpy (error.src[error.n_src].func, func, SCE_MAX_INFO_LEN);

    error.n_src++;
}


/**
 * \brief Returns the error message
 */
const char* SCE_Error_GetErrorMsg (void)
{
    return error.msg;
}

/**
 * \brief Get the error code
 */
int SCE_Error_GetErrorCode (void)
{
    return error.code;
}

/**
 * \brief Get a string associated with the error code
 */
const char* SCE_Error_GetCodeMsg (int code)
{
    static const char messages[SCE_NUM_ERRORS][18] =
    {
        "out of memory",
        "invalid operation",
        "invalid pointer",
        "invalid argument",
        "invalid size",
        "invalid enumerant",
        "file not found",
        "bad format"
    };

    if (code < 0 || code >= SCE_NUM_ERRORS)
        return NULL;

    strncpy (error.msg, messages[code], SCE_MAX_MSG_LEN);

    return error.msg;
}


/**
 * \brief Get the file where the error occured
 */
const char* SCE_Error_GetFile (void)
{
    return error.file;
}

/**
 * \brief Get the func where the error occured
 */
const char* SCE_Error_GetFunc (void)
{
    return error.func;
}

/**
 * \brief Get the line where the error occured
 */
int SCE_Error_GetLine (void)
{
    return error.line;
}

/**
 * \brief Get the date of the error
 */
time_t SCE_Error_GetTime (void)
{
    return error.date;
}


/**
 * \brief Returns 1 if there is already an error registered, else 0
 */
int SCE_Error_HaveError (void)
{
    return error.have_error;
}


/**
 * \brief Returns 1 if there is already an error message registered, else 0
 */
int SCE_Error_HaveMsg (void)
{
    return error.have_msg;
}


/**
 * \brief Out the error to the log stream.
 * \returns 0 if no error occured, else a negative integer
 *
 * Prints the error in the log stream. This function
 * prints the backtrace of the error.
 */
int SCE_Error_OutStream (void)
{
    char *indent = NULL; /* caracteres d'indentation du message final affiche */
    unsigned int posind = 0; /* important: posind doit etre initialise a zero */
    const struct tm *time_info = NULL; /* structure des donnees de la date */
    char date[32];
    int i = 0;

    if (!error.have_error)
    {
        /* no error was logged, aborting */
        return 0;
    }

    indent = malloc ((error.n_src+2)*2 + 1);
    if (!indent)
    {
        fputs ("error: cannot allocate memory in error manager!\n", stderr);
        return -1; /* hoho */
    }

    /* initialisation des chaines de caractere */
    memset (indent, '\0', (error.n_src+2)*2 + 1);
    memset (date, '\0', sizeof date);

    time_info = gmtime (&error.date);
    SCE_Time_MakeString (date, time_info);

    fprintf (stream, "\n[log %s ]\n>>\n", date);

    /* macro "d'indentation", incremente la chaine d'indentation */
#define INDENT()\
    indent[posind] = indent[posind+1] = ' ';    \
    posind += 2;

    for (i = error.n_src-1; i >= 0; i--)
    {
        INDENT ()
        fprintf (stream,
        "%sfile: '%s'\n"
        "%sfunction: '%s'\n"
        "%sline: %d\n"
        "%s>>\n",
        indent, error.src[i].file,
        indent, error.src[i].func,
        indent, error.src[i].line,
        indent);
    }

    INDENT ()

    fprintf (stream,
    "%sfile: '%s'\n"
    "%sfunction: '%s'\n"
    "%sline: %d\n"
    "%s>>\n",
    indent, error.file,
    indent, error.func,
    indent, error.line,
    indent);

    INDENT ()

#undef INDENT

    fprintf (stream,
    "%serror:\n"
    "%scode: %d\n"
    "%smessage: %s\n"
    "[end]\n",
    indent,
    indent, error.code,
    indent, error.msg);

    free (indent);
    return 0;
}

/** @} */
