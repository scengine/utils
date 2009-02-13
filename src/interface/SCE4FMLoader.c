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
 
/* Cree le : 3 novembre 2006
   derniere modification le 17/02/2008 */

#include <stdlib.h>

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
/* revise le 17/02/2008 */
void* SCE_4FM_Load (FILE *fp, const char *fname, void *nb_meshs)
{
    int i = -1;
    int *n_meshs = nb_meshs;  /* affectation importante ! */
    int unused;
    SCE_SMesh **m = NULL;
    SCEenum type;
    FFMesh **me = NULL;

    SCE_btstart ();
    if (!n_meshs)
        n_meshs = &unused;

#define SCE_4FM_ASSERT(c)\
        if ((c))\
        {\
            for (i++; i>0; i--)\
                SCE_Mesh_Delete (m[i-1]);\
            for (i=0; i<*n_meshs; i++)\
                ffm_free (me[i]);\
            free (me);\
            Logger_LogSrc ();\
            SCE_btend ();\
            return NULL;\
        }

    me = ffm_read (fp, n_meshs);
    if (!me)
    {
        Logger_Log (-1);
        Logger_LogMsg ("lib4fm can't load '%s' : %s", fname, ffm_geterror ());
        SCE_btend ();
        return NULL;
    }

    SCE_4FM_ASSERT (!(m = SCE_malloc ((*n_meshs + 1) * sizeof *m)))
    m[*n_meshs] = NULL; /* valeur nulle finale */

    for (i=0; i<*n_meshs; i++)
    {
        SCE_4FM_ASSERT (!(m[i] = SCE_Mesh_Create ()))
        SCE_4FM_ASSERT (SCE_Mesh_AddVerticesDup (m[i], 0, SCE_POSITION,
                        SCE_FLOAT, 3, me[i]->vcount, me[i]->pos) < 0)
        if (me[i]->tex)
            SCE_4FM_ASSERT (SCE_Mesh_AddVerticesDup (m[i], 0, SCE_TEXCOORD0,
                            SCE_FLOAT, 2, me[i]->vcount, me[i]->tex) < 0)
        if (me[i]->nor)
            SCE_4FM_ASSERT (SCE_Mesh_AddVerticesDup (m[i], 0, SCE_NORMAL,
                            SCE_FLOAT, 3, me[i]->vcount, me[i]->nor) < 0)
        switch (me[i]->size)
        {
        case 1: type = SCE_UNSIGNED_BYTE; break;
        case 2: type = SCE_UNSIGNED_SHORT; break;
        case 4: type = SCE_UNSIGNED_INT; break;
        default: type = 0;
        }
        if (type)
            SCE_4FM_ASSERT (SCE_Mesh_SetIndicesDup (m[i], 0, type,
                            me[i]->icount, me[i]->indices) < 0)

        SCE_Mesh_SetRenderMode (m[i], SCE_TRIANGLES);

        SCE_Mesh_ActivateVB (m[i], 0, SCE_TRUE);
        SCE_4FM_ASSERT (SCE_Mesh_Build (m[i]) < 0)
    }

    for (i=0; i<*n_meshs; i++)
        ffm_free (me[i]);
    free (me);

    SCE_btend ();
    return m;
}
