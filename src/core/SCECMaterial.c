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
 
/* Cree le : 31 mai 2007
   derniere modification : 21/10/2007 */

#include <string.h>

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECMaterial.h>


/* ajoute le 21/02/2008 */
void SCE_CInitMaterial (SCE_CMaterial *mat)
{
    mat->colors[0][0] = mat->colors[0][1] =
    mat->colors[0][2] = mat->colors[0][3] = 1.0f;
    mat->colors[1][0] = mat->colors[1][1] =
    mat->colors[1][2] = mat->colors[1][3] =
    mat->colors[2][0] = mat->colors[2][1] =
    mat->colors[2][2] = mat->colors[2][3] =
    mat->colors[3][0] = mat->colors[3][1] =
    mat->colors[3][2] = mat->colors[3][3] = 0.0f;
    mat->colors[4][0] = 64.0f;

    mat->use_blending = SCE_FALSE;
    mat->src = GL_ZERO;
    mat->dst = GL_ONE;
}

/* revise le 21/02/2008 */
SCE_CMaterial* SCE_CCreateMaterial (void)
{
    SCE_CMaterial *mat = SCE_malloc (sizeof *mat);
    if (!mat)
        Logger_LogSrc ();
    else
        SCE_CInitMaterial (mat);
    return mat;
}

void SCE_CDeleteMaterial (SCE_CMaterial *mat)
{
    SCE_free (mat);
}


void SCE_CSetMaterialColor (SCE_CMaterial *mat, SCEenum type,
                            float r, float g, float b, float a)
{
    mat->colors[type][0] = r;
    mat->colors[type][1] = g;
    mat->colors[type][2] = b;
    mat->colors[type][3] = a;
}
/* revise le 21/02/2008 */
void SCE_CSetMaterialColorv (SCE_CMaterial *mat, SCEenum type, float *color)
{
    memcpy (mat->colors[type], color, 4 * sizeof *color);
}

/* ajoute le 21/02/2008 */
float* SCE_CGetMaterialColor (SCE_CMaterial *mat, SCEenum type)
{
    return mat->colors[type];
}
/* ajoute le 21/02/2008 */
void SCE_CGetMaterialColorv (SCE_CMaterial *mat, SCEenum type, float *color)
{
    memcpy (color, mat->colors[type], 4 * sizeof *color);
}

/* ajoute le 21/02/2008 */
void SCE_CActiveMaterialBlending (SCE_CMaterial *mat, int use)
{
    mat->use_blending = use;
}
void SCE_CEnableMaterialBlending (SCE_CMaterial *mat)
{
    mat->use_blending = SCE_TRUE;
}
void SCE_CDisableMaterialBlending (SCE_CMaterial *mat)
{
    mat->use_blending = SCE_FALSE;
}

/* ajoute le 21/02/2008 */
void SCE_CSetMaterialBlending (SCE_CMaterial *mat, SCEenum src, SCEenum dst)
{
    mat->src = src;
    mat->dst = dst;
}

/* revise le 17/08/2008 */
void SCE_CUseMaterial (SCE_CMaterial *mat)
{
    static int not_defaults = SCE_TRUE;
    /* couleurs par defaut */
    static float diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    static float specular[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    static float ambient[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    static float emission[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    static float shininess[1] = {64.0f};

    if (mat)
    {
        glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE,
                      mat->colors[SCE_DIFFUSE_COLOR]);
        glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR,
                      mat->colors[SCE_SPECULAR_COLOR]);
        glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT,
                      mat->colors[SCE_AMBIENT_COLOR]);
        glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION,
                      mat->colors[SCE_EMISSIVE_COLOR]);
        glMaterialfv (GL_FRONT_AND_BACK, GL_SHININESS,
                      mat->colors[SCE_SHININESS_COLOR]);
        if (mat->use_blending)
        {
            glEnable (GL_BLEND);
            glBlendFunc (mat->src, mat->dst);
        }
        else
            glDisable (GL_BLEND);
        not_defaults = SCE_TRUE;
    }
    else if (not_defaults)
    {
        glDisable (GL_BLEND);
        /* on place les valeurs par defaut */
        glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
        glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, emission);
        glMaterialfv (GL_FRONT_AND_BACK, GL_SHININESS, shininess);

        not_defaults = SCE_FALSE;
    }
}
