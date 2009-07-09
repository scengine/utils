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
 
/* created: 11/02/2007
   updated: 12/03/2008 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEString.h>
#include <SCE/core/SCECSupport.h>

#include <SCE/core/SCECShader.h>

static unsigned int ncalled = 0;

unsigned int SCE_CGetShaderNumBatchs (void)
{
    unsigned int k = ncalled;
    ncalled = 0;
    return k;
}


/* Cg */
#ifdef SCE_USE_CG
/* constantes internes */
/* macros qui serviront a CgManager */
#define SCE_INIT 1
#define SCE_QUIT 2
#define SCE_GET_STATE 3


/* NOTE: truc douteux.. :/ ? */
/* a, b, c.. -> 3
   a, b, c, d, e, f, g.. -> 7
   Cg */
#define SCE_CG_ERROR (SCE_NUM_ERRORS+37)

/* variables statiques necessaires a l'utilisation de Cg */
static CGcontext context;
static CGprofile vs_profile;
static CGprofile ps_profile;


/* fonction callback en cas d'erreur Cg */
static void SCE_CCgOnError (void)
{
    SCEE_Log (SCE_CG_ERROR);
    SCEE_LogMsg ("a Cg error was occured :\n- %s\n- %s",
                   cgGetErrorString (cgGetError()), cgGetLastListing (context));
}


/* fonction de manipulation du runtime Cg */
static int SCE_CCgManager (int action)
{
    static int is_init = SCE_FALSE;

    SCE_btstart ();
    switch (action)
    {
    case SCE_INIT:
        if(!is_init)
        {
            /* envoie de la fonction callback pour la gestion des erreurs */
            cgSetErrorCallback (SCE_CCgOnError);

            /* creation du contexte */
            context = cgCreateContext ();
            if (SCEE_HaveError () && SCEE_GetCode () == SCE_CG_ERROR)
            {
                SCEE_LogSrc ();
                is_init = SCE_ERROR;
                break; /* on sort... */
            }

            /* chargement des 'profiles' */
            vs_profile = cgGLGetLatestProfile (CG_GL_VERTEX);
            ps_profile = cgGLGetLatestProfile (CG_GL_FRAGMENT);
            cgGLSetOptimalOptions (vs_profile);
            cgGLSetOptimalOptions (ps_profile);

            is_init = 1;
        }
        break;

    case SCE_QUIT:
        if (is_init)
        {
            if (context)
                cgDestroyContext (context);

            context = NULL; /* CGcontext n'est qu'un typedef struct* ... */
            vs_profile = CG_PROFILE_UNKNOWN;
            ps_profile = CG_PROFILE_UNKNOWN;

            is_init = 0;
        }
        break;

    /* rien a faire de particulier pour get state */
    case SCE_GET_STATE:
    }

    SCE_btend ();
    return is_init;
}
#endif /* SCE_USE_CG */

/* renvoie un booleen qui vaut true si type est un shader de pixel */
static int SCE_CIsPixelShader (SCEenum type)
{
    return (type == SCE_PIXEL_SHADER);
}


int SCE_CShaderInit (int use_cg)
{
    SCE_btstart ();
    #ifdef SCE_USE_CG
    if (use_cg)
    {
        if (!SCE_CCgManager (SCE_INIT))
        {
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
    }
    #else /* pour eviter un warning : unused parameter 'use_cg' */
    use_cg = 0;
    #endif

    SCE_btend ();
    return SCE_OK;
}

void SCE_CShaderQuit (void)
{
    #ifdef SCE_USE_CG
    SCE_CCgManager (SCE_QUIT);
    #endif
}


#ifdef SCE_USE_CG
CGprofile SCE_CGetCgProfile (SCEenum type)
{
    return (type == SCE_PIXEL_SHADER) ? ps_profile : vs_profile;
}
#endif


SCE_CShaderGLSL* SCE_CCreateShaderGLSL (SCEenum type)
{
    SCE_CShaderGLSL *shader = NULL;

    SCE_btstart ();
    shader = SCE_malloc (sizeof *shader);
    if (!shader)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    shader->data = NULL;
    shader->compiled = SCE_FALSE;
    shader->is_pixelshader = SCE_CIsPixelShader (type);
    shader->type = (shader->is_pixelshader) ?
                    GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;

    shader->id = glCreateShader (shader->type);
    if (shader->id == 0)
    {
        /* une erreur est survenue lors de la creation du shader */
        SCEE_Log (SCE_ERROR);
        SCEE_LogMsg ("I can't create a shader, what's the fuck ?");
        SCE_free (shader);
        SCE_btend ();
        return NULL;
    }

    SCE_btend ();
    return shader;
}

#ifdef SCE_USE_CG
SCE_CShaderCG* SCE_CCreateShaderCG (SCEenum type)
{
    SCE_CShaderCG *shader = NULL;

    SCE_btstart ();
    shader = SCE_malloc (sizeof *shader);
    if (!shader)
    {
        SCEE_LogSrc();
        SCE_btend ();
        return NULL;
    }

    shader->id = NULL;
    shader->args = NULL;
    shader->data = NULL;
    shader->compiled = SCE_FALSE;
    shader->is_pixelshader = SCE_CIsPixelShader (type);
    shader->type = SCE_CGetCgProfile (type);

    SCE_btend ();
    return shader;
}
#endif

/* *** */

void SCE_CDeleteShaderGLSL (SCE_CShaderGLSL *shader)
{
    SCE_btstart ();
    if (shader)
    {
        if (glIsShader (shader->id))
            glDeleteShader (shader->id);
        SCE_free (shader->data);
        SCE_free (shader);
    }
    SCE_btend ();
}

#ifdef SCE_USE_CG
void SCE_CDeleteShaderCG (SCE_CShaderCG *shader)
{
    SCE_btstart ();
    if (shader)
    {
        if (shader->id)
            cgDestroyProgram (shader->id);
        if (shader->args)
        {
            unsigned int i;
            for (i=0; shader->args[i]; i++)
                SCE_free (shader->args[i]);
            SCE_free (shader->args);
        }
        SCE_free (shader->data);
        SCE_free (shader);
    }
    SCE_btend ();
}
#endif


void SCE_CSetShaderGLSLSource (SCE_CShaderGLSL *shader, char *src)
{
    SCE_free (shader->data);
    shader->data = src;
}
int SCE_CSetShaderGLSLSourceDup (SCE_CShaderGLSL *shader, char *src)
{
    char *new = NULL;

    SCE_btstart ();
    new = SCE_String_Dup (src);
    if (!new)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_CSetShaderGLSLSource (shader, new);
    SCE_btend ();
    return SCE_OK;
}

#ifdef SCE_USE_CG
void SCE_CSetShaderCGSource (SCE_CShaderCG *shader, char *src)
{
    SCE_free (shader->data);
    shader->data = src;
}
int SCE_CSetShaderCGSourceDup (SCE_CShaderCG *shader, char *src)
{
    char *new = NULL;

    SCE_btstart ();
    new = SCE_String_Dup (src);
    if (!new)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    SCE_CSetShaderCGSource (shader, new);
    SCE_btend ();
    return SCE_OK;
}

void SCE_CSetShaderCGArgs (SCE_CShaderCG *shader, char **args)
{
    SCE_btstart ();
    if (shader->args)
    {
        usigned int i;
        for (i=0; shader->args[i]; i++)
            SCE_free (shader->args[i]);
        SCE_free (shader->args);
    }
    shader->args = args;
    SCE_btend ();
}
int SCE_CSetShaderCGArgsDup (SCE_CShaderCG *shader, char **args)
{
    size_t s = 0, i;
    char **new;

    SCE_btstart ();
    while (args[s])
        s++;
    new = SCE_malloc (s * sizeof *new + 1);
    if (!new)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return SCE_ERROR;
    }
    for (i=0; i<s; i++)
    {
        if (!(new[i] = SCE_String_Dup (args[i])))
        {
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
    }
    new[s] = NULL;
    SCE_CSetShaderCGArgs (shader, new);
    SCE_btend ();
    return SCE_OK;
}
#endif



/* fonctions generique des erreurs de construction */
static int SCE_CCantRecompile (void)
{
    SCEE_Log (SCE_INVALID_OPERATION);
    SCEE_LogMsg ("you can't re-compile a shader");
    return SCE_ERROR;
}
static int SCE_CNeedCode (void)
{
    SCEE_Log (SCE_INVALID_OPERATION);
    SCEE_LogMsg ("you can't compile a shader without source code");
    return SCE_ERROR;
}

int SCE_CBuildShaderGLSL (SCE_CShaderGLSL *shader)
{
    int compile_status = GL_TRUE;
    int loginfo_size = 0;
    char *loginfo = NULL;  /* journal de compilation (info log) */

    SCE_btstart ();
    /* NOTE: et pourquoi on pourrait pas recompiler un shader ?? */
    if (shader->compiled)
    {
        SCE_btend ();
        return SCE_CCantRecompile ();
    }

    if (!shader->data)
    {
        SCE_btend ();
        return SCE_CNeedCode ();
    }

    /* cast: fucking hack */
    glShaderSource (shader->id, 1, (const GLchar**)&shader->data, NULL);
    glCompileShader (shader->id);

    /* verification du succes de la compilation */
    glGetShaderiv (shader->id, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE)
    {
        /* erreur a la compilation
           recuperation du log d'erreur */

        SCEE_Log (SCE_INVALID_OPERATION);

        glGetShaderiv (shader->id, GL_INFO_LOG_LENGTH, &loginfo_size);
        loginfo = SCE_malloc (loginfo_size + 1);
        if (!loginfo)
        {
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }

        memset (loginfo, '\0', loginfo_size + 1);
        glGetShaderInfoLog (shader->id, loginfo_size, &loginfo_size, loginfo);

        SCEE_LogMsg ("error while compiling GLSL %s shader :\n%s",
                       shader->is_pixelshader ? "pixel":"vertex", loginfo);

        SCE_free (loginfo);
        SCE_btend ();
        return SCE_ERROR;
    }

    shader->compiled = SCE_TRUE;
    SCE_btend ();
    return SCE_OK;
}

#ifdef SCE_USE_CG
int SCE_CBuildShaderCG (SCE_CShaderCG *shader)
{
    SCE_btstart ();
    /* on verifie si Cg a ete initialise */
    if (!SCE_CCgManager (SCE_GET_STATE))
    {
        /* le contexte Cg n'a pas ete ou a ete detrui, arret */
        SCEE_Log (SCE_INVALID_OPERATION);
        SCEE_LogMsg ("you can't build a Cg shader if you have"
                       " not initialized the shaders manager");
        SCE_btend ();
        return SCE_ERROR;
    }

    if (shader->compiled)
    {
        SCE_btend ();
        return SCE_CCantRecompile ();
    }

    if (!shader->data)
    {
        SCE_btend ();
        return SCE_CNeedCode ();
    }

    #define SCE_CG_VERIF()\
    if (SCEE_HaveError () && SCEE_GetCode () == SCE_CG_ERROR)\
    {\
        SCEE_LogSrc ();\
        SCE_btend ();\
        return SCE_ERROR;\
    }

    shader->id = cgCreateProgram (context, CG_SOURCE, shader->data,
                                  shader->type, "main",
                                  (const char**)shader->args);
    SCE_CG_VERIF ()

    /* NOTE: faudra que je voye si c'est bien necessaire tout ca */
    /* compilation du program */
    if (!cgIsProgramCompiled (shader->id))
    {
        /* program non compile -> alors on le fait */
        cgCompileProgram (shader->id);
    }

    /* et on fait CA pour 'charger' le shader ?? */
    cgGLLoadProgram (shader->id);
    SCE_CG_VERIF ()

    #undef SCE_CG_VERIF

    shader->compiled = SCE_TRUE;

    SCE_btend ();
    return SCE_OK;
}
#endif

/* *** */

#ifdef SCE_USE_CG
void SCE_CUseShaderCG (SCE_CShaderCG *shader)
{
    static int is_ps = -1;

    if (shader)
    {
        cgGLBindProgram (shader->id);
        cgGLEnableProfile (shader->type);
        
        is_ps = (is_ps == SCE_TRUE) ? SCE_TRUE : shader->is_pixelshader;
    }
    else if (is_ps == SCE_TRUE)
    {
        cgGLDisableProfile (ps_profile);
        cgGLUnbindProgram (ps_profile);
        is_ps = SCE_FALSE;
    }
    else if (is_ps == SCE_FALSE)
    {
        /* NOTE: verifier l'ordre de desactivation!!! (Unbind & Disable) */
        cgGLDisableProfile (vs_profile);
        cgGLUnbindProgram (vs_profile);
        is_ps = -1;
    }
}
#endif

/* *** */

/** !! ** fonctions reservees aux shaders GLSL ** !! **/

SCE_CProgram* SCE_CCreateProgram (void)
{
    SCE_CProgram *prog = NULL;

    SCE_btstart ();
    prog = SCE_malloc (sizeof *prog);
    if (!prog)
    {
        SCEE_LogSrc ();
        SCE_btend ();
        return NULL;
    }

    prog->id = glCreateProgram ();
    /* et on teste pas la valeur de retour ?? */
    prog->compiled = SCE_FALSE;

    SCE_btend ();
    return prog;
}

void SCE_CDeleteProgram (SCE_CProgram *prog)
{
    SCE_btstart ();
    if (prog)
    {
        if (glIsProgram (prog->id))
            glDeleteProgram (prog->id);
        SCE_free (prog);
    }
    SCE_btend ();
}

int SCE_CSetProgramShader (SCE_CProgram *prog, SCE_CShaderGLSL *shader,
                           int attach)
{
    SCE_btstart ();
    if (attach)
        glAttachShader (prog->id, shader->id);
    else
        glDetachShader (prog->id, shader->id);

    /* il faut le recompiler pour appliquer les modifications! */
    prog->compiled = SCE_FALSE;
    SCE_btend ();
    return SCE_OK;
}

int SCE_CBuildProgram (SCE_CProgram *prog)
{
    int status = GL_TRUE;
    int loginfo_size = 0;
    char *loginfo = NULL;  /* chaine du log d'erreur */

    SCE_btstart ();
    /* lie le program a ses shaders 'attaches' */
    glLinkProgram (prog->id);

    /* recuperation du status du liage */
    glGetProgramiv (prog->id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        /* erreur de 'linkage', recuperation du message d'erreur */
        SCEE_Log (SCE_INVALID_OPERATION);

        glGetProgramiv (prog->id, GL_INFO_LOG_LENGTH, &loginfo_size);
        loginfo = SCE_malloc (loginfo_size + 1);
        if (!loginfo)
        {
            SCEE_LogSrc ();
            SCE_btend ();
            return SCE_ERROR;
        }
        memset (loginfo, '\0', loginfo_size + 1);
        glGetProgramInfoLog (prog->id, loginfo_size, &loginfo_size, loginfo);

        SCEE_LogMsg ("can't link program, reason : %s", loginfo);

        SCE_free (loginfo);
        SCE_btend ();
        return SCE_ERROR;
    }
    glValidateProgram (prog->id);
    glGetProgramiv (prog->id, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE)
    {
        /* what to do? */
        /* TODO: add program name */
        SCEE_SendMsg ("can't validate program");
    }
    prog->compiled = SCE_TRUE;
    SCE_btend ();
    return SCE_OK;
}

void SCE_CUseProgram (SCE_CProgram *prog)
{
    if (prog)
    {
        glUseProgram (prog->id);
        ncalled++;
    }
    else
        glUseProgram (0);
}

/** ********************************************* **/


#if 0
void SCE_CDisableShaderGLSL (void)
{
    glUseProgram (0);
}
#ifdef SCE_USE_CG
void SCE_CDisableShaderCG (int type)
{
    if (type == SCE_PIXEL_SHADER)
    {
        cgGLDisableProfile (ps_profile);
        cgGLUnbindProgram (ps_profile);
    }
    else
    {
        cgGLDisableProfile (vs_profile);
        cgGLUnbindProgram (vs_profile);
    }
}
#endif
#endif


SCEint SCE_CGetProgramIndex (SCE_CProgram *prog, const char *name)
{
    return glGetUniformLocation (prog->id, name);
}
#ifdef SCE_USE_CG
CGparameter SCE_CGetShaderCGIndex (SCE_CShaderCG *shader, const char *name)
{
    return cgGetNamedParameter (shader->id, name);
}
#endif
/* ajoute le 12/03/2008 */
SCEint SCE_CGetProgramAttribIndex (SCE_CProgram *prog, const char *name)
{
    return glGetAttribLocation (prog->id, name);
}


/* revise le 14/03/2008 */
void SCE_CSetProgramParam (SCEint index, int val)
{
    glUniform1i (index, val);
}
void SCE_CSetProgramParamf (SCEint index, float val)
{
    glUniform1f (index, val);
}
void SCE_CSetProgramParam1fv (SCEint index, size_t size, const float *val)
{
    glUniform1fv (index, size, val);
}
void SCE_CSetProgramParam2fv (SCEint index, size_t size, const float *val)
{
    glUniform2fv (index, size, val);
}
void SCE_CSetProgramParam3fv (SCEint index, size_t size, const float *val)
{
    glUniform3fv (index, size, val);
}
void SCE_CSetProgramParam4fv (SCEint index, size_t size, const float *val)
{
    glUniform4fv (index, size, val);
}
void SCE_CSetProgramMatrix2 (SCEint index, size_t size, const float *mat)
{
    glUniformMatrix2fv (index, size, SCE_TRUE, mat);
}
void SCE_CSetProgramMatrix3 (SCEint index, size_t size, const float *mat)
{
    glUniformMatrix3fv (index, size, SCE_TRUE, mat);
}
void SCE_CSetProgramMatrix4 (SCEint index, size_t size, const float *mat)
{
    glUniformMatrix4fv (index, size, SCE_TRUE, mat);
}


#ifdef SCE_USE_CG
/* revise le 14/03/2008 */
void SCE_CSetShaderCGParam (CGparameter param, int val)
{
    cgGLSetParameter1f (param, (float)val);
}
void SCE_CSetShaderCGParamf (CGparameter param, float val)
{
    cgGLSetParameter1f (param, val);
}
/* revise le 14/03/2008 */
#define SCE_CG_PARAM_FUNC(n)\
void SCE_CSetShaderCGParam##n##fv (CGparameter param, size_t size, const float *val)\
{\
    if (cgGetParameterType (param) == CG_ARRAY)\
    {\
        int asize = cgGetArraySize (param, 0), i;\
        /* TODO: ne gere theoriquement pas les tableaux tab[][]... */\
        for (i=0; i<asize && i<size; i++)\
            cgGLSetParameter##n##fv (cgGetArrayParameter (param, i), val);\
    }\
    else\
        cgGLSetParameter##n##fv (param, val);\
}
SCE_CG_PARAM_FUNC(1)
SCE_CG_PARAM_FUNC(2)
SCE_CG_PARAM_FUNC(3)
SCE_CG_PARAM_FUNC(4)
#undef SCE_CG_PARAM_FUNC

void SCE_CSetShaderCGMatrix (CGparameter param, const float *mat)
{
    cgGLSetMatrixParameterfc (param, mat);
}
#endif /* SCE_USE_CG */
