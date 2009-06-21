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
 
/* created: 03/11/2006
   updated: 20/06/2009 */

#include <stdlib.h>             /* included by SCEMinimal.h, isn't it? */

#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCEMesh.h>
#include <SCE/interface/lib4fm.h> /* haha, integrated library :-' */
#include <SCE/interface/SCE4FMLoader.h>

/*
 * d'apres les specifications du 26/01/2008
 */

/**
 * \todo types a revoir a cause des specifications
 */
void* SCE_4FM_Load (FILE *fp, const char *fname, void *nb_meshs)
{
    int i;
    int unused;
    SCE_SMesh **m = NULL;
    SCEenum type;
    FFMesh **me = NULL;
    int *n_meshs = nb_meshs;

    SCE_btstart ();
    if (!n_meshs)
        n_meshs = &unused;

    me = ffm_read (fp, n_meshs);
    if (!me)
    {
        Logger_Log (-1);
        Logger_LogMsg ("lib4fm can't load '%s' : %s", fname, ffm_geterror ());
        SCE_btend ();
        return NULL;
    }

    if (!(m = SCE_malloc ((*n_meshs + 1) * sizeof *m)))
        goto failure;
    for (i = 0; i <= *n_meshs; i++)
        m[i] = NULL;

    for (i = 0; i < *n_meshs; i++)
    {
        if (!(m[i] = SCE_Mesh_Create ()))
            goto failure;
        if (SCE_Mesh_AddVerticesDup (m[i], 0, SCE_POSITION, SCE_FLOAT, 3,
                                     me[i]->vcount, me[i]->pos) < 0)
            goto failure;
        if (me[i]->tex)
        {
            if (SCE_Mesh_AddVerticesDup (m[i], 0, SCE_TEXCOORD0, SCE_FLOAT, 2,
                                         me[i]->vcount, me[i]->tex) < 0)
                goto failure;
        }
        if (me[i]->nor)
        {
            if (SCE_Mesh_AddVerticesDup (m[i], 0, SCE_NORMAL, SCE_FLOAT, 3,
                                         me[i]->vcount, me[i]->nor) < 0)
                goto failure;
        }
        switch (me[i]->size)
        {
        case 1: type = SCE_UNSIGNED_BYTE; break;
        case 2: type = SCE_UNSIGNED_SHORT; break;
        case 4: type = SCE_UNSIGNED_INT; break;
        default: type = 0;
        }
        if (type)
        {
            if (SCE_Mesh_SetIndicesDup (m[i], 0, type, me[i]->icount,
                                        me[i]->indices) < 0)
                goto failure;
        }
        SCE_Mesh_SetRenderMode (m[i], SCE_TRIANGLES);
        if (SCE_Mesh_Build (m[i]) < 0)
            goto failure;
    }

    for (i = 0; i < *n_meshs; i++)
        ffm_free (me[i]);
    free (me);

    SCE_btend ();
    return m;
failure:
    for (i = 0; i > *n_meshs; i++)
    {
        SCE_Mesh_Delete (m[i]);
        ffm_free (me[i]);
    }
    free (me);                  /* I know. */
    Logger_LogSrc ();
    SCE_btend ();
    return NULL;
}
