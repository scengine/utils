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
 
/* created: 08/07/2007
   updated: 07/03/2008 */

#include <stdlib.h>
#include <SCE/SCEMinimal.h>

#include <SCE/interface/SCEMesh.h>
#include <SCE/interface/SCEOBJLoader.h>
#include <SCE/interface/libwar.h>

static int gen_indices = SCE_FALSE;

void SCE_OBJ_ActivateIndicesGeneration (int actived)
{
    gen_indices = actived;
}

void* SCE_OBJ_Load (FILE *fp, const char *fname, void *n_objs)
{
    int i = -1;
    int *n_meshs = n_objs;
    int unused;
    SCE_SMesh **m = NULL;
    WarMesh **me = NULL;

    SCE_btstart ();
    if (!n_meshs)
        n_meshs = &unused;

#define SCE_OBJ_ASSERT(c)\
        if ((c))\
        {\
            for (i++; i>0; i--)\
                SCE_Mesh_Delete (m[i-1]);\
            for (i=0; i<*n_meshs; i++)\
                war_free (me[i]);\
            free (me);\
            Logger_LogSrc ();\
            SCE_btend ();\
            return NULL;\
        }

    me = war_read (fp, gen_indices, n_meshs);
    if (!me)
    {
        Logger_Log (-1);
        Logger_LogMsg ("libwar can't load '%s' : %s", fname, war_geterror ());
        SCE_btend ();
        return NULL;
    }

    SCE_OBJ_ASSERT (!(m = SCE_malloc ((*n_meshs + 1) * sizeof *m)))
    m[*n_meshs] = NULL; /* valeur nulle finale */

    for (i=0; i<*n_meshs; i++)
    {
        SCE_OBJ_ASSERT (!(m[i] = SCE_Mesh_Create ()))
        SCE_OBJ_ASSERT (SCE_Mesh_AddVerticesDup (m[i], 0, SCE_POSITION,
                        SCE_FLOAT, 3, me[i]->vcount, me[i]->pos) < 0)
        if (me[i]->tex)
            SCE_OBJ_ASSERT (SCE_Mesh_AddVerticesDup (m[i], 0, SCE_TEXCOORD0,
                            SCE_FLOAT, 2, me[i]->vcount, me[i]->tex) < 0)
        if (me[i]->nor)
            SCE_OBJ_ASSERT (SCE_Mesh_AddVerticesDup (m[i], 0, SCE_NORMAL,
                            SCE_FLOAT, 3, me[i]->vcount, me[i]->nor) < 0)
        if (me[i]->indices)
            SCE_OBJ_ASSERT (SCE_Mesh_SetIndicesDup (m[i], 0, SCE_UNSIGNED_INT,
                            me[i]->icount, me[i]->indices) < 0)

        SCE_Mesh_SetRenderMode (m[i], SCE_TRIANGLES);

        SCE_Mesh_ActivateVB (m[i], 0, SCE_TRUE);
        SCE_OBJ_ASSERT (SCE_Mesh_Build (m[i]) < 0)
    }

    for (i=0; i<*n_meshs; i++)
        war_free (me[i]);
    free (me);

    SCE_btend ();
    return m;
}
