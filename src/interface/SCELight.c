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
 
/* Cree le : 13/03/2008
   derniere modification : 18/07/2008 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEMatrix.h>
#include <SCE/interface/SCELight.h>


#define SCE_COLOR_INDEX 0
#define SCE_POSITION_INDEX 1
#define SCE_DIRECTION_INDEX 2
#define SCE_ANGLE_INDEX 3
#define SCE_RADIUS_INDEX 4
#define SCE_NUMLIGHTS_INDEX 5

/* tableaux des valeurs a envoyer au shader */
static SCE_TVector4 col_[SCE_MAX_SHADER_LIGHTS];
static SCE_TVector4 pos_[SCE_MAX_SHADER_LIGHTS];
static SCE_TVector3 dir_[SCE_MAX_SHADER_LIGHTS];
static float ang_[SCE_MAX_SHADER_LIGHTS];
static float rad_[SCE_MAX_SHADER_LIGHTS];
/* nombre de lumieres specifiees dans 'toapply' */
static int num_lights = 0;

/* lumieres a appliquer lors de l'appel a Apply */
static SCE_SLight *toapply[SCE_MAX_SHADER_LIGHTS] = {NULL};

/* pointeur par defaut de la matrice camera */
static SCE_TMatrix4 camera_matrix = {1., 0., 0., 0., 0., 1., 0., 0.,
                                     0., 0., 1., 0., 0., 0., 0., 1.};
/* matrice de la camera */
static float *camera_ = camera_matrix;

/* defini si l'eclairage est active */
static int use_lighting = SCE_TRUE;


/* ajoute le 13/03/2008 */
/* revise le 20/03/2008 */
void SCE_Light_Init (SCE_SLight *light)
{
    light->clight = NULL;
    light->intensity = 1.0f;
    light->radius = 16.0f;
    light->gl = SCE_TRUE;
    light->actived = SCE_TRUE;
    light->node = NULL;
}

/* ajoute le 13/03/2008 */
/* revise le 23/03/2008 */
SCE_SLight* SCE_Light_Create (void)
{
    SCE_SLight *light = NULL;

    SCE_btstart ();
    if (!(light = SCE_malloc (sizeof *light)))
        Logger_LogSrc ();
    else
    {
        SCE_Light_Init (light);
        if (!(light->node = SCE_Node_Create ()))
        {
            SCE_Light_Delete (light);
            Logger_LogSrc ();
            SCE_btend ();
            return NULL;
        }
        if ((light->clight = SCE_CCreateLight ()))
            /*SCE_Light_SetRadius (light, light->radius);*/;
        else
        {
            SCE_Light_Delete (light), light = NULL;
            Logger_LogSrc ();
        }
    }
    SCE_btend ();
    return light;
}

/* ajoute le 13/03/2008 */
void SCE_Light_Delete (SCE_SLight *light)
{
    if (light)
    {
        SCE_CDeleteLight (light->clight);
        SCE_Node_Delete (light->node);
        SCE_free (light);
    }
}

/* ajoute le 05/04/2008 */
int SCE_Light_GetNumActived (void)
{
    return num_lights;
}

/* ajoute le 23/03/2008 */
void SCE_Light_SetCameraMatrix (const SCE_TMatrix4 cammat)
{
    camera_ = (float*)cammat;
}


/* ajoute le 20/03/2008 */
void SCE_Light_Activate (SCE_SLight *light, int actived)
{
    light->actived = actived;
}
/* ajoute le 11/07/2008 */
int SCE_Light_IsActived (SCE_SLight *light)
{
    return light->actived;
}

/* ajoute le 13/03/2008 */
void SCE_Light_SetType (SCE_SLight *light, int t)
{
    light->gl = t;
}
/* ajoute le 06/04/2008 */
int SCE_Light_GetType (SCE_SLight *light)
{
    return light->gl;
}

/* ajoute le 23/03/2008 */
SCE_SNode* SCE_Light_GetNode (SCE_SLight *light)
{
    return light->node;
}
/* ajoute le 23/03/2008 */
float* SCE_Light_GetNodeMatrix (SCE_SLight *light)
{
    return SCE_Node_GetMatrix (light->node);
}


/* ajoute le 13/03/2008 */
void SCE_Light_SetColor (SCE_SLight *light, float r, float g, float b)
{
    SCE_CSetLightColor (light->clight, r, g, b);
}
void SCE_Light_SetColorv (SCE_SLight *light, float *c)
{
    SCE_CSetLightColorv (light->clight, c);
}

/* ajoute le 13/03/2008 */
float* SCE_Light_GetColor (SCE_SLight *light)
{
    return SCE_CGetLightColor (light->clight);
}
void SCE_Light_GetColorv (SCE_SLight *light, float *c)
{
    SCE_CGetLightColorv (light->clight, c);
}

/* ajoute le 18/07/2008 */
void SCE_Light_CopyPosition (SCE_SLight *light, float *pos)
{
    SCE_Vector3_Set (pos, 0., 0., 0.);
    SCE_Matrix4_MulV3 (SCE_Node_GetFinalMatrix (light->node), pos);
}

/* ajoute le 18/07/2008 */
void SCE_Light_CopyDirection (SCE_SLight *light, float *dir)
{
    SCE_TMatrix3 mat;
    SCE_Vector3_Set (dir, 0., 0., -1.);
    SCE_Matrix3_CopyM4 (mat, SCE_Node_GetFinalMatrix (light->node));
    SCE_Matrix3_MulV3 (mat, dir);
}

/* ajoute le 07/04/2008 */
void SCE_Light_Infinite (SCE_SLight *light, int inf)
{
    SCE_CInfiniteLight (light->clight, inf);
}
int SCE_Light_IsInfinite (SCE_SLight *light)
{
    return SCE_CIsInfiniteLight (light->clight);
}

/* ajoute le 14/03/2008 */
void SCE_Light_SetAngle (SCE_SLight *light, float a)
{
    SCE_CSetLightAngle (light->clight, a);
}
float SCE_Light_GetAngle (SCE_SLight *light)
{
    return SCE_CGetLightAngle (light->clight);
}

/* ajoute le 22/03/2008 */
void SCE_Light_SetIntensity (SCE_SLight *light, float intensity)
{
    float *color = SCE_CGetLightColor (light->clight);
    SCE_Vector4_Operator1 (color, /=, light->intensity);
    SCE_Vector4_Operator1 (color, *=, intensity);
    light->intensity = intensity;
}
float SCE_Light_GetIntensity (SCE_SLight *light)
{
    return light->intensity;
}

/* ajoute le 19/03/2008 */
/* revise le 15/07/2008 */
void SCE_Light_SetRadius (SCE_SLight *light, float radius)
{
    light->radius = radius;
    if (radius > 1.0)                      /* 3 gives good results */
        SCE_CSetLightQuadraticAtt (light->clight, 3.0/radius);
    else if (radius <= 0.0f)
    {
        SCE_CSetLightQuadraticAtt (light->clight, 0.0);
        SCE_CSetLightLinearAtt (light->clight, 0.0);
    }
    else
        SCE_CSetLightLinearAtt (light->clight, 12.0/radius);
}
float SCE_Light_GetRadius (SCE_SLight *light)
{
    return light->radius;
}


/* ajoute le 15/07/2008 */
void SCE_Light_ActivateLighting (int actived)
{
    use_lighting = actived;
    SCE_CActivateLighting (actived);
}

/* ajoute le 13/03/2008 */
/* revise le 20/03/2008 */
void SCE_Light_Use (SCE_SLight *light)
{
    if (light && use_lighting)
    {
        if (light->actived)
        {
            SCE_TVector3 pos, dir;
            SCE_Light_CopyPosition (light, pos);
            SCE_Light_CopyDirection (light, dir);
            SCE_CSetLightPositionv (light->clight, pos);
            SCE_CSetLightDirectionv (light->clight, dir);
            if (light->gl)
                SCE_CUseLight (light->clight);
            else if (num_lights < SCE_MAX_SHADER_LIGHTS)
            {
                /* enregistrement de la lumiere pour une application future */
                toapply[num_lights] = light;
                num_lights++;
            }
        }
    }
    else
    {
        SCE_CUseLight (NULL);
        num_lights = 0;
    }
}


/* ajoute le 14/03/2008 */
void SCE_Light_MakeIndices (SCE_SShader *shader, int *index)
{
    /* deuxieme parametre a zero, car inutile pour les shaders GLSL
       TODO: ... sauf que maintenant je gere le Cg (en theorie) */
    index[SCE_COLOR_INDEX] =
        SCE_Shader_GetIndex (shader, 0, SCE_COLOR_NAME);
    index[SCE_POSITION_INDEX] =
        SCE_Shader_GetIndex (shader, 0, SCE_POSITION_NAME);
    index[SCE_DIRECTION_INDEX] =
        SCE_Shader_GetIndex (shader, 0, SCE_DIRECTION_NAME);
    index[SCE_ANGLE_INDEX] =
        SCE_Shader_GetIndex (shader, 0, SCE_ANGLE_NAME);
    index[SCE_RADIUS_INDEX] =
        SCE_Shader_GetIndex (shader, 0, SCE_RADIUS_NAME);
    index[SCE_NUMLIGHTS_INDEX] =
        SCE_Shader_GetIndex (shader, 0, SCE_NUMLIGHTS_NAME);
}

/* ajoute le 13/03/2008 */
/* ajoute le 20/07/2008 */
static void SCE_Light_MakeParamsArrays (void)
{
    unsigned int i;
    SCE_TMatrix4 mat;
    SCE_TVector4 pos = {0., 0., 0., 1.};
    SCE_TVector3 dir = {0., 0., -1.};
    for (i=0; i<num_lights; i++)
    {
        SCE_Matrix4_Mul (camera_, SCE_Node_GetFinalMatrix (toapply[i]->node), mat);
        SCE_Matrix4_MulV4 (mat, pos);
        SCE_Matrix4_MulV3 (mat, dir);

        SCE_Vector4_Copy (&col_[i], SCE_CGetLightColor (toapply[i]->clight));
        SCE_Vector4_Copy (&pos_[i], pos);
        SCE_Vector3_Copy (&dir_[i], dir);
        ang_[i] = SCE_CGetLightAngle (toapply[i]->clight);
        rad_[i] = toapply[i]->radius;
    }
}

/* ajoute le 13/03/2008 */
/* revise le 23/03/2008 */
void SCE_Light_Apply (int *index)
{
    if (num_lights > 0)
    {
        SCE_Light_MakeParamsArrays ();
        SCE_Shader_SetParam4fv (index[SCE_COLOR_INDEX], 4*num_lights, col_);
        SCE_Shader_SetParam4fv (index[SCE_POSITION_INDEX], 4*num_lights, pos_);
        SCE_Shader_SetParam3fv (index[SCE_DIRECTION_INDEX], 3*num_lights, dir_);
        SCE_Shader_SetParamfv (index[SCE_ANGLE_INDEX], num_lights, ang_);
        SCE_Shader_SetParamfv (index[SCE_RADIUS_INDEX], num_lights, rad_);
        SCE_Shader_SetParam (index[SCE_NUMLIGHTS_INDEX], num_lights);
    }
}
