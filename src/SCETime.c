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
 
/* Cree le : 26 fevrier 2007
   derniere modification le 14/02/2008 */

#include <stdio.h>

#include <SCE/utils/SCEMemory.h>
#include <SCE/utils/SCEString.h>

#include <SCE/utils/SCETime.h>

/**
 * \file SCETime.c
 * \copydoc time
 * 
 * \file SCETime.h
 * \copydoc time
 */

/**
 * \defgroup time Time utility functions
 * \ingroup utils
 * \brief Extension of the standards time utility functions
 *
 * This module provides extensions of the standard time functions.
 */

/** @{ */



/* retourne la chaine du mois nb */
/**
 * \brief Get the month's name
 * \param nb a month number -1 (from 0 (junuary) to 11 (december))
 * \returns the month's name
 */
static const char* SCE_Time_GetMonth (const unsigned int nb)
{
    /* chaines statiques des noms des mois de l'annee ranges
     * dans l'ordre croissant. (janvier a decembre)
     * - 12 mois
     * - 4 caracteres (caractere de fin de ligne compris)
     */
    static const char months[12][4] =
    {
        "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
    };

    return months[nb];
}

/* retourne la chaine du jour de la semaine nb */
/**
 * \brief Get the day's name
 * \param nb a day number -1 (from 0 (sunday) to 6 (saturady))
 * \returns the day's name
 */
static const char* SCE_Time_GetDay (const unsigned int nb)
{
    /* chaines statiques des noms des jours de la semaine ranges
     * dans l'ordre croissant. (lundi a dimanche)
     * - 7 jours
     * - 4 caracteres (caractere de fin de ligne compris)
     */
    static const char days[7][4] =
    {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    return days[nb];
}


/* revise le 14/02/2008 */
/**
 * \brief Get a time string
 * \param str a buffer of minimum 25 character where write the time string
 * \param info a tm structure representing the date to write to \p str
 * \see asctime_r()
 *
 * This function write the infos in \p to the \p str buffer, in the following
 * format: Sun Jan 1 00:00:00 1970
 */
void SCE_Time_MakeString (char *str, const struct tm* const info)
{
    sprintf (str, "%s %s %d %.2d:%.2d:%.2d %d",
             SCE_Time_GetDay (info->tm_wday),
             SCE_Time_GetMonth (info->tm_mon),
             info->tm_mday,
             info->tm_hour,
             info->tm_min,
             info->tm_sec, /* TODO: il manque le decalage horaire */
             /* voir -man 3 gmtime */
             info->tm_year+1900);
}


/** @} */
