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
 
/* created: 21/10/2007
   updated: 15/01/2008 */

#ifdef SCE_USE_BACKTRACER
#include <stdio.h>
#include <SCE/utils/SCEError.h>
#include <SCE/utils/SCEBacktracer.h>


/* flux de sortie des informations */
#define stream stderr

/* nombre d'indentation */
static unsigned int n_indent = 0;
/* chaine d'indentation */
static const char *const indent = "|  ";

/* pour determiner si le dernier appel a ete fait a BT_End ou BT_Start */
static int need_indent = 0;


/* ajoute le 21/10/2007 */
/* revise le 15/01/2008 */
void SCE_BT_Start (const char *name, unsigned int line)
{
    unsigned int i;
    if (!need_indent)
        fprintf (stream, "\n");
    for (i=0; i<n_indent; i++)
        fprintf (stream, "%s", indent);
    fprintf (stream, "|- %s%s (%u)... ",
             (SCEE_HaveError ()) ? "err " : "", name, line);
    n_indent++;
    need_indent = 0;
}

/* ajoute le 21/10/2007 */
/* revise le 15/01/2008 */
void SCE_BT_End (void)
{
    if (need_indent)
    {
        unsigned int i;
        for (i=0; i<n_indent; i++)
            fprintf (stream, "%s", indent);
    }
    if (SCEE_HaveError ())
        fprintf (stream, "error!\n");
    else
        fprintf (stream, "ok\n");
    n_indent--;
    need_indent = 1;
}

#endif /* SCE_USE_BACKTRACER */
