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
 
/* Cree le : 08/03/2008
   derniere modification : 19/10/2008 */

#include <SCE/SCEMinimal.h>

#include <SCE/core/SCECore.h>
#include <SCE/core/SCECLight.h>

static int use_lighting = SCE_FALSE;

/* ajoute le 08/03/2008 */
/* revise le 18/07/2008 */
void SCE_CInitLight (SCE_CLight *light)
{
    light->color[0] = light->color[1] = light->color[2] = light->color[3] =1.0f;
    SCE_Vector4_Set (light->pos, 0.0, 0.0, 0.0, 1.0);
    SCE_Vector3_Set (light->dir, 0.0, 0.0, -1.0);
    light->angle = 180.0f;
    light->gat = 1.0f;
    light->lat = 0.0f;
    light->qat = 0.0f;
    light->ccontrol = GL_SINGLE_COLOR;
}

/* ajoute le 08/03/2008 */
SCE_CLight* SCE_CCreateLight (void)
{
    SCE_CLight *light = NULL;

    SCE_btstart ();
    if (!(light = SCE_malloc (sizeof *light)))
        Logger_LogSrc ();
    else
        SCE_CInitLight (light);
    SCE_btend ();
    return light;
}

/* ajoute le 08/03/2008 */
void SCE_CDeleteLight (SCE_CLight *light)
{
    SCE_free (light);
}


/* ajoute le 08/03/2008 */
void SCE_CSetLightColor (SCE_CLight *light, float r, float g, float b)
{
    light->color[0] = r;
    light->color[1] = g;
    light->color[2] = b;
}
/* ajoute le 08/03/2008 */
void SCE_CSetLightColorv (SCE_CLight *light, float *c)
{
    memcpy (light->color, c, 3 * sizeof *c);
}
/* ajoute le 08/03/2008 */
float* SCE_CGetLightColor (SCE_CLight *light)
{
    return light->color;
}
/* ajoute le 08/03/2008 */
void SCE_CGetLightColorv (SCE_CLight *light, float *c)
{
    memcpy (c, light->color, 3 * sizeof *c);
}


/* ajoute le 08/03/2008 */
/* revise le 23/03/2008 */
void SCE_CSetLightPosition (SCE_CLight *light, float x, float y, float z)
{
    SCE_Vector3_Set (light->pos, x, y, z);
}
/* ajoute le 08/03/2008 */
void SCE_CSetLightPositionv (SCE_CLight *light, SCE_TVector3 pos)
{
    SCE_Vector3_Copy (light->pos, pos);
}
/* ajoute le 08/03/2008 */
float* SCE_CGetLightPosition (SCE_CLight *light)
{
    return light->pos;
}
/* ajoute le 08/03/2008 */
void SCE_CGetLightPositionv (SCE_CLight *light, SCE_TVector3 pos)
{
    SCE_Vector3_Copy (pos, light->pos);
}

/* ajoute le 07/04/2008 */
/* revise le 19/10/2008 */
void SCE_CInfiniteLight (SCE_CLight *light, int inf)
{
    light->pos[3] = (inf ? 0.0f : 1.0f);
}
int SCE_CIsInfiniteLight (SCE_CLight *light)
{
    return (light->pos[3] == 0.0f); /* TODO: bad */
}

/* ajoute le 18/07/2008 */
void SCE_CSetLightIndependantSpecular (SCE_CLight *light, int separate)
{
    if (separate)
        light->ccontrol = GL_SEPARATE_SPECULAR_COLOR;
    else
        light->ccontrol = GL_SINGLE_COLOR;
}
int SCE_CIsLightIndependantSpecular (SCE_CLight *light)
{
    return (!(light->ccontrol == GL_SINGLE_COLOR));
}

/* ajoute le 14/03/2008 */
void SCE_CSetLightDirection (SCE_CLight *light, float x, float y, float z)
{
    SCE_Vector3_Set (light->dir, x, y, z);
}
/* ajoute le 14/03/2008 */
void SCE_CSetLightDirectionv (SCE_CLight *light, SCE_TVector3 dir)
{
    SCE_Vector3_Copy (light->dir, dir);
}
/* ajoute le 14/03/2008 */
float* SCE_CGetLightDirection (SCE_CLight *light)
{
    return light->dir;
}
/* ajoute le 14/03/2008 */
void SCE_CGetLightDirectionv (SCE_CLight *light, SCE_TVector3 dir)
{
    SCE_Vector3_Copy (dir, light->dir);
}

/* ajoute le 14/03/2008 */
void SCE_CSetLightAngle (SCE_CLight *light, float angle)
{
    light->angle = angle;
}
/* ajoute le 14/03/2008 */
float SCE_CGetLightAngle (SCE_CLight *light)
{
    return light->angle;
}

/* ajoute le 19/03/2008 */
void SCE_CSetLightGlobalAtt (SCE_CLight *light, float att)
{
    light->gat = att;
}
float SCE_CGetLightGlobalAtt (SCE_CLight *light)
{
    return light->gat;
}
void SCE_CSetLightLinearAtt (SCE_CLight *light, float att)
{
    light->lat = att;
}
float SCE_CGetLightLinearAtt (SCE_CLight *light)
{
    return light->lat;
}
void SCE_CSetLightQuadraticAtt (SCE_CLight *light, float att)
{
    light->qat = att;
}
float SCE_CGetLightQuadraticAtt (SCE_CLight *light)
{
    return light->qat;
}


/* ajoute le 15/07/2008 */
void SCE_CActivateLighting (int actived)
{
    use_lighting = actived;
    SCE_CSetState (GL_LIGHTING, actived);
}

/* ajoute le 08/03/2008 */
/* revise le 18/07/2008 */
void SCE_CUseLight (SCE_CLight *light)
{
    static unsigned int num = 0;

    if (light && use_lighting)
    {
        if (num < SCE_CGetInteger (GL_MAX_LIGHTS))
        {
            glEnable  (GL_LIGHT0+num);
            glLightfv (GL_LIGHT0+num, GL_POSITION, light->pos);
            glLightfv (GL_LIGHT0+num, GL_DIFFUSE,  light->color);
            glLightfv (GL_LIGHT0+num, GL_SPECULAR, light->color);

            glLightfv (GL_LIGHT0+num, GL_SPOT_DIRECTION, light->dir);
            glLightf  (GL_LIGHT0+num, GL_SPOT_CUTOFF,    light->angle);

            glLightf (GL_LIGHT0+num, GL_CONSTANT_ATTENUATION,  light->gat);
            glLightf (GL_LIGHT0+num, GL_LINEAR_ATTENUATION,    light->lat);
            glLightf (GL_LIGHT0+num, GL_QUADRATIC_ATTENUATION, light->qat);

            glLightModeli (GL_LIGHT_MODEL_COLOR_CONTROL, light->ccontrol);
            /* force good specular computation */
            glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, SCE_TRUE);
            num++;
        }
    }
    else while (num > 0)
        glDisable (GL_LIGHT0 + (--num));
}
