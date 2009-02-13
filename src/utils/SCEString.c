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
 
/* Cree le : 28 fevrier 2007
   derniere modification le 24/02/2008 */

#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

/* mode debug trop lourd dans Valof et Strof
 * + flemme de changer ca...
 */
/*
#ifdef __SCE_DEBUG__
 #define DEBUG 1
#else
 #define DEBUG 0
#endif*/


#if (DEBUG)
 #include <stdio.h>
#endif

#include <SCE/utils/SCEError.h>
#include <SCE/utils/SCEMemory.h>
#include <SCE/utils/SCEMath.h>

#include <SCE/utils/SCEString.h>


/**
 * \file SCEString.c
 * \copydoc string
 * \file SCEString.h
 * \copydoc string
 */

/**
 * \defgroup string String utility functions
 * \ingroup utils
 * \brief Some useful string utiliy functions 
 * 
 * This module provides some useful fonctions, like conversions from numbers to
 * string, string to number, length in characters of a number and so on.
 * It also provides some extensions to the standard strings functions like
 * insensitive string comparison, extension extraction and multiple
 * contactenation.
 */

/** @{ */



/** 
 * \brief Counts the number of digits in a number
 * \param arg number to be counted
 * \returns The number of contained digits
 */
static unsigned int SCE_String_lenof (long arg)
{
    unsigned int size = 1;
    long nb = 1;

    if (arg < 0)
    {
        arg = -arg;
        size++;
    }

    while (1)
    {
        nb = nb*10;
        if (arg >= nb)
            size++;
        else
            break;
    }

    return size;
}

/** 
 * \brief Extracts a double from a string
 * \param str string containing the double
 * \returns The extracted double
 */
double SCE_String_Valof (const char *str)
{
    unsigned int i;         /* compteur */
    double res = 0.0;       /* valeur de retour */
    float coeff = 0.0f;
    int val = 0;
    unsigned int str_ilen = 0;  /* longueur de la valeur entiere */
    unsigned int str_dlen = 0;  /* longueur de la valeur décimale */
    unsigned int unit_tmp = 0;

    /* calcul de la longueur de la chaine str -->
       longueur de la partie entiere */
    while (str[str_ilen] != '\0' &&
           str[str_ilen] != '\n' &&
           str[str_ilen] != '.')
    {
        str_ilen++;
    }
    /* --> longueur de la partie decimale (si elle existe uniquement) */
    if (str[str_ilen] == '.')
    {
        while (str[str_dlen+str_ilen+1] != '\0' &&
               str[str_dlen+str_ilen] != '\n')
        {
            str_dlen++;
        }
    }

    if (str[0] == '-' || str[0] == '+')
        val = 1;

    coeff = SCE_Math_Powi (10, str_ilen - val);

    /* calcul de la valeur entiere */
    for (i=val; i<str_ilen; i++)
    {
        unit_tmp = str[i] - '0';
        if (unit_tmp > 9)
        {
            Logger_PrintMsg ("argument %d : '%c' is not a number\n", i, str[i]);
            /* on saute, on s'en occupe pas */
            unit_tmp = 0;
        }
        else
            coeff /= 10.0f;

        res += unit_tmp * coeff;
    }

    coeff = 1.0f;
    /* calcul de la valeur decimale */
    for (i=str_ilen+1; i<str_dlen+str_ilen+1; i++)
    {
        unit_tmp = str[i] - '0';
        if (unit_tmp > 9)
        {
            Logger_PrintMsg ("argument %d : '%c' is not a number\n", i, str[i]);
            /* on saute, on s'en occupe pas */
            unit_tmp = 0;
        }
        else
            coeff /= 10.0f;

        res += unit_tmp*coeff;
    }

    if (str[0] == '-')
        res = -res;

    return res;
}

/** 
 * \brief Writes a double to a string
 * \param arg double to write to a string
 * \param nd double's mantissa accuracy
 * \returns a staticly allocated string containing the double number
 * 
 * \warning This function is NOT reentrant.
 */
const char* SCE_String_Strof (double arg, unsigned int nd)
{
    static char str[32] = {0};
    double vf = 0.0;
    int dec = 0;
    int dec1 = 0;
    unsigned int i = 0, j = 0;
    unsigned int lenof_arg = 0, lenof_dec = 0;
    unsigned int div = 0;

    lenof_arg = SCE_String_lenof (arg);

    /* on ne garde que la valeur decimale */
    vf = arg - (long)arg;

    /* initialisation de str */
    memset (str, '\0', sizeof str);

    /* si arg a une valeur decimale : */
    if (vf != 0.0)
    {
        dec = vf * SCE_Math_Powi (10, nd);
        dec1 = vf * SCE_Math_Powi (10, nd+1);

        if (dec1 + 5 >= SCE_Math_Powi (10, nd+1))
        {
            arg++;
            dec = 0;
        }
        else
        {
            if (dec1 - (dec*10) >= 5)
                dec++;
        }

        lenof_dec = SCE_String_lenof (dec);

        if (arg < 0)
        {
            str[0] = '-';
            arg = -arg;
            i++;
        }

        while (i < lenof_arg)
        {
            div = SCE_Math_Powi (10, lenof_arg-i-1);

            str[i] = (arg/div) + '0';
            i++;

            arg = arg - ((int)(arg/div) * div);
        }

        str[i] = '.';
        i++;

        if (lenof_dec < nd)
        {
            for (j=0; j<(nd-lenof_dec); j++)
            {
                str[i] = '0';
                i++;
            }
            lenof_dec = nd;
        }

        while (i < lenof_arg+lenof_dec+1)
        {
            div = SCE_Math_Powi (10, lenof_dec-i+lenof_arg);

            str[i] = (dec/div) + '0';
            i++;

            dec = dec - ((int)(dec/div) * div);
        }
    }
    else
    {
        if (arg < 0)
        {
            str[0] = '-';
            arg = -arg;
            i++;
        }

        while (i < lenof_arg)
        {
            div = SCE_Math_Powi (10, lenof_arg-i-1);
            
            str[i] = (arg/div) + '0';
            i++;

            arg = arg-((int)(arg/div) * div);
        }
    }

    return str;
}

/** 
 * \brief Finds out how many times a character is in a string
 * \param str first String to be scanned
 * \param c second Character sought
 * \returns a string containing the double number
 */
unsigned int SCE_String_NChrInStr(const char *str, char c)
{
    /* nombre d'occurrences */
    unsigned int nb = 0;

    for (; *str != '\0'; str++)
    {
        if (*str == c)
            nb++;
    }

    return nb;
}

/**
 * \brief Get the extension part of a filename
 * \param str the filename from where search extension
 * \returns a pointer to the extension part of \p str or NULL if \p str has no
 * extension
 */
char* SCE_String_GetExt (char *str)
{
    size_t i = strlen (str);

    while (i != 0 && str[i] != '.')
        i--;

    if (i == 0 && str[0] != '.') /* sans extension... */
        return NULL;

    return &str[i];
}


#ifndef BAN_IMPL
/** 
 * \brief Compares two strings
 * \param str1 first string to compare
 * \param str2 second string to compare with first
 * \param cmp_case 1 for sensitive comparison, 0 else
 * \returns 0 if both strings are the same, 1 otherwise
 */
int SCE_String_Cmp (const char *str1, const char *str2, int cmp_case)
{
    size_t i;
    size_t len;
    int equal = 1;

    /* on recupere la plus petite taille */
    len = strlen (str1);

    if (len != strlen (str2))
        return 0;

    if (cmp_case)
    {
        for (i=0; i<len; i++)
        {
            if (str1[i] != str2[i])
            {
                equal = 0;
                break;
            }
        }
    }
    else
    {
        for (i=0; i<len; i++)
        {
            if (tolower (str1[i]) != tolower (str2[i]))
            {
                equal = 0;
                break;
            }
        }
    }

    return equal;
}

#else /* BAN_IMPL */

/** 
 * \brief Compares two strings
 * \param str1 first string to compare
 * \param str2 second string to compare
 * \param cmp_case 1 for case sensitive comparison, 0 else
 * \returns an integer. 0 if the strings are the same, a number greater than 0
 * if the first string is greater than the second, and a number below 0
 * otherwise
 * \see string.h's strcmp()
 */
int SCE_String_Cmp (const char *str1, const char *str2, int cmp_case)
{
    int rv;

    if (cmp_case)
    {
        for (rv = 0; (rv = str1 - str2) == 0; str1++, str2++)
        {
            if (*str1 == 0)
                break;
        }
    }
    else
    {
        for (rv = 0; (rv = tolower (*str1) - tolower (*str2)) == 0; str1++, str2++)
        {
            if (*str1 == 0)
                break;
        }
    }

    return rv;
}
#endif /* BAN_IMPL */

/* ajoute le 18/10/2007 */
/* revise le 19/10/2007 */
/**
 * \brief Duplicates a string
 * \param src the string to copy
 * \returns a newly allocated duplication of \p src or NULL on error
 */
char* SCE_String_Dup (const char *src)
{
    if (src)
    {
        size_t s = strlen (src) + 1;
        char *new = SCE_malloc (s);
        if (!new)
            Logger_LogSrc ();
        else
            memcpy (new, src, s);
        return new;
    }
    else
        return NULL;
}

/* ajoute le 18/10/2007 */
/* revise le 20/07/2008 */
/**
 * \brief Duplicates the concatenation of two strings
 * \param a first string
 * \param b second string to concatenante to first
 * \returns a newly allocated string containing both \p a and \p b strings 
 */
char* SCE_String_CatDup (const char *a, const char *b)
{
    size_t size = 1;
    char *new = NULL;

    if (a) size += strlen (a);
    if (b) size += strlen (b);

    new = SCE_malloc (size);
    if (!new)
        Logger_LogSrc ();
    else
    {
        memset (new, '\0', size);
        if (a) strcat (new, a);
        if (b) strcat (new, b);
    }
    return new;
}

/*#define SCE_String_CatDup(a, b) (SCE_String_CatDupV (a, b, NULL))*/

/**
 * \brief Duplicates concatenated strings
 * \param str the first string to be concatenated
 * \param ... a NULL-ended list of strings to concatenate
 * \returns a newly allocated string containing the concatenation of every
 * parameters
 */
char* SCE_String_CatDupMulti (const char* str, ...)
{
    size_t size = 1;
    char *new = NULL;
    char* tmp;
    va_list ap;
    
    size += strlen (str);
    va_start (ap, str);
    while ((tmp = va_arg (ap, char*)) != NULL)
        size += strlen (tmp);
    va_end (ap);
    
    new = SCE_malloc (size);
    if (!new)
        Logger_LogSrc ();
    else
    {
        /*memset (new, '\0', size);*/
        strcat (new, str);
        
        va_start (ap, str);
        while ((tmp = va_arg (ap, char*)) != NULL)
            strcat (new, tmp);
        va_end (ap);
        
        new[size -1] = '\0';
    }
    
    return new;
}

/** @} */
